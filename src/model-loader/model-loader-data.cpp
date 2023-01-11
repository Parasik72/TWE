#include "model-loader/model-loader-data.hpp"

namespace TWE {
    ModelLoaderData::ModelLoaderData(const std::vector<MeshComponent>& meshComponents, const std::string& fullPath, ShaderIndices vert, ShaderIndices frag)
    : meshComponents(meshComponents), fullPath(fullPath), vert(vert), frag(frag) {}
}