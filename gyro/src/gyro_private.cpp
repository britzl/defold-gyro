#include "gyro_private.h"
#include <dmsdk/sdk.h>

void Gyro_SetListener(struct GyroListener* listener, lua_State* L) {
	luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_pushvalue(L, 1);
    int cb = dmScript::Ref(L, LUA_REGISTRYINDEX);

    if (listener->m_Callback != LUA_NOREF) {
      dmScript::Unref(listener->m_L, LUA_REGISTRYINDEX, listener->m_Callback);
      dmScript::Unref(listener->m_L, LUA_REGISTRYINDEX, listener->m_Self);
    }

    listener->m_L = dmScript::GetMainThread(L);
    listener->m_Callback = cb;
    dmScript::GetInstance(L);
    listener->m_Self = dmScript::Ref(L, LUA_REGISTRYINDEX);
}

void Gyro_InvokeListener(struct GyroListener* listener, struct GyroData* data) {
	lua_State* L = listener->m_L;
	int top = lua_gettop(L);

	lua_rawgeti(L, LUA_REGISTRYINDEX, listener->m_Callback);

	// Setup self
	lua_rawgeti(L, LUA_REGISTRYINDEX, listener->m_Self);
	lua_pushvalue(L, -1);
	dmScript::SetInstance(L);

	// push quaternion values
	lua_pushnumber(L, data->quat[0]);
	lua_pushnumber(L, data->quat[1]);
	lua_pushnumber(L, data->quat[2]);
	lua_pushnumber(L, data->quat[3]);

	// invoke the callback function
	int ret = lua_pcall(L, 5, LUA_MULTRET, 0);
	if (ret != 0) {
		dmLogError("Error while invoking gyro callback: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	assert(top == lua_gettop(L));
}

#if !defined(DM_PLATFORM_ANDROID) && !defined(DM_PLATFORM_IOS)

int Gyro_PlatformStart(struct GyroData* gyro_data) {
	return 1;
}

int Gyro_PlatformStop() {
	return 1;
}

#endif
