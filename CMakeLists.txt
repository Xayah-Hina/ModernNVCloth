cmake_minimum_required(VERSION 3.24)
project(ModernNVCloth VERSION 1.0.0 DESCRIPTION "Proceed to Modern NV Cloth" LANGUAGES CXX)

add_executable(ModernNVCloth)
set_target_properties(ModernNVCloth PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_sources(ModernNVCloth PRIVATE main.cpp)
