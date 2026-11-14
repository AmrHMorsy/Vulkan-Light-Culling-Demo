#pragma once
#include "Includes.h"
#include "Engine.h"
#include "Buffer.h"




class LightCullingMatrixScatterPrepass{

public:
    
    static void Build(EngineInfo& engine, SceneInfo& scene)
    {
        DepthPrepassInfo& depthPrepass = scene.depthPrepass;
        LightCullingMatrixScatterPrepassInfo& lightCullingMatrixScatterPrepass = scene.lightCullingMatrixScatterPrepass;
                
        std::vector<VkShaderModule> shaderModules(1);
        std::vector<uint32_t> shaderData;
        std::ifstream file("../Resources/Shaders/LightCullingMatrixScatter/LightCullingMatrixScatterPrepass.spv", std::ios::binary | std::ios::ate ) ;
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

        
        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(7);
        descriptorSetLayoutBindings[0].binding = 0;
        descriptorSetLayoutBindings[0].descriptorCount = 1;
        descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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
        descriptorSetLayoutBindings[6].binding = 6;
        descriptorSetLayoutBindings[6].descriptorCount = 1;
        descriptorSetLayoutBindings[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[6].pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings[6].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;


        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        descriptorSetLayoutCreateInfo.bindingCount = 7,
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
        if(vkCreatePipelineLayout(engine.logicalDevice, &pipelineLayoutCreateInfo, nullptr, &lightCullingMatrixScatterPrepass.pipelineLayout) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO CREATE PIPELINE LAYOUT\n");
        
        VkComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo = {};
        computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.stage = pipelineShaderStage;
        computePipelineCreateInfo.layout = lightCullingMatrixScatterPrepass.pipelineLayout;
        if(vkCreateComputePipelines(engine.logicalDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &lightCullingMatrixScatterPrepass.computePipeline) != VK_SUCCESS)
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
        lightCullingMatrixScatterPrepass.commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        if(vkAllocateCommandBuffers(engine.logicalDevice, &commandBufferAllocateInfo, lightCullingMatrixScatterPrepass.commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO ALLOCATE COMMAND BUFFERS\n");
        
        
        lightCullingMatrixScatterPrepass.fences.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.pNext = nullptr;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            if(vkCreateFence(engine.logicalDevice, &fenceCreateInfo, nullptr, &lightCullingMatrixScatterPrepass.fences[i]) != VK_SUCCESS)
                throw std::runtime_error("\nFAILURE TO CREATE FENCES\n");
        }
        
        lightCullingMatrixScatterPrepass.finishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT) ;
        for( int i = 0 ; i < MAX_FRAMES_IN_FLIGHT ; i++ ){
            VkSemaphoreCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if(vkCreateSemaphore(engine.logicalDevice, &info, nullptr, &lightCullingMatrixScatterPrepass.finishedSemaphores[i]) != VK_SUCCESS)
                throw std::runtime_error("\nFAILURE TO CREATE SEMAPHORES");
        }
        
        lightCullingMatrixScatterPrepass.maxLightCountBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatterPrepass.maxLightCountBuffers[i] = Buffer::BuildBuffer(engine.logicalDevice, engine.physicalDevice.handle, sizeof(unsigned int), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
                
        lightCullingMatrixScatterPrepass.maxLightCountDescriptorBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatterPrepass.maxLightCountDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = lightCullingMatrixScatterPrepass.maxLightCountBuffers[i].buffer,
                .offset = 0,
                .range = sizeof(unsigned int)
            };
        
        lightCullingMatrixScatterPrepass.tileLightCountBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatterPrepass.tileLightCountBuffers[i] = Buffer::BuildBuffer(engine.logicalDevice, engine.physicalDevice.handle, scene.totalNumTiles * sizeof(unsigned int), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        lightCullingMatrixScatterPrepass.tileLightCountDescriptorBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatterPrepass.tileLightCountDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = lightCullingMatrixScatterPrepass.tileLightCountBuffers[i].buffer,
                .offset = 0,
                .range = scene.totalNumTiles * sizeof(unsigned int)
            };
        
        lightCullingMatrixScatterPrepass.uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatterPrepass.uniformBuffers[i] = Buffer::BuildHostVisibleBuffer(engine.logicalDevice, engine.physicalDevice.handle, sizeof(LightCullingMatrixScatterPrepassUniformVariables), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        
        
        std::vector<VkDescriptorBufferInfo> uniformDescriptorBuffers(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            uniformDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = lightCullingMatrixScatterPrepass.uniformBuffers[i].bufferPack.buffer,
                .offset = 0,
                .range = sizeof(LightCullingMatrixScatterPrepassUniformVariables)
            };
        
        std::vector<BufferInfo> tileLightCullingPrepassBuffers(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            tileLightCullingPrepassBuffers[i] = Buffer::BuildBuffer(engine.logicalDevice, engine.physicalDevice.handle, scene.totalNumTiles * sizeof(glm::vec2), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        lightCullingMatrixScatterPrepass.tileLightCullingPrepassDescriptorBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatterPrepass.tileLightCullingPrepassDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = tileLightCullingPrepassBuffers[i].buffer,
                .offset = 0,
                .range = scene.totalNumTiles * sizeof(glm::vec2)
            };
        
        std::vector<BufferInfo> tilesGeometryBitMaskBuffers(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            tilesGeometryBitMaskBuffers[i] = Buffer::BuildBuffer(engine.logicalDevice, engine.physicalDevice.handle, scene.totalNumTiles * sizeof(unsigned int), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        lightCullingMatrixScatterPrepass.tilesGeometryBitMaskDescriptorBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatterPrepass.tilesGeometryBitMaskDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = tilesGeometryBitMaskBuffers[i].buffer,
                .offset = 0,
                .range = scene.totalNumTiles * sizeof(unsigned int)
            };
        
        lightCullingMatrixScatterPrepass.LightTileIndicesBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatterPrepass.LightTileIndicesBuffers[i] = Buffer::BuildBuffer(engine.logicalDevice, engine.physicalDevice.handle, scene.totalNumTiles * scene.lights.size() * sizeof(int), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        lightCullingMatrixScatterPrepass.lightTileIndicesDescriptorBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            lightCullingMatrixScatterPrepass.lightTileIndicesDescriptorBuffers[i] = VkDescriptorBufferInfo{
                .buffer = lightCullingMatrixScatterPrepass.LightTileIndicesBuffers[i].buffer,
                .offset = 0,
                .range = scene.totalNumTiles * scene.lights.size() * sizeof(int)
            };
        
        std::vector<VkDescriptorPoolSize> descriptorPoolSizes(3);
        descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorPoolSizes[0].descriptorCount = 5 * MAX_FRAMES_IN_FLIGHT;
        descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;
        descriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorPoolSizes[2].descriptorCount = MAX_FRAMES_IN_FLIGHT;


        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{} ;
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO ;
        descriptorPoolCreateInfo.poolSizeCount = 3;
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
        lightCullingMatrixScatterPrepass.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        vkAllocateDescriptorSets(engine.logicalDevice, &descriptorSetAllocateInfo, lightCullingMatrixScatterPrepass.descriptorSets.data());
        
        
        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
            std::vector<VkWriteDescriptorSet> writeDescriptorSets(7);
            writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[0].dstSet = lightCullingMatrixScatterPrepass.descriptorSets[i];
            writeDescriptorSets[0].dstBinding = 0;
            writeDescriptorSets[0].dstArrayElement = 0;
            writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writeDescriptorSets[0].descriptorCount = 1;            
            writeDescriptorSets[0].pImageInfo = &depthPrepass.descriptorImages[i];
            writeDescriptorSets[0].pBufferInfo = nullptr;
            
            writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[1].dstSet = lightCullingMatrixScatterPrepass.descriptorSets[i];
            writeDescriptorSets[1].dstBinding = 1;
            writeDescriptorSets[1].dstArrayElement = 0;
            writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescriptorSets[1].descriptorCount = 1;
            writeDescriptorSets[1].pImageInfo = nullptr;
            
            writeDescriptorSets[1].pBufferInfo = &uniformDescriptorBuffers[i];
            writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[2].dstSet = lightCullingMatrixScatterPrepass.descriptorSets[i];
            writeDescriptorSets[2].dstBinding = 2;
            writeDescriptorSets[2].dstArrayElement = 0;
            writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[2].descriptorCount = 1;
            writeDescriptorSets[2].pImageInfo = nullptr;
            writeDescriptorSets[2].pBufferInfo = &lightCullingMatrixScatterPrepass.tileLightCullingPrepassDescriptorBuffers[i];
            
            writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[3].dstSet = lightCullingMatrixScatterPrepass.descriptorSets[i];
            writeDescriptorSets[3].dstBinding = 3;
            writeDescriptorSets[3].dstArrayElement = 0;
            writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[3].descriptorCount = 1;
            writeDescriptorSets[3].pImageInfo = nullptr;
            writeDescriptorSets[3].pBufferInfo = &lightCullingMatrixScatterPrepass.tilesGeometryBitMaskDescriptorBuffers[i];
            
            writeDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[4].dstSet = lightCullingMatrixScatterPrepass.descriptorSets[i];
            writeDescriptorSets[4].dstBinding = 4;
            writeDescriptorSets[4].dstArrayElement = 0;
            writeDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[4].descriptorCount = 1;
            writeDescriptorSets[4].pImageInfo = nullptr;
            writeDescriptorSets[4].pBufferInfo = &lightCullingMatrixScatterPrepass.tileLightCountDescriptorBuffers[i];
            
            writeDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[5].dstSet = lightCullingMatrixScatterPrepass.descriptorSets[i];
            writeDescriptorSets[5].dstBinding = 5;
            writeDescriptorSets[5].dstArrayElement = 0;
            writeDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[5].descriptorCount = 1;
            writeDescriptorSets[5].pImageInfo = nullptr;
            writeDescriptorSets[5].pBufferInfo = &lightCullingMatrixScatterPrepass.maxLightCountDescriptorBuffers[i];
            
            writeDescriptorSets[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[6].dstSet = lightCullingMatrixScatterPrepass.descriptorSets[i];
            writeDescriptorSets[6].dstBinding = 6;
            writeDescriptorSets[6].dstArrayElement = 0;
            writeDescriptorSets[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSets[6].descriptorCount = 1;
            writeDescriptorSets[6].pImageInfo = nullptr;
            writeDescriptorSets[6].pBufferInfo = &lightCullingMatrixScatterPrepass.lightTileIndicesDescriptorBuffers[i];
            
            vkUpdateDescriptorSets(engine.logicalDevice, 7, writeDescriptorSets.data(), 0, NULL);
        };
    }

    static void Record(EngineInfo& engine, SceneInfo& scene, uint32_t frame)
    {
        CameraInfo& camera = scene.camera;
        LightCullingMatrixScatterPrepassInfo& lightCullingMatrixScatterPrepass = scene.lightCullingMatrixScatterPrepass;
        
        vkWaitForFences(engine.logicalDevice, 1, &lightCullingMatrixScatterPrepass.fences[frame], VK_TRUE, UINT64_MAX);
        
        vkResetFences(engine.logicalDevice, 1, &lightCullingMatrixScatterPrepass.fences[frame]) ;
        
        if(vkResetCommandBuffer(lightCullingMatrixScatterPrepass.commandBuffers[frame], 0) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO RESET COMMAND BUFFER\n");
        
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;
        if(vkBeginCommandBuffer(lightCullingMatrixScatterPrepass.commandBuffers[frame], &commandBufferBeginInfo) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO BEGIN RECORDING COMMAND BUFFER\n");
        
        lightCullingMatrixScatterPrepass.uniformData.cameraNearPlane = camera.config.cameraNearPlane;
        lightCullingMatrixScatterPrepass.uniformData.numLights = scene.lights.size();
        lightCullingMatrixScatterPrepass.uniformData.cameraFarPlane = camera.config.cameraFarPlane;
        lightCullingMatrixScatterPrepass.uniformData.numCellsPerTile = scene.numCellsPerTile;
        lightCullingMatrixScatterPrepass.uniformData.numTiles2D = scene.numTiles2D;
        lightCullingMatrixScatterPrepass.uniformData.tileResolution = tileResolutions[scene.selectedTileResolution];
        lightCullingMatrixScatterPrepass.uniformData.flags = scene.model.flags;

        memcpy(lightCullingMatrixScatterPrepass.uniformBuffers[frame].memoryPointer, &lightCullingMatrixScatterPrepass.uniformData, sizeof(LightCullingMatrixScatterPrepassUniformVariables));
        vkCmdBindPipeline(lightCullingMatrixScatterPrepass.commandBuffers[frame], VK_PIPELINE_BIND_POINT_COMPUTE, lightCullingMatrixScatterPrepass.computePipeline);
        vkCmdBindDescriptorSets(lightCullingMatrixScatterPrepass.commandBuffers[frame], VK_PIPELINE_BIND_POINT_COMPUTE, lightCullingMatrixScatterPrepass.pipelineLayout, 0, 1, &lightCullingMatrixScatterPrepass.descriptorSets[frame], 0, nullptr);
        vkCmdDispatch(lightCullingMatrixScatterPrepass.commandBuffers[frame], scene.numTiles2D.x, scene.numTiles2D.y, 1);
        vkEndCommandBuffer(lightCullingMatrixScatterPrepass.commandBuffers[frame]);
    }

    static void Submit(EngineInfo& engine, DepthPrepassInfo& depthPrepass, LightCullingMatrixScatterPrepassInfo& lightCullingMatrixScatterPrepass, uint32_t frame)
    {
        VkSubmitInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &lightCullingMatrixScatterPrepass.commandBuffers[frame];
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &depthPrepass.finishedSemaphores[frame];
        VkPipelineStageFlags stageFlag = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        info.pWaitDstStageMask = &stageFlag;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &lightCullingMatrixScatterPrepass.finishedSemaphores[frame];
        
        if(vkQueueSubmit(engine.graphicsQueue, 1, &info, lightCullingMatrixScatterPrepass.fences[frame]) != VK_SUCCESS)
            throw std::runtime_error("\nFAILURE TO SUBMIT QUEUE\n");
    }
};
