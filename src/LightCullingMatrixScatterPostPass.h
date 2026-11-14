#pragma once
#include "Buffer.h"
#include "Includes.h"
#include "Engine.h"
#include "Model.h"




class LightCullingMatrixScatterPostPass{

public:
    
    static void Build(EngineInfo& engine, SceneInfo& scene)
    {
        scene.lightCullingMatrixScatterPostPass.uniformData.numLights = scene.lights.size();
        scene.lightCullingMatrixScatterPostPass.uniformData.numTiles2D = scene.numTiles2D;
        scene.lightCullingMatrixScatterPostPass.uniformData.tileResolution = tileResolutions[scene.selectedTileResolution];
        
        std::vector<VkShaderModule> shaderModules(1);
        std::vector<uint32_t> shaderData;
        std::ifstream file("../Resources/Shaders/LightCullingMatrixScatter/LightCullingMatrixScatterPostPass.spv", std::ios::binary | std::ios::ate ) ;
        if(!file.is_open())
            throw std::runtime_error("FAILURE TO OPEN SHADER FILE FOR READING");
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        shaderData.resize(fileSize / sizeof(uint32_t));
        file.read(reinterpret_cast<char*>(shaderData.data()), fileSize);
        
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderData.size() * sizeof(uint32_t);
        createInfo.pCode = shaderData.data();
        if(vkCreateShaderModule(engine.logicalDevice, &createInfo, nullptr, &shaderModules[0]) != VK_SUCCESS)
            throw std::runtime_error("Failed To Create Shader Module");
         
        
        struct SpecializationData{
            uint32_t x;
            uint32_t y;
        };
        SpecializationData specializationData;
        VkSpecializationInfo specializationInfo;
        VkPipelineShaderStageCreateInfo pipelineShaderStage;
        std::vector<VkSpecializationMapEntry> entries;
        entries.resize(2);
        entries[0] = {};
        entries[1] = {};
        entries[0].constantID = 0;
        entries[0].offset     = 0;
        entries[0].size       = sizeof(uint32_t);
        entries[1].constantID = 1;
        entries[1].offset     = sizeof(uint32_t);
        entries[1].size       = sizeof(uint32_t);

        specializationData.x = tileResolutions[scene.selectedTileResolution].x;
        specializationData.y = tileResolutions[scene.selectedTileResolution].y;
        
        specializationInfo = {};
        specializationInfo.mapEntryCount = 2;
        specializationInfo.pMapEntries = entries.data();
        specializationInfo.dataSize = sizeof(specializationData);
        specializationInfo.pData = &specializationData;
        
        pipelineShaderStage = {};
        pipelineShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineShaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        pipelineShaderStage.module = shaderModules[0];
        pipelineShaderStage.pName = "main";
        pipelineShaderStage.pSpecializationInfo = &specializationInfo;
        
        
        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(5);
        descriptorSetLayoutBindings[0].binding = 0;
        descriptorSetLayoutBindings[0].descriptorCount = 1;
        descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[0].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        descriptorSetLayoutBindings[1].binding = 1;
        descriptorSetLayoutBindings[1].descriptorCount = 1;
        descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[1].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        descriptorSetLayoutBindings[2].binding = 2;
        descriptorSetLayoutBindings[2].descriptorCount = 1;
        descriptorSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[2].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        descriptorSetLayoutBindings[3].binding = 3;
        descriptorSetLayoutBindings[3].descriptorCount = 1;
        descriptorSetLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorSetLayoutBindings[3].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        descriptorSetLayoutBindings[4].binding = 4;
        descriptorSetLayoutBindings[4].descriptorCount = 1;
        descriptorSetLayoutBindings[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[4].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[4].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
                

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        descriptorSetLayoutCreateInfo.bindingCount = 5,
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
        if(vkCreatePipelineLayout(engine.logicalDevice, &pipelineLayoutCreateInfo, nullptr, &scene.lightCullingMatrixScatterPostPass.pipelineLayout) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE PIPELINE LAYOUT\n");
        
        VkComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo = {};
        computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.stage = pipelineShaderStage;
        computePipelineCreateInfo.layout = scene.lightCullingMatrixScatterPostPass.pipelineLayout;
        if(vkCreateComputePipelines(engine.logicalDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &scene.lightCullingMatrixScatterPostPass.computePipeline) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE LIGHT CULLING PREPASS COMPUTE PIPELINES");

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
        scene.lightCullingMatrixScatterPostPass.commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        if(vkAllocateCommandBuffers(engine.logicalDevice, &commandBufferAllocateInfo, scene.lightCullingMatrixScatterPostPass.commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO ALLOCATE COMMAND BUFFERS\n");
        
        scene.lightCullingMatrixScatterPostPass.fences.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.pNext = nullptr;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            if(vkCreateFence(engine.logicalDevice, &fenceCreateInfo, nullptr, &scene.lightCullingMatrixScatterPostPass.fences[i]) != VK_SUCCESS)
                throw std::runtime_error("\nFAILURE TO CREATE FENCES\n");
        }
        
        scene.lightCullingMatrixScatterPostPass.finishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT) ;
        for( int i = 0 ; i < MAX_FRAMES_IN_FLIGHT ; i++ ){
            VkSemaphoreCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if(vkCreateSemaphore(engine.logicalDevice, &info, nullptr, &scene.lightCullingMatrixScatterPostPass.finishedSemaphores[i]) != VK_SUCCESS)
                throw std::runtime_error("\nFAILURE TO CREATE SEMAPHORES");
        }
        
        scene.lightCullingMatrixScatterPostPass.uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            scene.lightCullingMatrixScatterPostPass.uniformBuffers[i] = Buffer::BuildHostVisibleBuffer(engine.logicalDevice, engine.physicalDevice.handle, sizeof(LightCullingMatrixScatterPostPassUniformVariables), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        
        
        std::vector<VkDescriptorBufferInfo> uniformDescriptorBuffers(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            uniformDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = scene.lightCullingMatrixScatterPostPass.uniformBuffers[i].bufferPack.buffer,
                .offset = 0,
                .range = sizeof(LightCullingMatrixScatterPostPassUniformVariables)
            };
                
        
        scene.lightCullingMatrixScatterPostPass.tileLightIndicesBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            scene.lightCullingMatrixScatterPostPass.tileLightIndicesBuffers[i] = Buffer::BuildBuffer(engine.logicalDevice, engine.physicalDevice.handle, scene.totalNumTiles * scene.lights.size() * sizeof(unsigned int), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        scene.lightCullingMatrixScatterPostPass.tileLightIndicesDescriptorBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            scene.lightCullingMatrixScatterPostPass.tileLightIndicesDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = scene.lightCullingMatrixScatterPostPass.tileLightIndicesBuffers[i].buffer,
                .offset = 0,
                .range = scene.totalNumTiles * scene.lights.size() * sizeof(unsigned int)
            };

        
        std::vector<VkDescriptorPoolSize> descriptorPoolSizes(2);
        descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorPoolSizes[0].descriptorCount = 4 * MAX_FRAMES_IN_FLIGHT;
        descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{} ;
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO ;
        descriptorPoolCreateInfo.poolSizeCount = 2;
        descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
        descriptorPoolCreateInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
        VkDescriptorPool descriptorPool;
        if(vkCreateDescriptorPool(engine.logicalDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE DESCRIPTOR POOL");


        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
        
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();
        descriptorSetAllocateInfo.pNext = nullptr;
        scene.lightCullingMatrixScatterPostPass.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        vkAllocateDescriptorSets(engine.logicalDevice, &descriptorSetAllocateInfo, scene.lightCullingMatrixScatterPostPass.descriptorSets.data());
        
        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
            std::vector<VkWriteDescriptorSet> writeDescriptorSets(5);
            
            writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[0].dstSet = scene.lightCullingMatrixScatterPostPass.descriptorSets[i];
            writeDescriptorSets[0].dstBinding = 0;
            writeDescriptorSets[0].dstArrayElement = 0;
            writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[0].descriptorCount = 1;
            writeDescriptorSets[0].pImageInfo = nullptr;
            writeDescriptorSets[0].pBufferInfo = &scene.lightCullingMatrixScatterPrepass.lightTileIndicesDescriptorBuffers[i];
            
            writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[1].dstSet = scene.lightCullingMatrixScatterPostPass.descriptorSets[i];
            writeDescriptorSets[1].dstBinding = 1;
            writeDescriptorSets[1].dstArrayElement = 0;
            writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[1].descriptorCount = 1;
            writeDescriptorSets[1].pImageInfo = nullptr;
            writeDescriptorSets[1].pBufferInfo = &scene.lightCullingMatrixScatterPrepass.tileLightCountDescriptorBuffers[i];
            
            writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[2].dstSet = scene.lightCullingMatrixScatterPostPass.descriptorSets[i];
            writeDescriptorSets[2].dstBinding = 2;
            writeDescriptorSets[2].dstArrayElement = 0;
            writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[2].descriptorCount = 1;
            writeDescriptorSets[2].pImageInfo = nullptr;
            writeDescriptorSets[2].pBufferInfo = &scene.lightCullingMatrixScatterPostPass.tileLightIndicesDescriptorBuffers[i];
            
            writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[3].dstSet = scene.lightCullingMatrixScatterPostPass.descriptorSets[i];
            writeDescriptorSets[3].dstBinding = 3;
            writeDescriptorSets[3].dstArrayElement = 0;
            writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescriptorSets[3].descriptorCount = 1;
            writeDescriptorSets[3].pImageInfo = nullptr;
            writeDescriptorSets[3].pBufferInfo = &uniformDescriptorBuffers[i];
            
            writeDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[4].dstSet = scene.lightCullingMatrixScatterPostPass.descriptorSets[i];
            writeDescriptorSets[4].dstBinding = 4;
            writeDescriptorSets[4].dstArrayElement = 0;
            writeDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[4].descriptorCount = 1;
            writeDescriptorSets[4].pImageInfo = nullptr;
            writeDescriptorSets[4].pBufferInfo = &scene.lightCullingMatrixScatterPrepass.maxLightCountDescriptorBuffers[i];

            vkUpdateDescriptorSets(engine.logicalDevice, 5, writeDescriptorSets.data(), 0, NULL);
        };
    }
    
    static void Record(EngineInfo& engine, SceneInfo& scene, uint32_t frame)
    {
        vkWaitForFences(engine.logicalDevice, 1, &scene.lightCullingMatrixScatterPostPass.fences[frame], VK_TRUE, UINT64_MAX);
        
        vkResetFences(engine.logicalDevice, 1, &scene.lightCullingMatrixScatterPostPass.fences[frame]) ;
        
        if(vkResetCommandBuffer(scene.lightCullingMatrixScatterPostPass.commandBuffers[frame], 0) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO RESET COMMAND BUFFER\n");
        
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;
        if(vkBeginCommandBuffer(scene.lightCullingMatrixScatterPostPass.commandBuffers[frame], &commandBufferBeginInfo) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO BEGIN RECORDING COMMAND BUFFER\n");
        
        memcpy(scene.lightCullingMatrixScatterPostPass.uniformBuffers[frame].memoryPointer, &scene.lightCullingMatrixScatterPostPass.uniformData, sizeof(LightCullingMatrixScatterPostPassUniformVariables));
        vkCmdBindPipeline(scene.lightCullingMatrixScatterPostPass.commandBuffers[frame], VK_PIPELINE_BIND_POINT_COMPUTE, scene.lightCullingMatrixScatterPostPass.computePipeline);
        vkCmdBindDescriptorSets(scene.lightCullingMatrixScatterPostPass.commandBuffers[frame], VK_PIPELINE_BIND_POINT_COMPUTE, scene.lightCullingMatrixScatterPostPass.pipelineLayout, 0, 1, &scene.lightCullingMatrixScatterPostPass.descriptorSets[frame], 0, nullptr);
        vkCmdDispatch(scene.lightCullingMatrixScatterPostPass.commandBuffers[frame], scene.numTiles2D.x, scene.numTiles2D.y, 1);
        vkEndCommandBuffer(scene.lightCullingMatrixScatterPostPass.commandBuffers[frame]);
    }

    static void Submit(EngineInfo& engine, LightCullingMatrixScatterPostPassInfo& lightCullingMatrixScatterPostPass, LightCullingMatrixScatterInfo& lightCullingMatrixScatter, uint32_t frame)
    {
        VkSubmitInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &lightCullingMatrixScatterPostPass.commandBuffers[frame];
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &lightCullingMatrixScatter.finishedSemaphores[frame];
        VkPipelineStageFlags stageFlag = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        info.pWaitDstStageMask = &stageFlag;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &lightCullingMatrixScatterPostPass.finishedSemaphores[frame];
        
        if(vkQueueSubmit(engine.graphicsQueue, 1, &info, lightCullingMatrixScatterPostPass.fences[frame]) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO SUBMIT QUEUE\n");
    }
};
