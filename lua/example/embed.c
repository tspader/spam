#include <stdio.h>

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

static const char *SCRIPT =
    "local acc = 0\n"
    "for i = 1, 10 do acc = acc + i * i end\n"
    "sums = acc\n"
    "function report(label, n)\n"
    "  local t = {}\n"
    "  for i = 1, n do t[i] = host_scale(i, 3) end\n"
    "  return label .. ':' .. table.concat(t, ','), #t\n"
    "end\n";

static int host_scale(lua_State *L) {
  lua_Integer x = luaL_checkinteger(L, 1);
  lua_Integer k = luaL_checkinteger(L, 2);
  lua_pushinteger(L, x * k);
  return 1;
}

int main(void) {
  lua_State *L = luaL_newstate();
  if (L == NULL) {
    fprintf(stderr, "luaL_newstate failed\n");
    return 1;
  }
  luaL_openlibs(L);

  lua_pushcfunction(L, host_scale);
  lua_setglobal(L, "host_scale");

  if (luaL_dostring(L, SCRIPT) != LUA_OK) {
    fprintf(stderr, "script error: %s\n", lua_tostring(L, -1));
    lua_close(L);
    return 1;
  }

  printf("%s\n", LUA_RELEASE);

  lua_getglobal(L, "sums");
  printf("sum of squares 1..10 = %lld\n", (long long)lua_tointeger(L, -1));
  lua_pop(L, 1);

  lua_getglobal(L, "report");
  lua_pushstring(L, "scaled");
  lua_pushinteger(L, 5);
  if (lua_pcall(L, 2, 2, 0) != LUA_OK) {
    fprintf(stderr, "call error: %s\n", lua_tostring(L, -1));
    lua_close(L);
    return 1;
  }
  printf("report -> %s (count %lld)\n", lua_tostring(L, -2),
         (long long)lua_tointeger(L, -1));
  lua_pop(L, 2);

  if (luaL_dostring(L, "error('boom from lua')") == LUA_OK) {
    fprintf(stderr, "expected an error\n");
    lua_close(L);
    return 1;
  }
  printf("caught: %s\n", lua_tostring(L, -1));
  lua_pop(L, 1);

  printf("stack depth at exit = %d\n", lua_gettop(L));
  lua_close(L);
  return 0;
}
