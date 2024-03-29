"use strict";

const {
    LUA_VERSION_MAJOR,
    LUA_VERSION_MINOR
} = require('./lua');

const LUA_VERSUFFIX = "_" + LUA_VERSION_MAJOR + "_" + LUA_VERSION_MINOR;
module.exports.LUA_VERSUFFIX = LUA_VERSUFFIX;

module.exports.lua_assert = function(c) {};

module.exports.luaopen_base = require('./lbaselib').luaopen_base;

const LUA_COLIBNAME = "coroutine";
module.exports.LUA_COLIBNAME = LUA_COLIBNAME;
module.exports.luaopen_coroutine = require('./lcorolib').luaopen_coroutine;

const LUA_TABLIBNAME = "table";
module.exports.LUA_TABLIBNAME = LUA_TABLIBNAME;
module.exports.luaopen_table = require('./ltablib').luaopen_table;

if (typeof process !== "undefined") {
    const LUA_IOLIBNAME = "io";
    module.exports.LUA_IOLIBNAME = LUA_IOLIBNAME;
    module.exports.luaopen_io = require('./liolib').luaopen_io;
}

const LUA_OSLIBNAME = "os";
module.exports.LUA_OSLIBNAME = LUA_OSLIBNAME;
module.exports.luaopen_os = require('./loslib').luaopen_os;

const LUA_STRLIBNAME = "string";
module.exports.LUA_STRLIBNAME = LUA_STRLIBNAME;
module.exports.luaopen_string = require('./lstrlib').luaopen_string;

const LUA_UTF8LIBNAME = "utf8";
module.exports.LUA_UTF8LIBNAME = LUA_UTF8LIBNAME;
module.exports.luaopen_utf8 = require('./lutf8lib').luaopen_utf8;

const LUA_BITLIBNAME = "bit32";
module.exports.LUA_BITLIBNAME = LUA_BITLIBNAME;
// module.exports.luaopen_bit32 = require('./lbitlib').luaopen_bit32;

const LUA_MATHLIBNAME = "math";
module.exports.LUA_MATHLIBNAME = LUA_MATHLIBNAME;
module.exports.luaopen_math = require('./lmathlib').luaopen_math;

const LUA_JSNAME = "js";
module.exports.LUA_JSNAME = LUA_JSNAME;
module.exports.luaopen_js = require('./js').luaopen_js;

const LUA_DBLIBNAME = "debug";
module.exports.LUA_DBLIBNAME = LUA_DBLIBNAME;
module.exports.luaopen_debug = require('./ldblib').luaopen_debug;

const LUA_LOADLIBNAME = "package";
module.exports.LUA_LOADLIBNAME = LUA_LOADLIBNAME;
module.exports.luaopen_package = require('./loadlib').luaopen_package;

const LUA_FENGARILIBNAME = "ob";
module.exports.LUA_FENGARILIBNAME = LUA_FENGARILIBNAME;
module.exports.luaopen_fengari = require('./oblib').luaopen_fengari;

const linit = require('./linit');
module.exports.luaL_openlibs = linit.luaL_openlibs;
