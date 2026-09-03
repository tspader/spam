#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luajit.h"

#include <stdio.h>

static const char* script =
  "local ffi = require('ffi')\n"
  "ffi.cdef[[int printf(const char* fmt, ...);]]\n"
  "local sum = 0\n"
  "for i = 1, 1000000 do sum = sum + i end\n"
  "ffi.C.printf('%s sum=%.0f jit=%s\\n', jit.version, sum, tostring(jit.status()))\n";

int main(void) {
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);
  if (luaL_dostring(L, script)) {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    lua_close(L);
    return 1;
  }
  lua_close(L);
  return 0;
}
