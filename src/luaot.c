/*
 * Lua bytecode-to-C compiler
 */

// This luac-derived code is incompatible with lua_assert because it calls the
// GETARG macros even for opcodes where it is not appropriate to do so.
#undef LUAI_ASSERT

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"

#include "ldebug.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lopnames.h"
#include "lstate.h"
#include "lundump.h"

//
// Command-line arguments and main function
// ----------------------------------------
//
// This part should not depend much on the Lua version
//

static const char *program_name    = "luaot";
static char *input_filename  = NULL;
static char *output_filename = NULL;
static char *module_name     = NULL;

static FILE * output_file = NULL;
static int nfunctions = 0;
static TString **tmname;

int type = 4; // 0 = wasm, 2 = c, 4 = auto
int opt = 0;
int alloc = 0;
int debug = 0;
//int partial = 0;
int shrink = 1;
int goto_mode = 0;
int closureopt = 0;
char secondaryFiles[100][20];

static
void usage()
{
    fprintf(stderr,
          "usage: %s [options] [filenames]\n"
          "Available options are:\n"
          " Basic options:\n"
          "  -h                 show this help\n"
          "  -v                 show version\n"
          "  -o name            output to file 'name'\n"
          "  -g                 debug mode\n"
          " Optimization options:\n"
          //"  -p                 partial evaluator (experimental)\n"
          "  -f                 apply -O3 to emscripten\n"
          "  -z                 disable shrink optimization for WASM\n"
          "  -a                 enable pool memory allocation (for memory-heavy scripts)\n"
          "  -p                 run production optimizations\n"
          "  -t                 use gotos instead of switches in generated code (experimental)\n"
          " Language options:\n"
          //"  -js                output JavaScript\n"
          "  -wasm              output WebAssembly (via Emscripten)\n"
          "  -c                 output C\n",
          //"  -rust              output Rust\n"
          //"  -html              output HTML\n"
          program_name);
}

static
void fatal_error(const char *msg)
{
    fprintf(stderr, "%s: %s\n", program_name, msg);
    exit(1);
}

static
__attribute__ ((format (printf, 1, 2)))
void print(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(output_file, fmt, args);
    va_end(args);
}

static
__attribute__ ((format (printf, 1, 2)))
void println(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(output_file, fmt, args);
    va_end(args);
    fprintf(output_file, "\n");
}

static
void printnl()
{
    // This separate function avoids Wformat-zero-length warnings with println
    fprintf(output_file, "\n");
}


static void doargs(int argc, char **argv)
{
    // I wonder if I should just use getopt instead of parsing options by hand
    program_name = argv[0];

    int do_opts = 1;
    int npos = 0;
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (do_opts && arg[0] == '-') {
            if (0 == strcmp(arg, "--")) {
                do_opts = 0;
            } else if (0 == strcmp(arg, "-h")) {
                usage();
                exit(0);
            } else if (0 == strcmp(arg, "-v")) {
                printf("%s\n", LUA_COPYRIGHT);
                exit(0);
            /*} else if (0 == strcmp(arg, "-js")) {
                type = 1;
            */} else if (0 == strcmp(arg, "-c")) {
                type = 2;
            /*} else if (0 == strcmp(arg, "-rust")) {
                type = 3;
            */} else if (0 == strcmp(arg, "-wasm")) {
                type = 0;
            /*} else if (0 == strcmp(arg, "-html")) {
                type = 5;
            */} else if (0 == strcmp(arg, "-p")) {
                closureopt = 0;
            } else if (0 == strcmp(arg, "-f")) {
                opt = 1;
            }  else if (0 == strcmp(arg, "-z")) {
                shrink = 0;
            } else if (0 == strcmp(arg, "-t")) {
                goto_mode = 1;
            } else if (0 == strcmp(arg, "-g")) {
                debug = 1;
            } else if (0 == strcmp(arg, "-a")){
                alloc = 1;
            } else if (0 == strcmp(arg, "-o")) {
                i++;
                if (i >= argc) { fatal_error("missing argument for -o"); }
                output_filename = argv[i];
            } else {
                fprintf(stderr, "unknown option %s\n", arg);
                exit(1);
            }
        } else {
            if (npos == 0) {
                input_filename = arg;
            } else if (npos < 100) {
                strcpy(secondaryFiles[npos-1], arg);
            } else {
                fatal_error("too many input files");
            }
            npos++;
        }
    }
}

