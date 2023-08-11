#ifndef VULKANTEST_HEADERS_H
#define VULKANTEST_HEADERS_H

#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

#include "cglm/cglm.h"
#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"

typedef struct {
    const char *applicationName;
    const char *engineName;
    const char *windowTitle;

    int width, height;
    bool windowFullScreen;

    uint32_t api_version;
    VkAllocationCallbacks *allocator;
    VkComponentMapping swapchainComponentsMapping;
    uint32_t swapchainBuffering;
    VkClearValue backgroundColor;
} Config;

typedef struct {
    // Vulkan
    VkInstance instance;

    uint32_t queueFamily;

    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue queue;
} Context;

typedef struct {
    VkSwapchainKHR handle;

    uint32_t imageCount;
    VkImage *images;
    VkImageView *imageViews;

    VkFormat format;
    VkColorSpaceKHR colorSpace;
    VkExtent2D imageExtent;
    uint32_t imageAcquiredIndex;
} Swapchain;

typedef struct {
    // GLFW
    GLFWwindow *handle;
    GLFWmonitor *windowMonitor;
    VkSurfaceKHR surface;
    Swapchain swapchain;
} Window;

typedef struct {

} Renderer;

// State Struct holds all info about the program
typedef struct {
    Config config;
    Context context;
    Window window;
    Renderer renderer;
} State;

enum SwapchainBuffering {
    SWAPCHAIN_DOUBLE_BUFFERING = 2,
    SWAPCHAIN_TRIPLE_BUFFERING = 3,
};

#endif //VULKANTEST_HEADERS_H
