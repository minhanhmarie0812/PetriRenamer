cmake_minimum_required(VERSION 3.21)
project(PetriRenamerCpp VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)
qt_add_executable(PetriRenamerCpp WIN32
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
)
target_link_libraries(PetriRenamerCpp PRIVATE Qt6::Widgets)
if(MSVC)
  target_compile_options(PetriRenamerCpp PRIVATE /W4 /permissive- /utf-8)
else()
  target_compile_options(PetriRenamerCpp PRIVATE -Wall -Wextra -Wpedantic)
endif()
install(TARGETS PetriRenamerCpp RUNTIME DESTINATION .)
