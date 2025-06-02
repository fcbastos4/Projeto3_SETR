cmake_minimum_required(VERSION 3.13)
project(UnityTests C)

include_directories(../src/lib ../unity)

add_definitions(-DUNIT_TEST)

add_executable(test_runner testing.c ../src/lib/cmd_proc.c ../src/lib/RTDB.c ../unity/unity.c) 