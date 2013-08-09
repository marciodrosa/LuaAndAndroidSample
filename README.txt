Sample projet that executes a Lua script inside the Android app as a plugin.

It uses the following languages:
- Java (Android app)
- C / C++ (the "bridge" between the Java app and the plugin; it uses Android NDK)
- Lua (the plugin itself and a set of functions called "engine" that manages the plugin)

(See the "languagesflow" image)

BUILD:

Before run the app, execute the command "ndk-build" on root folder to build the libraries. Needs Android NDK, see this link: http://developer.android.com/tools/sdk/ndk/index.html)

RUN:

Open the project using eclipse as usual. When executing the app, type some numeric values on text fields to see the plugin executing (the sum of the values of the first and second fields are set into the third one).

NOTES:

The Lua scripts (the plugin and the engine) are loaded from the "assets" folder.

The project uses Lua 5.2.1 port for Android: https://github.com/marciodrosa/LuaAndroid

I build this sample for ARM architecture only. If you try to run on a device or emulator with Intel processor, the app will crash.
