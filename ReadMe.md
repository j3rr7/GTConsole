# Building

```
cmake -S "." -B "build" -G "Visual Studio 17 2022" -A "x64"
```
### DEBUG BUILD
```cmake --build "build" --config Debug```
### RELEASE BUILD
```cmake --build "build" --config Release```