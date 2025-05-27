#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::max, std::min

#include "glad/glad.h"
#include <GLFW/glfw3.h>

// Engine-specific headers
#include "MyFirstEngine/Renderer.h"
#include "MyFirstEngine/GameObject.h"
#include "MyFirstEngine/Camera.h"
#include "MyFirstEngine/Framebuffer.h"
#include "SimpleMath.h"

// ImGui Headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// --- Global Variables ---
// Window dimensions
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

// Camera
Camera editorCamera(Vec3(0.0f, 2.0f, 7.0f), Vec3(0.0f, 0.5f, 0.0f)); // Start further back, looking slightly down
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Scene Objects
unsigned int GameObject::nextID = 0; // Define static member for GameObject IDs
std::vector<GameObject> sceneGameObjects;
GameObject* selectedGameObject = nullptr;

// ImGui Related
Framebuffer* sceneFramebuffer = nullptr;
ImVec2 sceneViewSize(1.0f, 1.0f); // Start with minimal valid size
bool sceneViewFocused = false;
bool sceneViewHovered = false;
bool RMB_Pressed_in_SceneView = false; // Track if RMB was pressed while scene view was active


// --- Callbacks and Input Processing ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (width > 0 && height > 0) { // Ensure valid dimensions
        glViewport(0, 0, width, height);
        SCR_WIDTH = width;
        SCR_HEIGHT = height;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // ImGui's own key callback (set by ImGui_ImplGlfw_InitForOpenGL with true)
    // will be called first and will update io.WantCaptureKeyboard.
    // Then, this user callback (if set before ImGui_ImplGlfw_Init) will be called.
    
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }

    // Further engine-specific single-press key logic can go here,
    // after checking io.WantCaptureKeyboard
    // if (io.WantCaptureKeyboard) return;
    // ... your engine key press/release logic ...
}

