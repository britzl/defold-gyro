#include "gyro_private.h"
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_IOS)

#import <CoreMotion/CoreMotion.h>



struct Gyro {
  CMMotionManager* motionManager;
  GyroListener listener;
} g_Gyro;

int Gyro_PlatformStart(lua_State* L) {
    Gyro* gyro = &g_Gyro;
    
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_pushvalue(L, 1);
    int cb = dmScript::Ref(L, LUA_REGISTRYINDEX);

    if (gyro->listener.m_Callback != LUA_NOREF) {
      dmScript::Unref(gyro->listener.m_L, LUA_REGISTRYINDEX, gyro->listener.m_Callback);
      dmScript::Unref(gyro->listener.m_L, LUA_REGISTRYINDEX, gyro->listener.m_Self);
    }

    gyro->listener.m_L = dmScript::GetMainThread(L);
    gyro->listener.m_Callback = cb;
    dmScript::GetInstance(L);
    gyro->listener.m_Self = dmScript::Ref(L, LUA_REGISTRYINDEX);
    
    
    gyro->motionManager = [[CMMotionManager alloc] init];
    gyro->motionManager.gyroUpdateInterval = 1.0/60.0;
    
    [gyro->motionManager startDeviceMotionUpdatesUsingReferenceFrame: CMAttitudeReferenceFrameXArbitraryCorrectedZVertical
                                                toQueue: [NSOperationQueue currentQueue]
                                            withHandler: ^(CMDeviceMotion *data, NSError *error)
                                            {
                                                
                                                lua_State* L = gyro->listener.m_L;
                                                int top = lua_gettop(L);
        
                                                lua_rawgeti(L, LUA_REGISTRYINDEX, gyro->listener.m_Callback);

                                                // Setup self
                                                lua_rawgeti(L, LUA_REGISTRYINDEX, gyro->listener.m_Self);
                                                lua_pushvalue(L, -1);
                                                dmScript::SetInstance(L);
                                                
                                                CMAttitude* attitude = data.attitude;
                                                NSLog(@"attitude = [pitch %f, roll %f, yaw %f]", attitude.pitch, attitude.roll, attitude.yaw);
                                                lua_pushnumber(L, attitude.pitch);
                                                lua_pushnumber(L, attitude.roll);
                                                lua_pushnumber(L, attitude.yaw);

                                                CMQuaternion quat = attitude.quaternion;
                                                lua_pushnumber(L, quat.x);
                                                lua_pushnumber(L, quat.y);
                                                lua_pushnumber(L, quat.z);
                                                lua_pushnumber(L, quat.w);

                                                //Vectormath::Aos::Quat rot(quat.x, quat.y, quat.z, quat.w);
                                                //dmScript::PushQuat(L, rot);
                                                
                                                int ret = lua_pcall(L, 8, LUA_MULTRET, 0);
                                                if (ret != 0) {
                                                    dmLogError("Error running gyro callback: %s", lua_tostring(L, -1));
                                                    lua_pop(L, 1);
                                                }
                                                assert(top == lua_gettop(L));
                                            }];
    printf("GYRO HAS STARTED\n");
    return 1;
}

int Gyro_PlatformStop(lua_State* L) {
  Gyro* gyro = &g_Gyro;
  if (gyro->motionManager != nil) {
    [gyro->motionManager stopGyroUpdates];
    [gyro->motionManager release];
    gyro->motionManager = nil;
    printf("GYRO HAS STOPPED\n");
  }
  return 1;
}

#else

int Gyro_PlatformStart(lua_State* L) {
  return 1;
}

int Gyro_PlatformStop(lua_State* L) {
  return 1;
}

#endif // DM_PLATFORM_IOS
