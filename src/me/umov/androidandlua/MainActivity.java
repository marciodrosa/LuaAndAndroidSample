package me.umov.androidandlua;

import java.util.Arrays;
import java.util.List;

import android.app.Activity;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.widget.EditText;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		initFields();
	}
	
	@Override
	protected void onStart() {
		super.onStart();
		installPlugin("myscript");
	}
	
	private void initFields() {
		List<EditText> fields = Arrays.asList((EditText) findViewById(R.id.value_one), (EditText) findViewById(R.id.value_two), (EditText) findViewById(R.id.value_three));
		LuaFunctions.setFields(fields);
		for (final EditText editText : fields) {
			editText.addTextChangedListener(new TextWatcher() {
				
				boolean recursivityControl = false;
				
				@Override
				public void onTextChanged(CharSequence s, int start, int before, int count) {
					if (!recursivityControl) {
						recursivityControl = true;
						Log.i("androidandlua", "EditText '" + editText.getTag() + "' changed, executing script...");
						callPluginFunction("onfieldvaluechangedbyuser");
						recursivityControl = false;
					}
				}
				
				@Override
				public void beforeTextChanged(CharSequence s, int start, int count,
						int after) {
				}
				
				@Override
				public void afterTextChanged(Editable s) {
				}
			});
		}
	}
	
	private native void installPlugin(String scriptName);
	
	private native void callPluginFunction(String functionName);

}
