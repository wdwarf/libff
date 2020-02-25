
cmake_minimum_required(VERSION 2.6)
project(ff)
add_compile_options(-std=c++11 -g)

include_directories(
  include
)

add_compile_options(-Wno-narrowing)

file(GLOB src_files "*.cpp" "src/*.cpp" "src/*/*.cpp")

add_library(ff
	${src_files}
)
