#ifndef VULKANTEST_CONTEXT_H
#define VULKANTEST_CONTEXT_H

void createVKInstance(State *state) {
    uint32_t extensions_count;
    const char **extensions = glfwGetRequiredInstanceExtensions(&extensions_count);

    VkApplicationInfo applicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .apiVersion = state->config.api_version,
            .pApplicationName = state->config.applicationName,
            .pEngineName = state->config.engineName
    };

    VkInstanceCreateInfo instanceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &applicationInfo,
            .enabledExtensionCount = extensions_count,
            .ppEnabledExtensionNames = extensions,
    };
    PANIC(vkCreateInstance(&instanceCreateInfo, state->config.allocator, &state->context.instance), "Could not create VK Instance!")
}

void selectPhysicalDevice(State *state) {
    uint32_t count;
    PANIC(vkEnumeratePhysicalDevices(state->context.instance, &count, NULL), "Could not enumerate physical devices count!")
    PANIC(count == 0, "Could not find a vulkan supported physical device!")
    VkResult result = vkEnumeratePhysicalDevices(state->context.instance, &count, &state->context.physicalDevice);
    if(result != VK_INCOMPLETE) {PANIC(result, "Could not enumerate physical devices count!")}
}

void selectQueueFamily(State *state) {
    state->context.queueFamily = UINT32_MAX;

    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(state->context.physicalDevice, &count, NULL);
    VkQueueFamilyProperties *queueFamilies = malloc(count*sizeof(VkQueueFamilyProperties));
    PANIC(queueFamilies == NULL, "Could not allocate memory for queue families!")
    vkGetPhysicalDeviceQueueFamilyProperties(state->context.physicalDevice, &count, queueFamilies);

    for (int queueFamilyIndex = 0; queueFamilyIndex < count; ++queueFamilyIndex) {
        VkQueueFamilyProperties properties = queueFamilies[queueFamilyIndex];
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT
            && glfwGetPhysicalDevicePresentationSupport(state->context.instance, state->context.physicalDevice, queueFamilyIndex)) {
            state->context.queueFamily = queueFamilyIndex;
            break;
        }
    }
    PANIC(state->context.queueFamily == UINT32_MAX, "Could not find a suitable Queue Family!")
    free(queueFamilies);
}

void createVKDevice(State *state) {
    PANIC(vkCreateDevice(state->context.physicalDevice, &(VkDeviceCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pQueueCreateInfos =  &(VkDeviceQueueCreateInfo) {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = state->context.queueFamily,
                    .queueCount = 1,
                    .pQueuePriorities = &(float){1.0},
            },
            .queueCreateInfoCount = 1,
            .enabledExtensionCount = 1,
            .ppEnabledExtensionNames = &(const char*) {VK_KHR_SWAPCHAIN_EXTENSION_NAME}
    }, state->config.allocator, &state->context.device), "Could not create VK Device and Queues!")
}

void getVKQueue(State *state) {
    vkGetDeviceQueue(state->context.device, state->context.queueFamily, 0, &state->context.queue);
}

void createContext(State *state) {
    createVKInstance(state);
    selectPhysicalDevice(state);
    selectQueueFamily(state);
    createVKDevice(state);
    getVKQueue(state);
}

void destroyContext(State *state) {
    vkDestroyDevice(state->context.device, state->config.allocator);
    vkDestroyInstance(state->context.instance, state->config.allocator);
}

#endif //VULKANTEST_CONTEXT_H