void processKeyboardInput(GLFWwindow *window) {
    // This function is called every frame for continuous input (polling)
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard && !sceneViewFocused) {
        return;
    }

    if (sceneViewFocused) { // Only allow FPS camera movement if scene view is focused
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) editorCamera.processKeyboardFPS("FORWARD", deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) editorCamera.processKeyboardFPS("BACKWARD", deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) editorCamera.processKeyboardFPS("LEFT", deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) editorCamera.processKeyboardFPS("RIGHT", deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) editorCamera.processKeyboardFPS("UP", deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) editorCamera.processKeyboardFPS("DOWN", deltaTime);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // ImGui's callback chain will handle its needs.
    ImGuiIO& io = ImGui::GetIO();
    
    // Allow RMB interaction with scene view even if ImGui technically wants mouse capture
    // (e.g. for dragging an ImGui window title bar, but we still want RMB orbit if over scene)
    bool allow_engine_rmb_interaction = (button == GLFW_MOUSE_BUTTON_RIGHT && sceneViewHovered);

    if (io.WantCaptureMouse && !allow_engine_rmb_interaction) {
        // If ImGui wants mouse and it's not our special RMB case for scene view,
        // we might still need to handle RMB release if an orbit was active.
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE && RMB_Pressed_in_SceneView) {
            RMB_Pressed_in_SceneView = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        return; // ImGui handles it
    }
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS && sceneViewHovered) {
            RMB_Pressed_in_SceneView = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true; 
        } else if (action == GLFW_RELEASE) {
            if (RMB_Pressed_in_SceneView) { 
                RMB_Pressed_in_SceneView = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xposIn, double yposIn) {
    // ImGui's callback chain handles its needs.
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse && !RMB_Pressed_in_SceneView) {
        firstMouse = true; // Reset if ImGui is capturing and we're not orbiting
        return;
    }
    
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (RMB_Pressed_in_SceneView) {
        editorCamera.processMouseOrbit(xoffset, yoffset);
    } else if (sceneViewHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        editorCamera.processMousePan(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // ImGui's callback chain handles its needs.
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse && !sceneViewHovered) {
        return;
    }

    if (sceneViewHovered) {
        editorCamera.processMouseZoom(static_cast<float>(yoffset));
    }
}

// --- Main Function ---
int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 2. Create GLFW Window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SimpleEngine Editor", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // --- Set YOUR GLFW callbacks BEFORE ImGui_ImplGlfw_InitForOpenGL ---
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback); // Handles single key presses/releases
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // glfwSetCharCallback(...) // If you need text input for engine directly

    // 3. Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 3.5. Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    const char* glsl_version = "#version 330 core";
    // true: ImGui will install its own callbacks AND chain to your previously set callbacks.
    ImGui_ImplGlfw_InitForOpenGL(window, true); 
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 4. Initialize Renderer
    Renderer renderer;
    if (!renderer.init()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown(); ImGui::DestroyContext();
        glfwTerminate(); return -1;
    }

    // Create GameObjects
    sceneGameObjects.emplace_back("Triangle Alpha");
    sceneGameObjects.back().transform.position = Vec3(0.0f, 0.0f, 0.0f);
    sceneGameObjects.emplace_back("Triangle Beta");
    sceneGameObjects.back().transform.position = Vec3(2.0f, 0.5f, -0.5f);
    sceneGameObjects.back().transform.scale = Vec3(0.5f, 0.5f, 0.5f);
    sceneGameObjects.emplace_back("Ground Plane");
    sceneGameObjects.back().transform.position = Vec3(0.0f, -1.0f, 0.0f);
    sceneGameObjects.back().transform.scale = Vec3(5.0f, 0.1f, 5.0f);

    if (!sceneGameObjects.empty()) {
        selectedGameObject = &sceneGameObjects[0];
    }

    sceneFramebuffer = new Framebuffer(static_cast<int>(sceneViewSize.x), static_cast<int>(sceneViewSize.y));

    // --- 5. Main Game Loop ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); 

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processKeyboardInput(window); // For continuous key holds (engine movement)

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // DockSpace
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        // window_flags |= ImGuiWindowFlags_MenuBar; // Add if you want a menu bar for the dockspace window

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
        ImGui::PopStyleVar(); // WindowPadding
        ImGui::PopStyleVar(2); // WindowRounding, WindowBorderSize

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::End(); // End DockSpaceWindow

        // --- Hierarchy Window ---
        ImGui::Begin("Hierarchy");
        {
            for (size_t i = 0; i < sceneGameObjects.size(); ++i) {
                ImGui::PushID(static_cast<int>(sceneGameObjects[i].id));
                bool is_selected = (selectedGameObject == &sceneGameObjects[i]);
                if (ImGui::Selectable(sceneGameObjects[i].name.c_str(), is_selected)) {
                    selectedGameObject = &sceneGameObjects[i];
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
                ImGui::PopID();
            }
        }
        ImGui::End();

        // --- Inspector Window ---
        ImGui::Begin("Inspector");
        {
            if (selectedGameObject) {
                ImGui::Text("Name: %s", selectedGameObject->name.c_str());
                ImGui::Text("ID: %u", selectedGameObject->id);
                ImGui::Separator();
                ImGui::Text("Transform");
                ImGui::DragFloat3("Position##Insp", &selectedGameObject->transform.position.x, 0.01f);
                ImGui::DragFloat3("Rotation##Insp", &selectedGameObject->transform.rotation.x, 0.1f);
                ImGui::DragFloat3("Scale##Insp", &selectedGameObject->transform.scale.x, 0.01f);
                 selectedGameObject->transform.scale.x = std::max(0.001f, selectedGameObject->transform.scale.x);
                 selectedGameObject->transform.scale.y = std::max(0.001f, selectedGameObject->transform.scale.y);
                 selectedGameObject->transform.scale.z = std::max(0.001f, selectedGameObject->transform.scale.z);
            } else {
                ImGui::Text("No object selected.");
            }
            ImGui::Separator();
            ImGui::Text("Editor Camera");
            ImGui::Text("Pos: %.2f, %.2f, %.2f", editorCamera.position.x, editorCamera.position.y, editorCamera.position.z);
            ImGui::Text("Focal: %.2f, %.2f, %.2f", editorCamera.focalPoint.x, editorCamera.focalPoint.y, editorCamera.focalPoint.z);
            ImGui::Text("Dist: %.2f", editorCamera.distanceToFocalPoint);
            ImGui::Text("Yaw: %.2f, Pitch: %.2f", editorCamera.yaw, editorCamera.pitch);
            ImGui::SliderFloat("FOV", &editorCamera.fov, 1.0f, 120.0f);
        }
        ImGui::End();

        // --- Scene View Window ---
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoCollapse);
        {
            sceneViewFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
            sceneViewHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

            ImVec2 currentWindowSize = ImGui::GetContentRegionAvail();
            if (currentWindowSize.x > 0 && currentWindowSize.y > 0 &&
                (currentWindowSize.x != sceneViewSize.x || currentWindowSize.y != sceneViewSize.y)) {
                sceneViewSize = currentWindowSize;
                if (sceneFramebuffer) {
                    sceneFramebuffer->resize(static_cast<int>(sceneViewSize.x), static_cast<int>(sceneViewSize.y));
                }
            }

            if (sceneFramebuffer && sceneFramebuffer->getColorTexture() != 0 && sceneViewSize.x > 0 && sceneViewSize.y > 0) {
                ImTextureID texID = (ImTextureID)sceneFramebuffer->getColorTexture(); // Cast to ImU64
                ImGui::Image(texID, sceneViewSize, ImVec2(0, 1), ImVec2(1, 0));
            }
        }
        ImGui::End();
        ImGui::PopStyleVar(); // WindowPadding for Scene View

        ImGui::ShowDemoWindow(); // Optional for debugging ImGui features

        // --- Render 3D Scene to FBO ---
        if (sceneFramebuffer && sceneFramebuffer->getWidth() > 0 && sceneFramebuffer->getHeight() > 0) { // Check FBO valid dimensions
            sceneFramebuffer->bind();
            glEnable(GL_DEPTH_TEST); // Important: ensure depth test is on for FBO

            // Clear the FBO
            glClearColor(0.1f, 0.12f, 0.15f, 1.0f); // Scene view background
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Mat4 viewMatrix = editorCamera.getViewMatrix();
            float sceneAspectRatio = (sceneFramebuffer->getHeight() == 0) ? 1.0f : static_cast<float>(sceneFramebuffer->getWidth()) / static_cast<float>(sceneFramebuffer->getHeight());
            Mat4 projectionMatrix = editorCamera.getProjectionMatrix(sceneAspectRatio);

            for (const auto& go : sceneGameObjects) {
                Mat4 modelMatrix = Mat4::translate(go.transform.position);
                // Rudimentary scaling (proper matrix math for S*R*T or T*R*S needed for full transform)
                modelMatrix.elements[0] *= go.transform.scale.x; // Scale X
                modelMatrix.elements[5] *= go.transform.scale.y; // Scale Y
                modelMatrix.elements[10] *= go.transform.scale.z; // Scale Z
                renderer.draw(modelMatrix, viewMatrix, projectionMatrix);
            }
            sceneFramebuffer->unbind();
        }

        // --- Render ImGui to Main Window ---
        ImGui::Render(); // Gathers all ImGui draw data into ImGui::GetDrawData()

        // Set viewport to main window size and clear it (optional, if dockspace doesn't cover all)
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // e.g. black background for areas not covered by ImGui
        // glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows (if viewports are enabled)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // 6. Cleanup
    delete sceneFramebuffer;
    sceneFramebuffer = nullptr;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}