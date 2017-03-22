#pragma once

#include <dmsdk/sdk.h>


struct GyroListener {
  GyroListener() {
    m_L = 0;
    m_Callback = LUA_NOREF;
    m_Self = LUA_NOREF;
  }
  lua_State* m_L;
  int        m_Callback;
  int        m_Self;
};


extern int Gyro_PlatformStart(lua_State* L);
extern int Gyro_PlatformStop(lua_State* L);

