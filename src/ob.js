/**
@license MIT

Copyright © 2019-2024 Unexex
Copyright © 2017-2019 Benoit Giannangeli
Copyright © 2017-2019 Daurnimator
Copyright © 1994–2017 Lua.org, PUC-Rio.
*/

"use strict";

const core = require('./obcore');

module.exports.FENGARI_AUTHORS         = core.FENGARI_AUTHORS;
module.exports.FENGARI_COPYRIGHT       = core.FENGARI_COPYRIGHT;
module.exports.FENGARI_RELEASE         = core.FENGARI_RELEASE;
module.exports.FENGARI_VERSION         = core.FENGARI_VERSION;
module.exports.FENGARI_VERSION_MAJOR   = core.FENGARI_VERSION_MAJOR;
module.exports.FENGARI_VERSION_MINOR   = core.FENGARI_VERSION_MINOR;
module.exports.FENGARI_VERSION_NUM     = core.FENGARI_VERSION_NUM;
module.exports.FENGARI_VERSION_RELEASE = core.FENGARI_VERSION_RELEASE;

module.exports.luastring_eq      = core.luastring_eq;
module.exports.luastring_indexOf = core.luastring_indexOf;
module.exports.luastring_of      = core.luastring_of;
module.exports.to_jsstring       = core.to_jsstring;
module.exports.to_luastring      = core.to_luastring;
module.exports.to_uristring      = core.to_uristring;

const luaconf = require('./luaconf');
const lua     = require('./lua');
const lauxlib = require('./lauxlib');
const lualib  = require('./lualib');

module.exports.luaconf = luaconf;
module.exports.lua     = lua;
module.exports.lauxlib = lauxlib;
module.exports.lualib  = lualib;
