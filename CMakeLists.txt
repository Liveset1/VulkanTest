cmake_minimum_required(VERSION 3.26)
project(VulkanTest C)

if (WIN32)
    if (NOT MSVC)
        list(APPEND CMAKE_PREFIX_PATH "C:\\Program Files (x86)")
    endif()
endif()
set(CMAKE_C_STANDARD 11)

find_package(Vulkan REQUIRED)

add_executable(VulkanTest main.c)

target_include_directories(${PROJECT_NAME} PUBLIC
        external/cglm/include
        external/glfw/include
        ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${VULKAN_LIBRARIES} glfw)

add_subdirectory(external/cglm)
add_subdirectory(external/glfw)