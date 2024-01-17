# Makefile for building Lua
# See ../doc/readme.html for installation and customization instructions.

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= guess

WASMCC= emcc -std=gnu99 -fPIC
WASMAR = emar rcs
WASMRANLIB = emranlib

CC= gcc -std=gnu99 -O3 -fPIC
CFLAGS= -O2 -Wall -Wextra $(SYSCFLAGS) $(MYCFLAGS)
LDFLAGS= $(SYSLDFLAGS) $(MYLDFLAGS)
LIBS= -lm $(SYSLIBS) $(MYLIBS)

AR= ar rcu
RANLIB= ranlib
RM= rm -f
UNAME= uname

SYSCFLAGS=
SYSLDFLAGS=
SYSLIBS=

MYCFLAGS=
MYLDFLAGS=
MYLIBS=
MYOBJS=
MYWASM=

# Special flags for compiler modules; -Os reduces code size.
CMCFLAGS= 

# == END OF USER SETTINGS -- NO NEED TO CHANGE ANYTHING BELOW THIS LINE =======

PLATS= guess aix bsd c89 freebsd generic linux linux-readline macosx mingw posix solaris

LUA_A=	liblua.a
WASMLUA_A=	libwasmlua.a
CORE_O=	lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o lundump.o lvm.o lzio.o
LIB_O=	lauxlib.o lbaselib.o lcorolib.o ldblib.o liolib.o lmathlib.o loadlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o linit.o lalloc.o ljs.o
BASE_O= $(CORE_O) $(LIB_O) $(MYOBJS)
CORE_WASM=	lapi.wasm lcode.wasm lctype.wasm ldebug.wasm ldo.wasm ldump.wasm lfunc.wasm lgc.wasm llex.wasm lmem.wasm lobject.wasm lopcodes.wasm lparser.wasm lstate.wasm lstring.wasm ltable.wasm ltm.wasm lundump.wasm lvm.wasm lzio.wasm
LIB_WASM=	lauxlib.wasm lbaselib.wasm lcorolib.wasm ldblib.wasm liolib.wasm lmathlib.wasm loadlib.wasm loslib.wasm lstrlib.wasm ltablib.wasm lutf8lib.wasm linit.wasm lalloc.wasm ljs.wasm
BASE_WASM= $(CORE_WASM) $(LIB_WASM) $(MYWASM)

AOT_T=	ob
AOT_O=	luaot.o

AOT2_T=	luaot-trampoline
AOT2_O=	luaot-trampoline.o

ALL_O= $(BASE_O) $(AOT_O) $(AOT2_O)
ALL_T= $(LUA_A) $(AOT_T) $(AOT2_T) $(WASMLUA_A)
ALL_A= $(LUA_A) $(WASMLUA_A)
ALL_WASM= $(BASE_WASM)

# Targets start here.
default: $(PLAT)

all:	$(ALL_T)

o:	$(ALL_O)
wasm: $(ALL_WASM)
a:	$(ALL_A)

$(LUA_A): $(BASE_O)
	$(AR) $@ $(BASE_O)
	$(RANLIB) $@

$(WASMLUA_A): $(BASE_WASM)
	$(WASMAR) $@ $(BASE_WASM)
	$(WASMRANLIB) $@

$(AOT_T): $(AOT_O) $(LUA_A)
	$(CC) -o $@  $(LDFLAGS) $(AOT_O) $(LUA_A) $(LIBS)

$(AOT2_T): $(AOT2_O) $(LUA_A)
	$(CC) -o $@ $(LDFLAGS) $(AOT2_O) $(LUA_A) $(LIBS)
test:
	./$(LUA_T) -v

clean:
	$(RM) $(ALL_O) $(ALL_WASM) $(ALL_A) $(ALL_T)

depend:
	@$(CC) $(CFLAGS) -MM l*.c

echo:
	@echo "PLAT= $(PLAT)"
	@echo "CC= $(CC)"
	@echo "WASMAR= $(WASMAR)"
	@echo "WASMRANLIB= $(WASMRANLIB)"
	@echo "WASMCC= $(WASMCC)"
	@echo "ALLWASM= $(ALL_WASM)"
	@echo "ALLA= $(ALL_A)"
	@echo "ALLT= $(ALL_T)"
	@echo "ALL= $(ALL)"
	@echo "ALL_O= $(ALL_O)"
	@echo "CFLAGS= $(CFLAGS)"
	@echo "LDFLAGS= $(SYSLDFLAGS)"
	@echo "LIBS= $(LIBS)"
	@echo "AR= $(AR)"
	@echo "RANLIB= $(RANLIB)"
	@echo "RM= $(RM)"
	@echo "UNAME= $(UNAME)"