static char *get_module_name_from_filename(const char *);
static void check_module_name(const char *);
static void replace_dots(char *);
static void print_functions(Proto *, char *);
static void print_source_code(char *, char *);

int main(int argc, char **argv)
{
    // Process input arguments

    doargs(argc, argv);
    if (output_filename == NULL) {
        usage();
        exit(1);
    }
    if (type == 4){ // Auto
        char *filext = strrchr(output_filename, '.');
        if (filext && strcmp(filext, ".c") == 0){
            type = 2;
        }else if (filext && strcmp(filext, ".wasm") == 0){
            type = 0;
        }else{
            fatal_error("unknown file extension");
        }
    }
    if (!module_name) {
        module_name = get_module_name_from_filename(output_filename);
    }
    check_module_name(module_name);
    replace_dots(module_name);

    // Read the input

    lua_State *L = luaL_newstate();
    if (luaL_loadfile(L, input_filename) != LUA_OK) {
        fatal_error(lua_tostring(L,-1));
    }
    Proto *proto = getproto(s2v(L->top-1));
    tmname = G(L)->tmname;

    // Generate the file

    output_file = fopen(type != 2 ? "ob_temp.c" : output_filename, "w");
    if (output_file == NULL) { fatal_error(strerror(errno)); }

    if (goto_mode) {
        println("#include \"luaot_header.c\"");
    } else {
        println("#include \"trampoline_header.c\"");
    }
    printnl();
    print_functions(proto, "");
    for (int i = 0; i < 100; i++){
        if (strlen(secondaryFiles[i]) > 0){
            if (luaL_loadfile(L, secondaryFiles[i]) != LUA_OK) {
                fatal_error(lua_tostring(L,-1));
            }
            Proto *module = getproto(s2v(L->top-1));

            char str[12];
            sprintf(str, "%d", i);

            print_functions(module, str);
        }
    }
    printnl();
    print_source_code(input_filename, "");
    for (int i = 0; i < 100; i++){
        if (strlen(secondaryFiles[i]) > 0){
            char str[12];
            sprintf(str, "%d", i);

            print_source_code(secondaryFiles[i], str);
        }
    }
    printnl();
    println("#define LUAOT_MODULE_NAME \"%s\"", module_name);
    println("#define LUAOT_LUAOPEN_NAME luaopen_%s", module_name);
    printnl();
    if (goto_mode) {
        println("#include \"luaot_footer.c\"");
    } else {
        println("#include \"trampoline_footer.c\"");
    }
    if (alloc) {
        println("#include \"lalloc.h\"");
    }
    for (int i = 0; i < 100; i++){
        if (strlen(secondaryFiles[i]) > 0){
            char str[12];
            sprintf(str, "%d", i);

            println("int luaopen_submodule_%s(lua_State *L) {", str);
            println("    int ok = luaL_loadbuffer(L, LUAOT_MODULE_SOURCE_CODE_%s, sizeof(LUAOT_MODULE_SOURCE_CODE_%s)-1, \"Obsidian compiled module \\\"\"LUAOT_MODULE_NAME\"\\\"\");", str, str);
            println("    switch (ok) {" );
            println("      case LUA_OK:" );
            println("        /* No errors */" );
            println("        break;" );
            println("      case LUA_ERRSYNTAX:" );
            println("        fprintf(stderr, \"syntax error in bundled source code.\\n\");");
            println("        exit(1);" );
            println("        break;" );
            println("      case LUA_ERRMEM:" );
            println("        fprintf(stderr, \"memory allocation (out-of-memory) error in bundled source code.\\n\");" );
            println("        exit(1);" );
            println("        break;" );
            println("      default:" );
            println("        fprintf(stderr, \"unknown error. This should never happen\\n\");" );
            println("        exit(1);" );
            println("    }" );

            println("    LClosure *cl = (void *) lua_topointer(L, -1);" );
            println("    bind_magic(cl->p);" );

            println("    lua_call(L, 0, 1);" );
            println("    return 1;" );
            println("}" );
        }
    }
    //if (executable) {
      printnl();
      printnl();
      println("int main(int argc, char *argv[]) {");
      println(" lua_State *L = luaL_newstate();");
      println(" luaL_openlibs(L);");
      if (alloc){
        println(" init_pool_alloc();" );
      }
      /*for (int i = 0; i < 100; i++){ TODO: Add libs
        if (strlen(secondaryFiles[i]) > 0){
            char str[12];
            sprintf(str, "%d", i);
            
            char* modName = strtok(secondaryFiles[i], ".");
            println(" luaL_requiref(L, \"%s\", luaopen_submodule_%s, 1);", modName, str);
        }
      }*/
      println(" int i;");
      println(" lua_createtable(L, argc + 1, 0);");
      println(" for (i = 0; i < argc; i++) {");
      println("   lua_pushstring(L, argv[i]);");
      println("   lua_rawseti(L, -2, i);");
      println(" }");
      println(" lua_setglobal(L, \"arg\");");
      println(" lua_pushcfunction(L, LUAOT_LUAOPEN_NAME);");
      println("i = lua_pcall(L, 0, 0, 0);");
      println(" if (i != LUA_OK) {");
      println("   fprintf(stderr, \"%%s\\n\", lua_tostring(L, -1));");
      println("   return 1;");
      println(" }");
      println("lua_close(L);");
      println(" return 0;");
      println("}");
    //}

    fclose(output_file);

    // Compile the generated C code
    if (type == 0){
        if (system("emcc -v > /dev/null 2>&1") != 0) {
            fatal_error("emcc not found");
        }
    }
    

    char command[1024];
    char style[200] = "";
    if (type == 0){ // WASM
        if (opt){
            strcat(style, " -O3");
        }else if (shrink){
            strcat(style, " -Oz");
        }
        if (closureopt){
            strcat(style, " --closure 1 -sMODULARIZE");
        }
        if (debug) {
            strcat(style, " -g");
        }

        if (output_filename)
            printf("warning: ignoring -o option when compiling to WebAssembly\n");
        strcat(style, " -s WASM=1");// -sEXPORTED_RUNTIME_METHODS=_main");
    
        sprintf(command, "emcc -I/usr/local/include -L/usr/local/lib -lm -lwasmlua -s SUPPORT_LONGJMP=1 %s ob_temp.c", style);

        if (debug) printf("Compiling with command: %s\n", command);
        system(command);
        if (!debug){
            remove("ob_temp.c");
        }
    }
    return 0;
}

