cmake_minimum_required(VERSION 3.31.3)

# Укажите имя проекта
project(MainServer)

# Установите стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указываем пути для поиска заголовочных файлов и бинарных файлов
set(OpenSSL_INCLUDE_DIR "D:/Repo/Group-Project-course-1-semester-1/module main/lib/include/openssl")
set(OpenSSL_LIBRARIES "D:/Repo/Group-Project-course-1-semester-1/module main/lib/lib")

# Указываем путь для jwt-cpp
set(jwt-cpp_INCLUDE_DIR "D:/Repo/Group-Project-course-1-semester-1/module main/lib/include/jwt-cpp")

# Указываем пути для поиска библиотек
include_directories(${OpenSSL_INCLUDE_DIR} ${jwt-cpp_INCLUDE_DIR})

# Укажите исходные файлы проекта
add_executable(main_server main.cpp handleClient.cpp getAddress.cpp)

# Привязываем библиотеки
target_link_libraries(main_server PRIVATE
  ${OpenSSL_LIBRARIES}/libcrypto.lib
  ${jwt-cpp_LIBRARIES}
)
