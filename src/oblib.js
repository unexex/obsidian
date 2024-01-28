const {
    lua_pushinteger,
    lua_pushliteral,
    lua_setfield,
    LUA_OK,
    lua_newtable
} = require('./lua');
const {
    luaL_newlib,
    luaL_checkstring,
    luaL_optstring,
    luaL_error
} = require('./lauxlib');
const {
    FENGARI_AUTHORS,
    FENGARI_COPYRIGHT,
    FENGARI_RELEASE,
    FENGARI_VERSION,
    FENGARI_VERSION_MAJOR,
    FENGARI_VERSION_MINOR,
    FENGARI_VERSION_NUM,
    FENGARI_VERSION_RELEASE,
    to_luastring,
    to_jsstring
} = require('./obcore');
const parser = require('./oparser');
const js = require('./js');

const parse = function(L) {
    const buff = luaL_checkstring(L, 1);
    var ast = parser.parse(to_jsstring(buff));

    js.pushjs(L, ast);
    
    return 1;
}


const fengari_lib = {
    "parse": parse
};

const luaopen_fengari = function(L) {
    luaL_newlib(L, fengari_lib);
    lua_pushliteral(L, FENGARI_AUTHORS);
    lua_setfield(L, -2, to_luastring("AUTHORS"));
    lua_pushliteral(L, FENGARI_COPYRIGHT);
    lua_setfield(L, -2, to_luastring("COPYRIGHT"));
    lua_pushliteral(L, FENGARI_RELEASE);
    lua_setfield(L, -2, to_luastring("RELEASE"));
    lua_pushliteral(L, FENGARI_VERSION);
    lua_setfield(L, -2, to_luastring("VERSION"));
    lua_pushliteral(L, FENGARI_VERSION_MAJOR);
    lua_setfield(L, -2, to_luastring("VERSION_MAJOR"));
    lua_pushliteral(L, FENGARI_VERSION_MINOR);
    lua_setfield(L, -2, to_luastring("VERSION_MINOR"));
    lua_pushinteger(L, FENGARI_VERSION_NUM);
    lua_setfield(L, -2, to_luastring("VERSION_NUM"));
    lua_pushliteral(L, FENGARI_VERSION_RELEASE);
    lua_setfield(L, -2, to_luastring("VERSION_RELEASE"));
    return 1;
};

module.exports.luaopen_fengari = luaopen_fengari;