# Convenience targets for popular platforms.
ALL= all

help:
	@echo "Do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"
	@echo "See doc/readme.html for complete instructions."

guess:
	@echo Guessing `$(UNAME)`
	@$(MAKE) `$(UNAME)`

AIX aix:
	$(MAKE) $(ALL) CC="xlc" CFLAGS="-O2 -DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-ldl" SYSLDFLAGS="-brtl -bexpall"

bsd:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-Wl,-E"

c89:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_C89" CC="gcc -std=c89"
	@echo ''
	@echo '*** C89 does not guarantee 64-bit integers for Lua.'
	@echo '*** Make sure to compile all external Lua libraries'
	@echo '*** with LUA_USE_C89 to ensure consistency'
	@echo ''

FreeBSD NetBSD OpenBSD freebsd:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX -I/usr/include/edit" SYSLIBS="-Wl,-E -ledit" CC="cc"

generic: $(ALL)

Linux linux:	linux-noreadline

linux-noreadline:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX" SYSLIBS="-Wl,-E -ldl"

linux-readline:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX" SYSLIBS="-Wl,-E -ldl"

Darwin macos macosx:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_MACOSX"

mingw:
	@echo "WINDOWS & MingW are not supported yet"
	$(MAKE) "WASMLUA_A=wasmlua54.dll" "LUA_A=lua54.dll" "LUA_T=lua.exe" \
	"AR=$(CC) -shared -o" "RANLIB=strip --strip-unneeded" \
	"SYSCFLAGS=-DLUA_BUILD_AS_DLL" "SYSLIBS=" "SYSLDFLAGS=-s" lua.exe
	$(MAKE) "LUAC_T=luac.exe" luac.exe

posix:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX"

SunOS solaris:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN -D_REENTRANT" SYSLIBS="-ldl"

# Targets that do not create files (not all makes understand .PHONY).
.PHONY: all $(PLATS) help test clean default o a depend echo

# Compiler modules may use special flags.
llex.o:
	$(CC) $(CFLAGS) $(CMCFLAGS) -c llex.c

lparser.o:
	$(CC) $(CFLAGS) $(CMCFLAGS) -c lparser.c

lcode.o:
	$(CC) $(CFLAGS) $(CMCFLAGS) -c lcode.c

llex.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c llex.c -o llex.wasm

lparser.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lparser.c -o lparser.wasm

lcode.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lcode.c -o lcode.wasm

# LuaAOT has extra if-defs

luaot.o: luaot.c luaot_gotos.c \
 lua.h lauxlib.h ldebug.h lobject.h lopcodes.h lopnames.h lstate.h lundump.h
	$(CC) $(CFLAGS) -c $< -o $@ -DLUAOT_USE_GOTOS

luaot-trampoline.o: luaot.c luaot_switches.c \
 lua.h lauxlib.h ldebug.h lobject.h lopcodes.h lopnames.h lstate.h lundump.h
	$(CC) $(CFLAGS) -c $< -o $@ -DLUAOT_USE_SWITCHES

# DO NOT DELETE

lapi.o: lapi.c lprefix.h lua.h luaconf.h lapi.h llimits.h lstate.h \
 lobject.h ltm.h lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h lstring.h \
 ltable.h lundump.h lvm.h
lauxlib.o: lauxlib.c lprefix.h lua.h luaconf.h lauxlib.h
lbaselib.o: lbaselib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
lcode.o: lcode.c lprefix.h lua.h luaconf.h lcode.h llex.h lobject.h \
 llimits.h lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h \
 ldo.h lgc.h lstring.h ltable.h lvm.h
lcorolib.o: lcorolib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
lctype.o: lctype.c lprefix.h lctype.h lua.h luaconf.h llimits.h
ldblib.o: ldblib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
ldebug.o: ldebug.c lprefix.h lua.h luaconf.h lapi.h llimits.h lstate.h \
 lobject.h ltm.h lzio.h lmem.h lcode.h llex.h lopcodes.h lparser.h \
 ldebug.h ldo.h lfunc.h lstring.h lgc.h ltable.h lvm.h
ldo.o: ldo.c lprefix.h lua.h luaconf.h lapi.h llimits.h lstate.h \
 lobject.h ltm.h lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h lopcodes.h \
 lparser.h lstring.h ltable.h lundump.h lvm.h
ldump.o: ldump.c lprefix.h lua.h luaconf.h lobject.h llimits.h lstate.h \
 ltm.h lzio.h lmem.h lundump.h
