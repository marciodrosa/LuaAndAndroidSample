#include "../include/Bridge.h"
#include "../include/Lua.h"
#include "../include/Android.h"
#include "../include/Log.h"

#include <sstream>
#include <list>
#include <string>

using namespace androidandlua;
using namespace std;

ANativeActivity* activity;
lua_State *luaState;
JNIEnv *javaEnv;

/**
Lua function to set a value (first parameter) to a field (second parameter is the field identifier).
It calls the Java method of the main activity.
*/
int SetValueToFieldLuaFunction(lua_State *luaState)
{
	Log::Info("SetValueToFieldLuaFunction called.");
	
	const char* value = lua_tolstring(luaState, -2, NULL);
	const char* fieldId = lua_tolstring(luaState, -1, NULL);

	// Accessing the MainActivity java class and the method setValueToField:
	jclass mainActivityClass = javaEnv->FindClass("me/umov/androidandlua/MainActivity");
	jmethodID methodId =  javaEnv->GetStaticMethodID(mainActivityClass, "setValueToField", "(Ljava/lang/String;Ljava/lang/String;)V");
	
	// Calling the setValueToField method:
	jstring jValue = javaEnv->NewStringUTF(value);
	jstring jFieldId = javaEnv->NewStringUTF(fieldId);
	javaEnv->CallStaticVoidMethod(mainActivityClass, methodId, jValue, jFieldId);
	
	return 0;
}

/**
Searches for the asset in the directory. Returns the full asset name (like "dir/asset") or empty
if not found. Used to load the scripts from the assets.
*/
string SearchAssetOnDir(string dirName, string assetName)
{
	string foundAssetName;
	bool found = false;
	AAssetDir* dir = AAssetManager_openDir(activity->assetManager, dirName.c_str());
	const char* dirAsset = NULL;
	while ((dirAsset = AAssetDir_getNextFileName(dir)) != NULL)
	{
		string s = dirAsset;
		if (s == assetName)
		{
			found = true;
			if (dirName.length() > 0)
			{
				stringstream ss;
				ss << dirName << '/' << assetName;
				foundAssetName = ss.str().c_str();
			}
			else
				foundAssetName = assetName;
			break;
		}
	}
	AAssetDir_close(dir);
	if (found)
		return foundAssetName;
	else
		return "";
}

/**
Function used by the "require" function to load a module from the assets. It expects two input parameters
(the extra value = the asset name, and the module name, used for debug and messages) and returns 1 value
for the Lua (the value returned after run the loaded asset).
*/
int LoadModuleFromAssetsLuaFunction(lua_State *luaState)
{
	string extraValue = lua_tostring(luaState, -1);
	string moduleName = lua_tostring(luaState, -2);
	AAsset* asset = AAssetManager_open(activity->assetManager, extraValue.c_str(), AASSET_MODE_BUFFER);
	const void *data = AAsset_getBuffer(asset);
	size_t dataSize = AAsset_getLength(asset);
	if (luaL_loadbuffer(luaState, (const char*)data, dataSize, moduleName.c_str()))
		Log::Error(lua_tostring(luaState, -1));
	else if (lua_pcall(luaState, 0, 1, 0) != 0)
		Log::Error(lua_tostring(luaState, -1));
	AAsset_close(asset);
	return 1;
}

/**
The function to be added to the package.searches on Lua. It searches the module name (the input parameter)
in the assets folder. If not found, returns 0 results. Otherwise, returns 2 results: the load function and
the full asset name (directory and file name).
*/
int SearchModuleInAssetsLuaFunction(lua_State *luaState)
{
	string moduleName = lua_tostring(luaState, -1);
	stringstream moduleNameWithLuaExtension;
	moduleNameWithLuaExtension << moduleName << ".lua";
	stringstream moduleNameWithLCExtension;
	moduleNameWithLCExtension << moduleName << ".lc";
	// todo: is looking for the file on root dir and lua dir; must be changedto look into the package.searchpath Lua field.
	list<string> dirs;
	dirs.push_back("");
	dirs.push_back("lua");
	list<string> assetsNames;
	assetsNames.push_back(moduleName);
	assetsNames.push_back(moduleNameWithLuaExtension.str().c_str());
	assetsNames.push_back(moduleNameWithLCExtension.str().c_str());
	for (list<string>::iterator i = dirs.begin(); i != dirs.end(); ++i)
	{
		for (list<string>::iterator j = assetsNames.begin(); j != assetsNames.end(); ++j)
		{
			string fullAssetNameFound = SearchAssetOnDir(*i, *j);
			if (fullAssetNameFound.length() > 0)
			{
				lua_pushcfunction(luaState, LoadModuleFromAssetsLuaFunction);
				lua_pushstring(luaState, fullAssetNameFound.c_str());
				return 2;
			}
		}
	}
	return 0;
}

