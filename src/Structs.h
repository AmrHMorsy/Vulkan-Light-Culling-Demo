#pragma once
#include "Data.h"
#include "Includes.h"


struct ImageInfo{
    VkImage image;
    VkDeviceMemory imageMemory;
};

struct BufferInfo{
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
};

struct HostVisibleBuffer{
    void * memoryPointer;
    BufferInfo bufferPack;
};

struct WindowInfo{
    float xScale;
    int fbWidth;
    int fbHeight;
    int windowWidth;
    int windowHeight;
    GLFWwindow * handle;
};

struct MaterialConfig{
    std::string albedoFilePath;
};

struct TextureInfo{
    VkSampler sampler = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
};

struct MaterialInfo{
    glm::vec4 kd = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    TextureInfo albedo;
    MaterialConfig config;
};

struct Mesh{
    std::vector<uint32_t> indices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoordinates;
};

struct LightInfo{
    float random01;
    LightConfig config;
    glm::vec4 position;
    glm::vec4 boundingSphere;
    float varianceControlValue;
    float maxLightIntensityComponent;
};

struct CameraInfo{
    glm::vec4 up;
    glm::vec4 right;
    float fovRadians;
    float angle = 0.0f;
    glm::vec4 direction;
    CameraConfig config;
    glm::mat4 viewMatrix;
    VkExtent2D resolution;
    glm::mat4 projectionMatrix;
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 lookAroundRotation = glm::mat4(1.0f);
};

struct ModelShaderUniform{
    struct Vertex {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 projection;
    };
    struct alignas(16) Fragment {
        uint flags;
        uint numLights;
        float cameraNearPlane;
        float cameraFarPlane;
        uint numCellsPerTile;
        float depthPowerValue;
        glm::uvec2 tileResolution;
        glm::vec2 screenResolution;
        glm::uvec2 numTiles2D;
        glm::vec4 cameraPosition;
        std::array<glm::vec4, MAX_NUM_MODELS> kd;
        std::array<glm::vec4, MAX_NUM_LIGHTS> lightPositions;
        std::array<glm::vec4, MAX_NUM_LIGHTS> lightColors;
        std::array<glm::vec4, MAX_NUM_LIGHTS> lightVarianceControlValues;
    };
};

struct ModelInfo{
    uint32_t numDrawCommands;
    VkPipeline graphicsPipeline;
    uint32_t totalNumTriangles = 0;
    VkDescriptorPool descriptorPool;
    VkPipelineLayout pipelineLayout;
    ModelShaderUniform::Vertex vsData;
    BufferInfo textureCoordinateBuffer;
    uint flags = (1u << 5) | (1u << 7) | (1u << 8) | (1u << 9);
    ModelShaderUniform::Fragment fsData;
    std::vector<VkBuffer> vertexBuffers;
    std::vector<MaterialInfo> materials;
    BufferInfo vertexBuffer, normalBuffer;
    BufferInfo indexBuffer, drawCommandsBuffer;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkDeviceSize> vertexBufferOffsets;
    glm::vec3 max = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);
    glm::vec3 min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    std::vector<HostVisibleBuffer> vertexShaderUniformBuffers;
    std::vector<HostVisibleBuffer> fragmentShaderUniformBuffers;
    std::vector<VkDescriptorBufferInfo> fragmentShaderUniformDescriptorBuffers;
    std::vector<std::vector<VkDescriptorImageInfo>> albedoDescriptorImages;
    std::vector<VkDescriptorBufferInfo> vertexShaderUniformDescriptorBuffers;
};

struct DepthPrepassShaderUniform{
    struct Vertex{
        alignas(16) glm::mat4 cameraViewMatrix;
        alignas(16) glm::mat4 cameraProjectionMatrix;
    };
};

struct DepthPrepassInfo{
    VkSampler sampler;
    VkRenderPass renderPass;
    std::vector<VkFence> fences;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkBuffer> vertexBuffers;
    std::vector<VkImageView> imageViews;
    std::vector<VkFramebuffer> frameBuffers;
    DepthPrepassShaderUniform::Vertex vsData;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkSemaphore> finishedSemaphores;
    std::vector<VkDeviceSize> vertexBufferOffsets;
    std::vector<VkDescriptorImageInfo> descriptorImages;
    std::vector<HostVisibleBuffer> vertexShaderUniformBuffers;
};

struct LightCullingAtomicScatterPrepassUniformVariables{
    float cameraNearPlane;
    float cameraFarPlane;
    glm::uvec2 numTiles2D;
    unsigned int numCellsPerTile;
    uint flags;
    glm::uvec2 tileResolution;
};

struct LightCullingAtomicScatterPrepassInfo{
    std::vector<BufferInfo> maxLightCountBuffers;
    std::vector<VkDescriptorBufferInfo> maxLightCountDescriptorBuffers;
    std::vector<BufferInfo> tileLightCountBuffers;
    std::vector<VkDescriptorBufferInfo> tileLightCountDescriptorBuffers;
    
