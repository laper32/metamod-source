Metamod:Source
==============

Forked from https://github.com/alliedmodders/metamod-source but with a lot of individual modifications.

In order to use github Actions, re-create a repository.

Try my best with parallel updating.

---

Metamod:Source - A C++ Plugin Environment and Detour Library for the Source Engine.

Build instructions:
1. Configure Source Engine version (Source1|Source2). Variable: `-DSOURCE2=ON` (By default is `OFF`)
   1. If you want to build with Source 2, please specify with -DSOURCE2=ON, and go to the process 2.1.
   2. if you want to build with Source 1, specify with nothing, and go to the process 2.2.
2. Configure Source Engine.
   1. Currently no SDK yet, not supported, LOL. What are you expecting? Expect Gaben release Source 2 SDK for us? Better you ask PerfectWorld to ask Gaben to release Source 2 SDK to us. LOL.
   2. Configure SDK path. If you want to configure with environment variable, goto 2.2.1, otherwise 2.2.2, with variable: `-DHL2SDK_USE_VENDOR=OFF` (By default is ON)
      1. Go to `cmake/Source1/DetectSDK.cmake` and check the commented blocks, and check what I have labelled part what named `sdk`. This is the environment variable name what we used to locate the HL2SDK path.
      2. Put the SDK into the path of the root project with `dependency/` folder inside, and specify the name, aka: `hl2sdk-${hl2sdk engine branch}`. You can check the supported engine name in `cmake/Source1/DetectSDK.cmake`.
   3. Specify the target arch, with variable `-DTARGET_ARCH`
   4. Create a folder to build. For example: `mkdir build && cd build`
   5. Pass in the variable what we have specified above. I will assume you know how to use CMake. If not, I will provide you an example about how to configure it: `cmake -G "Visual Studio 16 2019" -A Win32 -DCMAKE_BUILD_TYPE=Release -DHL2SDK_ENGINE_NAME=csgo -DTARGET_ARCH=x86 ../`
      > Note:
      > 1. On Linux you just need to pass in `cmake -DCMAKE_BUILD_TYPE=Release -DHL2SDK_ENGINE_NAME=csgo -DTARGET_ARCH=x86 ../` 
      > 2. Add `-DTARGET_ARCH` is for generic linking library. :) Check hl2sdk, then you will know everything.
   6. Build the project. `cmake --build . --config Release`
      > Note: On Linux, you need to specify the process count used. For example: `cmake --build . --config Release -j 4` 

Stable build snapshots: <http://www.metamodsource.net/downloads.php/?branch=stable>

Development build snapshots: <http://www.metamodsource.net/downloads.php/?branch=master>

General documentation: <https://wiki.alliedmods.net/Category:Metamod:Source_Documentation>

Detouring with SourceHook: <https://wiki.alliedmods.net/SourceHook_Development>

Development: <https://wiki.alliedmods.net/Category:Metamod:Source_Development>
