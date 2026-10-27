cmake_minimum_required(VERSION 3.20)
project(atlas_maker LANGUAGES C RC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# libwebp encodes the atlas. Only the library is built, not its command-line tools.
include(FetchContent)
foreach(option ANIM_UTILS CWEBP DWEBP GIF2WEBP IMG2WEBP VWEBP WEBPINFO WEBPMUX EXTRAS LIBWEBPMUX)
  set(WEBP_BUILD_${option} OFF CACHE BOOL "" FORCE)
endforeach()
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(libwebp
  GIT_REPOSITORY https://github.com/webmproject/libwebp.git
  GIT_TAG v1.6.0
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(libwebp)

set(WARNINGS -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
set(DEFINES UNICODE _UNICODE _WIN32_WINNT=0x0A00 WINVER=0x0A00)

# Everything except the window: layout, image I/O, atlas assembly.
add_library(atlas_core STATIC src/core/atlas.c src/core/image.c src/core/layout.c)
target_include_directories(atlas_core PUBLIC src/core)
target_compile_definitions(atlas_core PUBLIC ${DEFINES})
target_compile_options(atlas_core PRIVATE ${WARNINGS})
target_link_libraries(atlas_core PUBLIC webp windowscodecs ole32 uuid)

add_executable(atlas_maker WIN32 src/gui/main.c src/gui/window.c src/gui/actions.c res/app.rc)
target_include_directories(atlas_maker PRIVATE res)
target_compile_options(atlas_maker PRIVATE ${WARNINGS})
# Rebuild the resources when the icon or manifest changes; windres doesn't report those dependencies.
set_source_files_properties(res/app.rc PROPERTIES OBJECT_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/favicon.ico;${CMAKE_CURRENT_SOURCE_DIR}/res/app.manifest")
target_link_libraries(atlas_maker PRIVATE atlas_core comctl32 comdlg32)
target_link_options(atlas_maker PRIVATE -municode -static)

enable_testing()
add_executable(test_layout tests/test_layout.c)
target_compile_options(test_layout PRIVATE ${WARNINGS})
target_link_libraries(test_layout PRIVATE atlas_core)
add_test(NAME layout COMMAND test_layout)

add_executable(test_atlas tests/test_atlas.c)
target_compile_options(test_atlas PRIVATE ${WARNINGS})
target_link_libraries(test_atlas PRIVATE atlas_core)
target_link_options(test_atlas PRIVATE -municode)
add_test(NAME atlas COMMAND test_atlas ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)