    std::vector<VkFence> fences;
    VkPipelineLayout pipelineLayout;
    VkPipeline computePipeline;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> finishedSemaphores;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<HostVisibleBuffer> uniformBuffers;
    LightCullingAtomicScatterPrepassUniformVariables uniformData;
    std::vector<VkDescriptorBufferInfo> tilesGeometryBitMaskDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> tileLightCullingPrepassDescriptorBuffers;
};

struct LightCullingAtomicScatterUniformVariables{
    float cameraNearPlane;
    float cameraFarPlane;
    glm::uvec2 numTiles2D;
    unsigned int numCellsPerTile;
    uint flags;
    glm::uvec2 tileResolution;
    glm::vec2 screenResolution;
};

struct LightCullingAtomicScatterInfo{
    uint32_t numThreadsPerWorkgroup, numWorkgroups;
    
    std::vector<BufferInfo> tileLightIndicesBuffers;
    VkPipeline computePipeline;
    
    std::vector<VkFence> fences;
    VkPipelineLayout pipelineLayout;
    VkDescriptorPool descriptorPool;
    LightCullingAtomicScatterUniformVariables uniformData;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkSemaphore> finishedSemaphores;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<HostVisibleBuffer> uniformBuffers;
    std::vector<glm::vec4> lightBoundingSpheresViewSpace;
    std::vector<HostVisibleBuffer> tilesFrustumPlanesBuffers;
    std::vector<HostVisibleBuffer> lightBoundingSphereViewSpaceBuffers;
    std::vector<VkDescriptorBufferInfo> tileLightIndicesDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> lightBoundingSphereViewSpaceDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> tilesFrustumPlanesDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> uniformDescriptorBuffers;
};

struct LightCullingMatrixScatterPostPassUniformVariables{
    glm::uvec2 numTiles2D;
    glm::uvec2 tileResolution;
    uint numLights;
    glm::vec3 padding;
};

struct LightCullingMatrixScatterPostPassInfo{
    VkPipeline computePipeline;
    std::vector<VkFence> fences;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> finishedSemaphores;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<HostVisibleBuffer> uniformBuffers;
    std::vector<BufferInfo> tileLightIndicesBuffers;
    LightCullingMatrixScatterPostPassUniformVariables uniformData;
    std::vector<VkDescriptorBufferInfo> tileLightIndicesDescriptorBuffers;
};

struct LightCullingMatrixScatterPrepassUniformVariables{
    float cameraNearPlane;
    float cameraFarPlane;
    glm::uvec2 numTiles2D;
    unsigned int numCellsPerTile;
    uint flags;
    glm::uvec2 tileResolution;
    uint numLights;
    glm::vec3 padding;
};

struct LightCullingMatrixScatterPrepassInfo{
    std::vector<BufferInfo> LightTileIndicesBuffers;
    std::vector<VkDescriptorBufferInfo> lightTileIndicesDescriptorBuffers;
    
    std::vector<BufferInfo> maxLightCountBuffers;
    std::vector<VkDescriptorBufferInfo> maxLightCountDescriptorBuffers;
    std::vector<BufferInfo> tileLightCountBuffers;
    std::vector<VkDescriptorBufferInfo> tileLightCountDescriptorBuffers;
    
    std::vector<VkFence> fences;
    VkPipelineLayout pipelineLayout;
    VkPipeline computePipeline;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> finishedSemaphores;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<HostVisibleBuffer> uniformBuffers;
    LightCullingMatrixScatterPrepassUniformVariables uniformData;
    std::vector<VkDescriptorBufferInfo> tilesGeometryBitMaskDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> tileLightCullingPrepassDescriptorBuffers;
};

struct LightCullingMatrixScatterUniformVariables{
    float cameraNearPlane;
    float cameraFarPlane;
    glm::uvec2 numTiles2D;
    unsigned int numCellsPerTile;
    uint flags;
    glm::uvec2 tileResolution;
    glm::vec2 screenResolution;
};

struct LightCullingMatrixScatterInfo{
    uint32_t numThreadsPerWorkgroup, numWorkgroups;
    VkPipeline computePipeline;
    
    std::vector<VkFence> fences;
    VkPipelineLayout pipelineLayout;
    VkDescriptorPool descriptorPool;
    LightCullingMatrixScatterUniformVariables uniformData;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkSemaphore> finishedSemaphores;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<HostVisibleBuffer> uniformBuffers;
    std::vector<glm::vec4> lightBoundingSpheresViewSpace;
    std::vector<HostVisibleBuffer> tilesFrustumPlanesBuffers;
    std::vector<HostVisibleBuffer> lightBoundingSphereViewSpaceBuffers;
    std::vector<VkDescriptorBufferInfo> lightBoundingSphereViewSpaceDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> tilesFrustumPlanesDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> uniformDescriptorBuffers;
};

