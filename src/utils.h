#ifndef VULKANTEST_UTILS_H
#define VULKANTEST_UTILS_H

#define PANIC(ERROR, FORMAT, ...) { \
    int errCode;\
    if ((errCode = ERROR)) {\
        fprintf(stderr, "%s -> %s -> %i -> Error(%i)" FORMAT "\n", __FILE_NAME__, __FUNCTION__, __LINE__, errCode, ##__VA_ARGS__); raise(SIGABRT);\
    }\
}

void infoLog() {
    uint32_t instanceApiVersion;
    PANIC(vkEnumerateInstanceVersion(&instanceApiVersion), "Couldn't enumerate instance version")
    uint32_t apiVersionVariant = VK_API_VERSION_VARIANT(instanceApiVersion);
    uint32_t apiVersionMajor = VK_API_VERSION_MAJOR(instanceApiVersion);
    uint32_t apiVersionMinor = VK_API_VERSION_MINOR(instanceApiVersion);
    uint32_t apiVersionPatch = VK_API_VERSION_PATCH(instanceApiVersion);
    printf("Vulkan API %i.%i.%i.%i\n", apiVersionVariant, apiVersionMajor, apiVersionMinor, apiVersionPatch);
    printf("GLFW %s\n", glfwGetVersionString());
}

// Exit Callback
void exitCallback() {
    glfwTerminate();
}

// GLFW Error Callback
void glfwErrorCallback(int error_code, const char *description) {
    PANIC(error_code, "GLFW Error Callback: %s", description)
}

void setupErrorHandling() {
    glfwSetErrorCallback(glfwErrorCallback);
    atexit(exitCallback);
}

uint32_t clamp(uint32_t value, uint32_t min, uint32_t max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    }
    return value;
}

#endif //VULKANTEST_UTILS_H
