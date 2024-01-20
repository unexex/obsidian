#ifndef LUAINPYTHON_H
#define LUAINPYTHON_H

#if LUA_VERSION_NUM == 501
  #define luaL_len lua_objlen
  #define luaL_setfuncs(L, l, nup) luaL_register(L, NULL, (l))
  #define luaL_newlib(L, l) (lua_newtable(L), luaL_register(L, NULL, (l)))
#endif

typedef struct
{
    PyObject_HEAD
    int ref;
    int refiter;
} LuaObject;

extern PyTypeObject LuaObject_Type;

#define LuaObject_Check(op) PyObject_TypeCheck(op, &LuaObject_Type)

PyObject* LuaConvert(lua_State *L, int n);

extern lua_State *LuaState;

#if PY_MAJOR_VERSION < 3
#  define PyInit_lua initlua
#endif
PyMODINIT_FUNC PyInit_lua(void);

#endif