# CMake minimum version required
cmake_minimum_required(VERSION 3.10)

# Project name (this will also be the default name for your .sln file and executable)
project(SimpleEngine LANGUAGES CXX C)

# Set C++ standard (e.g., C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS OFF)


# --- Define Project Structure Variables (for clarity) ---
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_SOURCE_DIR ${PROJECT_ROOT_DIR}/src)
set(PROJECT_INCLUDE_DIR ${PROJECT_ROOT_DIR}/include)
set(PROJECT_ASSETS_DIR ${PROJECT_ROOT_DIR}/assets)
set(THIRD_PARTY_DIR ${PROJECT_ROOT_DIR}/third_party)

# --- Source Files ---
add_executable(SimpleEngine
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/Renderer.cpp
    ${PROJECT_SOURCE_DIR}/Shader.cpp
    ${PROJECT_SOURCE_DIR}/Camera.cpp
    ${PROJECT_SOURCE_DIR}/glad.c
    ${PROJECT_SOURCE_DIR}/Framebuffer.cpp
)

# Define BUNDLED_GLFW_INCLUDE_DIR early for use by ImGuiLib
set(BUNDLED_GLFW_INCLUDE_DIR ${THIRD_PARTY_DIR}/glfw/include) # Moved up

# --- Include Directories for SimpleEngine ---
target_include_directories(SimpleEngine PRIVATE
    ${PROJECT_INCLUDE_DIR}
    ${THIRD_PARTY_DIR}/glad/include
)

# --- Find and Link Libraries ---
find_package(OpenGL REQUIRED)
target_link_libraries(SimpleEngine PRIVATE OpenGL::GL)

# --- GLFW ---
find_package(glfw3 QUIET)
if (glfw3_FOUND)
    message(STATUS "Found system GLFW via find_package(). Linking target 'glfw'.")
    target_link_libraries(SimpleEngine PRIVATE glfw)
else()
    message(STATUS "System GLFW not found by find_package(). Attempting to use bundled GLFW.")
    # BUNDLED_GLFW_INCLUDE_DIR already set
    set(BUNDLED_GLFW_LIBRARY_FILE ${THIRD_PARTY_DIR}/glfw/lib/glfw3.lib)

    if(NOT EXISTS ${BUNDLED_GLFW_INCLUDE_DIR})
        message(FATAL_ERROR "Bundled GLFW include directory not found: ${BUNDLED_GLFW_INCLUDE_DIR}")
    endif()
    if(NOT EXISTS ${BUNDLED_GLFW_LIBRARY_FILE})
        message(FATAL_ERROR "Bundled GLFW library file not found: ${BUNDLED_GLFW_LIBRARY_FILE}")
    endif()

    message(STATUS "Using bundled GLFW: ${BUNDLED_GLFW_LIBRARY_FILE}")
    add_library(glfw_bundled STATIC IMPORTED)
    set_target_properties(glfw_bundled PROPERTIES
        IMPORTED_LOCATION "${BUNDLED_GLFW_LIBRARY_FILE}"
        INTERFACE_INCLUDE_DIRECTORIES "${BUNDLED_GLFW_INCLUDE_DIR}"
    )
    target_link_libraries(SimpleEngine PRIVATE glfw_bundled)
endif()

# --- Dear ImGui ---
set(IMGUI_DIR ${THIRD_PARTY_DIR}/imgui)
add_library(ImGuiLib STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp       # <-- ADD THIS LINE
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(ImGuiLib PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${BUNDLED_GLFW_INCLUDE_DIR}
)
target_link_libraries(SimpleEngine PRIVATE ImGuiLib)


# --- Platform Specifics ---
if (APPLE)
    target_link_libraries(SimpleEngine PRIVATE "-framework Cocoa -framework IOKit -framework CoreVideo")
endif()

# --- Shader Files ---
set(SHADER_FILES
    ${PROJECT_ASSETS_DIR}/shaders/triangle.vert
    ${PROJECT_ASSETS_DIR}/shaders/triangle.frag
)
foreach(SHADER_FILE_PATH ${SHADER_FILES})
    get_filename_component(SHADER_FILENAME ${SHADER_FILE_PATH} NAME)
    if(NOT EXISTS ${SHADER_FILE_PATH})
        message(WARNING "Shader source file not found: ${SHADER_FILE_PATH}. It will not be copied.")
    else()
        add_custom_command(
            TARGET SimpleEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SHADER_FILE_PATH}"
            "$<TARGET_FILE_DIR:SimpleEngine>/shaders/${SHADER_FILENAME}"
            COMMENT "Copying shader: ${SHADER_FILENAME} to shaders/ subdirectory"
        )
    endif()
endforeach()

message(STATUS "CMake configuration complete for SimpleEngine.")
message(STATUS "Project Root directory: ${PROJECT_ROOT_DIR}")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Executable will be: $<TARGET_FILE:SimpleEngine>")
message(STATUS "Shaders will be copied to: $<TARGET_FILE_DIR:SimpleEngine>/shaders/")