#include "Window.h"
#include "Animation.cpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "Cloth.cpp"
#include "ParticleSystem.cpp"

// Bools to switch between model and cloth sim
bool modelDraw = false;
bool clothDraw = true;
bool particleSim = false;
clock_t prevTime;


// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Model Environment";

// Objects to render
Cube* Window::cube;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

bool rotateX = false;
bool rotateY = false;
bool rotateZ = false;
bool showColor = false;
bool pause = false;
bool showWind = true;
bool showwind = false;

//Skeleton Variable
Skeleton* skel = new Skeleton();
bool drawSkel = false;

Skin* skin = new Skin();
bool drawSkin = true;

char* animFile = "C:/Users/mjalb/Desktop/CSE169/animation/wasp_walk.anim";
Animation* anim = new Animation();

// Cloth Variable
Cloth* cloth = nullptr;

//Particle System Variable
ParticleSystem* sim = nullptr;
Ground* ground = nullptr;

//File Paths
char* fileTest = "C:/Users/mjalb/Desktop/CSE169/skeletons/test.skel";
char* fileWasp = "C:/Users/mjalb/Desktop/CSE169/skeletons/wasp.skel";
char* fileDragon = "C:/Users/mjalb/Desktop/CSE169/skeletons/Dragon.skel";
char* fileTube = "C:/Users/mjalb/Desktop/CSE169/skeletons/tube.skel";

//File Variable
char* waspSkin = "C:/Users/mjalb/Desktop/CSE169/skins/wasp.skin";
char* tubeSkin = "C:/Users/mjalb/Desktop/CSE169/skins/tube.skin";

//char* skelFile = fileTube;
//char* skinFile = tubeSkin;

char* skelFile = fileWasp;
char* skinFile = waspSkin;

float times = 0;
float lastframe = 0;

// Constructors and desctructors
bool Window::initializeProgram() {
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    if (modelDraw) {
        // Intializes skeleton
        if (skel->load(skelFile))
            std::cout << "Loaded skeleton file \n";


        if (skin->load(skinFile)) {
            std::cout << "Loaded skin file \n";
        }

        if (anim->load(animFile)) {
            std::cout << "Loaded animation file \n";
        }
    }

    if (clothDraw) {
        cloth = new Cloth(20, 20);
        std::cout << "Cloth loaded\n";
    }

    if (particleSim) {
        ground = new Ground();
        sim = new ParticleSystem(ground);
        std::cout << "Sim loaded\n";
    }

    return true;
}

bool Window::initializeObjects() {
    // Create a cube
    if (modelDraw) {
        skin->getJoint(skel->getIndexJoints());
        skin->update();
    }

    return true;
}

void Window::cleanUp() {
    // Deallcoate the objects.
    delete cube;
    delete skel;
    delete skin;
    delete cloth;
    delete sim;

    // Delete the shader program.
    glDeleteProgram(shaderProgram);
}

// for the Window
GLFWwindow* Window::createWindow(int width, int height) {
    // Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    // 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    // Check if the window could not be created.
    if (!window) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return NULL;
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewInit();

    // Set swap interval to 1.
    glfwSwapInterval(0);

    // set up the camera
    Cam = new Camera();
    Cam->SetAspect(float(width) / float(height));

    // initialize the interaction variables
    LeftDown = RightDown = false;
    MouseX = MouseY = 0;

    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);

    return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
    Window::width = width;
    Window::height = height;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->SetAspect(float(width) / float(height));
}



// update and draw functions
void Window::idleCallback() {
    // Perform any updates as necessary.
    Cam->Update();
    if (clothDraw) {
        clock_t c = clock();
        float inter = ((float)c - (float)prevTime);
        prevTime = c;
        
        float delta = inter / 1000.0f;
        cloth->update(delta);
    }

    if (particleSim) {
        clock_t c = clock();
        float inter = ((float)c - (float)prevTime);
        prevTime = c;

        float delta = inter / 1000.0f;
        sim->Update(delta);
    }
}

