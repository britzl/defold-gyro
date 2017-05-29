#include "gyro_private.h"
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_IOS)

#import <CoreMotion/CoreMotion.h>

CMMotionManager* g_MotionManager = nil;

int Gyro_PlatformStart(struct GyroData* gyro_data) {
    g_MotionManager = [[CMMotionManager alloc] init];
    g_MotionManager.gyroUpdateInterval = 1.0/60.0;

    [g_MotionManager startDeviceMotionUpdatesUsingReferenceFrame: CMAttitudeReferenceFrameXArbitraryCorrectedZVertical
                                                toQueue: [NSOperationQueue currentQueue]
                                            withHandler: ^(CMDeviceMotion *data, NSError *error)
                                            {
                                                CMAttitude* attitude = data.attitude;
                                                // CMRotationMatrix doesn't provide useful values
                                                // It seems as though the rotation matrix has to realign
                                                // it's axis somehow.
                                                // CMQuaternion works well though
                                                //CMRotationMatrix matrix = attitude.rotationMatrix;
                                                CMQuaternion quat = attitude.quaternion;
                                                gyro_data->quat[0] = quat.x;
                                                gyro_data->quat[1] = quat.y;
                                                gyro_data->quat[2] = quat.z;
                                                gyro_data->quat[3] = quat.w;
                                            }];
    return 1;
}

int Gyro_PlatformStop() {
  if (g_MotionManager != nil) {
    [g_MotionManager stopGyroUpdates];
    [g_MotionManager release];
    g_MotionManager = nil;
  }
  return 1;
}

#endif // DM_PLATFORM_IOS
