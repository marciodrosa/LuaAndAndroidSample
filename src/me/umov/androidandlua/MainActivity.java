package me.umov.androidandlua;

import java.util.Arrays;

import android.app.Activity;
import android.os.Bundle;
import android.widget.EditText;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		LuaFunctions.setFields(Arrays.asList((EditText) findViewById(R.id.value_one), (EditText) findViewById(R.id.value_two), (EditText) findViewById(R.id.value_three)));
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		executeLuaScript("myscript");
	}
	
	private native void executeLuaScript(String scriptName);

}
