#pragma once
#include "Includes.h"



class Material{
    
public:
    
    static void Load(EngineInfo& engine, const aiScene * scene, std::vector<MaterialInfo>& materials, std::string baseFilePath)
    {
        if(scene->mNumMaterials >= MAX_NUM_MODELS)
            throw std::runtime_error("\nNUM_MODELS >= MAX_NUM_MODELS");
        
        bool success;
        materials.resize(scene->mNumMaterials);
        for(uint32_t i = 0; i < scene->mNumMaterials; i++){
            success = false;
            aiString albedoFilePath;
            if(scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &albedoFilePath) == AI_SUCCESS)
                success = true;
            else if(scene->mMaterials[i]->GetTexture(aiTextureType_BASE_COLOR, 0, &albedoFilePath) == AI_SUCCESS)
                success = true;
            
            if(success){
                materials[i].config.albedoFilePath = albedoFilePath.C_Str();
                materials[i].config.albedoFilePath = baseFilePath + materials[i].config.albedoFilePath;
                std::replace(materials[i].config.albedoFilePath.begin(), materials[i].config.albedoFilePath.end(), '\\', '/');
                Texture::Build(engine, materials[i].albedo, materials[i].config.albedoFilePath);
            }
            
            aiColor4D color4D;
            if(scene->mMaterials[i]->Get(AI_MATKEY_BASE_COLOR, color4D) == AI_SUCCESS)
                materials[i].kd = glm::vec4(color4D.r, color4D.g, color4D.b, color4D.a);
            else{
                aiColor3D color3D;
                if(scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, color3D) == AI_SUCCESS)
                    materials[i].kd = glm::vec4(color3D.r, color3D.g, color3D.b, 1.0f);
            }

            std::cout << materials[i].kd.r << " " << materials[i].kd.g << " " << materials[i].kd.b << std::endl;
        }
    }
};
