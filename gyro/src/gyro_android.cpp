#include "gyro_private.h"
#include <dmsdk/sdk.h>
#include <math.h>

#if defined(DM_PLATFORM_ANDROID)

    GyroData* g_GyroData;

static JNIEnv* Attach()
{
  JNIEnv* env;
  JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
  vm->AttachCurrentThread(&env, NULL);
  return env;
}

static bool Detach(JNIEnv* env)
{
  bool exception = (bool) env->ExceptionCheck();
  env->ExceptionClear();
  JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
  vm->DetachCurrentThread();
  return !exception;
}

namespace {
  struct AttachScope
  {
    JNIEnv* m_Env;
    AttachScope() : m_Env(Attach())
    {
    }
    ~AttachScope()
    {
      Detach(m_Env);
    }
  };
}

static jclass GetClass(JNIEnv* env, const char* classname)
{
    jclass activity_class = env->FindClass("android/app/NativeActivity");
    jmethodID get_class_loader = env->GetMethodID(activity_class,"getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), get_class_loader);
    jclass class_loader = env->FindClass("java/lang/ClassLoader");
    jmethodID find_class = env->GetMethodID(class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring str_class_name = env->NewStringUTF(classname);
    jclass outcls = (jclass)env->CallObjectMethod(cls, find_class, str_class_name);
    env->DeleteLocalRef(str_class_name);
    return outcls;
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_defold_android_gyro_GyroExtension_HandleSensorChanged(JNIEnv *env, jobject obj, jfloatArray matrix) {
    // convert rotation matrix to a quaternion since this is what is most useful
    // in Defold. go.set_rotation() operates on a quaternion for instance.
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    jfloat *matrix_elements = env->GetFloatArrayElements(matrix, 0);

	float m00 = matrix_elements[0];
	float m01 = matrix_elements[1];
	float m02 = matrix_elements[2];
	float m10 = matrix_elements[3];
	float m11 = matrix_elements[4];
	float m12 = matrix_elements[5];
	float m20 = matrix_elements[6];
	float m21 = matrix_elements[7];
	float m22 = matrix_elements[8];

    env->ReleaseFloatArrayElements(matrix , (jfloat *)matrix_elements, 0);
    env->DeleteLocalRef(matrix);

	float tr = m00 + m11 + m22;

	if (tr > 0) {
		float S = sqrt(tr+1.0) * 2; // S=4*qw
		g_GyroData->quat[3] = 0.25 * S;
		g_GyroData->quat[0] = (m21 - m12) / S;
		g_GyroData->quat[1] = (m02 - m20) / S;
		g_GyroData->quat[2] = (m10 - m01) / S;
	} else if ((m00 > m11)&(m00 > m22)) {
		float S = sqrt(1.0 + m00 - m11 - m22) * 2; // S=4*qx
		g_GyroData->quat[3] = (m21 - m12) / S;
		g_GyroData->quat[0] = 0.25 * S;
		g_GyroData->quat[1] = (m01 + m10) / S;
		g_GyroData->quat[2] = (m02 + m20) / S;
	} else if (m11 > m22) {
		float S = sqrt(1.0 + m11 - m00 - m22) * 2; // S=4*qy
		g_GyroData->quat[3] = (m02 - m20) / S;
		g_GyroData->quat[0] = (m01 + m10) / S;
		g_GyroData->quat[1] = 0.25 * S;
		g_GyroData->quat[2] = (m12 + m21) / S;
	} else {
		float S = sqrt(1.0 + m22 - m00 - m11) * 2; // S=4*qz
		g_GyroData->quat[3] = (m10 - m01) / S;
		g_GyroData->quat[0] = (m02 + m20) / S;
		g_GyroData->quat[1] = (m12 + m21) / S;
		g_GyroData->quat[2] = 0.25 * S;
	}
}

#ifdef __cplusplus
}
#endif



int Gyro_PlatformStart(struct GyroData* data) {
    g_GyroData = data;

	// prepare JNI
	AttachScope attachscope;
	JNIEnv* env = attachscope.m_Env;
	jclass cls = GetClass(env, "com.defold.android.gyro.GyroExtension");

    // call method
    jmethodID start_gyro = env->GetStaticMethodID(cls, "StartGyro", "(Landroid/content/Context;)V");
    env->CallStaticVoidMethod(cls, start_gyro, dmGraphics::GetNativeAndroidActivity());

	return 1;
}

int Gyro_PlatformStop() {
    // prepare JNI
	AttachScope attachscope;
	JNIEnv* env = attachscope.m_Env;
	jclass cls = GetClass(env, "com.defold.android.gyro.GyroExtension");

    // call method
    jmethodID stop_gyro = env->GetStaticMethodID(cls, "StopGyro", "(Landroid/content/Context;)V");
    env->CallStaticVoidMethod(cls, stop_gyro, dmGraphics::GetNativeAndroidActivity());

	return 1;
}



#endif // DM_PLATFORM_ANDROID
