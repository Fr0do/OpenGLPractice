# OpenGLPractice
CMC MSU 2020 OpenGL practice task. Выполнил Куркин Максим Леонидович, группа 309, maxkurkin@inbox.ru

## Эффекты:  
Кубическая текстура в режиме окружающей среды(skybox) - в polygonal,
Попиксельный расчёт освещённости - в polygonal,
Отбрасывание теней на объекты и плоскость - в polygonal,
Нечёткие тени - в polygonal, 
Normal mapping - в polygonal,
Parallax relief mapping - в polygonal,
Нестандартное освещение(модель Кука-Торранса) - в pbr,
Процедурные текстуры(тор, сфера) - в pbr.



## Сборка на Windows
Необходимые библиотеки находятся в /lib, нужно только создать CMake проект в VS `https://docs.microsoft.com/ru-ru/cpp/build/cmake-projects-in-visual-studio?view=vs-2019` или собрать напрямую с помощью CMake.

## Сборка на Linux/OS X
```
mkdir build
cd build
cmake ..
make
```
Запуск: 
```
cd ./build/bin/pbr
./pbr
 
или 

cd ./build/bin/polygonal
./polygonal
```

CodeBlocks, VS или другие IDE могут не найти нужные файлы, установите переменную окружения:

    `export LOGL_ROOT_PATH=/your_path/openglpractice`

К проекту приложены исполняемые файлы, сгенерированные на OS X.

