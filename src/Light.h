#pragma once
#include "Data.h"
#include "Includes.h"
#include "Structs.h"
#include <random>


class Light{
    
public:
    
    static void Build(EngineInfo& engine, SceneInfo& scene, float intensity, int numLights)
    {
        std::vector<LightConfig> lightConfigs = GenerateLights(scene.model.max, scene.model.min, intensity, numLights);
        
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
        for(int i = 0; i < lightConfigs.size(); i++){
            LightInfo light;
            light.random01 = dist01(rng);
            light.config = lightConfigs[i];
            light.maxLightIntensityComponent = ComputeMaxLightIntensityComponent(lightConfigs[i].color, lightConfigs[i].intensity);
            light.varianceControlValue = ((2.0f * PI * scene.stochasticQualityValue) / (light.maxLightIntensityComponent));
            light.boundingSphere = ComputeBoundingSphere(lightConfigs[i].position, light.maxLightIntensityComponent, light.varianceControlValue, light.random01);
            light.position = lightConfigs[i].position;
            scene.lights.push_back(light);
            scene.lightNames.push_back("Light " + std::to_string(i+1));
        }
        
        scene.lightNamesPtr.resize(scene.lights.size());
        for(uint32_t i = 0; i < scene.lights.size(); i++)
            scene.lightNamesPtr[i] = scene.lightNames[i].c_str();
    }
    
    static void UpdateErrorMax(SceneInfo& scene)
    {
        for(int i = 0; i < scene.lights.size(); i++){
            scene.lights[i].varianceControlValue = ((2.0f * PI * scene.stochasticQualityValue) / (scene.lights[i].maxLightIntensityComponent));
            scene.lights[i].boundingSphere = ComputeBoundingSphere(scene.lights[i].position, scene.lights[i].maxLightIntensityComponent, scene.lights[i].varianceControlValue, scene.lights[i].random01);
        }
    }
    
    static float ComputeMaxLightIntensityComponent(glm::vec3 lightColor, float lightIntensity)
    {
        glm::vec3 color = lightIntensity * lightColor;
        
        return std::max({color.r, color.g, color.b});
    }
        
    static glm::vec4 ComputeBoundingSphere(glm::vec4 lightPosition, float maxLightIntensityComponent, float varianceControlValue, float random01)
    {
        glm::vec3 center = lightPosition;
        float radius = (1.0f / (sqrt(varianceControlValue * random01)));
        
        return glm::vec4(center, radius);
    }
            
    static std::vector<LightConfig> GenerateLights(glm::vec3 max, glm::vec3 min, float intensity, int numLights)
    {
        std::vector<LightConfig> lightConfigs;
        
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> distX(min.x, max.x);
        std::uniform_real_distribution<float> distY(min.y, max.y);
        std::uniform_real_distribution<float> distZ(min.z, max.z);
        
        for(uint32_t i = 0; i < numLights; ++i){
            glm::vec3 pos = {distX(rng), distY(rng), distZ(rng)};
            lightConfigs.push_back(LightConfig{
                .intensity = intensity,
                .color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                .position = glm::vec4(pos, 1.0f)
            });
        }
        
        return lightConfigs;
    }
};
