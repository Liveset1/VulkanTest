#ifndef VULKANTEST_WINDOW_H
#define VULKANTEST_WINDOW_H

void createSurface(State *state) {
    PANIC(glfwCreateWindowSurface(state->context.instance, state->window.handle, state->config.allocator, &state->window.surface), "Could not create window surface!")
}

VkSurfaceCapabilitiesKHR getSurfaceCapabilities(State *state) {
    VkSurfaceCapabilitiesKHR capabilities;

    PANIC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            state->context.physicalDevice, state->window.surface, &capabilities), "Failed to get physical device surface capabilities!")
    return capabilities;
}

VkSurfaceFormatKHR selectSurfaceFormat(State *state) {
    uint32_t formatCount;
    PANIC(vkGetPhysicalDeviceSurfaceFormatsKHR(
            state->context.physicalDevice, state->window.surface, &formatCount, NULL), "Could not get surface formats!")
    VkSurfaceFormatKHR *formats = malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    PANIC(!formats, "Failed to allocate memory for surface formats")
    PANIC(vkGetPhysicalDeviceSurfaceFormatsKHR(
            state->context.physicalDevice, state->window.surface, &formatCount, formats), "Could not get surface formats!")

    uint32_t formatIndex = 0;
    for (int i = 0; i < formatCount; ++i) {
        VkSurfaceFormatKHR format = formats[i];
        if (format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB) {
            formatIndex = i;
            break;
        }
    }
    VkSurfaceFormatKHR format = formats[formatIndex];
    free(formats);
    return format;
}

VkPresentModeKHR selectSurfacePresentMode(State *state) {
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    uint32_t presentModeCount;
    PANIC(vkGetPhysicalDeviceSurfacePresentModesKHR(
            state->context.physicalDevice, state->window.surface, &presentModeCount, NULL), "Failed to get surface present mode count!")
    VkPresentModeKHR *modes = malloc(presentModeCount * sizeof(VkPresentModeKHR));
    PANIC(!modes, "Failed to allocate memory for surface present modes!")
    PANIC(vkGetPhysicalDeviceSurfacePresentModesKHR(
            state->context.physicalDevice, state->window.surface, &presentModeCount, modes), "Failed to get surface present modes!")

    uint32_t modeIndex = UINT32_MAX;
    for (int i = 0; i < presentModeCount; ++i) {
        VkPresentModeKHR mode = modes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            modeIndex = i;
            break;
        }
    }

    if (modeIndex != UINT32_MAX) {
        presentMode = modes[modeIndex];
    }

    free(modes);
    return presentMode;
}

void getSwapchainImages(State *state) {
    PANIC(vkGetSwapchainImagesKHR(state->context.device, state->window.swapchain.handle, &state->window.swapchain.imageCount, NULL), "Failed to get swap chain image count!")
    state->window.swapchain.images = malloc(state->window.swapchain.imageCount * sizeof(VkImage));
    PANIC(!state->window.swapchain.images, "Failed to allocate memory for Swap chain images!")
    PANIC(vkGetSwapchainImagesKHR(state->context.device, state->window.swapchain.handle, &state->window.swapchain.imageCount, state->window.swapchain.images), "Failed to get swap chain images!")
}

void createSwapchainImageViews(State *state) {
    state->window.swapchain.imageViews = malloc(state->window.swapchain.imageCount * sizeof(VkImageView));
    PANIC(!state->window.swapchain.imageViews, "Failed to allocate memory for Swap chain image views!")

    for (int i = 0; i < state->window.swapchain.imageCount; ++i) {
        PANIC(vkCreateImageView(state->context.device,  &(VkImageViewCreateInfo){
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .format = state->window.swapchain.format,
                .image = state->window.swapchain.images[i],
                .components = (VkComponentMapping) {},
                .subresourceRange = (VkImageSubresourceRange) {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .layerCount = 1,
                        .levelCount = 1
                },
                .viewType = VK_IMAGE_VIEW_TYPE_2D
        }, state->config.allocator, &state->window.swapchain.imageViews[i]), "Failed to create Image %i view!", i)
    }
}

void createSwapchain(State *state) {
    VkSurfaceCapabilitiesKHR capabilities = getSurfaceCapabilities(state);
    VkSurfaceFormatKHR surfaceFormat = selectSurfaceFormat(state);
    VkPresentModeKHR presentMode = selectSurfacePresentMode(state);

    state->window.swapchain.format = surfaceFormat.format;
    state->window.swapchain.colorSpace = surfaceFormat.colorSpace;
    state->window.swapchain.imageExtent = capabilities.currentExtent;

    PANIC(vkCreateSwapchainKHR(state->context.device, &(VkSwapchainCreateInfoKHR){
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = state->window.surface,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &state->context.queueFamily,
            .clipped = true,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = capabilities.currentTransform,
            .imageExtent = state->window.swapchain.imageExtent,
            .imageFormat = state->window.swapchain.format,
            .imageColorSpace = state->window.swapchain.colorSpace,
            .presentMode = presentMode,
            .minImageCount = clamp(3, capabilities.minImageCount, capabilities.maxImageCount ? capabilities.maxImageCount : UINT32_MAX)
    }, state->config.allocator, &state->window.swapchain.handle), "Failed to create Swap chain!"
   )

   getSwapchainImages(state);
   createSwapchainImageViews(state);
}

void initializeWindow(State *state) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if (state->config.windowFullScreen) {
        state->window.windowMonitor = glfwGetPrimaryMonitor();
        // Setting Width and Height to Monitor Resolution
        const GLFWvidmode *mode = glfwGetVideoMode(state->window.windowMonitor);
        state->config.width = mode->width;
        state->config.height = mode->height;
    }
    state->window.handle = glfwCreateWindow(state->config.width, state->config.height, state->config.windowTitle, state->window.windowMonitor, NULL);
    glfwSetWindowUserPointer(state->window.handle, state);
    createSurface(state);
    createSwapchain(state);
}

void destroySwapchainContext(State *state) {
    for (int i = 0; i < state->window.swapchain.imageCount; ++i) {
        vkDestroyImageView(state->context.device, state->window.swapchain.imageViews[i], state->config.allocator);
    }
    free(state->window.swapchain.imageViews);
    free(state->window.swapchain.images);
}

void destroyWindow(State *state) {
    destroySwapchainContext(state);
    vkDestroySwapchainKHR(state->context.device, state->window.swapchain.handle, state->config.allocator);
    vkDestroySurfaceKHR(state->context.instance, state->window.surface, state->config.allocator);
    glfwDestroyWindow(state->window.handle);
}

#endif //VULKANTEST_WINDOW_H
