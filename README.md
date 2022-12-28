# The Worst Engine

The game engine based on OpenGL

![TWE2022-12-27 193127](https://user-images.githubusercontent.com/82779713/209701495-714ab995-ecad-4477-a941-4d85389fef81.png)

## Requirements
1. Visual Studio 16 2019

2. GNU Make: Recommended version - 3.81

3. CMake: Recommended version - 3.20

4. OpenCL SDK

5. External libs `(link: https://github.com/Parasik72/TWE-external-libs)`

## Downloading OpenCL SDK
First of all you have to download OpenCl SDK for your graphics hardware`(example for Nvidia users is CUDA Toolkit)`.

Afterwards you have to specify the path to your OpenCL SDK files in CMakeLists file`(these paths are marked with comments)`.

## Build
Firstly recommended to create a new directory`(example "mkdir build")` and move to`(example "cd build")`.

The next step is generating build files:
>`cmake .. -G "Visual Studio 16 2019"`

Afterwards you have to build the project:
>`cmake --build .`

## Author
[Dmytro Vorobets](https://github.com/Parasik72)
