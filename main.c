#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

#include <cglm/cglm.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#define PANIC(ERROR, FORMAT, ...) {if (ERROR) { fprintf(stderr, "%s -> %s -> %i -> Error(%i)" FORMAT "\n", __FILE_NAME__, __FUNCTION__, __LINE__, ERROR, ##__VA_ARGS__); raise(SIGABRT); }}

// GLFW Error Callback
void glfwErrorCallback(int error_code, const char *description) {
    PANIC(error_code, "GLFW Error Callback: %s", description);
}

// Exit Callback
void exitCallback() {
    glfwTerminate();
}

// State Struct holds all info about the program
typedef struct {
    // Window Properties / Configurable Values
    const char *applicationName;
    const char *engineName;
    const char *windowTitle;
    int width, height;
    bool resizable;
    bool windowFullScreen;

    // GLFW
    GLFWwindow *window;
    GLFWmonitor *windowMonitor;

    // Vulkan
    VkAllocationCallbacks *allocator;
    VkInstance instance;
    uint32_t api_version;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
    uint32_t queueFamily;
    VkDevice device;
    VkQueue queue;
} State;

void setupErrorHandling() {
    glfwSetErrorCallback(glfwErrorCallback);
    atexit(exitCallback);
}

void initializeWindow(State *state) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, state->resizable);

    if (state->windowFullScreen) {
        state->windowMonitor = glfwGetPrimaryMonitor();
        // Setting Width and Height to Monitor Resolution
        GLFWvidmode *mode = glfwGetVideoMode(state->windowMonitor);
        state->width = mode->width;
        state->height = mode->height;
    }
    state->window = glfwCreateWindow(state->width, state->height, state->windowTitle, state->windowMonitor, NULL);
}

void createVKInstance(State *state) {
    uint32_t extensions_count;
    const char **extensions = glfwGetRequiredInstanceExtensions(&extensions_count);

    VkApplicationInfo applicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .apiVersion = state->api_version,
            .pApplicationName = state->applicationName,
            .pEngineName = state->engineName
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &applicationInfo,
        .enabledExtensionCount = extensions_count,
        .ppEnabledExtensionNames = extensions,
    };
    PANIC(vkCreateInstance(&instanceCreateInfo, state->allocator, &state->instance), "Could not create VK Instance!");
}

void selectPhysicalDevice(State *state) {
    uint32_t count;
    PANIC(vkEnumeratePhysicalDevices(state->instance, &count, NULL), "Could not enumerate physical devices count!");
    PANIC(count == 0, "Could not find a vulkan supported physical device!");
    PANIC(vkEnumeratePhysicalDevices(state->instance, &count, &state->physicalDevice), "Could not enumerate physical devices count!");
}

void createSurface(State *state) {
    PANIC(glfwCreateWindowSurface(state->instance, state->window, state->allocator, &state->surface), "Could not create window surface!");
}

void selectQueueFamily(State *state) {
    state->queueFamily = UINT32_MAX;

    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(state->physicalDevice, &count, NULL);
    VkQueueFamilyProperties *queueFamilies = malloc(count*sizeof(VkQueueFamilyProperties));
    PANIC(queueFamilies == NULL, "Could not allocate memory for queue families!")
    vkGetPhysicalDeviceQueueFamilyProperties(state->physicalDevice, &count, queueFamilies);

    for (int queueFamilyIndex = 0; queueFamilyIndex < count; ++queueFamilyIndex) {
        VkQueueFamilyProperties properties = queueFamilies[queueFamilyIndex];
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT
        && glfwGetPhysicalDevicePresentationSupport(state->instance, state->physicalDevice, queueFamilyIndex)) {
            state->queueFamily = queueFamilyIndex;
            break;
        }
    }
    PANIC(state->queueFamily == UINT32_MAX, "Could not find a suitable Queue Family!");
    free(queueFamilies);
}

void createVKDevice(State *state) {
    PANIC(vkCreateDevice(state->physicalDevice, &(VkDeviceCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
       .pQueueCreateInfos =  &(VkDeviceQueueCreateInfo) {
                  .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                  .queueFamilyIndex = state->queueFamily,
                  .queueCount = 1,
                  .pQueuePriorities = &(float){1.0},
        },
        .queueCreateInfoCount = 1,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = &(const char*) {VK_KHR_SWAPCHAIN_EXTENSION_NAME}
    }, state->allocator, &state->device), "Could not create VK Device and Queues!");
}

void getVKQueue(State *state) {
    vkGetDeviceQueue(state->device, state->queueFamily, 0, &state->queue);
}

void initializeEngine(State *state) {
    setupErrorHandling();
    initializeWindow(state);
    createVKInstance(state);

    selectPhysicalDevice(state);
    createSurface(state);
    selectQueueFamily(state);
    createVKDevice(state);
    getVKQueue(state);
}

void engineLoop(State *state) {
    while (!glfwWindowShouldClose(state->window)) {
        glfwPollEvents();
    }
}

void engineCleanup(State *state) {
    vkDestroyDevice(state->device, state->allocator);
    vkDestroySurfaceKHR(state->instance, state->surface, state->allocator);
    glfwDestroyWindow(state->window);
    vkDestroyInstance(state->instance, state->allocator);
}

int main() {
    const char *name = "Vulkan Test";
    State state = {
            .applicationName = name,
            .engineName = name,
            .windowTitle = name,
            .width = 800,
            .height = 600,
            .resizable = true,
            .windowFullScreen = false,
            .api_version = VK_API_VERSION_1_0,
    };

    initializeEngine(&state);
    engineLoop(&state);
    engineCleanup(&state);

    return 0;
}