lfunc.o: lfunc.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h
lgc.o: lgc.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h lstring.h ltable.h
linit.o: linit.c lprefix.h lua.h luaconf.h lualib.h lauxlib.h
liolib.o: liolib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
llex.o: llex.c lprefix.h lua.h luaconf.h lctype.h llimits.h ldebug.h \
 lstate.h lobject.h ltm.h lzio.h lmem.h ldo.h lgc.h llex.h lparser.h \
 lstring.h ltable.h
lmathlib.o: lmathlib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
ljs.o: ljs.c lprefix.h lua.h luaconf.h ljs.h
lmem.o: lmem.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lgc.h
loadlib.o: loadlib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
lobject.o: lobject.c lprefix.h lua.h luaconf.h lctype.h llimits.h \
 ldebug.h lstate.h lobject.h ltm.h lzio.h lmem.h ldo.h lstring.h lgc.h \
 lvm.h
lopcodes.o: lopcodes.c lprefix.h lopcodes.h llimits.h lua.h luaconf.h
loslib.o: loslib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
lparser.o: lparser.c lprefix.h lua.h luaconf.h lcode.h llex.h lobject.h \
 llimits.h lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h \
 ldo.h lfunc.h lstring.h lgc.h ltable.h
lstate.o: lstate.c lprefix.h lua.h luaconf.h lapi.h llimits.h lstate.h \
 lobject.h ltm.h lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h llex.h \
 lstring.h ltable.h
lstring.o: lstring.c lprefix.h lua.h luaconf.h ldebug.h lstate.h \
 lobject.h llimits.h ltm.h lzio.h lmem.h ldo.h lstring.h lgc.h
lstrlib.o: lstrlib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
ltable.o: ltable.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lgc.h lstring.h ltable.h lvm.h
ltablib.o: ltablib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
ltm.o: ltm.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lgc.h lstring.h ltable.h lvm.h
lua.o: lua.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
luac.o: luac.c lprefix.h lua.h luaconf.h lauxlib.h ldebug.h lstate.h \
 lobject.h llimits.h ltm.h lzio.h lmem.h lopcodes.h lopnames.h lundump.h
lundump.o: lundump.c lprefix.h lua.h luaconf.h ldebug.h lstate.h \
 lobject.h llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lstring.h lgc.h \
 lundump.h
lutf8lib.o: lutf8lib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
lvm.o: lvm.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h lopcodes.h lstring.h \
 ltable.h lvm.h ljumptab.h
lzio.o: lzio.c lprefix.h lua.h luaconf.h llimits.h lmem.h lstate.h \
 lobject.h ltm.h lzio.h
lalloc.o: lalloc.c lalloc.h lua.h lauxlib.h
lapi.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lapi.c -o lapi.wasm
lauxlib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lauxlib.c -o lauxlib.wasm
lbaselib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lbaselib.c -o lbaselib.wasm
lcode.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lcode.c -o lcode.wasm
lcorolib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lcorolib.c -o lcorolib.wasm
lctype.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lctype.c -o lctype.wasm
ldblib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c ldblib.c -o ldblib.wasm
ldebug.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c ldebug.c -o ldebug.wasm
ldo.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c ldo.c -o ldo.wasm
ldump.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c ldump.c -o ldump.wasm
lfunc.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lfunc.c -o lfunc.wasm
lgc.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lgc.c -o lgc.wasm
linit.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c linit.c -o linit.wasm
liolib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c liolib.c -o liolib.wasm
llex.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c llex.c -o llex.wasm
lmathlib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lmathlib.c -o lmathlib.wasm
ljs.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c ljs.c -o ljs.wasm
lmem.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lmem.c -o lmem.wasm
loadlib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c loadlib.c -o loadlib.wasm
lobject.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lobject.c -o lobject.wasm
lopcodes.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lopcodes.c -o lopcodes.wasm
loslib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c loslib.c -o loslib.wasm
lparser.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lparser.c -o lparser.wasm
lstate.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lstate.c -o lstate.wasm
lstring.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lstring.c -o lstring.wasm
lstrlib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lstrlib.c -o lstrlib.wasm
ltable.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c ltable.c -o ltable.wasm
ltablib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c ltablib.c -o ltablib.wasm
ltm.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c ltm.c -o ltm.wasm
lua.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lua.c -o lua.wasm
luac.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c luac.c -o luac.wasm
lundump.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lundump.c -o lundump.wasm
lutf8lib.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lutf8lib.c -o lutf8lib.wasm
lvm.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lvm.c -o lvm.wasm
lzio.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lzio.c -o lzio.wasm
lalloc.wasm:
	$(WASMCC) $(CFLAGS) $(CMCFLAGS) -c lalloc.c -o lalloc.wasm

# (end of Makefile)