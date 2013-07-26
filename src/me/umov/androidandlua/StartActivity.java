package me.umov.androidandlua;

import android.app.NativeActivity;
import android.content.Intent;
import android.os.Bundle;

/**
 * A native activity used to load the C engine. It immediatelly calls the MainActivity.
 */
public class StartActivity extends NativeActivity {
	
	static {
		System.loadLibrary("stlport_shared");
		System.loadLibrary("lua");
		System.loadLibrary("bridge");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent intent = new Intent(this, MainActivity.class);
		startActivity(intent);
	}

}
