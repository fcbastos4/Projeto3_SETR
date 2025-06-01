# Set the source files that make up the library
set(unity_lib
   unity.c unity.h unity_internals.h
)

# Set the library type (can be static or dynamic )
set(LIB_TYPE STATIC)

# Create the library
add_library(unity ${LIB_TYPE} ${unity_lib})