// Deduce the Lua module name given the file name
// Example:  ./foo/bar/baz.c -> foo.bar.baz
static
char *get_module_name_from_filename(const char *filename)
{
    size_t n = strlen(filename);

    int has_extension = 0;
    size_t sep = 0;
    for (size_t i = 0; i < n; i++) {
        if (filename[i] == '.') {
            has_extension = 1;
            sep = i;
        }
    }

    char *module_name = malloc(sep+1);
    for (size_t i = 0; i < sep; i++) {
        int c = filename[i];
        if (c == '/') {
            module_name[i] = '.';
        } else {
            module_name[i] = c;
        }
    }
    module_name[sep] = '\0';

    return module_name;
}

// Check if a module name contains only allowed characters
static
void check_module_name(const char *module_name)
{
    for (size_t i = 0; module_name[i] != '\0'; i++) {
        int c = module_name[i];
        if (!isalnum(c) && c != '_' && c != '.') {
            fatal_error("output module name must contain only letters, numbers, or '.'");
        }
    }
}

// Convert a module name to the internal "luaopen" name
static
void replace_dots(char *module_name)
{
    for (size_t i = 0; module_name[i] != '\0'; i++) {
        if (module_name[i] == '.') {
            module_name[i] = '_';
        }
    }
}


//
// Printing bytecode information
// -----------------------------
//
// These functions are copied from luac.c (and reindented)
//

#define UPVALNAME(x) ((f->upvalues[x].name) ? getstr(f->upvalues[x].name) : "-")
#define VOID(p) ((const void*)(p))
#define eventname(i) (getstr(tmname[i]))

static
void PrintString(const TString* ts)
{
    const char* s = getstr(ts);
    size_t i,n = tsslen(ts);
    print("\"");
    for (i=0; i<n; i++) {
        int c=(int)(unsigned char)s[i];
        switch (c) {
            case '"':
                print("\\\"");
                break;
            case '\\':
                print("\\\\");
                break;
            case '\a':
                print("\\a");
                break;
            case '\b':
                print("\\b");
                break;
            case '\f':
                print("\\f");
                break;
            case '\n':
                print("\\n");
                break;
            case '\r':
                print("\\r");
                break;
            case '\t':
                print("\\t");
                break;
            case '\v':
                print("\\v");
                break;
            default:
                if (isprint(c)) {
                    print("%c",c);
                } else {
                    print("\\%03d",c);
                }
                break;
        }
    }
    print("\"");
}

