cmake_minimum_required(VERSION 2.8.12)
project(shm_test)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

add_executable(${PROJECT_NAME}
  main.cpp
)

target_link_libraries(${PROJECT_NAME} 
	ff
)
