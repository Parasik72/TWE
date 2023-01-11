cmake_minimum_required(VERSION 3.20)
project(TWELIB)

file(GLOB_RECURSE CPPFILES
    "../../../../src/*.cpp"
)

file(GLOB_RECURSE LIBFILES 
    "../../../../external/*.lib" 
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7/lib/*.lib" #Your OpenCL SDK lib path
)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Debug" "${CMAKE_BINARY_DIR}/Release")

add_library(TWELIB STATIC ${CPPFILES})
target_compile_features(TWELIB PRIVATE cxx_std_17)
target_include_directories(TWELIB PRIVATE
    "../../../../include"
    "../../../../external"
    "../../../../external/GLFW"
    "../../../../external/glad"
    "../../../../external/stb"
    "../../../../external/glm"
    "../../../../external/imgui"
    "../../../../external/imgui-filedialog"
    "../../../../external/ImGuizmo"
    "../../../../external/assimp"
    "../../../../external/bullet3"
    "../../../../external/entt"
    "../../../../external/json"
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7/include" #Your OpenCL SDK include path
)
target_link_libraries(TWELIB PRIVATE ${LIBFILES})