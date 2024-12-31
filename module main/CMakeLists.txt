cmake_minimum_required(VERSION 3.31.3)

# Укажите имя проекта
project(ConsoleApp)

# Установите стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Укажите исходные файлы проекта
add_executable(ConsoleApp main.cpp)