#if 0
static
void PrintType(const Proto* f, int i)
{
    const TValue* o=&f->k[i];
    switch (ttypetag(o)) {
        case LUA_VNIL:
            printf("N");
            break;
        case LUA_VFALSE:
        case LUA_VTRUE:
            printf("B");
            break;
        case LUA_VNUMFLT:
            printf("F");
            break;
        case LUA_VNUMINT:
            printf("I");
            break;
        case LUA_VSHRSTR:
        case LUA_VLNGSTR:
            printf("S");
            break;
        default: /* cannot happen */
            printf("?%d",ttypetag(o));
            break;
    }
    printf("\t");
}
#endif

static
void PrintConstant(const Proto* f, int i)
{
    const TValue* o=&f->k[i];
    switch (ttypetag(o)) {
        case LUA_VNIL:
            print("nil");
            break;
        case LUA_VFALSE:
            print("false");
            break;
        case LUA_VTRUE:
            print("true");
            break;
        case LUA_VNUMFLT:
            {
                char buff[100];
                sprintf(buff,LUA_NUMBER_FMT,fltvalue(o));
                print("%s",buff);
                if (buff[strspn(buff,"-0123456789")]=='\0') print(".0");
                break;
            }
        case LUA_VNUMINT:
            print(LUA_INTEGER_FMT, ivalue(o));
            break;
        case LUA_VSHRSTR:
        case LUA_VLNGSTR:
            PrintString(tsvalue(o));
            break;
        default: /* cannot happen */
            print("?%d",ttypetag(o));
            break;
    }
}

#define COMMENT		"\t; "
#define EXTRAARG	GETARG_Ax(code[pc+1])
#define EXTRAARGC	(EXTRAARG*(MAXARG_C+1))
#define ISK		(isk ? "k" : "")

