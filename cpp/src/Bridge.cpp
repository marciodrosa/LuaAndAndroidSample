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

/**
Searches for the asset in the directory. Returns the full asset name (like "dir/asset") or empty
if not found.
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

	//Log::Info("Requiring 'vega' Lua script...");
	//luaL_loadstring(luaState, "require 'vega'");
	//lua_pcall(luaState, 0, 0, 0);

	/*
	Log::Info("Creating the capi Lua table...");
	lua_getglobal(luaState, "vega");
	lua_getfield(luaState, -1, "capi");
	
	Log::Info("Creating the Lua functions into the capi table...");
	lua_pushstring(luaState, "checkinput");
	lua_pushcfunction(luaState, CheckInputLuaFunction);
	lua_settable(luaState, -3);
	
	lua_pushstring(luaState, "syncend");
	lua_pushcfunction(luaState, SyncEndLuaFunction);
	lua_settable(luaState, -3);
	
	lua_pushstring(luaState, "syncbegin");
	lua_pushcfunction(luaState, SyncBeginLuaFunction);
	lua_settable(luaState, -3);
	
	lua_pushstring(luaState, "render");
	lua_pushcfunction(luaState, RenderLuaFunction);
	lua_settable(luaState, -3);
	
	lua_pushstring(luaState, "clearscreen");
	lua_pushcfunction(luaState, ClearScreenLuaFunction);
	lua_settable(luaState, -3);
	
	lua_pushstring(luaState, "screensize");
	lua_pushcfunction(luaState, ScreenSizeLuaFunction);
	lua_settable(luaState, -3);
	
	lua_pushstring(luaState, "loadtexture");
	lua_pushcfunction(luaState, LoadTextureLuaFunction);
	lua_settable(luaState, -3);
	
	lua_pushstring(luaState, "releasetextures");
	lua_pushcfunction(luaState, ReleaseTexturesLuaFunction);
	lua_settable(luaState, -3);

	lua_pop(luaState, 2);
	Log::Info("capi Lua table created.");
	*/
}

void ExecuteScript(string scriptName)
{
	stringstream ss;
	ss << "require '" << scriptName << "'";
	if (luaL_loadstring(luaState, ss.str().c_str()) != 0)
		Log::Error(lua_tostring(luaState, -1));
	else if (lua_pcall(luaState, 0, 0, 0) != 0)
		Log::Error(lua_tostring(luaState, -1));
}

extern "C"
{
	/**
	Native implementation of the "on create" event of the activity.
	*/
	void ANativeActivity_onCreate(ANativeActivity* a, void* savedState, size_t savedStateSize)
	{
		Log::Info("Native activity created.");
		activity = a;
		Log::Info("Initiating Lua...");
		InitLua();
	}
	
	/**
	Native function available on MainActivity class.
	*/
	JNIEXPORT void JNICALL Java_me_umov_MainActivity_executeLuaScript(JNIEnv *env, jobject obj, jstring s)
	{
		Log::Info("Executing script...");
		string scriptName = env->GetStringUTFChars(s, 0);
		Log::Info("Preparing to execute script:");
		Log::Info(scriptName);
		ExecuteScript(scriptName);
	}
}