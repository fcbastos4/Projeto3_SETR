cmake_minimum_required(VERSION 3.10)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(mini_projeto)


target_sources(app PRIVATE  src/main.c  src/lib/cmd_proc.c  src/lib/cmd_proc.h src/lib/task_control.c src/lib/task_control.h src/lib/sensor_temp.c src/lib/sensor_temp.h src/lib/RTDB.c src/lib/RTDB.h) 

target_include_directories(app PRIVATE src)