struct LightCullingGatherUniformVariables{
    float cameraNearPlane;
    float cameraFarPlane;
    glm::uvec2 numTiles2D;
    unsigned int numCellsPerTile;
    uint flags;
    glm::uvec2 tileResolution;
};

struct LightCullingGatherInfo{
    std::vector<VkFence> fences;
    VkPipelineLayout pipelineLayout;
    VkDescriptorPool descriptorPool;
    VkPipeline computePipeline;
    
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkSemaphore> finishedSemaphores;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<BufferInfo> maxLightCountBuffers;
    std::vector<BufferInfo> tileLightCountBuffers;
    std::vector<HostVisibleBuffer> uniformBuffers;
    LightCullingGatherUniformVariables uniformData;
    std::vector<BufferInfo> tileLightIndicesBuffers;
    std::vector<glm::vec4> lightBoundingSpheresViewSpace;
    std::vector<HostVisibleBuffer> tilesFrustumPlanesBuffers;
    std::vector<VkDescriptorBufferInfo> uniformDescriptorBuffers;
    std::vector<HostVisibleBuffer> lightBoundingSphereViewSpaceBuffers;
    std::vector<VkDescriptorBufferInfo> maxLightCountDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> tileLightCountDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> tileLightIndicesDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> tilesFrustumPlanesDescriptorBuffers;
    std::vector<VkDescriptorBufferInfo> lightBoundingSphereViewSpaceDescriptorBuffers;
};

struct SceneInfo{
    float stochasticQualityValue = 0.1;
    LightCullingGatherInfo lightCullingGather;
    
    LightCullingAtomicScatterInfo lightCullingAtomicScatter;
    LightCullingAtomicScatterPrepassInfo lightCullingAtomicScatterPrepass;
    
    LightCullingMatrixScatterInfo lightCullingMatrixScatter;
    LightCullingMatrixScatterPrepassInfo lightCullingMatrixScatterPrepass;
    LightCullingMatrixScatterPostPassInfo lightCullingMatrixScatterPostPass;

    
    
    glm::uvec2 numTiles2D;
    unsigned int totalNumTiles;
    std::vector<glm::vec4> tilesFrustumPlanes;

    unsigned int minNumCellsPerTile = 2;
    unsigned int maxNumCellsPerTile = 32;

    int selectedTileResolution;
    unsigned int numCellsPerTile = 32;
    int lightCullingApproach = GATHER;
    SceneConfig config;
    bool displayDepth = false;
    ModelInfo model;
    int selectedLight;
    CameraInfo camera;
    bool displayAlbedo = false;
    bool displayNormal = false;
    bool heatMap = false;
    VkRenderPass renderPass;
    VkExtent2D screenResolution;
    VkPipeline graphicsPipeline;
    bool applyToAllLights = false;
    DepthPrepassInfo depthPrepass;
    std::vector<LightInfo> lights;
    std::vector<VkRect2D> scissors;
    bool lightCullingRecorded = false;
    bool lightCullingEnabled = true;
    VkPipelineLayout pipelineLayout;
    std::vector<VkViewport> viewports;
    std::vector<std::string> lightNames;
    std::vector<VkFence> inflightFences;
    std::vector<VkClearValue> clearValues;
    std::vector<VkFramebuffer> frameBuffers;
    std::vector<const char *> lightNamesPtr;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> renderFinishedSemaphores;
};

struct UIInfo{
    VkRect2D scissor;
    VkViewport viewport;
    VkRenderPass renderPass;
    std::vector<VkFence> inflightFences;
    std::vector<VkClearValue> clearValues;
    std::vector<VkFramebuffer> frameBuffers;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> renderFinishedSemaphores;
};

struct SwapChainInfo{
    VkExtent2D extent;
    uint32_t imageCount;
    VkSwapchainKHR handle;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    VkSurfaceFormatKHR swapChainSurface;
    VkPresentModeKHR swapChainPresentMode;
};

struct PhysicalDeviceInfo{
    uint32_t queueIndex;
    VkPhysicalDevice handle;
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct EngineInfo{
    uint32_t imageIndex;
    VkQueue presentQueue;
    VkQueue graphicsQueue;
    VkFormat depthFormat;
    VkDevice logicalDevice;
    SwapChainInfo swapChain;
    bool isQueueBusy = false;
    VkInstance vulkanInstance;
    VkSurfaceKHR windowSurface;
    PhysicalDeviceInfo physicalDevice;
    std::vector<VkSemaphore> imageAvailableSemaphores;
};

struct ApplicationInfo{
    UIInfo ui;
    SceneInfo scene;
    EngineInfo engine;
    WindowInfo window;
    uint32_t frame = 0;
};
