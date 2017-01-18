# Simple Emscripten Example
Calling XMLHttpRequest in your browser from C++

## Compiling
```sh
cd /path/to/emscripten
source emsdk_env.sh 

cd /path/to/this/project
mkdir buildjs/
cd buildjs
cmake -GNinja .. -DCMAKE_TOOLCHAIN_FILE=${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake
ninja
```

## Running
Open cxx2js-client.html !
