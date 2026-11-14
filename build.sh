cmake ../

make

glslc ../Resources/Shaders/Main/Main.vert -o ../Resources/Shaders/Main/vert.spv

glslc ../Resources/Shaders/Main/Main.frag -o ../Resources/Shaders/Main/frag.spv

glslc ../Resources/Shaders/LightCullingAtomicScatter/LightCullingAtomicScatter.comp -o ../Resources/Shaders/LightCullingAtomicScatter/LightCullingAtomicScatter.spv

glslc ../Resources/Shaders/LightCullingAtomicScatter/LightCullingAtomicScatterPrepass.comp -o ../Resources/Shaders/LightCullingAtomicScatter/LightCullingAtomicScatterPrepass.spv


glslc ../Resources/Shaders/LightCullingMatrixScatter/LightCullingMatrixScatter.comp -o ../Resources/Shaders/LightCullingMatrixScatter/LightCullingMatrixScatter.spv

glslc ../Resources/Shaders/LightCullingMatrixScatter/LightCullingMatrixScatterPrepass.comp -o ../Resources/Shaders/LightCullingMatrixScatter/LightCullingMatrixScatterPrepass.spv

glslc ../Resources/Shaders/LightCullingMatrixScatter/LightCullingMatrixScatterPostPass.comp -o ../Resources/Shaders/LightCullingMatrixScatter/LightCullingMatrixScatterPostPass.spv

glslc ../Resources/Shaders/LightCullingGather/LightCullingGather.comp -o ../Resources/Shaders/LightCullingGather/LightCullingGather.spv

glslc ../Resources/Shaders/DepthPrepass/DepthPrepass.vert -o ../Resources/Shaders/DepthPrepass/DepthPrepass_vs.spv

rm -f imgui.ini
