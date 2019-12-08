# OpenGLPractice
CMC MSU 2020 OpenGL practice task.

## Windows building
All relevant libraries are found in /lib, just create the CMake project in VS `https://docs.microsoft.com/ru-ru/cpp/build/cmake-projects-in-visual-studio?view=vs-2019` or build directly with CMake.

## Linux building
```
apt-get install g++ cmake git
apt-get install libglm-dev libglew-dev libglfw3-dev libxinerama-dev libxcursor-dev  libxi-dev
mkdir build
cd build
cmake ..
make
```

CodeBlocks or VS or other IDEs can have troubles finding files, so set the environment variable:

    `export LOGL_ROOT_PATH=/your_path/openglpractice`

## Mac OS X building

```
brew install cmake glm glfw
mkdir build
cd build
cmake ..
make
```
