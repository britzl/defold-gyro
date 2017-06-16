package com.defold.android.gyro;


import android.hardware.SensorManager;
import android.hardware.SensorEventListener;
import android.hardware.SensorEvent;
import android.hardware.Sensor;
import android.content.Context;

import android.util.Log;


class GyroExtension implements SensorEventListener {
	private static final String TAG = "defold";

	private SensorManager mSensorManager;
	private Sensor mSensor;

	private static GyroExtension instance = null;

	private float[] matrix = new float[9];

	private GyroExtension() {

	}

	public static GyroExtension getInstance() {
		if (instance == null) {
			instance = new GyroExtension();
		}
		return instance;
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
	}

	@Override
	public void onSensorChanged(SensorEvent event) {
		if (event.sensor.getType() == Sensor.TYPE_GAME_ROTATION_VECTOR || event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {
			// Note: SensorManager.getQuaternionFromVector doesn't return values
			// that I was able to use as they were. The values seemed to be device
			// orientation dependent somehow.
			//SensorManager.getQuaternionFromVector(quat, event.values);
			SensorManager.getRotationMatrixFromVector(matrix, event.values);
			HandleSensorChanged(matrix);
		}
	}

	private native void HandleSensorChanged(float[] matrix);

	private void start(Context context) {
		mSensorManager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);
		if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR2) {
			mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
		}
		// Use the TYPE_ROTATION_VECTOR sensor if below Jelly Bean or if TYPE_GAME_ROTATION_VECTOR
		// isn't available (such as on Samsung Galaxy S4 devices)
		if (mSensor == null) {
			mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
		}
		if (mSensor == null) {
			Log.w(TAG, "Unable to find a rotation sensor. The Gyro extension will not function properly without it.");
		}
		mSensorManager.registerListener(this, mSensor, SensorManager.SENSOR_DELAY_GAME);
	}

	private void stop(Context context) {
		mSensorManager.unregisterListener(this);
	}

	public static void StartGyro(Context context) {
		getInstance().start(context);
	}

	public static void StopGyro(Context context) {
		getInstance().stop(context);
	}
}
