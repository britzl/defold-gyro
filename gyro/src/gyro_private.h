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

struct GyroData {
    float quat[4];
};

void Gyro_SetListener(struct GyroListener* listener, lua_State* L);
void Gyro_InvokeListener(struct GyroListener* listener, struct GyroData* data);

extern int Gyro_PlatformStart(struct GyroData* data);
extern int Gyro_PlatformStop();
