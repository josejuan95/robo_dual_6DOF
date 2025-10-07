cmake_minimum_required(VERSION 3.8)
project(controlador_plataforma)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Busca dependencias de ROS2
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

# Busca la librería ncurses del sistema
find_package(Curses REQUIRED)

include_directories(
  include
  ${CURSES_INCLUDE_DIR}
)

add_executable(controlador_plataforma_node src/main.cpp)
ament_target_dependencies(controlador_plataforma_node rclcpp geometry_msgs sensor_msgs)
target_link_libraries(controlador_plataforma_node ${CURSES_LIBRARIES})

install(TARGETS
  controlador_plataforma_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()

