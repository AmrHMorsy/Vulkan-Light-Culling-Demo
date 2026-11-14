#pragma once
#include "Buffer.h"
#include "Includes.h"
#include "Engine.h"
#include "Model.h"




class LightCullingMatrixScatter{

public:
    
    static void Build(EngineInfo& engine, SceneInfo& scene)
    {
        LightCullingMatrixScatterInfo& lightCullingMatrixScatter = scene.lightCullingMatrixScatter;
        LightCullingMatrixScatterPrepassInfo& lightCullingMatrixScatterPrepass = scene.lightCullingMatrixScatterPrepass;
        CameraInfo& camera = scene.camera;
        std::vector<LightInfo>& lights = scene.lights;
                        
        lightCullingMatrixScatter.lightBoundingSpheresViewSpace.resize(scene.lights.size());

        std::vector<VkShaderModule> shaderModules(1);
        std::vector<uint32_t> shaderData;
        std::ifstream file("../Resources/Shaders/LightCullingMatrixScatter/LightCullingMatrixScatter.spv", std::ios::binary | std::ios::ate ) ;
        if(!file.is_open())
            throw std::runtime_error("FAILURE TO OPEN SHADER FILE FOR READING");
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        shaderData.resize(fileSize / sizeof(uint32_t));
        file.read( reinterpret_cast<char*>(shaderData.data()), fileSize);
        
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderData.size() * sizeof(uint32_t);
        createInfo.pCode = shaderData.data();
        if(vkCreateShaderModule(engine.logicalDevice, &createInfo, nullptr, &shaderModules[0]) != VK_SUCCESS)
            throw std::runtime_error("Failed To Create Shader Module");
        
        lightCullingMatrixScatter.numThreadsPerWorkgroup = 64;
        lightCullingMatrixScatter.numWorkgroups = (scene.lights.size() + 63) / 64;
        
        
        uint32_t specializationData;
        VkPipelineShaderStageCreateInfo pipelineShaderStage;
        std::vector<VkSpecializationMapEntry> entries;
        VkSpecializationInfo specializationInfo;
        
        entries.resize(1);
        entries[0] = {};
        entries[0].constantID = 0;
        entries[0].offset     = 0;
        entries[0].size       = sizeof(uint32_t);
        
        specializationData = lightCullingMatrixScatter.numThreadsPerWorkgroup;
        
        specializationInfo = {};
        specializationInfo.mapEntryCount = 1;
        specializationInfo.pMapEntries = entries.data();
        specializationInfo.dataSize = sizeof(uint32_t);
        specializationInfo.pData = &specializationData;
        
        pipelineShaderStage = {};
        pipelineShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineShaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        pipelineShaderStage.module = shaderModules[0];
        pipelineShaderStage.pName = "main";
        pipelineShaderStage.pSpecializationInfo = &specializationInfo;
        
        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(6);
        descriptorSetLayoutBindings[0].binding = 0;
        descriptorSetLayoutBindings[0].descriptorCount = 1;
        descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[0].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        descriptorSetLayoutBindings[1].binding = 1;
        descriptorSetLayoutBindings[1].descriptorCount = 1;
        descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorSetLayoutBindings[1].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        descriptorSetLayoutBindings[2].binding = 2;
        descriptorSetLayoutBindings[2].descriptorCount = 1;
        descriptorSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[2].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        descriptorSetLayoutBindings[3].binding = 3;
        descriptorSetLayoutBindings[3].descriptorCount = 1;
        descriptorSetLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[3].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        descriptorSetLayoutBindings[4].binding = 4;
        descriptorSetLayoutBindings[4].descriptorCount = 1;
        descriptorSetLayoutBindings[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[4].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[4].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        descriptorSetLayoutBindings[5].binding = 5;
        descriptorSetLayoutBindings[5].descriptorCount = 1;
        descriptorSetLayoutBindings[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[5].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[5].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        descriptorSetLayoutCreateInfo.bindingCount = 6,
        descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();
        descriptorSetLayoutCreateInfo.pNext = nullptr;
        VkDescriptorSetLayout descriptorSetLayout;
        if(vkCreateDescriptorSetLayout(engine.logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE DESCRIPTOR SET LAYOUT");
        
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        if(vkCreatePipelineLayout(engine.logicalDevice, &pipelineLayoutCreateInfo, nullptr, &lightCullingMatrixScatter.pipelineLayout) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE PIPELINE LAYOUT\n");
        
        VkComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.stage = pipelineShaderStage;
        computePipelineCreateInfo.layout = lightCullingMatrixScatter.pipelineLayout;
        if(vkCreateComputePipelines(engine.logicalDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &lightCullingMatrixScatter.computePipeline) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE LIGHT CULLING COMPUTE PIPELINES");
        
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
        lightCullingMatrixScatter.commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        if(vkAllocateCommandBuffers(engine.logicalDevice, &commandBufferAllocateInfo, lightCullingMatrixScatter.commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO ALLOCATE COMMAND BUFFERS\n");
        
        lightCullingMatrixScatter.fences.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.pNext = nullptr;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            if(vkCreateFence(engine.logicalDevice, &fenceCreateInfo, nullptr, &lightCullingMatrixScatter.fences[i]) != VK_SUCCESS)
                throw std::runtime_error("\nFAILURE TO CREATE FENCES\n");
        }

        lightCullingMatrixScatter.finishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        for( int i = 0 ; i < MAX_FRAMES_IN_FLIGHT ; i++ ){
            VkSemaphoreCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if(vkCreateSemaphore(engine.logicalDevice, &info, nullptr, &lightCullingMatrixScatter.finishedSemaphores[i]) != VK_SUCCESS)
                throw std::runtime_error("\nFAILURE TO CREATE SEMAPHORES");
        }

        lightCullingMatrixScatter.uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatter.uniformBuffers[i] = Buffer::BuildHostVisibleBuffer(engine.logicalDevice, engine.physicalDevice.handle, sizeof(LightCullingGatherUniformVariables), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        
        lightCullingMatrixScatter.uniformDescriptorBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatter.uniformDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = lightCullingMatrixScatter.uniformBuffers[i].bufferPack.buffer,
                .offset = 0,
                .range = sizeof(LightCullingGatherUniformVariables)
            };
        
        lightCullingMatrixScatter.tilesFrustumPlanesBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatter.tilesFrustumPlanesBuffers[i] = Buffer::BuildHostVisibleBuffer(engine.logicalDevice, engine.physicalDevice.handle, scene.totalNumTiles * 4 * sizeof(glm::vec4), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
                
        lightCullingMatrixScatter.tilesFrustumPlanesDescriptorBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatter.tilesFrustumPlanesDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = lightCullingMatrixScatter.tilesFrustumPlanesBuffers[i].bufferPack.buffer,
                .offset = 0,
                .range = scene.totalNumTiles * 4 * sizeof(glm::vec4)
            };
        
        lightCullingMatrixScatter.lightBoundingSphereViewSpaceBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatter.lightBoundingSphereViewSpaceBuffers[i] = Buffer::BuildHostVisibleBuffer(engine.logicalDevice, engine.physicalDevice.handle, sizeof(lightCullingMatrixScatter.lightBoundingSpheresViewSpace[0]) * lightCullingMatrixScatter.lightBoundingSpheresViewSpace.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
        
        lightCullingMatrixScatter.lightBoundingSphereViewSpaceDescriptorBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatter.lightBoundingSphereViewSpaceDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = lightCullingMatrixScatter.lightBoundingSphereViewSpaceBuffers[i].bufferPack.buffer,
                .offset = 0,
                .range = sizeof(lightCullingMatrixScatter.lightBoundingSpheresViewSpace[0]) * lightCullingMatrixScatter.lightBoundingSpheresViewSpace.size()
            };
                        
        std::vector<VkDescriptorPoolSize> descriptorPoolSizes(2);
        descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorPoolSizes[0].descriptorCount = 5 * MAX_FRAMES_IN_FLIGHT;
        descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;
        
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{} ;
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = 2;
        descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
        descriptorPoolCreateInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
        lightCullingMatrixScatter.descriptorPool;
        if(vkCreateDescriptorPool(engine.logicalDevice, &descriptorPoolCreateInfo, nullptr, &lightCullingMatrixScatter.descriptorPool) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE DESCRIPTOR POOL");

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = lightCullingMatrixScatter.descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();
        descriptorSetAllocateInfo.pNext = nullptr;
        lightCullingMatrixScatter.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if(vkAllocateDescriptorSets(engine.logicalDevice, &descriptorSetAllocateInfo, lightCullingMatrixScatter.descriptorSets.data())  != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE RENDER PASS");
        
        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
            std::vector<VkWriteDescriptorSet> writeDescriptorSets(6);
            
            writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[0].dstSet = lightCullingMatrixScatter.descriptorSets[i];
            writeDescriptorSets[0].dstBinding = 0;
            writeDescriptorSets[0].dstArrayElement = 0;
            writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[0].descriptorCount = 1;
            writeDescriptorSets[0].pBufferInfo = &lightCullingMatrixScatter.lightBoundingSphereViewSpaceDescriptorBuffers[i];
            writeDescriptorSets[0].pImageInfo = nullptr;
            
            writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[1].dstSet = lightCullingMatrixScatter.descriptorSets[i];
            writeDescriptorSets[1].dstBinding = 1;
            writeDescriptorSets[1].dstArrayElement = 0;
            writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescriptorSets[1].descriptorCount = 1;
            writeDescriptorSets[1].pBufferInfo = &lightCullingMatrixScatter.uniformDescriptorBuffers[i];
            writeDescriptorSets[1].pImageInfo = nullptr;

            writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[2].dstSet = lightCullingMatrixScatter.descriptorSets[i];
            writeDescriptorSets[2].dstBinding = 2;
            writeDescriptorSets[2].dstArrayElement = 0;
            writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[2].descriptorCount = 1;
            writeDescriptorSets[2].pBufferInfo = &lightCullingMatrixScatterPrepass.tileLightCullingPrepassDescriptorBuffers[i];
            writeDescriptorSets[2].pImageInfo = nullptr;
            
            writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[3].dstSet = lightCullingMatrixScatter.descriptorSets[i];
            writeDescriptorSets[3].dstBinding = 3;
            writeDescriptorSets[3].dstArrayElement = 0;
            writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[3].descriptorCount = 1;
            writeDescriptorSets[3].pBufferInfo = &lightCullingMatrixScatter.tilesFrustumPlanesDescriptorBuffers[i];
            writeDescriptorSets[3].pImageInfo = nullptr;
            
            writeDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[4].dstSet = lightCullingMatrixScatter.descriptorSets[i];
            writeDescriptorSets[4].dstBinding = 4;
            writeDescriptorSets[4].dstArrayElement = 0;
            writeDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[4].descriptorCount = 1;
            writeDescriptorSets[4].pBufferInfo = &lightCullingMatrixScatterPrepass.tilesGeometryBitMaskDescriptorBuffers[i];
            writeDescriptorSets[4].pImageInfo = nullptr;
            
            writeDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[5].dstSet = lightCullingMatrixScatter.descriptorSets[i];
            writeDescriptorSets[5].dstBinding = 5;
            writeDescriptorSets[5].dstArrayElement = 0;
            writeDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[5].descriptorCount = 1;
            writeDescriptorSets[5].pBufferInfo = &lightCullingMatrixScatterPrepass.lightTileIndicesDescriptorBuffers[i];
            writeDescriptorSets[5].pImageInfo = nullptr;
            
            
            
            
            
            
            
            vkUpdateDescriptorSets(engine.logicalDevice, 6, writeDescriptorSets.data(), 0, NULL);
        }
    }
    
    static void Record(EngineInfo& engine, SceneInfo& scene, uint32_t frame)
    {
        CameraInfo& camera = scene.camera;
        
        LightCullingMatrixScatterInfo& lightCullingMatrixScatter = scene.lightCullingMatrixScatter;
        
        vkWaitForFences(engine.logicalDevice, 1, &lightCullingMatrixScatter.fences[frame], VK_TRUE, UINT64_MAX);
        
        vkResetFences(engine.logicalDevice, 1, &lightCullingMatrixScatter.fences[frame]);
        
        if(vkResetCommandBuffer(lightCullingMatrixScatter.commandBuffers[frame], 0) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO RESET COMMAND BUFFER\n");
        
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;
        if(vkBeginCommandBuffer(lightCullingMatrixScatter.commandBuffers[frame], &commandBufferBeginInfo) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO BEGIN RECORDING COMMAND BUFFER\n");
        
        for(size_t i = 0; i < scene.lights.size(); i++)
            lightCullingMatrixScatter.lightBoundingSpheresViewSpace[i] = glm::vec4(glm::vec3(camera.viewMatrix * glm::vec4(scene.lights[i].boundingSphere.x, scene.lights[i].boundingSphere.y, scene.lights[i].boundingSphere.z, 1.0f)), scene.lights[i].boundingSphere.w);
        
        memcpy(lightCullingMatrixScatter.lightBoundingSphereViewSpaceBuffers[frame].memoryPointer, lightCullingMatrixScatter.lightBoundingSpheresViewSpace.data(), sizeof(lightCullingMatrixScatter.lightBoundingSpheresViewSpace[0]) * lightCullingMatrixScatter.lightBoundingSpheresViewSpace.size());
        
        lightCullingMatrixScatter.uniformData.cameraNearPlane = camera.config.cameraNearPlane;
        lightCullingMatrixScatter.uniformData.screenResolution = glm::vec2(engine.swapChain.extent.width, engine.swapChain.extent.height);
        lightCullingMatrixScatter.uniformData.cameraFarPlane = camera.config.cameraFarPlane;
        lightCullingMatrixScatter.uniformData.numCellsPerTile = scene.numCellsPerTile;
        lightCullingMatrixScatter.uniformData.numTiles2D = scene.numTiles2D;
        lightCullingMatrixScatter.uniformData.tileResolution = tileResolutions[scene.selectedTileResolution];
        lightCullingMatrixScatter.uniformData.flags = scene.model.flags;
        
        memcpy(lightCullingMatrixScatter.uniformBuffers[frame].memoryPointer, &lightCullingMatrixScatter.uniformData, sizeof(LightCullingGatherUniformVariables));
        memcpy(lightCullingMatrixScatter.tilesFrustumPlanesBuffers[frame].memoryPointer, scene.tilesFrustumPlanes.data(), scene.totalNumTiles * 4 * sizeof(glm::vec4));
        vkCmdBindPipeline(lightCullingMatrixScatter.commandBuffers[frame], VK_PIPELINE_BIND_POINT_COMPUTE, lightCullingMatrixScatter.computePipeline);
        vkCmdBindDescriptorSets(lightCullingMatrixScatter.commandBuffers[frame], VK_PIPELINE_BIND_POINT_COMPUTE, lightCullingMatrixScatter.pipelineLayout, 0, 1, &lightCullingMatrixScatter.descriptorSets[frame], 0, nullptr);
        vkCmdDispatch(lightCullingMatrixScatter.commandBuffers[frame], lightCullingMatrixScatter.numWorkgroups, 1, 1);
        vkEndCommandBuffer(lightCullingMatrixScatter.commandBuffers[frame]);
    }

    static void Submit(EngineInfo& engine, LightCullingMatrixScatterInfo& lightCullingMatrixScatter, LightCullingMatrixScatterPrepassInfo& lightCullingMatrixScatterPrepass, uint32_t frame)
    {
        VkSubmitInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &lightCullingMatrixScatter.commandBuffers[frame];
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &lightCullingMatrixScatterPrepass.finishedSemaphores[frame];
        VkPipelineStageFlags stageFlag = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        info.pWaitDstStageMask = &stageFlag;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &lightCullingMatrixScatter.finishedSemaphores[frame];
        
        if(vkQueueSubmit(engine.graphicsQueue, 1, &info, lightCullingMatrixScatter.fences[frame]) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO SUBMIT QUEUE\n");
    }
    
    static std::vector<glm::vec4> ExtractTilesFrustumPlanes(glm::uvec2 numTiles2D, glm::mat4 cameraProjectionMatrix)
    {
        std::vector<glm::vec4> tilesFrustumPlanes(numTiles2D.x * numTiles2D.y * 4);
        
        glm::vec4 row1 = glm::vec4(cameraProjectionMatrix[0][0], cameraProjectionMatrix[1][0], cameraProjectionMatrix[2][0], cameraProjectionMatrix[3][0]);
        glm::vec4 row2 = glm::vec4(cameraProjectionMatrix[0][1], cameraProjectionMatrix[1][1], cameraProjectionMatrix[2][1], cameraProjectionMatrix[3][1]);
        glm::vec4 row4 = glm::vec4(cameraProjectionMatrix[0][3], cameraProjectionMatrix[1][3], cameraProjectionMatrix[2][3], cameraProjectionMatrix[3][3]);
        
        float stepX = 2.0f / (float)numTiles2D.x;
        float stepY = 2.0f / (float)numTiles2D.y;
        
        for(int x = 0; x < numTiles2D.x; x++){
            float offsetX = (float)x * stepX;
            float xMin = -1.0f + offsetX;
            float xMax = xMin + stepX;
            for(int y = 0; y < numTiles2D.y; y++){
                std::array<glm::vec4, 4> tileFrustumPlanes;
                tileFrustumPlanes[0] = row1 - (row4 * xMin);
                tileFrustumPlanes[1] = (xMax * row4) - row1;
                
                float offsetY = (float)y * stepY;
                float yMin = -1.0f + offsetY;
                float yMax = yMin + stepY;
                tileFrustumPlanes[2] = row2 - (row4 * yMin);
                tileFrustumPlanes[3] = (yMax * row4) - row2;
                
                for(int i = 0; i < 4; i++)
                    tileFrustumPlanes[i] /= glm::length(glm::vec3(tileFrustumPlanes[i]));

                int baseIndex = (y * numTiles2D.x * 4) + (x * 4);
                for(int i = 0; i < 4; i++)
                    tilesFrustumPlanes[baseIndex + i] = tileFrustumPlanes[i];
            }
        }
        
        return tilesFrustumPlanes;
    }
};
