#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::max, std::min, std::swap
#include <cmath>     // For FLT_MAX, std::sqrt, etc.

#include "glad/glad.h"
#include <GLFW/glfw3.h>

// Engine-specific headers
#include "MyFirstEngine/GameObject.h" 
#include "SimpleMath.h"               
#include "MyFirstEngine/Renderer.h"
#include "MyFirstEngine/Camera.h"
#include "MyFirstEngine/Framebuffer.h"

// ImGui Headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// --- Global Variables ---
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

Camera editorCamera(Vec3(0.0f, 2.0f, 7.0f), Vec3(0.0f, 0.5f, 0.0f));
double lastX = static_cast<double>(SCR_WIDTH) / 2.0;
double lastY = static_cast<double>(SCR_HEIGHT) / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int GameObject::nextID = 0;
std::vector<GameObject> sceneGameObjects;
GameObject* selectedGameObject = nullptr;

Framebuffer* sceneFramebuffer = nullptr;
ImVec2 sceneViewSize(1.0f, 1.0f); // Start with minimal valid, will be updated
bool sceneViewFocused = false;
bool sceneViewHovered = false;
bool RMB_Pressed_in_SceneView = false;

// Globals to store Scene View window properties for picking
ImVec2 g_SceneViewWindowPos;     
ImVec2 g_SceneViewContentMinRel; 


// --- Picking Function ---
void PerformMousePicking(float mouseX_scene_content, float mouseY_scene_content, 
                         float sceneView_content_Width, float sceneView_content_Height,
                         const Mat4& viewMatrix, const Mat4& projectionMatrix) {
    if (sceneGameObjects.empty() || sceneView_content_Width <= 0 || sceneView_content_Height <= 0) return;

    // 1. Normalized Device Coordinates (NDC)
    float ndcX = (2.0f * mouseX_scene_content) / sceneView_content_Width - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY_scene_content) / sceneView_content_Height; // Y is flipped

    // 2. Create Ray in World Space
    Mat4 invProjection = projectionMatrix.inverse();
    Mat4 invView = viewMatrix.inverse();

    Vec4 ray_clip_near(ndcX, ndcY, -1.0f, 1.0f); 
    Vec4 ray_clip_far(ndcX, ndcY, 1.0f, 1.0f);   

    Vec4 ray_eye_near = invProjection * ray_clip_near;
    Vec4 ray_eye_far  = invProjection * ray_clip_far;

    if (std::abs(ray_eye_near.w) > 1e-6f) { 
        ray_eye_near.x /= ray_eye_near.w; ray_eye_near.y /= ray_eye_near.w; ray_eye_near.z /= ray_eye_near.w;
    } else { /* Handle error or assume w=1 if appropriate for ortho */ ray_eye_near.w = 1.0f; }
    if (std::abs(ray_eye_far.w) > 1e-6f) { 
        ray_eye_far.x /= ray_eye_far.w; ray_eye_far.y /= ray_eye_far.w; ray_eye_far.z /= ray_eye_far.w;
    } else { /* Handle error */ ray_eye_far.w = 1.0f; }

    Vec3 ray_world_near_pt = Mat4::transformPoint(invView, Vec3(ray_eye_near.x, ray_eye_near.y, ray_eye_near.z));
    Vec3 ray_world_far_pt  = Mat4::transformPoint(invView, Vec3(ray_eye_far.x, ray_eye_far.y, ray_eye_far.z));
    
    Ray pickRay(ray_world_near_pt, (ray_world_far_pt - ray_world_near_pt).normalize());
    
    GameObject* hitObject = nullptr;
    float closest_t = FLT_MAX;

    for (size_t i = 0; i < sceneGameObjects.size(); ++i) {
        Vec3 halfExtents = sceneGameObjects[i].transform.scale * 0.5f; 
        AABB objectAABB(sceneGameObjects[i].transform.position, halfExtents); 
        
        float t_intersection;
        if (intersectRayAABB(pickRay, objectAABB, t_intersection)) {
            if (t_intersection >= 0 && t_intersection < closest_t) {
                closest_t = t_intersection;
                hitObject = &sceneGameObjects[i];
            }
        }
    }

    if (hitObject) {
        selectedGameObject = hitObject;
        std::cout << "Picked: " << selectedGameObject->name << " (ID: " << selectedGameObject->id << ")" << std::endl;
        if (selectedGameObject) { 
            editorCamera.setFocalPoint(selectedGameObject->transform.position);
        }
    } else {
        std::cout << "Picked: Nothing" << std::endl;
    }
}