void modelIMGUI() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Simple GUI");
        ImGui::Checkbox("Draw Skeleton ", &drawSkel);
        ImGui::Checkbox("Draw Skin ", &drawSkin);

        ImGui::Checkbox("Rotate Joints on X ", &rotateX);
        ImGui::Checkbox("Rotate Joints on Y ", &rotateY);
        ImGui::Checkbox("Rotate Joints on Z ", &rotateZ);
        ImGui::Checkbox("Change Color", &showColor);
        ImGui::Checkbox("Pause Animation", &pause);

        if (rotateX) {

            ImGui::SetNextWindowSize(ImVec2(300, 300));
            if (ImGui::Begin("Rotate X")) {

                for (int i = 0; i < skel->indexJoints.size(); i++) {
                    float min = -3.14;
                    float max = 3.14;
                    if (skel->indexJoints[i]->rotzlimit.x != -10000) min = skel->indexJoints[i]->rotxlimit.x;;
                    if (skel->indexJoints[i]->rotzlimit.y != 10000) max = skel->indexJoints[i]->rotxlimit.y;

                    ImGui::SliderFloat(skel->indexJoints[i]->name.data(), &skel->indexJoints[i]->pose.x, min, max);  //X lims
                }

            }ImGui::End();
        }
        if (rotateY) {

            ImGui::SetNextWindowSize(ImVec2(300, 300));
            if (ImGui::Begin("Rotate Y")) {

                for (int i = 0; i < skel->indexJoints.size(); i++) {
                    float min = -3.14;
                    float max = 3.14;
                    if (skel->indexJoints[i]->rotzlimit.x != -10000) min = skel->indexJoints[i]->rotylimit.x;;
                    if (skel->indexJoints[i]->rotzlimit.y != 10000) max = skel->indexJoints[i]->rotylimit.y;
                    ImGui::SliderFloat(skel->indexJoints[i]->name.data(), &skel->indexJoints[i]->pose.y, min, max);  //X lims
                }

            }ImGui::End();
        }
        if (rotateZ) {

            ImGui::SetNextWindowSize(ImVec2(300, 300));
            if (ImGui::Begin("Rotate Z")) {

                for (int i = 0; i < skel->indexJoints.size(); i++) {
                    float min = -3.14;
                    float max = 3.14;
                    if (skel->indexJoints[i]->rotzlimit.x != -10000) min = skel->indexJoints[i]->rotzlimit.x;;
                    if (skel->indexJoints[i]->rotzlimit.y != 10000) max = skel->indexJoints[i]->rotzlimit.y;
                    ImGui::SliderFloat(skel->indexJoints[i]->name.data(), &skel->indexJoints[i]->pose.z, min, max);
                }

            }ImGui::End();
        }

        if (showColor) {
            ImGui::ColorPicker4("Color", (float*)&skin->color);
        }
        ImGui::End();
        ImGui::Begin("NEW WINDOW");
        int channelcnt = 0;
        for (auto Channel : anim->channels) {
            float arr[100];

            for (int i = 0; i < 100; i++) {
                float t = anim->startTime + i * ((anim->endTime - anim->startTime) / 100);

                arr[i] = Channel->evaluate(t);
            }
            std::string named = "Channel " + std::to_string(channelcnt);
            ImGui::PlotLines(named.c_str(), arr, IM_ARRAYSIZE(arr));
            channelcnt++;
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void clothIMGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Simple GUI");
    if (ImGui::Button("Decrease X")){
        cloth->updateFixed(glm::vec3(-0.2, -0.0, 0));
    }
    if (ImGui::Button("Increase X")) {
        cloth->updateFixed(glm::vec3(0.2, 0.0, 0));
    }

    if (ImGui::Button("Decrease Y")) {
        cloth->updateFixed(glm::vec3(0.0, -0.2, 0));
    }
    if (ImGui::Button("Increase Y")) {
        cloth->updateFixed(glm::vec3(0.0, 0.2, 0));
    }

    if (ImGui::Button("Decrease Z")) {
        cloth->updateFixed(glm::vec3(0.0, 0.0, -0.2));
    }
    if (ImGui::Button("Increase Z")) {
        cloth->updateFixed(glm::vec3(0.0, 0.0, 0.2));
    }


    ImGui::Checkbox("Change Color", &showColor);
    if (showColor) {
       ImGui::ColorPicker4("Color", (float*)&cloth->color);
    }

    ImGui::Checkbox("Wind", &showWind);
    if (showWind) {
        float min = -1;
        float max = 1;
        ImGui::SliderFloat("X Wind", &cloth->air.x, min, max);
        ImGui::SliderFloat("Y Wind", &cloth->air.y, min, max);
        ImGui::SliderFloat("Z Wind", &cloth->air.z, min, max);
    }

    if (!showWind) {
        cloth->air = glm::vec3(0.0f);
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void simIMGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Simple GUI");
    ImGui::InputFloat("Radius", &sim->rad);
    ImGui::InputFloat("Mass", &sim->particleMass);
    ImGui::InputFloat("Gravity", &sim->gravitY);
    ImGui::Checkbox("Wind", &showwind);
    if (showwind) {
        ImGui::SliderFloat("Wind X", &sim->wind.x, -1, 1);
        ImGui::SliderFloat("Wind Y", &sim->wind.y, -1, 1);
        ImGui::SliderFloat("Wind Z", &sim->wind.z, -1, 1);
    }
    if (!showwind) {
        sim->wind = glm::vec3(0.0f);
    }

    ImGui::SliderFloat("X", &sim->x, -10, 10);
    ImGui::SliderFloat("Y", &sim->y, -10, 10);
    ImGui::SliderFloat("Z", &sim->z, -10, 10);

    ImGui::InputFloat("Creation Rate", &sim->particle_per_sec, 0, 100000);
    ImGui::InputFloat("# Particles", &sim->numPartilces, 0, 100000);
    ImGui::SliderFloat("Life Span", &sim->lifeSpan, 0, 10);
    ImGui::SliderFloat("Elascticty", &sim->elasticity, 0, 1);
    ImGui::SliderFloat("Friction", &sim->friction, 0, 100);
    ImGui::SliderFloat("Air Density", &sim->airDensity, 0, 10);
    ImGui::SliderFloat("Drag", &sim->drag, 0, 1);
    ImGui::SliderFloat("Var X", &sim->varX, 0, 10);
    ImGui::SliderFloat("Var Y", &sim->varY, 0, 10);
    ImGui::SliderFloat("Var Z", &sim->varZ, 0, 10);
    ImGui::SliderFloat("Var X Velocity", &sim->varVelocityX, 0, 10);
    ImGui::SliderFloat("Var Y Velocity", &sim->varVelocityY, 0, 10);
    ImGui::SliderFloat("Var Z Velocity", &sim->varVelocityZ, 0, 10);
    ImGui::SliderFloat("Var Life", &sim->varLife, 0, 10);


    

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void Window::displayCallback(GLFWwindow* window) {
    // Clear the color and depth buffers.
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   if (modelDraw) {
       //ImGui Code
       modelIMGUI();

       // Render the object.
       if (drawSkel) {
           skel->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
       }
       skin->getJoint(skel->getIndexJoints());
       skin->update();
       if (drawSkin)
           skin->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

       float currentFrame = glfwGetTime();
       skel->update();
       if (!pause) {
           times += currentFrame - lastframe;
           anim->update(currentFrame, skel->indexJoints, skel);
           lastframe = currentFrame;
       }
   }

   if (clothDraw) {
       clothIMGUI();
       cloth->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
   }

   if (particleSim) {
       ground->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
       sim->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
       simIMGUI();
   }

    // Gets events, including input such as keyboard and mouse or window resizing.
    glfwPollEvents();
    // Swap buffers.
    glfwSwapBuffers(window);
}

// helper to reset the camera
void Window::resetCamera() {
    Cam->Reset();
    Cam->SetAspect(float(Window::width) / float(Window::height));
}

// callbacks - for Interaction
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    /*
     * TODO: Modify below to add your key callbacks.
     */

    // Check for a key press.
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_R:
                resetCamera();
                break;

            case GLFW_KEY_A:
                Cam->SetDistance(Cam->GetDistance() - 2.0f);
                break;

            case GLFW_KEY_Z:
                Cam->SetDistance(Cam->GetDistance() + 2.0f);
                break;

            default:
                break;
        }
    }
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        LeftDown = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        RightDown = (action == GLFW_PRESS);
    }
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {
    int maxDelta = 100;
    int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
    int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

    MouseX = (int)currX;
    MouseY = (int)currY;

    // Move camera
    // NOTE: this should really be part of Camera::Update()
    if (LeftDown) {
        const float rate = 1.0f;
        Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
        Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
    }
    if (RightDown) {
        const float rate = 0.005f;
        float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
        Cam->SetDistance(dist);
    }
}