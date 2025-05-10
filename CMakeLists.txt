# CMake minimum version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(SimpleEngine LANGUAGES CXX)

# Set C++ standard (e.g., C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Include Directories ---
# Add the directory containing glad.h and KHR/khrplatform.h
# This assumes glad/include/ is relative to this CMakeLists.txt
include_directories(glad/include)

# Add GLFW include directory. This assumes you have a folder structure like:
# MyFirstEngine/  <- Project Root (where this CMakeLists.txt is)
# |-- include/
# |   |-- GLFW/
# |       |-- glfw3.h
# |-- lib/
# |   |-- glfw3.lib
# |-- glad/
# ... other files
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


# --- Source Files ---
# Add all your .cpp source files
# This includes the glad.c file
add_executable(SimpleEngine
    main.cpp
    Renderer.cpp
    Shader.cpp
    glad/src/glad.c  # Add the GLAD source file
)

# --- Find and Link Libraries ---

# Find OpenGL (typically part of the system)
find_package(OpenGL REQUIRED)

# Find GLFW - Try to find it, but don't fail immediately if not found
find_package(glfw3)

if (glfw3_FOUND)
    message(STATUS "Found GLFW via find_package(). Linking automatically.")
    target_link_libraries(SimpleEngine PRIVATE glfw) # CMake uses 'glfw' as the target name
else()
    message(WARNING "GLFW not found by find_package(). Attempting manual linking.")
    
    # Define the path to your GLFW library file
    # Assumes glfw3.lib is in MyFirstEngine/lib/
    set(GLFW_LIBRARY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib)
    
    # Check if the library file actually exists at the specified path
    if(EXISTS ${GLFW_LIBRARY_FILE})
        message(STATUS "Manually linking GLFW library: ${GLFW_LIBRARY_FILE}")
        target_link_libraries(SimpleEngine PRIVATE ${GLFW_LIBRARY_FILE})
    else()
        # If the file doesn't exist, stop with an error.
        # Common issues:
        # 1. Path is incorrect.
        # 2. Library file is missing from the 'lib' folder.
        # 3. Library name is different (e.g. glfw.lib, libglfw3.a for MinGW).
        #    Make sure it matches the actual file name in your lib folder.
        message(FATAL_ERROR "Manual GLFW linking failed: Library not found at ${GLFW_LIBRARY_FILE}. "
                            "Please check the path and ensure the library file (e.g., glfw3.lib for MSVC) exists.")
    endif()
endif()

# Link OpenGL to your executable
target_link_libraries(SimpleEngine PRIVATE OpenGL::GL) # Modern way to link OpenGL

# --- Platform Specifics ---
# For macOS, you often need to link against specific frameworks
if (APPLE)
    target_link_libraries(SimpleEngine PRIVATE "-framework Cocoa -framework IOKit -framework CoreVideo")
endif()

# --- Shader Files ---
# This section ensures shaders are copied to the build directory.
set(SHADER_FILES
    triangle.vert
    triangle.frag
)

foreach(SHADER_FILE ${SHADER_FILES})
    add_custom_command(
        TARGET SimpleEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}" # Source (assumes shaders are in the same dir as CMakeLists.txt)
        "$<TARGET_FILE_DIR:SimpleEngine>"            # Destination (directory where the executable is built)
        COMMENT "Copying shader: ${SHADER_FILE}"
    )
endforeach()

# --- Output ---
# Optional: Print a message when configuration is done
message(STATUS "CMake configuration complete for SimpleEngine.")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Executable will be: $<TARGET_FILE:SimpleEngine>")
