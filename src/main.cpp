#include "UI.h"
#include "Scene.h"
#include "Engine.h"
#include "Events.h"
#include "SwapChain.h"
#include <nlohmann/json.hpp>


void Build(ApplicationInfo& application, std::string sceneName, int numLights, int lightCullingApproach, int selectedTileResolution);
void Launch(ApplicationInfo& application);
void Destroy(ApplicationInfo& application);


int main(int argc, char* argv[])
{
    int numLights = 64;
    std::string sceneName = "sibenik"; // "sponza
    int lightCullingApproach = GATHER; // MATRIX_SCATTER - ATOMIC_SCATTER
    int selectedTileResolution = _16x16; // _8x8 - _32x32
    
    ApplicationInfo application;
    Build(application, sceneName, numLights, lightCullingApproach, selectedTileResolution);
    Launch(application);
    Destroy(application);
    
    return 0;
}

void Build(ApplicationInfo& application, std::string sceneName, int numLights, int lightCullingApproach, int selectedTileResolution)
{
    Engine::Build(application.engine, application.window);
    UI::Build(application.ui, application.engine, application.window);
    Scene::Build(application.scene, application.engine, sceneConfigs[sceneName], numLights, lightCullingApproach, selectedTileResolution);
}

void Launch(ApplicationInfo& application)
{
    while(!glfwWindowShouldClose(application.window.handle)){
        glfwPollEvents();
        Events::Handle(application.window.handle, application.scene);
        application.engine.imageIndex = SwapChain::AcquireNextImage(application.engine, application.window, application.frame);
        
        if(application.engine.imageIndex == -1)
            continue;
        
        Scene::Record(application.scene, application.engine, application.frame);
        UI::Record(application.ui, application.scene, application.window, application.engine, application.frame);
        Scene::Submit(application.scene, application.engine, application.frame);
        UI::Submit(application.ui, application.scene, application.window, application.frame, application.engine);
        
        if(!SwapChain::PresentImage(application.engine, application.ui.renderFinishedSemaphores[application.frame]))
            continue;
        
        application.frame = (application.frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}

void Destroy(ApplicationInfo& application)
{
    vkDeviceWaitIdle(application.engine.logicalDevice) ;
    glfwSetWindowUserPointer(application.window.handle, nullptr);
    glfwDestroyWindow(application.window.handle);
    vkDestroyDevice(application.engine.logicalDevice, NULL) ;
    vkDestroySurfaceKHR(application.engine.vulkanInstance, application.engine.windowSurface, NULL);
    vkDestroyInstance(application.engine.vulkanInstance, NULL);
    glfwTerminate();
}
