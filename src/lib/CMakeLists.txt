set(sensorcmd_lib
   cmd_proc.h cmd_proc.c sensor_temp.c sensor_temp.h RTDB.c RTDB.h task_control.c task_control.h
)

# Set the library type (can be static or dynamic )
set(LIB_TYPE STATIC)

# Create the library
add_library(sensorcmdLib ${LIB_TYPE} ${sensorcmd_lib})