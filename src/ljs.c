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
#include <time.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "ljs.h"

EM_JS(void, returnFunc, (const char* js), {
    eval(UTF8ToString(js));
})

static int js_eval (lua_State *L) {
    const char* js = luaL_checkstring(L, 1);
    returnFunc(js);
      
    return 1;
}

static const luaL_Reg jslib[] = {
  {"eval",   js_eval},
  {NULL, NULL}
};


/*
** Open JS library
*/
LUAMOD_API int luaopen_js (lua_State *L) {
  luaL_newlib(L, jslib);
  return 1;
}

