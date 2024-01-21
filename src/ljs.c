/*
** $Id: ljs.c $
** Standard mathematical library
** See Copyright Notice in lua.h
*/

#define ljs_c
#define LUA_LIB

#include "lprefix.h"


#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "ljs.h"

int stackSize = 0;
const char* stack = "";
const char* stacknew = "";

/** STACK */
EM_JS(void, returnFunc, (const char* js, const char* stack, const char* stacknew), {
    return eval("const latest = "+UTF8ToString(stacknew)+";\n const stack = ["+UTF8ToString(stack)+"];\n"+UTF8ToString(js));
})

static int js_eval (lua_State *L) {
    const char* js = luaL_checkstring(L, 1);
    returnFunc(js, stack, stacknew);
    

    return 1;
}

static int js_pushraw (lua_State *L) {
    const char* val = luaL_checkstring(L, 1);

    if (stackSize > 0) {
        stack = strcat(stack, ", ");
    }
    strcat(stack, val);
    stackSize++;

    stacknew = val;

    return 0;
}

static int js_push (lua_State *L) {
    luaL_checkany(L, 1);
    const char* pushVal = "";
    if (lua_type(L, 1) == LUA_TNUMBER) {
        pushVal = lua_tostring(L, 1);
    } else if (lua_type(L, 1) == LUA_TSTRING) {
        pushVal = lua_tostring(L, 1);
    } else if (lua_type(L, 1) == LUA_TBOOLEAN) {
        pushVal = lua_toboolean(L, 1) ? "true" : "false";
    } else if (lua_type(L, 1) == LUA_TTABLE) {
        luaL_Buffer b;
        luaL_buffinit(L, &b);
        luaL_addstring(&b, "{");
        lua_pushnil(L);
        while (lua_next(L, 1) != 0) {
            lua_pushvalue(L, -2);  // copy key to top
            const char* key = lua_tostring(L, -1);
            const char* value = lua_tostring(L, -2);
            luaL_addstring(&b, key);
            luaL_addstring(&b, ": ");
            luaL_addstring(&b, value);
            luaL_addstring(&b, ", ");
            lua_pop(L, 2);  // pop value and copied key
        }
        luaL_addstring(&b, "}");
        luaL_pushresult(&b);
        pushVal = lua_tostring(L, -1);
    } else if (lua_type(L, 1) == LUA_TFUNCTION) {
        luaL_error(L, "function cannot be pushed to JS");
    } else if (lua_type(L, 1) == LUA_TUSERDATA) {
        luaL_error(L, "userdata cannot be pushed to JS");
    } else if (lua_type(L, 1) == LUA_TTHREAD) {
        luaL_error(L, "thread cannot be pushed to JS");
    } else if (lua_type(L, 1) == LUA_TLIGHTUSERDATA) {
        luaL_error(L, "lightuserdata cannot be pushed to JS");
    } else {
        pushVal = "null";
    }

    if (stackSize > 0) {
        stack = strcat(stack, ", ");
    }
    strcat(stack, pushVal);
    stackSize++;

    stacknew = pushVal;
    
    return 0;
}

static int js_clear (lua_State *L) {
    stack = "";
    stackSize = 0;
    return 0;
}

/** RAW */
static int js_run (lua_State *L) {
  int argc = lua_gettop(L) - 1;
  const char* argv[argc];

  void* ptr = EM_ASM_PTR({
    return eval("const latest = "+UTF8ToString(stacknew)+";\n const stack = ["+UTF8ToString(stack)+"];\n"+UTF8ToString(js));
  }, luaL_checkstring(L, 1), argv, argc);

  lua_pushlightuserdata(L, ptr);
  return 1;
}

static int toString(lua_State *L) {
  void *ptr = lua_touserdata(L, 1);
  lua_pushstring(L, &ptr);
  return 1;
}

static int toNumber(lua_State *L) {
  void *ptr = lua_touserdata(L, 1);
  lua_pushnumber(L, &ptr);
  return 1;
}

static int toBoolean(lua_State *L) {
  void *ptr = lua_touserdata(L, 1);
  lua_pushboolean(L, &ptr);
  return 1;
}

static const luaL_Reg jslib[] = {
  /* Stack based */
  {"eval",   js_eval},
  {"pushraw",   js_pushraw},
  {"push",   js_push},
  {"clear",   js_clear},

  /* Raw */
  {"run",   js_run},
  {"toString",   toString},
  {"toNumber",   toNumber},
  {"toBoolean",   toBoolean},

  {NULL, NULL}
};


/*
** Open JS library
*/
LUAMOD_API int luaopen_js (lua_State *L) {
  luaL_newlib(L, jslib);
  return 1;
}

