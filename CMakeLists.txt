cmake_minimum_required(VERSION 3.24)

project(SoftGLRender)

set(CMAKE_CXX_STANDARD 20)
set(TARGET_NAME SoftGLRender)

# enable SIMD
add_definitions("-DSOFTGL_SIMD_OPT")

# add defines for build
if(MSVC)
    add_definitions(-DMSVC_COMPILER)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions(-DCLANG_COMPILER)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_definitions(-DGCC_COMPILER)
endif()

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")
include_directories(
    "${THIRD_PARTY_DIR}/glfw/include"
    "${THIRD_PARTY_DIR}/glad/include"
    "${THIRD_PARTY_DIR}/imgui"
    "${THIRD_PARTY_DIR}/json11"
    "${THIRD_PARTY_DIR}/md5"
    "${THIRD_PARTY_DIR}/stb/include"
    "${THIRD_PARTY_DIR}/vulkan/include"
    "${THIRD_PARTY_DIR}/glslang"
    "${THIRD_PARTY_DIR}/vma"
    "${THIRD_PARTY_DIR}/renderdoc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

find_package(assimp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# glslang
set(SKIP_GLSLANG_INSTALL ON)
set(ENABLE_SPVREMAPPER OFF)
set(ENABLE_GLSLANG_BINARIES OFF)
set(ENABLE_CTEST OFF)
set(ENABLE_PCH OFF)
set(BUILD_TESTING OFF)
set(USE_CCACHE ON)
add_subdirectory("${THIRD_PARTY_DIR}/glslang")

# main src
file(GLOB SOFTGL_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Base/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Software/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/OpenGL/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Vulkan/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Vulkan/SPIRV/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/Shader/GLSL/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/Shader/Software/*.cpp
        )

# imgui src
file(GLOB IMGUI_SRC
        ${THIRD_PARTY_DIR}/imgui/imgui/*.cpp
        )

add_executable(${TARGET_NAME}
        "${SOFTGL_SRC}"
        "${IMGUI_SRC}"
        "${THIRD_PARTY_DIR}/glad/src/glad.c"
        "${THIRD_PARTY_DIR}/json11/json11.cpp"
        "${THIRD_PARTY_DIR}/md5/md5.c"
        )

set(LINK_LIBS
        assimp::assimp
        glslang
        glslang-default-resource-limits
        SPIRV
        )

if (WIN32)
    set(LINK_LIBS ${LINK_LIBS}
            "${THIRD_PARTY_DIR}/vulkan/lib-win-x64/vulkan-1.lib"
            )
    if (MSVC)
        set(LINK_LIBS ${LINK_LIBS}
                "${THIRD_PARTY_DIR}/glfw/lib-vc2022/glfw3.lib"
                "${THIRD_PARTY_DIR}/glfw/lib-vc2022/glfw3dll.lib"
                )
    else ()
        set(LINK_LIBS ${LINK_LIBS}
                "${THIRD_PARTY_DIR}/glfw/lib-mingw-w64/libglfw3.a"
                "${THIRD_PARTY_DIR}/glfw/lib-mingw-w64/libglfw3dll.a"
                )
    endif ()
endif ()

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit")
    add_compile_definitions(GL_SILENCE_DEPRECATION)
    set(LINK_LIBS ${LINK_LIBS}
            "${THIRD_PARTY_DIR}/vulkan/lib-macos-universal/libvulkan.1.dylib"
            "${THIRD_PARTY_DIR}/glfw/lib-macos-universal/libglfw3.a"
            )
endif ()

if (UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    set(LINK_LIBS ${LINK_LIBS}
            pthread
            glfw
            OpenGL::GL
            "${THIRD_PARTY_DIR}/vulkan/lib-linux-x64/libvulkan.so.1"
            ${CMAKE_DL_LIBS}
            )
endif ()

# if (MSVC)
#     target_compile_options(${TARGET_NAME} PRIVATE $<$<BOOL:${MSVC}>:/arch:AVX2 /std:c++11>)
# else ()
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -O3")
# endif ()

target_link_libraries(${TARGET_NAME} ${LINK_LIBS})

# copy assets
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${TARGET_NAME}>/assets

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/Shader/GLSL $<TARGET_FILE_DIR:${TARGET_NAME}>/shaders/GLSL

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/cache $<TARGET_FILE_DIR:${TARGET_NAME}>/cache

    COMMAND ${CMAKE_COMMAND} -E copy
    ${THIRD_PARTY_DIR}/renderdoc/renderdoc.dll $<TARGET_FILE_DIR:${TARGET_NAME}>/renderdoc.dll
    )
