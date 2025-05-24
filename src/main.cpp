#include <iostream>
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

// Window dimensions
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

Camera camera(Vec3(0.0f, 1.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GameObject triangleObject(0, "MyTriangle");

Framebuffer* sceneFramebuffer = nullptr;
ImVec2 sceneViewSize(1, 1);
bool sceneViewFocused = false;
bool sceneViewHovered = false;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard || sceneViewFocused) {
        if (sceneViewFocused) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard("FORWARD", deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard("BACKWARD", deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard("LEFT", deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard("RIGHT", deltaTime);
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.processKeyboard("UP", deltaTime);
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.processKeyboard("DOWN", deltaTime);
        }
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        bool canControlCamera = ImGui::IsMouseDown(ImGuiMouseButton_Right) && sceneViewHovered;
        if (!canControlCamera) {
            firstMouse = true;
            return;
        }
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

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && sceneViewHovered) {
        camera.processMouseMovement(xoffset, yoffset);
    } else {
        firstMouse = true;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse && !sceneViewHovered)
        return;

    if (sceneViewHovered) {
        camera.processMouseScroll(static_cast<float>(yoffset));
    }
}

int main() {
    if (!glfwInit()) { /* ... error ... */ return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SimpleEngine Editor", NULL, NULL);
    if (!window) { /* ... error ... */ glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { /* ... error ... */ return -1; }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

#ifdef ImGuiConfigFlags_DockingEnable // Check if the flag exists
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif
#ifdef ImGuiConfigFlags_ViewportsEnable // Check if the flag exists
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


    ImGui::StyleColorsDark();
#ifdef ImGuiConfigFlags_ViewportsEnable
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
#endif

    const char* glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Renderer renderer;
    if (!renderer.init()) { /* ... error ... */ return -1; }
    triangleObject.transform.position = Vec3(0.0f, 0.0f, 0.0f);
    sceneFramebuffer = new Framebuffer(static_cast<int>(sceneViewSize.x), static_cast<int>(sceneViewSize.y));

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Poll events early

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Setup Dockspace
#ifdef ImGuiConfigFlags_DockingEnable // Check if docking is available/enabled
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
#ifdef IMGUI_HAS_VIEWPORT // ImGui::SetNextWindowViewport needs this
            ImGui::SetNextWindowViewport(viewport->ID);
#endif
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
#ifndef ImGuiWindowFlags_NoDocking // Check if this specific flag exists
            window_flags |= ImGuiWindowFlags_NoDocking; 
#endif

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
            ImGui::PopStyleVar();
            ImGui::PopStyleVar(2);

            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
#ifdef IMGUI_HAS_DOCK // ImGui::DockSpace needs this
    #ifdef ImGuiDockNodeFlags_PassthruCentralNode
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    #else
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0); // Fallback if flag doesn't exist
    #endif
#endif
            ImGui::End();
        }
#endif // ImGuiConfigFlags_DockingEnable


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoCollapse);
        {
            sceneViewFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
            sceneViewHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
            ImVec2 currentSceneViewSize = ImGui::GetContentRegionAvail();
            if (currentSceneViewSize.x > 0 && currentSceneViewSize.y > 0 &&
                (currentSceneViewSize.x != sceneViewSize.x || currentSceneViewSize.y != sceneViewSize.y)) {
                sceneViewSize = currentSceneViewSize;
                if (sceneFramebuffer) {
                    sceneFramebuffer->resize(static_cast<int>(sceneViewSize.x), static_cast<int>(sceneViewSize.y));
                }
            }
            if (sceneFramebuffer && sceneFramebuffer->getColorTexture() != 0 && sceneViewSize.x > 0 && sceneViewSize.y > 0) {
                ImTextureID texture_id_to_render = (ImTextureID)(intptr_t)sceneFramebuffer->getColorTexture();
                const ImVec2 image_display_size = sceneViewSize; // sceneViewSize is already ImVec2
                const ImVec2 uv0 = ImVec2(0.0f, 1.0f); // Explicitly use floats
                const ImVec2 uv1 = ImVec2(1.0f, 0.0f); // Explicitly use floats

                ImGui::Image(texture_id_to_render, image_display_size, uv0, uv1);
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::Begin("Inspector");
        {
            ImGui::Text("Triangle Object"); ImGui::DragFloat3("Position##Tri", &triangleObject.transform.position.x, 0.01f);
            ImGui::Separator();
            ImGui::Text("Camera"); ImGui::Text("Pos: %.2f, %.2f, %.2f", camera.position.x, camera.position.y, camera.position.z);
            ImGui::Text("Yaw: %.2f, Pitch: %.2f", camera.yaw, camera.pitch); ImGui::SliderFloat("FOV", &camera.fov, 1.0f, 120.0f);
        }
        ImGui::End();
        ImGui::ShowDemoWindow();

        if (sceneFramebuffer && sceneViewSize.x > 0 && sceneViewSize.y > 0) {
            sceneFramebuffer->bind();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Mat4 modelMatrix = Mat4::translate(triangleObject.transform.position);
            Mat4 viewMatrix = camera.getViewMatrix();
            float sceneAspectRatio = (sceneViewSize.y == 0) ? 1.0f : static_cast<float>(sceneViewSize.x) / static_cast<float>(sceneViewSize.y);
            Mat4 projectionMatrix = camera.getProjectionMatrix(sceneAspectRatio);
            renderer.draw(modelMatrix, viewMatrix, projectionMatrix);
            sceneFramebuffer->unbind();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Main window background, if desired
        // glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#ifdef ImGuiConfigFlags_ViewportsEnable
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
#ifdef IMGUI_HAS_VIEWPORT // ImGui::UpdatePlatformWindows and RenderPlatformWindowsDefault need this
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
#endif
        }
#endif

        glfwSwapBuffers(window);
    }

    delete sceneFramebuffer;
    sceneFramebuffer = nullptr;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}