#include <dmsdk/sdk.h>

#include "gyro_private.h"

#define LIB_NAME "Gyro"
#define MODULE_NAME "gyro"

static int Start(lua_State* L) {
    Gyro_PlatformStart(L);
    return 0;
}

static int Stop(lua_State* L) {
    Gyro_PlatformStop(L);
    return 0;
}

static const luaL_reg Module_methods[] = {
  {"start", Start},
  {"stop", Stop},
  {0, 0}
};

static void LuaInit(lua_State* L) {
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeGyroExtension(dmExtension::AppParams* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeGyroExtension(dmExtension::Params* params) {
    LuaInit(params->m_L);
    printf("Registered %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeGyroExtension(dmExtension::AppParams* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeGyroExtension(dmExtension::Params* params) {
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(Gyro, LIB_NAME, AppInitializeGyroExtension, AppFinalizeGyroExtension, InitializeGyroExtension, 0, 0, FinalizeGyroExtension)
