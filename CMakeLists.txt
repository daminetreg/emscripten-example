cmake_minimum_required(VERSION 3.0.0)
project (cxx2js-client)

add_definitions(-std=c++14)
set (CMAKE_CXX_STANDARD 14)

# Boost
include_directories (AFTER /opt/libs/boost_1_61_0/)
link_directories(/opt/libs/boost_1_61_0/stage-emscripten/lib/)

# Beast
include_directories(/opt/libs/Beast.js/include/)

if (DEFINED EMSCRIPTEN)
  if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-g)
    add_definitions(-g4) # Generates emscripten C++ sourcemaps
  endif()
endif()

include_directories(BEFORE ${CMAKE_CURRENT_LIST_DIR}/src)
file (GLOB_RECURSE SRC_FILES src/*.cpp)

if (DEFINED EMSCRIPTEN)
  

  #add_library(cxx2js-client SHARED ${SRC_FILES})
  add_executable(cxx2js-client ${SRC_FILES})

else (DEFINED EMSCRIPTEN)
  add_executable(cxx2js-client ${SRC_FILES})
endif(DEFINED EMSCRIPTEN)

target_link_libraries(cxx2js-client boost_system)

#file (COPY ${CMAKE_CURRENT_LIST_DIR}/Arial DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (DEFINED EMSCRIPTEN)
  set_target_properties(cxx2js-client 
    PROPERTIES SUFFIX ".html"
    LINK_FLAGS " --bind -s  NO_EXIT_RUNTIME=1 -g4")
  em_link_js_library(cxx2js-client ${libraryJsFiles})
  # add -O2 to minify
endif (DEFINED EMSCRIPTEN)