// --- GLFW Callbacks ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (width > 0 && height > 0) {
        glViewport(0, 0, width, height); SCR_WIDTH = width; SCR_HEIGHT = height;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO(); // ImGui's own callback (if chained) already ran
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
    if (action == GLFW_PRESS && key == GLFW_KEY_F && sceneViewFocused && selectedGameObject) {
        editorCamera.setFocalPoint(selectedGameObject->transform.position);
    }
}

void processKeyboardInput(GLFWwindow *window) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard && !sceneViewFocused) return;
    if (sceneViewFocused) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) editorCamera.processKeyboardFPS("FORWARD", deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) editorCamera.processKeyboardFPS("BACKWARD", deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) editorCamera.processKeyboardFPS("LEFT", deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) editorCamera.processKeyboardFPS("RIGHT", deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) editorCamera.processKeyboardFPS("UP", deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) editorCamera.processKeyboardFPS("DOWN", deltaTime);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    bool processed_by_engine_interaction = false;

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS && sceneViewHovered) {
            RMB_Pressed_in_SceneView = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwGetCursorPos(window, &lastX, &lastY); 
            firstMouse = true; 
            processed_by_engine_interaction = true;
        } else if (action == GLFW_RELEASE) {
            if (RMB_Pressed_in_SceneView) { 
                RMB_Pressed_in_SceneView = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                processed_by_engine_interaction = true;
            }
        }
    }

    // If ImGui wants the mouse AND the engine didn't just handle an interaction (like starting RMB orbit)
    if (io.WantCaptureMouse && !processed_by_engine_interaction) {
        return; 
    }

    // LMB Picking Logic - only if ImGui doesn't want the mouse OR if the click is for picking (not on an ImGui widget)
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && sceneViewHovered && !RMB_Pressed_in_SceneView) {
        // A simple check: if ImGui isn't using any item, allow picking.
        // This avoids picking when clicking on ImGui window title bars if they overlap scene view.
        // For more robustness, you'd check if the mouse is truly over the rendered image part of scene view
        // and not over some potential future UI element within the scene view.
        if (!ImGui::IsAnyItemActive() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) { 
            ImVec2 mainMousePos = ImGui::GetMousePos(); 

            // Use the globally updated g_SceneViewWindowPos and g_SceneViewContentMinRel
            float mouseX_in_scene_content = mainMousePos.x - g_SceneViewWindowPos.x - g_SceneViewContentMinRel.x;
            float mouseY_in_scene_content = mainMousePos.y - g_SceneViewWindowPos.y - g_SceneViewContentMinRel.y;
            
            // sceneViewSize is the ImGui::GetContentRegionAvail() from the Scene View window
            if (mouseX_in_scene_content >= 0 && mouseX_in_scene_content < sceneViewSize.x &&
                mouseY_in_scene_content >= 0 && mouseY_in_scene_content < sceneViewSize.y &&
                sceneViewSize.x > 0 && sceneViewSize.y > 0) { // Ensure sceneViewSize is valid
                
                Mat4 currentViewMatrix = editorCamera.getViewMatrix();
                float currentSceneAspectRatio = sceneViewSize.x / sceneViewSize.y;
                Mat4 currentProjectionMatrix = editorCamera.getProjectionMatrix(currentSceneAspectRatio);

                PerformMousePicking(mouseX_in_scene_content, mouseY_in_scene_content, 
                                    sceneViewSize.x, sceneViewSize.y,
                                    currentViewMatrix, currentProjectionMatrix);
            }
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xposIn, double yposIn) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse && !RMB_Pressed_in_SceneView) {
        firstMouse = true; return;
    }
    float xpos = static_cast<float>(xposIn); float ypos = static_cast<float>(yposIn);
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoffset = xpos - static_cast<float>(lastX);
    float yoffset = static_cast<float>(lastY) - ypos;
    lastX = xpos; lastY = ypos;
    if (RMB_Pressed_in_SceneView) editorCamera.processMouseOrbit(xoffset, yoffset);
    else if (sceneViewHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) editorCamera.processMousePan(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse && !sceneViewHovered) return;
    if (sceneViewHovered) editorCamera.processMouseZoom(static_cast<float>(yoffset));
}

