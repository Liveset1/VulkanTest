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
    const char *window_title;
    int width, height;
    bool resizable;
    bool window_fullscreen;

    // GLFW
    GLFWwindow *window;
    GLFWmonitor *window_monitor;

    // Vulkan
    VkAllocationCallbacks *allocator;
    VkInstance instance;
    uint32_t api_version;

} State;

void setupErrorHandling() {
    glfwSetErrorCallback(glfwErrorCallback);
    atexit(exitCallback);
}

void initializeWindow(State *state) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, state->resizable);

    if (state->window_fullscreen) {
        state->window_monitor = glfwGetPrimaryMonitor();
    }
    state->window = glfwCreateWindow(state->width, state->height, state->window_title, state->window_monitor, NULL);
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

void initializeEngine(State *state) {
    setupErrorHandling();
    initializeWindow(state);
    createVKInstance(state);
}

void engineLoop(State *state) {
    while (!glfwWindowShouldClose(state->window)) {
        glfwPollEvents();
    }
}

void engineCleanup(State *state) {
    glfwDestroyWindow(state->window);
    vkDestroyInstance(state->instance, state->allocator);
}

int main() {
    const char *name = "Vulkan Test";
    State state = {
            .applicationName = name,
            .engineName = name,
            .window_title = name,
            .width = 800,
            .height = 600,
            .resizable = true,
            .window_fullscreen = false,
            .api_version = VK_API_VERSION_1_0,
    };

    initializeEngine(&state);
    engineLoop(&state);
    engineCleanup(&state);

    return 0;
}
