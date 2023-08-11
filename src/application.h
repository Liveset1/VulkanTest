#ifndef VULKANTEST_APPLICATION_H
#define VULKANTEST_APPLICATION_H

void initializeEngine(State *state) {
    setupErrorHandling();
    glfwInit();
    createContext(state);
    initializeWindow(state);
    createRenderer(state);
}

int windowShouldClose(State *state) {
    return glfwWindowShouldClose(state->window.handle);
}

void engineLoop(State *state) {
    while (!windowShouldClose(state)) {
        glfwPollEvents();
    }
}

void engineCleanup(State *state) {
    destroyRenderer(state);
    destroyWindow(state);
    destroyContext(state);
}

#endif //VULKANTEST_APPLICATION_H