AABB::AABB(const GameObject& go) {
    Vec3 halfExtents = go.transform.scale * 0.5f; 
    min = go.transform.position - halfExtents;
    max = go.transform.position + halfExtents;
}

int main() {
    if (!glfwInit()) { std::cerr << "Failed to initialize GLFW" << std::endl; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SimpleEngine Editor", NULL, NULL);
    if (!window) { std::cerr << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cerr << "Failed to initialize GLAD" << std::endl; glfwTerminate(); return -1; }

    IMGUI_CHECKVERSION(); ImGui::CreateContext(); ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark(); ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) { style.WindowRounding = 0.0f; style.Colors[ImGuiCol_WindowBg].w = 1.0f; }
    ImGui_ImplGlfw_InitForOpenGL(window, true); ImGui_ImplOpenGL3_Init("#version 330 core");

    Renderer renderer; if (!renderer.init()) { std::cerr << "Renderer init failed" << std::endl; /* cleanup */ return -1; }
    
    sceneGameObjects.emplace_back("Triangle Alpha"); sceneGameObjects.back().transform.position = Vec3(0.0f, 0.0f, 0.0f);
    sceneGameObjects.emplace_back("Cube Beta"); 
        sceneGameObjects.back().transform.position = Vec3(1.5f, 0.0f, 0.0f); 
        sceneGameObjects.back().transform.scale = Vec3(0.5f,0.5f,0.5f);
        sceneGameObjects.back().transform.rotation = Vec3(0.0f, 45.0f, 30.0f);
    sceneGameObjects.emplace_back("Ground Plane"); 
        sceneGameObjects.back().transform.position = Vec3(0.0f, -0.75f, 0.0f); 
        sceneGameObjects.back().transform.scale = Vec3(5.0f, 0.1f, 5.0f);

    if (!sceneGameObjects.empty()) { selectedGameObject = &sceneGameObjects[0]; if (selectedGameObject) editorCamera.setFocalPoint(selectedGameObject->transform.position); }
    sceneFramebuffer = new Framebuffer(static_cast<int>(sceneViewSize.x), static_cast<int>(sceneViewSize.y));

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        float cf = static_cast<float>(glfwGetTime()); deltaTime = cf - lastFrame; lastFrame = cf;
        processKeyboardInput(window);

        ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
        
        ImGuiViewport* vp = ImGui::GetMainViewport(); ImGui::SetNextWindowPos(vp->WorkPos); ImGui::SetNextWindowSize(vp->WorkSize); ImGui::SetNextWindowViewport(vp->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGuiWindowFlags wf = ImGuiWindowFlags_NoDocking|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoBringToFrontOnFocus|ImGuiWindowFlags_NoNavFocus;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f,0.0f)); ImGui::Begin("DockSpaceWindow", nullptr, wf); ImGui::PopStyleVar(); ImGui::PopStyleVar(2);
        ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f,0.0f), ImGuiDockNodeFlags_PassthruCentralNode); ImGui::End();

        ImGui::Begin("Hierarchy");
        for (size_t i = 0; i < sceneGameObjects.size(); ++i) {
            ImGui::PushID(static_cast<int>(sceneGameObjects[i].id));
            bool is_sel = (selectedGameObject == &sceneGameObjects[i]);
            if (ImGui::Selectable(sceneGameObjects[i].name.c_str(), is_sel)) {
                selectedGameObject = &sceneGameObjects[i];
                if (selectedGameObject) editorCamera.setFocalPoint(selectedGameObject->transform.position);
            }
            if (is_sel) ImGui::SetItemDefaultFocus();
            ImGui::PopID();
        } ImGui::End();

        ImGui::Begin("Inspector");
        if (selectedGameObject) {
            ImGui::Text("Name: %s (ID: %u)", selectedGameObject->name.c_str(), selectedGameObject->id); ImGui::Separator();
            ImGui::Text("Transform");
            if (ImGui::DragFloat3("Position##Insp", &selectedGameObject->transform.position.x, 0.01f)) editorCamera.setFocalPoint(selectedGameObject->transform.position);
            ImGui::DragFloat3("Rotation##Insp", &selectedGameObject->transform.rotation.x, 1.0f); 
            ImGui::DragFloat3("Scale##Insp", &selectedGameObject->transform.scale.x, 0.01f);
            selectedGameObject->transform.scale.x = std::max(0.001f,selectedGameObject->transform.scale.x); selectedGameObject->transform.scale.y = std::max(0.001f,selectedGameObject->transform.scale.y); selectedGameObject->transform.scale.z = std::max(0.001f,selectedGameObject->transform.scale.z);
        } else { ImGui::Text("No object selected."); }
        ImGui::Separator(); ImGui::Text("EditorCam"); ImGui::Text("P:%.1f,%.1f,%.1f F:%.1f,%.1f,%.1f",editorCamera.position.x,editorCamera.position.y,editorCamera.position.z,editorCamera.focalPoint.x,editorCamera.focalPoint.y,editorCamera.focalPoint.z);
        ImGui::SliderFloat("FOV",&editorCamera.fov,1,120);
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f,0.0f));
        ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoCollapse);
        {
            g_SceneViewWindowPos = ImGui::GetWindowPos();           // Store for picking
            g_SceneViewContentMinRel = ImGui::GetWindowContentRegionMin(); // Store for picking

            sceneViewFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows); 
            sceneViewHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
            ImVec2 cws = ImGui::GetContentRegionAvail();
            if (cws.x > 0 && cws.y > 0) {
                if (cws.x != sceneViewSize.x || cws.y != sceneViewSize.y) {
                    sceneViewSize = cws; 
                    if (sceneFramebuffer) sceneFramebuffer->resize(static_cast<int>(cws.x), static_cast<int>(cws.y));
                }
                if (sceneFramebuffer && sceneFramebuffer->getColorTexture()!=0) 
                    ImGui::Image((ImTextureID)sceneFramebuffer->getColorTexture(), sceneViewSize, ImVec2(0,1), ImVec2(1,0));
            } 
        }
        ImGui::End(); ImGui::PopStyleVar();
        
        ImGui::ShowDemoWindow();

        if (sceneFramebuffer && sceneFramebuffer->getWidth()>0 && sceneFramebuffer->getHeight()>0) {
            sceneFramebuffer->bind(); glEnable(GL_DEPTH_TEST);
            glClearColor(0.1f,0.12f,0.15f,1.0f); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Mat4 vM = editorCamera.getViewMatrix();
            float sar = static_cast<float>(sceneFramebuffer->getWidth())/std::max(1.0f,static_cast<float>(sceneFramebuffer->getHeight()));
            Mat4 pM = editorCamera.getProjectionMatrix(sar);
            for (const auto& go : sceneGameObjects) {
                Mat4 transMatrix = Mat4::translate(go.transform.position);
                Mat4 rotMatrix   = Mat4::rotateEuler(go.transform.rotation); 
                Mat4 scaleMatrix = Mat4::scale(go.transform.scale);
                Mat4 modelMatrix = transMatrix * rotMatrix * scaleMatrix; 
                renderer.draw(modelMatrix, vM, pM);
            } sceneFramebuffer->unbind();
        }
        ImGui::Render();
        int dw, dh; glfwGetFramebufferSize(window, &dw, &dh); glViewport(0,0,dw,dh);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* bctx = glfwGetCurrentContext(); ImGui::UpdatePlatformWindows(); ImGui::RenderPlatformWindowsDefault(); glfwMakeContextCurrent(bctx);
        }
        glfwSwapBuffers(window);
    }
    delete sceneFramebuffer;
    ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown(); ImGui::DestroyContext();
    glfwTerminate(); return 0;
}