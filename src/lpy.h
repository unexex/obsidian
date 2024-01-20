#ifndef PYTHONINLUA_H
#define PYTHONINLUA_H

#define POBJECT "POBJECT"

#if PY_MAJOR_VERSION < 3
  #define PyBytes_Check           PyString_Check
  #define PyBytes_AsStringAndSize PyString_AsStringAndSize
#endif

int py_convert(lua_State *L, PyObject *o);

typedef struct
{
    PyObject *o;
    int asindx;
} py_object;

py_object*    luaPy_to_pobject(lua_State *L, int n);
LUA_API int   luaopen_python(lua_State *L);

#endif