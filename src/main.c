#include "toolkit.h"

int main() {
    const char *name = "Vulkan Test";
    State state = {
            .config = {
                    .applicationName = name,
                    .engineName = name,
                    .windowTitle = name,
                    .width = 800,
                    .height = 600,
                    .windowFullScreen = false,
                    .api_version = VK_API_VERSION_1_0,
                    .swapchainComponentsMapping = (VkComponentMapping) {},
                    .swapchainBuffering = SWAPCHAIN_TRIPLE_BUFFERING,
                    .backgroundColor = {{0.01f, 0.01f, 0.01f, 0.01f}}
            },
    };

    initializeEngine(&state);
    engineLoop(&state);
    engineCleanup(&state);

    return 0;
}
