#pragma once
#include "Engine.h"
#include "Scene.h"
#include "Light.h"



class UI{
    
private:
    
    static bool RenderDropDownList(char * label, int& selectedIndex, std::vector<const char *> names, float width)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - width) * 0.5f);
        ImGui::SetNextItemWidth(width);
        ImGui::AlignTextToFramePadding();
        
        return ImGui::Combo(label, &selectedIndex, names.data(), (int)names.size());
    }
    
    static void RenderText(std::string text, float width)
    {
        ImGui::SetCursorPosX(((ImGui::GetColumnWidth() - ImGui::CalcTextSize(text.c_str()).x - width) * 0.5f) + ImGui::GetCursorPosX());
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(text.c_str());
    }
    
    static void RenderCheckBox(char * label, float width, uint32_t& flag, uint32_t bit, uint32_t disable)
    {
        bool enabled = (flag & bit) != 0;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetColumnWidth() - width) * 0.5f));
        ImGui::AlignTextToFramePadding();
        if(ImGui::Checkbox(label, &enabled)){
            if(enabled){
                flag &= ~disable;
                flag |= bit;
            }
            else
                flag &= ~bit;
        }
    }
    
public:
    
    static void Build(UIInfo& ui, EngineInfo& engine, WindowInfo& window)
    {
        ui.clearValues = std::vector<VkClearValue>{
            VkClearValue{
                .color = {0.0f, 0.0f, 0.0f, 1.0f}
            },
            VkClearValue{
                .depthStencil = {1.0f, 0}
            }
        };
        
        ui.viewport = VkViewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(engine.swapChain.extent.width),
            .height = static_cast<float>(engine.swapChain.extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        
        ui.scissor = VkRect2D{
            .offset = { 0, 0 },
            .extent = engine.swapChain.extent
        };
        
        VkSubpassDependency subpassDependency{};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        
        VkAttachmentDescription attachmentDescription{};
        attachmentDescription.format = engine.swapChain.swapChainSurface.format;
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        VkSubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        VkAttachmentReference colorAttachmentReference{};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        subpassDescription.pColorAttachments = &colorAttachmentReference;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pDepthStencilAttachment = nullptr;
        
        VkRenderPassCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO ;
        info.attachmentCount = 1;
        info.pAttachments = &attachmentDescription;
        info.pSubpasses = &subpassDescription;
        info.subpassCount = 1;
        info.pDependencies = &subpassDependency;
        info.dependencyCount = 1;
        if(vkCreateRenderPass(engine.logicalDevice, &info, nullptr, &ui.renderPass) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE RENDER PASS");
        
        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = engine.physicalDevice.queueIndex;
        VkCommandPool commandPool;
        if(vkCreateCommandPool(engine.logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE COMMAND POOL\n");
        
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = commandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
        ui.commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        if(vkAllocateCommandBuffers(engine.logicalDevice, &commandBufferAllocateInfo, ui.commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO ALLOCATE COMMAND BUFFERS\n");
        
        ui.inflightFences.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.pNext = nullptr;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            if(vkCreateFence(engine.logicalDevice, &fenceCreateInfo, nullptr, &ui.inflightFences[i]) != VK_SUCCESS)
                throw std::runtime_error("\nFAILURE TO CREATE FENCES\n");
        }
        
        ui.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT) ;
        for( int i = 0 ; i < MAX_FRAMES_IN_FLIGHT ; i++ ){
            VkSemaphoreCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if(vkCreateSemaphore(engine.logicalDevice, &info, nullptr, &ui.renderFinishedSemaphores[i]) != VK_SUCCESS)
                throw std::runtime_error("\nFAILURE TO CREATE SEMAPHORES");
        }

        uint32_t numSwapChainImages = engine.swapChain.imageViews.size();
        ui.frameBuffers.resize(numSwapChainImages);
        for(size_t i = 0; i < numSwapChainImages; i++){
            VkFramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = ui.renderPass;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = &engine.swapChain.imageViews[i];
            framebufferCreateInfo.width = engine.swapChain.extent.width;
            framebufferCreateInfo.height = engine.swapChain.extent.height;
            framebufferCreateInfo.layers = 1;
            if(vkCreateFramebuffer(engine.logicalDevice, &framebufferCreateInfo, nullptr, &ui.frameBuffers[i]) != VK_SUCCESS)
                throw std::runtime_error("\nFAILURE TO CREATE FRAMEBUFFERS");
        }
        
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        
        static const ImWchar ranges[] = {0x0020, 0x00FF, 0x2190, 0x21FF, 0};
        io.Fonts->AddFontFromFileTTF("../Resources/assets/Fonts/EpundaSans.ttf", 25.0f, nullptr, ranges);
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard ;
        ImGui::StyleColorsDark();

        ImGui::GetIO().IniFilename = nullptr;
        ImGui_ImplGlfw_InitForVulkan(window.handle, true) ;

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = engine.vulkanInstance;
        initInfo.PhysicalDevice = engine.physicalDevice.handle;
        initInfo.Device = engine.logicalDevice;
        initInfo.Queue = engine.graphicsQueue;
        initInfo.DescriptorPool = nullptr;
        initInfo.DescriptorPoolSize = 1000;
        initInfo.MinImageCount = engine.swapChain.imageCount;
        initInfo.ImageCount = engine.swapChain.imageCount;
        initInfo.UseDynamicRendering = false;
        initInfo.PipelineInfoMain.RenderPass = ui.renderPass;
        initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        if(!ImGui_ImplVulkan_Init(&initInfo))
            throw std::runtime_error("\nFAILURE TO INITIALIZE IMGUI VULKAN BACKEND\n");
    }
    
    static void Record(UIInfo& ui, SceneInfo& scene, WindowInfo& window, EngineInfo& engine, uint32_t frame)
    {
        vkWaitForFences(engine.logicalDevice, 1, &ui.inflightFences[frame], VK_TRUE, UINT64_MAX);
        
        vkResetFences(engine.logicalDevice, 1, &ui.inflightFences[frame]) ;
        
        if(vkResetCommandBuffer(ui.commandBuffers[frame], 0) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO RESET COMMAND BUFFER\n");
        
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;
        if(vkBeginCommandBuffer(ui.commandBuffers[frame], &commandBufferBeginInfo) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO BEGIN RECORDING COMMAND BUFFER\n");
        
        VkRenderPassBeginInfo info{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = ui.renderPass,
            .framebuffer = ui.frameBuffers[frame],
            .renderArea.offset = {0, 0},
            .renderArea.extent = engine.swapChain.extent,
            .clearValueCount = static_cast<uint32_t>(ui.clearValues.size()),
            .pClearValues = ui.clearValues.data()
        };
        vkCmdBeginRenderPass(ui.commandBuffers[frame], &info, VK_SUBPASS_CONTENTS_INLINE);
        
        vkCmdSetViewport(ui.commandBuffers[frame], 0, 1, &ui.viewport);
        
        vkCmdSetScissor(ui.commandBuffers[frame], 0, 1, &ui.scissor) ;
        
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        RenderPerformancePanel(scene, window.windowWidth, window.windowHeight, frame);
        RenderLightPanel(engine, scene, window.windowWidth, window.windowHeight);
        RenderScenePanel(engine, scene, window.windowWidth, window.windowHeight);
        
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), ui.commandBuffers[frame]);
        
        vkCmdEndRenderPass(ui.commandBuffers[frame]);
        
        if(vkEndCommandBuffer(ui.commandBuffers[frame]) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO END RECORDING COMMAND BUFFER\n");
    }
            
    static void RenderLightPanel(EngineInfo& engine, SceneInfo& scene, int windowWidth, int windowHeight)
    {
        LightInfo& light = scene.lights[scene.selectedLight];
        
        ImGui::SetNextWindowSizeConstraints(ImVec2(500, 90), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::SetNextWindowPos(ImVec2(windowWidth, windowHeight), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
        ImGui::Begin("Light", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        if(ImGui::BeginTable("Table", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg)){
            ImGui::TableNextRow();
            
            ImGui::TableSetColumnIndex(0);
            
            RenderText("NumLights", 0.0f);
            
            ImGui::TableSetColumnIndex(1);
            
            std::string numLights = std::to_string(scene.lights.size());
            RenderText(numLights, 0.0f);
                        
            ImGui::EndTable();
        }
        
        ImGui::End();
    }
        
    static void RenderScenePanel(EngineInfo& engine, SceneInfo& scene, int windowWidth, int windowHeight)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(600, 450), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::SetNextWindowPos(ImVec2(windowWidth, 0), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        if(ImGui::BeginTable("Table", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg)){
            
            ImGui::TableNextRow();
            
            ImGui::TableSetColumnIndex(0);
            
            RenderText("Scene", 0.0f);
            
            ImGui::Separator();
            
            RenderText("Display Albedo", 0.0f);
            RenderText("Display Normal", 0.0f);
            RenderText("Display Depth", 0.0f);
            if((scene.model.flags & FORWARD_PLUS_PIPELINE) != 0u)
                RenderText("Display Heat Map", 0.0f);

            ImGui::Separator();
            
            RenderText("Forward Rendering Pipeline", 0.0f);
            RenderText("Forward+ Rendering Pipeline", 0.0f);
            ImGui::Separator();
            
            if((scene.model.flags & FORWARD_PLUS_PIPELINE) != 0u){
                RenderText("Tile Resolution", 0.0f);
                RenderText("Light Culling Approach", 0.0f);
                
                ImGui::Separator();
                
                RenderText("2.5D Light Culling", 0.0f);
                
                if((scene.model.flags & LIGHT_CULLING_25D) != 0u){
                    RenderText("Depth Slicing", 0.0f);
                    RenderText("Cells Per Tile", 0.0f);
                }
                ImGui::Separator();
                
                RenderText("Stochastic Light Culling", 0.0f);
                if((scene.model.flags & STOCHASTIC_LIGHT_CULLING) != 0u)
                    RenderText("Maximum Error", 0.0f);
            }
            
            ImGui::TableSetColumnIndex(1);
            
            RenderText(scene.config.name, 0.0f);
            
            ImGui::Separator();
            
            RenderCheckBox("##displayAlbedo", 50, scene.model.flags, SHOW_ALBEDO, ~SHOW_ALBEDO);
            RenderCheckBox("##displayNormal", 50, scene.model.flags, SHOW_NORMAL, ~SHOW_NORMAL);
            RenderCheckBox("##displayDepth", 50, scene.model.flags, SHOW_DEPTH, ~SHOW_DEPTH);
            
            if((scene.model.flags & FORWARD_PLUS_PIPELINE) != 0u)
                RenderCheckBox("##displayHeatMap", 50, scene.model.flags, SHOW_HEAT_MAP, ~(SHOW_HEAT_MAP | FORWARD_PLUS_PIPELINE | DEPTH_SLICING_LINEAR | DEPTH_SLICING_LOGARITHMIC | LIGHT_CULLING_25D | STOCHASTIC_LIGHT_CULLING));

            ImGui::Separator();
                        
            RenderCheckBox("##forwardPipelien", 50, scene.model.flags, FORWARD_PIPELINE, ~FORWARD_PIPELINE);
            RenderCheckBox("##forward+Pipelien", 50, scene.model.flags, FORWARD_PLUS_PIPELINE, ~FORWARD_PLUS_PIPELINE);
            ImGui::Separator();

            if((scene.model.flags & FORWARD_PLUS_PIPELINE) != 0u){
                
                RenderText(tileResolutionStrings[scene.selectedTileResolution], 0);
                
                RenderText(lightCullingApproaches[scene.lightCullingApproach], 0.0f);
                                
                ImGui::Separator();
                
                RenderCheckBox("##25DLightCulling", 50, scene.model.flags, LIGHT_CULLING_25D, ~(LIGHT_CULLING_25D | STOCHASTIC_LIGHT_CULLING | FORWARD_PLUS_PIPELINE | DEPTH_SLICING_LINEAR | DEPTH_SLICING_LOGARITHMIC | SHOW_HEAT_MAP));
                
                if((scene.model.flags & LIGHT_CULLING_25D) != 0u){
                    RenderText("Linear", 200.0f);
                    ImGui::SameLine();
                    RenderCheckBox("##depthSlicingLinear", 200, scene.model.flags, DEPTH_SLICING_LINEAR, ~(SHOW_HEAT_MAP | FORWARD_PLUS_PIPELINE | DEPTH_SLICING_LINEAR));
                    ImGui::SameLine();
                    RenderText("Logarithmic", 45.0f);
                    ImGui::SameLine();
                    RenderCheckBox("##depthSlicingLog", 50, scene.model.flags, DEPTH_SLICING_LOGARITHMIC, ~(SHOW_HEAT_MAP | FORWARD_PLUS_PIPELINE | DEPTH_SLICING_LOGARITHMIC));
                    
                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                    
                    if(ImGui::SliderScalar("##numCellsPerTile", ImGuiDataType_U32, &scene.numCellsPerTile, &scene.minNumCellsPerTile, &scene.maxNumCellsPerTile)){
                        scene.lightCullingGather.uniformData.numCellsPerTile = scene.numCellsPerTile;
                        scene.lightCullingMatrixScatter.uniformData.numCellsPerTile = scene.numCellsPerTile;
                        scene.lightCullingMatrixScatterPrepass.uniformData.numCellsPerTile = scene.numCellsPerTile;
                        scene.lightCullingAtomicScatter.uniformData.numCellsPerTile = scene.numCellsPerTile;
                        scene.lightCullingAtomicScatterPrepass.uniformData.numCellsPerTile = scene.numCellsPerTile;
                        scene.model.fsData.numCellsPerTile = scene.numCellsPerTile;
                    }
                }
                
                ImGui::Separator();
                
                RenderCheckBox("##stochastic", 50, scene.model.flags, STOCHASTIC_LIGHT_CULLING, ~(STOCHASTIC_LIGHT_CULLING | LIGHT_CULLING_25D | FORWARD_PLUS_PIPELINE | DEPTH_SLICING_LINEAR | DEPTH_SLICING_LOGARITHMIC | SHOW_HEAT_MAP));
                
                if((scene.model.flags & STOCHASTIC_LIGHT_CULLING) != 0u){
                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                    if(ImGui::SliderFloat("##stochasticquality", &scene.stochasticQualityValue, 0.01f, 1.0f, "%.2f"))
                        Light::UpdateErrorMax(scene);
                }
            }
            
            scene.model.fsData.flags = scene.model.flags;
            
            scene.lightCullingGather.uniformData.flags = scene.model.flags;
            scene.lightCullingGather.uniformData.flags = scene.model.flags;
            
            ImGui::EndTable();
        }
        
        ImGui::End();
    }
    
    static void RenderPerformancePanel(SceneInfo& scene, int windowWidth, int windowHeight, uint32_t frame)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(300, 100), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::SetNextWindowPos(ImVec2(0.0f, windowHeight), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
        ImGui::Begin("Performance", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        if(ImGui::BeginTable("Table", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg)){
            ImGui::TableNextRow();
            
            ImGui::TableSetColumnIndex(0);
            
            RenderText("FPS", 0.0f);
            RenderText("Total Frame Time", 0.0f);
            RenderText("Triangles Count", 0.0f);

            ImGui::TableSetColumnIndex(1);
            
            float fps = ImGui::GetIO().Framerate;
            char fpsText[32];
            snprintf(fpsText, sizeof(fpsText), "%.1f", fps);
            RenderText(fpsText, 0.0f);
            
            char timeText[32];
            snprintf(timeText, sizeof(timeText), "%.3f ms", (1000.0f/fps));
            RenderText(timeText, 0.0f);
            
            char trianglesText[32];
            snprintf(trianglesText, sizeof(trianglesText), "%d", scene.model.totalNumTriangles);
            RenderText(trianglesText, 0.0f);
            
            ImGui::EndTable();
        }
        
        ImGui::End();
    }
    
    static void Submit(UIInfo& ui, SceneInfo& scene, WindowInfo& window, uint32_t frame, EngineInfo& engine)
    {
        VkSubmitInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &ui.commandBuffers[frame];
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &scene.renderFinishedSemaphores[frame];
        VkPipelineStageFlags stageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        info.pWaitDstStageMask = &stageFlag;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &ui.renderFinishedSemaphores[frame];
        
        if(vkQueueSubmit(engine.graphicsQueue, 1, &info, ui.inflightFences[frame]) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO SUBMIT QUEUE\n");
    }
    
    static void Destroy()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};
