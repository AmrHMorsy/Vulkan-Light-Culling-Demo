#pragma once
#include "Includes.h"

#define LINEAR 0
#define LOGARITHMIC 1

#define GATHER 0
#define MATRIX_SCATTER 1
#define ATOMIC_SCATTER 2

#define _8x8 0
#define _16x16 1
#define _32x32 2

const float PI = 3.14159;
const int NUM_SUPPORTED_TILE_RESOLUTIONS = 3;
const int MAX_NUM_THREADS_PER_WORKGROUP = 1024;
const int MAX_NUM_LIGHTS = 8192;
const int MAX_NUM_MODELS = 150;
const int MAX_FRAMES_IN_FLIGHT = 3;

const std::array<glm::uvec2, NUM_SUPPORTED_TILE_RESOLUTIONS> tileResolutions{
    glm::uvec2(8, 8),
    glm::uvec2(16, 16),
    glm::uvec2(32, 32)
};

std::unordered_map<int, int> tileResolutionsMap{
    {8, _8x8},
    {16, _16x16},
    {32, _32x32}
};

const std::vector<const char *> tileResolutionStrings = {
    "8 x 8",
    "16 x 16",
    "32 x 32"
};

const std::vector<const char *> lightCullingApproaches = {
    "Gather",
    "Matrix Scatter",
    "Atomic Scatter"
};

std::unordered_map<std::string, int> lightCullingApproachMap{
    {"gather", GATHER},
    {"matrixscatter", MATRIX_SCATTER},
    {"atomicscatter", ATOMIC_SCATTER}
};

const uint SHOW_ALBEDO                   = 1u << 0 ;
const uint SHOW_NORMAL                   = 1u << 1 ;
const uint SHOW_DEPTH                    = 1u << 2 ;
const uint SHOW_HEAT_MAP                 = 1u << 3 ;
const uint FORWARD_PIPELINE              = 1u << 4 ;
const uint FORWARD_PLUS_PIPELINE         = 1u << 5 ;
const uint DEPTH_SLICING_LINEAR          = 1u << 6 ;
const uint DEPTH_SLICING_LOGARITHMIC     = 1u << 7 ;
const uint LIGHT_CULLING_25D             = 1u << 8 ;
const uint STOCHASTIC_LIGHT_CULLING      = 1u << 9 ;


struct LightConfig{
    float intensity = 1.0f;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 position;
};

struct CameraConfig{
    float speed;
    float fovDegrees;
    glm::vec4 position;
    glm::vec4 direction;
    float cameraFarPlane;
    float lookAroundSpeed;
    float cameraNearPlane;
};

struct ModelConfig{
    std::string filePath;
    std::string albedoFilePath = "";
};

struct SceneConfig{
    float depthPowerValue;
    float lightIntensity;
    std::string baseFilePath;
    std::string sceneName;
    std::string name;
    CameraConfig cameraConfig;
    std::vector<ModelConfig> models;
};

std::unordered_map<std::string, SceneConfig> sceneConfigs{
    {
        "sponza",
        SceneConfig{
            .depthPowerValue = 10000.0f,
            .lightIntensity = 10000.0f,
            .sceneName = "sponza.obj",
            .baseFilePath = "../Resources/assets/sponza/",
            .name = "Sponza",
            .cameraConfig = CameraConfig{
                .speed = 2.0,
                .fovDegrees = 45.0,
                .lookAroundSpeed = 0.05,
                .cameraNearPlane = 0.1,
                .cameraFarPlane = 5000.0,
                .position = glm::vec4(874.27, 573.227, -24.1419, 1.0f),
                .direction = glm::vec4(-0.994953, -0.100303, 0.00284207, 0.0f)
            }
        }
    },
    {
        "bistro",
        SceneConfig{
            .depthPowerValue = 50.0f,
            .lightIntensity = 70.0f,
            .sceneName = "Bistro.obj",
            .baseFilePath = "../Resources/assets/Bistro/",
            .name = "Bistro",
            .cameraConfig = CameraConfig{
                .speed = 0.05,
                .fovDegrees = 45.0,
                .lookAroundSpeed = 0.05,
                .cameraNearPlane = 0.1,
                .cameraFarPlane = 500.0,
                .position = glm::vec4(-26.8892, 3.28912, -5.91592, 1.0f),
                .direction = glm::vec4(0.968075, 0.0563501, 0.244249, 0.0f)
            }
        }
    },
    {
        "sibenik",
        SceneConfig{
            .depthPowerValue = 50.0f,
            .lightIntensity = 70.0f,
            .sceneName = "sibenik.obj",
            .baseFilePath = "../Resources/assets/sibenik/",
            .name = "sibenik",
            .cameraConfig = CameraConfig{
                .speed = 0.05,
                .fovDegrees = 45.0,
                .lookAroundSpeed = 0.05,
                .cameraNearPlane = 0.1,
                .cameraFarPlane = 500.0,
                .position = glm::vec4(-19.8822, -12.717, -3.8208, 1.0f),
                .direction = glm::vec4(0.968075, 0.0563501, 0.244249, 0.0f)
            }
        }
    }
};
