# CMake minimum version required
cmake_minimum_required(VERSION 3.10)

# Project name (this will also be the default name for your .sln file and executable)
project(SimpleEngine LANGUAGES CXX)

# Set C++ standard (e.g., C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Define Project Structure Variables (for clarity) ---
# CMAKE_CURRENT_SOURCE_DIR is the directory where this CMakeLists.txt is located
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PROJECT_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

# --- Include Directories ---
# For your project's headers (e.g., include/MyFirstEngine/GameObject.h)
include_directories(${PROJECT_INCLUDE_DIR})

# For GLAD headers (e.g., third_party/glad/include/glad/glad.h)
include_directories(${THIRD_PARTY_DIR}/glad/include)

# For GLFW headers (e.g., third_party/glfw/include/GLFW/glfw3.h)
include_directories(${THIRD_PARTY_DIR}/glfw/include)


# --- Source Files ---
# Add all your .cpp source files from the src/ directory
# Also include glad.c from its new location
add_executable(SimpleEngine
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/Renderer.cpp
    ${PROJECT_SOURCE_DIR}/Shader.cpp
    ${PROJECT_SOURCE_DIR}/glad.c  # Assuming you moved glad.c to src/
)

# --- Find and Link Libraries ---

# Find OpenGL (typically part of the system)
find_package(OpenGL REQUIRED)

# Find GLFW - Try to find it, but don't fail immediately if not found
find_package(glfw3)

if (glfw3_FOUND)
    message(STATUS "Found GLFW via find_package(). Linking automatically.")
    target_link_libraries(SimpleEngine PRIVATE glfw) 
else()
    message(WARNING "GLFW not found by find_package(). Attempting manual linking.")
    
    # Define the path to your GLFW library file (e.g., third_party/glfw/lib/glfw3.lib)
    set(GLFW_LIBRARY_FILE ${THIRD_PARTY_DIR}/glfw/lib/glfw3.lib)
    
    if(EXISTS ${GLFW_LIBRARY_FILE})
        message(STATUS "Manually linking GLFW library: ${GLFW_LIBRARY_FILE}")
        target_link_libraries(SimpleEngine PRIVATE ${GLFW_LIBRARY_FILE})
    else()
        message(FATAL_ERROR "Manual GLFW linking failed: Library not found at ${GLFW_LIBRARY_FILE}. "
                            "Please check the path and ensure the library file exists.")
    endif()
endif()

# Link OpenGL to your executable
target_link_libraries(SimpleEngine PRIVATE OpenGL::GL)

# --- Platform Specifics ---
if (APPLE)
    target_link_libraries(SimpleEngine PRIVATE "-framework Cocoa -framework IOKit -framework CoreVideo")
endif()

# --- Shader Files ---
# Copy shader files from assets/shaders/ to the build output directory
set(SHADER_FILES
    ${PROJECT_ASSETS_DIR}/shaders/triangle.vert
    ${PROJECT_ASSETS_DIR}/shaders/triangle.frag
)

foreach(SHADER_FILE_PATH ${SHADER_FILES})
    # Get just the filename from the full path
    get_filename_component(SHADER_FILENAME ${SHADER_FILE_PATH} NAME)
    
    if(NOT EXISTS ${SHADER_FILE_PATH})
        message(WARNING "Shader source file not found: ${SHADER_FILE_PATH}. It will not be copied.")
    else()
        add_custom_command(
            TARGET SimpleEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SHADER_FILE_PATH}"                         # Source (full path to shader)
            "$<TARGET_FILE_DIR:SimpleEngine>/shaders/${SHADER_FILENAME}" # Destination (e.g., build/Debug/shaders/triangle.vert)
            COMMENT "Copying shader: ${SHADER_FILENAME} to shaders/ subdirectory"
        )
    endif()
endforeach()

# --- Output ---
message(STATUS "CMake configuration complete for SimpleEngine.")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Executable will be: $<TARGET_FILE:SimpleEngine>")
message(STATUS "Shaders will be copied to: $<TARGET_FILE_DIR:SimpleEngine>/shaders/")
