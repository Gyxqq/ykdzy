cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=E:\programfiles\mingw64\bin\g++.exe -DCMAKE_BUILD_TYPE=Debug --no-warn-unused-cli -S. -B./build -G "Unix Makefiles"
cmake --build ./build --config Debug
for %%i in (build\*.dll) do copy %%i .\
for %%i in (build\*.dbg) do copy %%i .\