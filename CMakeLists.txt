cmake_minimum_required(VERSION 3.20)
project(TWE)

file(GLOB_RECURSE CPPFILES 
    "src/*.cpp"
)

file(GLOB_RECURSE LIBFILES 
    "external/*.lib" 
)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Debug" "${CMAKE_BINARY_DIR}/Release")

add_executable(TWE ${CPPFILES})
target_compile_features(TWE PRIVATE cxx_std_17)
target_include_directories(TWE PRIVATE
    "include"
    "external"
    "external/GLFW"
    "external/glad"
    "external/stb"
    "external/glm"
    "external/imgui"
    "external/imgui-filedialog"
    "external/ImGuizmo"
    "external/assimp"
    "external/bullet3"
    "external/entt"
    "external/json"
)
target_link_libraries(TWE PRIVATE ${LIBFILES})

add_custom_target(ASSIMPCopy
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/bin/assimp-vc142-mtd.dll ${CMAKE_BINARY_DIR}/Debug
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/bin/assimp-vc142-mtd.dll ${CMAKE_BINARY_DIR}/Release
)
add_dependencies(TWE ASSIMPCopy)