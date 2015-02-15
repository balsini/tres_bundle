cmake_minimum_required (VERSION 2.6)
project (tres_base)

# Header files are in "include/tres" (global header files,
# to be shared across libraries and bindings), and in "src"
# (local to this projects)
include_directories(include)
include_directories(src)

# The code is inside the directory "src"
add_subdirectory (src)

# Export. FIXME
#export(PACKAGE tres_base)