static
void luaot_PrintOpcodeComment(Proto *f, int pc)
{
    // Adapted from the PrintCode function of luac.c
    const Instruction *code = f->code;
    const Instruction i = code[pc];
    OpCode o = GET_OPCODE(i);
    int a=GETARG_A(i);
    int b=GETARG_B(i);
    int c=GETARG_C(i);
    int ax=GETARG_Ax(i);
    int bx=GETARG_Bx(i);
    int sb=GETARG_sB(i);
    int sc=GETARG_sC(i);
    int sbx=GETARG_sBx(i);
    int isk=GETARG_k(i);
    int line=luaG_getfuncline(f,pc);

    print("  //");
    print(" %d\t", pc);
    if (line > 0) {
        print("[%d]\t", line);
    } else {
        print("[-]\t");
    }
    print("%-9s\t", opnames[o]);
    switch (o) {
        case OP_MOVE:
            print("%d %d",a,b);
            break;
        case OP_LOADI:
            print("%d %d",a,sbx);
            break;
        case OP_LOADF:
            print("%d %d",a,sbx);
            break;
        case OP_LOADK:
            print("%d %d",a,bx);
            print(COMMENT); PrintConstant(f,bx);
            break;
        case OP_LOADKX:
            print("%d",a);
            print(COMMENT); PrintConstant(f,EXTRAARG);
            break;
        case OP_LOADFALSE:
            print("%d",a);
            break;
        case OP_LFALSESKIP:
            print("%d",a);
            break;
        case OP_LOADTRUE:
            print("%d",a);
            break;
        case OP_LOADNIL:
            print("%d %d",a,b);
            print(COMMENT "%d out",b+1);
            break;
        case OP_GETUPVAL:
            print("%d %d",a,b);
            print(COMMENT "%s", UPVALNAME(b));
            break;
        case OP_SETUPVAL:
            print("%d %d",a,b);
            print(COMMENT "%s", UPVALNAME(b));
            break;
        case OP_GETTABUP:
            print("%d %d %d",a,b,c);
            print(COMMENT "%s", UPVALNAME(b));
            print(" "); PrintConstant(f,c);
            break;
        case OP_GETTABLE:
            print("%d %d %d",a,b,c);
            break;
        case OP_GETI:
            print("%d %d %d",a,b,c);
            break;
        case OP_GETFIELD:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_SETTABUP:
            print("%d %d %d%s",a,b,c,ISK);
            print(COMMENT "%s",UPVALNAME(a));
            print(" "); PrintConstant(f,b);
            if (isk) { print(" "); PrintConstant(f,c); }
            break;
        case OP_SETTABLE:
            print("%d %d %d%s",a,b,c,ISK);
            if (isk) { print(COMMENT); PrintConstant(f,c); }
            break;
        case OP_SETI:
            print("%d %d %d%s",a,b,c,ISK);
            if (isk) { print(COMMENT); PrintConstant(f,c); }
            break;
        case OP_SETFIELD:
            print("%d %d %d%s",a,b,c,ISK);
            print(COMMENT); PrintConstant(f,b);
            if (isk) { print(" "); PrintConstant(f,c); }
            break;
        case OP_NEWTABLE:
            print("%d %d %d",a,b,c);
            print(COMMENT "%d",c+EXTRAARGC);
            break;
        case OP_SELF:
            print("%d %d %d%s",a,b,c,ISK);
            if (isk) { print(COMMENT); PrintConstant(f,c); }
            break;
        case OP_ADDI:
            print("%d %d %d",a,b,sc);
            break;
        case OP_ADDK:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_SUBK:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_MULK:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_MODK:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_POWK:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_DIVK:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_IDIVK:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_BANDK:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_BORK:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_BXORK:
            print("%d %d %d",a,b,c);
            print(COMMENT); PrintConstant(f,c);
            break;
        case OP_SHRI:
            print("%d %d %d",a,b,sc);
            break;
        case OP_SHLI:
            print("%d %d %d",a,b,sc);
            break;
        case OP_ADD:
            print("%d %d %d",a,b,c);
            break;
        case OP_SUB:
            print("%d %d %d",a,b,c);
            break;
        case OP_MUL:
            print("%d %d %d",a,b,c);
            break;
        case OP_MOD:
            print("%d %d %d",a,b,c);
            break;
        case OP_POW:
            print("%d %d %d",a,b,c);
            break;
        case OP_DIV:
            print("%d %d %d",a,b,c);
            break;
        case OP_IDIV:
            print("%d %d %d",a,b,c);
            break;
        case OP_BAND:
            print("%d %d %d",a,b,c);
            break;
        case OP_BOR:
            print("%d %d %d",a,b,c);
            break;
        case OP_BXOR:
            print("%d %d %d",a,b,c);
            break;
        case OP_SHL:
            print("%d %d %d",a,b,c);
            break;
        case OP_SHR:
            print("%d %d %d",a,b,c);
            break;
        case OP_MMBIN:
            print("%d %d %d",a,b,c);
            print(COMMENT "%s",eventname(c));
            break;
        case OP_MMBINI:
            print("%d %d %d %d",a,sb,c,isk);
            print(COMMENT "%s",eventname(c));
            if (isk) print(" flip");
            break;
        case OP_MMBINK:
            print("%d %d %d %d",a,b,c,isk);
            print(COMMENT "%s ",eventname(c)); PrintConstant(f,b);
            if (isk) print(" flip");
            break;
        case OP_UNM:
            print("%d %d",a,b);
            break;
        case OP_BNOT:
            print("%d %d",a,b);
            break;
        case OP_NOT:
            print("%d %d",a,b);
            break;
        case OP_LEN:
            print("%d %d",a,b);
            break;
        case OP_CONCAT:
            print("%d %d",a,b);
            break;
        case OP_CLOSE:
            print("%d",a);
            break;
        case OP_TBC:
            print("%d",a);
            break;
        case OP_JMP:
            print("%d",GETARG_sJ(i));
            print(COMMENT "to %d",GETARG_sJ(i)+pc+2);
            break;
        case OP_EQ:
            print("%d %d %d",a,b,isk);
            break;
        case OP_LT:
            print("%d %d %d",a,b,isk);
            break;
        case OP_LE:
            print("%d %d %d",a,b,isk);
            break;
        case OP_EQK:
            print("%d %d %d",a,b,isk);
            print(COMMENT); PrintConstant(f,b);
            break;
        case OP_EQI:
            print("%d %d %d",a,sb,isk);
            break;
        case OP_LTI:
            print("%d %d %d",a,sb,isk);
            break;
        case OP_LEI:
            print("%d %d %d",a,sb,isk);
            break;
        case OP_GTI:
            print("%d %d %d",a,sb,isk);
            break;
        case OP_GEI:
            print("%d %d %d",a,sb,isk);
            break;
        case OP_TEST:
            print("%d %d",a,isk);
            break;
        case OP_TESTSET:
            print("%d %d %d",a,b,isk);
            break;
        case OP_CALL:
            print("%d %d %d",a,b,c);
            print(COMMENT);
            if (b==0) print("all in "); else print("%d in ",b-1);
            if (c==0) print("all out"); else print("%d out",c-1);
            break;
        case OP_TAILCALL:
            print("%d %d %d",a,b,c);
            print(COMMENT "%d in",b-1);
            break;
        case OP_RETURN:
            print("%d %d %d",a,b,c);
            print(COMMENT);
            if (b==0) print("all out"); else print("%d out",b-1);
            break;
        case OP_RETURN0:
            break;
        case OP_RETURN1:
            print("%d",a);
            break;
        case OP_FORLOOP:
            print("%d %d",a,bx);
            print(COMMENT "to %d",pc-bx+2);
            break;
        case OP_FORPREP:
            print("%d %d",a,bx);
            print(COMMENT "to %d",pc+bx+2);
            break;
        case OP_TFORPREP:
            print("%d %d",a,bx);
            print(COMMENT "to %d",pc+bx+2);
            break;
        case OP_TFORCALL:
            print("%d %d",a,c);
            break;
        case OP_TFORLOOP:
            print("%d %d",a,bx);
            print(COMMENT "to %d",pc-bx+2);
            break;
        case OP_SETLIST:
            print("%d %d %d",a,b,c);
            break;
        case OP_CLOSURE:
            print("%d %d",a,bx);
            print(COMMENT "%p",VOID(f->p[bx]));
            break;
        case OP_VARARG:
            print("%d %d",a,c);
            print(COMMENT);
            if (c==0) print("all out"); else print("%d out",c-1);
            break;
        case OP_VARARGPREP:
            print("%d",a);
            break;
        case OP_EXTRAARG:
            print("%d",ax);
            break;
#if 0
        default:
            print("%d %d %d",a,b,c);
            print(COMMENT "not handled");
            break;
#endif
    }
    print("\n");
}
static void gcreate_function(Proto *f);
static void screate_function(Proto *f);