/**
Setup Lua to search the scripts (when the "require" function is used) in the assets folder.
It adds a new function into the package.searches function.
*/
void InitLuaSearches()
{
	Log::Info("Adding the assets search function into the package.searches function...");
	lua_getglobal(luaState, "package");
	lua_getfield(luaState, -1, "searchers");
	lua_len(luaState, -1);
	int searchesLength = lua_tonumber(luaState, -1);
	lua_pop(luaState, 1);
	lua_pushcfunction(luaState, SearchModuleInAssetsLuaFunction);
	lua_rawseti(luaState, -2, searchesLength + 1);
	lua_pop(luaState, 2);
}

/**
Initializes Lua and loads the engine script.
*/
void InitLua()
{
	Log::Info("Creating a new Lua state...");
	luaState = luaL_newstate();
	Log::Info("Opening Lua libraries...");
	luaL_openlibs(luaState);
	Log::Info("Configuring Lua package path...");
	luaL_loadstring(luaState, "package.path = package.path..';lua/?.lua;lua/?'");
	lua_pcall(luaState, 0, 0, 0);
	
	Log::Info("Configuring Android Lua searches...");
	InitLuaSearches();
	
	Log::Info("Creating the functions to comunicate with Java app.");
	lua_pushcfunction(luaState, SetValueToFieldLuaFunction);
	lua_setglobal(luaState, "setvaluetofield");
	
	Log::Info("Requiring engine script...");
	lua_getglobal(luaState, "require");
	lua_pushstring(luaState, "engine");
	if (lua_pcall(luaState, 1, 1, 0) != 0)
	{
		Log::Error("Error when require the engine script:");
		Log::Error(lua_tostring(luaState, -1));
		lua_pop(luaState, 1);
	}
}

extern "C"
{
	/**
	Native implementation of the "on create" event of the activity. Initializes Lua.
	*/
	void ANativeActivity_onCreate(ANativeActivity* a, void* savedState, size_t savedStateSize)
	{
		Log::Info("Native activity created.");
		activity = a;
		Log::Info("Initiating Lua...");
		InitLua();
	}
	
	/**
	Native function available on MainActivity class. Installs the plugin.
	*/
	JNIEXPORT void JNICALL Java_me_umov_androidandlua_MainActivity_installPlugin(JNIEnv *env, jobject obj, jstring s)
	{
		javaEnv = env;
		Log::Info("Installing script...");
		string scriptName = env->GetStringUTFChars(s, 0);
		Log::Info("Preparing to install script:");
		Log::Info(scriptName);
		
		lua_getglobal(luaState, "engine");
		
		Log::Info("Requiring script...");
		lua_getglobal(luaState, "require");
		lua_pushstring(luaState, scriptName.c_str());
		if (lua_pcall(luaState, 1, 1, 0) != 0)
		{
			Log::Error("Error when require the script:");
			Log::Error(lua_tostring(luaState, -1));
			lua_pop(luaState, 1);
		}
		else
		{
			Log::Info("Storing the plugin table...");
			lua_setfield(luaState, -2, "plugin");
		}
		
		lua_pop(luaState, 1); // pops "engine"
	}
	
	/**
	Native function available on MainActivity class. Executes a function of the previous installed
	plugin, with the contex as parameter (context is a string to be parsed as a Lua table).
	*/
	JNIEXPORT void JNICALL Java_me_umov_androidandlua_MainActivity_callPluginFunction(JNIEnv *env, jobject obj, jstring jFunctionName, jstring jContextString)
	{
		javaEnv = env;
		Log::Info("Calling script function...");
		string functionName = env->GetStringUTFChars(jFunctionName, 0);
		Log::Info("Preparing to execute script function:");
		Log::Info(functionName);
		
		lua_getglobal(luaState, "engine");
		lua_pushstring(luaState, "callplugin");
		lua_gettable(luaState, -2);
		
		lua_pushstring(luaState, functionName.c_str());
		
		Log::Info("Preparing the context table:");
		string contextString = env->GetStringUTFChars(jContextString, 0);
		luaL_loadstring(luaState, contextString.c_str());
		lua_pcall(luaState, 0, 1, 0);
		
		if (lua_pcall(luaState, 2, 0, 0) != 0) // call engine.callplugin(functionname, context)
		{
			Log::Error("Error executing the script function:");
			Log::Error(lua_tostring(luaState, -1));
			lua_pop(luaState, 1);
		}
		lua_pop(luaState, 1); // pops "engine"
	}
}