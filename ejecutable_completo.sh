#!/bin/bash

source /opt/ros/jazzy/setup.bash
source ./ros2_ws/install/setup.bash
~/Programas/CoppeliaSim/coppeliaSim ./escenas/ESCENA_7.ttt -s0 > /dev/null 2>&1 &
sleep 5
gnome-terminal -- bash -c "source /opt/ros/jazzy/setup.bash && source ./ros2_ws/install/setup.bash && ros2 run controlador_plataforma controlador_plataforma_node;exec bash"
