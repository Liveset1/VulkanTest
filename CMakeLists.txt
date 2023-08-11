cmake_minimum_required(VERSION 3.26)
project(VulkanTest C)

if (WIN32)
    if (NOT MSVC)
        list(APPEND CMAKE_PREFIX_PATH "C:\\Program Files (x86)")
    endif()
endif()
set(CMAKE_C_STANDARD 11)

find_package(Vulkan REQUIRED)

add_executable(VulkanTest src/main.c
        src/utils.h
        src/toolkit.h
        src/headers.h
        src/context.h
        src/window.h
        src/application.h
        src/renderer.h)

target_include_directories(${PROJECT_NAME} PUBLIC
        lib/cglm/include
        lib/glfw/include
        ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)

add_subdirectory(lib/cglm)
add_subdirectory(lib/glfw)