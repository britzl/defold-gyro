#include <dmsdk/sdk.h>

#include "gyro_private.h"

#define LIB_NAME "Gyro"
#define MODULE_NAME "gyro"


struct Gyro {
    GyroListener m_Listener;
    GyroData m_Data;

    int m_Started;
} g_Gyro;

static int Start(lua_State* L) {
    Gyro* gyro = &g_Gyro;
    gyro->m_Started = 1;

    Gyro_SetListener(&gyro->m_Listener, L);
    Gyro_PlatformStart(&gyro->m_Data);
    return 0;
}

static int Stop(lua_State* L) {
    Gyro* gyro = &g_Gyro;
    gyro->m_Started = 0;

    Gyro_PlatformStop();
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
	#if defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_IOS)
    	LuaInit(params->m_L);
	    printf("Registered %s Extension\n", MODULE_NAME);
	#else
	    printf("Extension %s is not supported\n", MODULE_NAME);
	#endif
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeGyroExtension(dmExtension::AppParams* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeGyroExtension(dmExtension::Params* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result UpdateGyroExtension(dmExtension::Params* params) {
    Gyro* gyro = &g_Gyro;
    if (gyro->m_Started == 1) {
        Gyro_InvokeListener(&gyro->m_Listener, &gyro->m_Data);
    }

    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(Gyro, LIB_NAME, AppInitializeGyroExtension, AppFinalizeGyroExtension, InitializeGyroExtension, UpdateGyroExtension, 0, FinalizeGyroExtension)
