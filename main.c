#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

#include <cglm/cglm.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#define PANIC(ERROR, FORMAT, ...) {if (ERROR) { fprintf(stderr, "%s -> %s -> %i -> Error(%i)" FORMAT, __FILE_NAME__, __FUNCTION__, __LINE__, ERROR, ##__VA_ARGS__); raise(SIGABRT); }}

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
    GLFWwindow *window;
    GLFWmonitor *window_monitor;

    char **window_title;
    int width, height;
    bool resizable;
    bool window_fullscreen;

    VkAllocationCallbacks allocator;
    VkInstance instance;
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
    state->window = glfwCreateWindow(state->width, state->height, (const char*) state->window_title, state->window_monitor, NULL);
}

void createVKInstance(State *state) {

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
    state->window = NULL;
}

int main() {
    State state = {
            .window_title = (char**)"Vulkan Test",
            .width = 800,
            .height = 600,
            .resizable = true,
            .window_fullscreen = false,
    };

    initializeEngine(&state);
    engineLoop(&state);
    engineCleanup(&state);

    return 0;
}
