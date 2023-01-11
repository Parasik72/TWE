#ifndef MODEL_LOADER_DATA_HPP
#define MODEL_LOADER_DATA_HPP

#include <vector>

#include "scene/components/components.hpp"

namespace TWE {
    struct ModelLoaderData {
        ModelLoaderData(const std::vector<MeshComponent>& meshComponents, const std::string& fullPath, ShaderIndices vert, ShaderIndices frag);
        std::vector<MeshComponent> meshComponents;
        std::string fullPath;
        ShaderIndices vert;
        ShaderIndices frag;
    };
}

#endif