#include "luaot_gotos.c"
#include "luaot_switches.c"
   
static void create_function(Proto *f)
{
    if (goto_mode) {
        gcreate_function(f);
    } else {
        screate_function(f);
    }
}

static
void create_functions(Proto *p)
{
    // luaot_footer.c should use the same traversal order as this.
    create_function(p);
    for (int i = 0; i < p->sizep; i++) {
        create_functions(p->p[i]);
    }
}

static
void print_functions(Proto *p, char* prefix)
{
    create_functions(p);

    if (prefix == "") {
        println("static AotCompiledFunction LUAOT_FUNCTIONS[] = {");
    } else {
        println("static AotCompiledFunction LUAOT_FUNCTIONS_%s[] = {", prefix);
    }
    for (int i = 0; i < nfunctions; i++) {
        println("  magic_implementation_%02d,", i);
    }
    println("  NULL");
    println("};");
}

static
void print_source_code(char* filename, char* prefix)
{
    // Since the code we are generating is lifted from lvm.c, we need it to use
    // Lua functions instead of C functions. And to create the Lua functions,
    // we have to `load` them from source code.
    //
    // The most readable approach would be to bundle this Lua source code as a
    // big C string literal. However, C compilers have limits on how big a
    // string literal can be, so instead of using a string literal, we use a
    // plain char array instead.

    FILE *infile = fopen(filename, "r");
    if (!infile) { fatal_error("could not open input file a second time"); }

    if (prefix == "") {
        println("static const char LUAOT_MODULE_SOURCE_CODE[] = {");
    } else {
        println("static const char LUAOT_MODULE_SOURCE_CODE_%s[] = {", prefix);
    }

    int c;
    int col = 0;
    do {
        if (col == 0) {
            print(" ");
        }

        c = fgetc(infile);
        if (c == EOF) {
            print(" %3d", 0);
        } else {
            print(" %3d", c);
            print(",");
        }

        col++;
        if (col == 16 || c == EOF) {
            print("\n");
            col = 0;
        }
    } while (c != EOF);
    println("};");

    fclose(infile);
}