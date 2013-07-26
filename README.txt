Sample projet that runs a Lua script inside the Android app as a plugin.

It uses:
- Java (Android app)
- C / C++ (the "bridge" between the Java app and the plugin, uses Android NDK)
- Lua (the plugin itself and a set of functions called "engine", that manages the plugin)

(See the languagesflow image)

BUILD:

Before run the app, executes the command "ndk-build" on root folder to build the libraries. Needs Android NDK, see this link: http://developer.android.com/tools/sdk/ndk/index.html)

RUN:

Open the project using eclipse as usual. When executing the app, type some values on text fields to see the plugin executing (it manipulates one of the text fields).

NOTES:

The Lua scripts (the plugin and the engine) are loaded from the "assets" folder.

This project usess Lua 5.2.1 port for Android: https://github.com/marciodrosa/LuaAndroid

I build this sample for ARM architecture only.