# Makefile for installing Lua
# See doc/readme.html for installation and customization instructions.

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= guess

# Where to install. The installation starts in the src and doc directories,
# so take care if INSTALL_TOP is not an absolute path. See the local target.
# You may want to make INSTALL_LMOD and INSTALL_CMOD consistent with
# LUA_ROOT, LUA_LDIR, and LUA_CDIR in luaconf.h.
INSTALL_TOP= /usr/local
INSTALL_BIN= $(INSTALL_TOP)/bin
INSTALL_INC= $(INSTALL_TOP)/include
INSTALL_LIB= $(INSTALL_TOP)/lib
INSTALL_LMOD= $(INSTALL_TOP)/share/lua/$V
INSTALL_CMOD= $(INSTALL_TOP)/lib/lua/$V

# How to install. If your install program does not support "-p", then
# you may have to run ranlib on the installed libwasmlua.a or liblua.a.
INSTALL= install -p
INSTALL_EXEC= $(INSTALL) -m 0755
INSTALL_DATA= $(INSTALL) -m 0644
#
# If you don't have "install" you can use "cp" instead.
# INSTALL= cp -p
# INSTALL_EXEC= $(INSTALL)
# INSTALL_DATA= $(INSTALL)

# Other utilities.
MKDIR= mkdir -p
RM= rm -f

# == END OF USER SETTINGS -- NO NEED TO CHANGE ANYTHING BELOW THIS LINE =======

# Convenience platforms targets.
PLATS= guess aix bsd c89 freebsd generic linux linux-readline macosx mingw posix solaris

# What to install.
TO_BIN= ob
TO_INC= lauxlib.c luaot_switches.c liolib.c lopcodes.c lstate.c lobject.c luaot_header.c luaot_footer.c lmathlib.c ljs.h ljs.c loadlib.c lvm.c lfunc.c lstrlib.c lua.c linit.c lstring.c trampoline_header.c lundump.c lctype.c luac.c ltable.c trampoline_footer.c ldump.c luaot_gotos.c loslib.c lgc.c lzio.c ldblib.c lutf8lib.c lmem.c lcorolib.c lcode.c ltablib.c luaot.c lapi.c lbaselib.c ldebug.c lparser.c llex.c ltm.c ldo.c lmem.h llimits.h luaconf.h lzio.h lgc.h lualib.h lopnames.h ldebug.h lcode.h lapi.h ldo.h ljumptab.h llex.h ltm.h lparser.h lobject.h lstate.h lopcodes.h lauxlib.h lvm.h lstring.h lua.h lfunc.h lprefix.h ltable.h lctype.h lundump.h lalloc.h
TO_LIB= libwasmlua.a liblua.a

# Lua version and release.
V= 5.4
R= $V.4

# Targets start here.
all:	$(PLAT)

$(PLATS) help test clean:
	@cd src && $(MAKE) $@

install: dummy
	cd src && $(MKDIR) $(INSTALL_BIN) $(INSTALL_INC) $(INSTALL_LIB) $(INSTALL_LMOD) $(INSTALL_CMOD)
	cd src && $(INSTALL_EXEC) $(TO_BIN) $(INSTALL_BIN)
	cd src && $(INSTALL_DATA) $(TO_INC) $(INSTALL_INC)
	cd src && $(INSTALL_DATA) $(TO_LIB) $(INSTALL_LIB)

uninstall:
	cd src && cd $(INSTALL_BIN) && $(RM) $(TO_BIN)
	cd src && cd $(INSTALL_INC) && $(RM) $(TO_INC)
	cd src && cd $(INSTALL_LIB) && $(RM) $(TO_LIB)

local:
	$(MAKE) install INSTALL_TOP=../install

# make may get confused with install/ if it does not support .PHONY.
dummy:

# Echo config parameters.
echo:
	@cd src && $(MAKE) -s echo
	@echo "PLAT= $(PLAT)"
	@echo "V= $V"
	@echo "R= $R"
	@echo "TO_BIN= $(TO_BIN)"
	@echo "TO_INC= $(TO_INC)"
	@echo "TO_LIB= $(TO_LIB)"
	@echo "INSTALL_TOP= $(INSTALL_TOP)"
	@echo "INSTALL_BIN= $(INSTALL_BIN)"
	@echo "INSTALL_INC= $(INSTALL_INC)"
	@echo "INSTALL_LIB= $(INSTALL_LIB)"
	@echo "INSTALL_LMOD= $(INSTALL_LMOD)"
	@echo "INSTALL_CMOD= $(INSTALL_CMOD)"
	@echo "INSTALL_EXEC= $(INSTALL_EXEC)"
	@echo "INSTALL_DATA= $(INSTALL_DATA)"

# Echo pkg-config data.
pc:
	@echo "version=$R"
	@echo "prefix=$(INSTALL_TOP)"
	@echo "libdir=$(INSTALL_LIB)"
	@echo "includedir=$(INSTALL_INC)"

# Targets that do not create files (not all makes understand .PHONY).
.PHONY: all $(PLATS) help test clean install uninstall local dummy echo pc

# (end of Makefile)
