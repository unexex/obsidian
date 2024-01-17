#include "luaot_header.c"

// source = @test.ob
// main function
static
CallInfo *magic_implementation_00(lua_State *L, struct CallInfo *ci)
{
  LClosure *cl;
  TValue *k;
  StkId base;
  const Instruction *pc;
  int trap;

  trap = L->hookmask;
  cl = clLvalue(s2v(ci->func));
  k = cl->p->k;
  pc = ci->u.l.savedpc;
  if (l_unlikely(trap)) {
    if (pc == cl->p->code) {  /* first instruction (not resuming)? */
      if (cl->p->is_vararg)
        trap = 0;  /* hooks will start after VARARGPREP instruction */
      else  /* check 'call' hook */
        luaD_hookcall(L, ci);
    }
    ci->u.l.trap = 1;  /* assume trap is on, for now */
  }
  base = ci->func + 1;
  /* main loop of interpreter */
  Instruction *code = cl->p->code;
  Instruction i;
  StkId ra;

  switch (pc - code) {
    case 0: goto label_00;
    case 1: goto label_01;
    case 2: goto label_02;
    case 3: goto label_03;
    case 4: goto label_04;
    case 5: goto label_05;
  }

  // 0	[1]	VARARGPREP	0
  #undef  LUAOT_PC
  #define LUAOT_PC (code + 1)
  #undef  LUAOT_NEXT_JUMP
  #undef  LUAOT_SKIP1
  #define LUAOT_SKIP1 label_02
  label_00: {
    aot_vmfetch(0x00000051);
    ProtectNT(luaT_adjustvarargs(L, GETARG_A(i), ci, cl->p));
    if (l_unlikely(trap)) {  /* previous "Protect" updated trap */
      luaD_hookcall(L, ci);
      L->oldpc = 1;  /* next opcode will be seen as a "new" line */
    }
    updatebase(ci);  /* function has new base after adjustment */
  }

  // 1	[1]	GETTABUP 	0 0 0	; _ENV "js"
  #undef  LUAOT_PC
  #define LUAOT_PC (code + 2)
  #undef  LUAOT_NEXT_JUMP
  #undef  LUAOT_SKIP1
  #define LUAOT_SKIP1 label_03
  label_01: {
    aot_vmfetch(0x0000000b);
    const TValue *slot;
    TValue *upval = cl->upvals[GETARG_B(i)]->v;
    TValue *rc = KC(i);
    TString *key = tsvalue(rc);  /* key must be a string */
    if (luaV_fastget(L, upval, key, slot, luaH_getshortstr)) {
      setobj2s(L, ra, slot);
    }
    else
      Protect(luaV_finishget(L, upval, rc, ra, slot));
  }

  // 2	[1]	GETFIELD 	0 0 1	; "eval"
  #undef  LUAOT_PC
  #define LUAOT_PC (code + 3)
  #undef  LUAOT_NEXT_JUMP
  #undef  LUAOT_SKIP1
  #define LUAOT_SKIP1 label_04
  label_02: {
    aot_vmfetch(0x0100000e);
    const TValue *slot;
    TValue *rb = vRB(i);
    TValue *rc = KC(i);
    TString *key = tsvalue(rc);  /* key must be a string */
    if (luaV_fastget(L, rb, key, slot, luaH_getshortstr)) {
      setobj2s(L, ra, slot);
    }
    else
      Protect(luaV_finishget(L, rb, rc, ra, slot));
  }

  // 3	[1]	LOADK    	1 2	; "console.log('test.ob')"
  #undef  LUAOT_PC
  #define LUAOT_PC (code + 4)
  #undef  LUAOT_NEXT_JUMP
  #undef  LUAOT_SKIP1
  #define LUAOT_SKIP1 label_05
  label_03: {
    aot_vmfetch(0x00010083);
    TValue *rb = k + GETARG_Bx(i);
    setobj2s(L, ra, rb);
  }

  // 4	[1]	CALL     	0 2 1	; 1 in 0 out
  #undef  LUAOT_PC
  #define LUAOT_PC (code + 5)
  #undef  LUAOT_NEXT_JUMP
  #undef  LUAOT_SKIP1
  label_04: {
    aot_vmfetch(0x01020044);
    CallInfo *newci;
    int b = GETARG_B(i);
    int nresults = GETARG_C(i) - 1;
    if (b != 0)  /* fixed number of arguments? */
        L->top = ra + b;  /* top signals number of arguments */
    /* else previous instruction set top */
    savepc(L);  /* in case of errors */
    if ((newci = luaD_precall(L, ra, nresults)) == NULL)
        updatetrap(ci);  /* C call; nothing else to be done */
    else {
        ci = newci;
        ci->callstatus = 0;  /* call re-uses 'luaV_execute' */
        return ci;
    }
  }

  // 5	[1]	RETURN   	0 1 1	; 0 out
  #undef  LUAOT_PC
  #define LUAOT_PC (code + 6)
  #undef  LUAOT_NEXT_JUMP
  #undef  LUAOT_SKIP1
  label_05: {
    aot_vmfetch(0x01010046);
    int n = GETARG_B(i) - 1;  /* number of results */
    int nparams1 = GETARG_C(i);
    if (n < 0)  /* not fixed? */
      n = cast_int(L->top - ra);  /* get what is available */
    savepc(ci);
    if (TESTARG_k(i)) {  /* may there be open upvalues? */
      if (L->top < ci->top)
        L->top = ci->top;
      luaF_close(L, base, CLOSEKTOP, 1);
      updatetrap(ci);
      updatestack(ci);
    }
    if (nparams1)  /* vararg function? */
      ci->func -= ci->u.l.nextraargs + nparams1;
    L->top = ra + n;  /* set call for 'luaD_poscall' */
    luaD_poscall(L, ci, n);
    updatetrap(ci);  /* 'luaD_poscall' can change hooks */
    if (ci->callstatus & CIST_FRESH)
        return NULL;  /* end this frame */
    else {
        ci = ci->previous;
        return ci;
    }
  }

}

static AotCompiledFunction LUAOT_FUNCTIONS[] = {
  magic_implementation_00,
  NULL
};

static const char LUAOT_MODULE_SOURCE_CODE[] = {
  106, 115,  46, 101, 118,  97, 108,  40,  34,  99, 111, 110, 115, 111, 108, 101,
   46, 108, 111, 103,  40,  39, 116, 101, 115, 116,  46, 111,  98,  39,  41,  34,
   41,   0
};

#define LUAOT_MODULE_NAME "test"
#define LUAOT_LUAOPEN_NAME luaopen_test

#include "luaot_footer.c"


int main(int argc, char *argv[]) {
 lua_State *L = luaL_newstate();
 luaL_openlibs(L);
 int i;
 lua_createtable(L, argc + 1, 0);
 for (i = 0; i < argc; i++) {
   lua_pushstring(L, argv[i]);
   lua_rawseti(L, -2, i);
 }
 lua_setglobal(L, "arg");
 lua_pushcfunction(L, LUAOT_LUAOPEN_NAME);
i = lua_pcall(L, 0, 0, 0);
 if (i != LUA_OK) {
   fprintf(stderr, "%s\n", lua_tostring(L, -1));
   return 1;
 }
lua_close(L);
 return 0;
}
