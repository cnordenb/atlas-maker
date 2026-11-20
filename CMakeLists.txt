cmake_minimum_required(VERSION 3.20)
project(atlas_maker LANGUAGES C RC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

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

add_library(atlas_core STATIC src/core/atlas.c src/core/image.c src/core/layout.c)
target_include_directories(atlas_core PUBLIC src/core)
target_compile_definitions(atlas_core PUBLIC ${DEFINES})
target_compile_options(atlas_core PRIVATE ${WARNINGS})
target_link_libraries(atlas_core PUBLIC webp windowscodecs ole32 uuid)

add_executable(atlas_maker WIN32 src/gui/main.c src/gui/window.c src/gui/actions.c res/app.rc)
target_include_directories(atlas_maker PRIVATE res)
target_compile_options(atlas_maker PRIVATE ${WARNINGS})
set_source_files_properties(res/app.rc PROPERTIES OBJECT_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/favicon.ico;${CMAKE_CURRENT_SOURCE_DIR}/res/app.manifest")
target_link_libraries(atlas_maker PRIVATE atlas_core comctl32 comdlg32)
target_link_options(atlas_maker PRIVATE -municode -static)

enable_testing()
add_executable(run_tests tests/run_tests.c tests/test_layout.c tests/test_atlas.c tests/test_rules.c)
target_compile_options(run_tests PRIVATE ${WARNINGS})
target_link_libraries(run_tests PRIVATE atlas_core)
target_link_options(run_tests PRIVATE -municode)
add_test(NAME tests COMMAND run_tests ${CMAKE_CURRENT_SOURCE_DIR})
