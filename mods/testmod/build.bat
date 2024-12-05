cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE --no-warn-unused-cli -S. -B./build -G "Visual Studio 17" -T host=x64 -A x64
cmake --build ./build --config Debug --target ALL_BUILD
for %%i in (build\Debug\*.dll) do copy %%i .\