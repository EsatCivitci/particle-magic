#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <random>

#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../headers/Shader.h"
#include "../headers/Utils.h"
#include "../headers/Camera.h"

using namespace std;

// ************** FUNCTION DECLERATIONS ***************
void init();
void display();
void reshape(GLFWwindow* window, int width, int height);
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
static void scroll_callback(GLFWwindow* window, double xoff, double yoff);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void initFonts(int windowWidth, int windowHeight);
void initBuffersAndTextures();

float calculateDeltaTime();
void calculateFPS();
void renderAllTexts();
void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

// ************** VARIABLES ****************
GLFWwindow* window;
int gWidth = 1920, gHeight = 1080;

Camera* camera;

Shader* textShader;
Shader* computeShader;
Shader* particleShader;

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view;
glm::mat4 projection;

float rotationAmount = 180.0f;
float exposure = 1;

bool pressR = false;
bool pressT = false;
bool pressF = false;
bool pressG = false;

float currentFrame = 0.0f;
float lastFrame = 0.0f;
float deltaTime = 0;
float sumDelta = 0;

int fps = 0;
float fpsDisplayTime = 0;

float speed = 1.0f;
int mass = 50;

int windowedPosX = 100;
int windowedPosY = 100;
int windowedWidth = 1920;
int windowedHeight = 1080;

string topLeft, topRight, bottomLeft, bottomRight;

int particle_count = 5;
int pointSize = 2;
int attractorSize = 3;
glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);

GLuint position_buffer, velocity_buffer;
GLuint buffers[2];
GLuint position_tbo, velocity_tbo;
GLuint render_vao;
GLuint attractor_ubo;

std::mt19937 rng{ std::random_device{}() };
std::uniform_real_distribution<float> distPos(-20.0f, 20.0f);
std::uniform_real_distribution<float> distAge( 0.0f, 1.0f);
std::uniform_real_distribution<float> distVel( 0.0f, 0.01f);

float boundaryX;
float boundaryY;

int main(int argc, char* argv[]) {
    if (argc >= 2) {
        particle_count = std::atoi(argv[1]);
        pointSize = std::atoi(argv[2]);
        cout << particle_count << "    "  << pointSize << endl;
    }

    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW!!" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(gWidth, gHeight, "HW2", NULL, NULL);
    if (!window) {
        cerr << "Failed to create window!!" << endl;
        glfwTerminate();
		exit(-1);
    }

    glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

    if (glewInit() != GLEW_OK){
        cerr << "Failed to initialize GLEW!!" << endl;
        return -1;
    }

    init();

	glfwSetKeyCallback(window, keyboard); 
    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, reshape);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    while (!glfwWindowShouldClose(window))
	{

		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glm::quat q_up = glm::angleAxis(glm::radians(89.9f), glm::vec3(1.0f, 0.0f, 0.0f));  
    glm::quat q_down = glm::angleAxis(glm::radians(-89.9f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    
    camera = new Camera(
        glm::vec3(0.0f, 0.0f, -1.0f), 
        glm::vec3(0.0f, 1.0f, 0.0f), 
        glm::vec3(0.0f, 0.0f, 40.0f));

    textShader = new Shader("shaders/vert_text.glsl", "shaders/frag_text.glsl");

    computeShader = new Shader("shaders/compute_particles.glsl");

    particleShader = new Shader("shaders/vert_particles.glsl", "shaders/frag_particles.glsl");


    initFonts(gWidth, gHeight);

    initBuffersAndTextures();

    glEnable(GL_PROGRAM_POINT_SIZE);

    bottomLeft = "Attractor";


    // TO CHECK MAX WORK GROUP NUMBERS TO BE CREATED (x = 2147483646, y = 2147483646, z = 2147483646)
    /*
    GLint max_compute_work_group_count[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_compute_work_group_count[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_compute_work_group_count[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_compute_work_group_count[2]);

    std::cout << "Maximum compute work group count (X, Y, Z): "
              << max_compute_work_group_count[0] << ", "
              << max_compute_work_group_count[1] << ", "
              << max_compute_work_group_count[2] << std::endl;
    */

    // TO CHECK MAX COMPUTE WORK GROUP INVOCATIONS (x = 1024, y = 1024, z = 1024)
    /*
    GLint max_compute_work_group_invocations = 0;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);
    std::cout << "Maximum compute work group invocations: " << max_compute_work_group_invocations << std::endl;

    GLint max_compute_work_group_size[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &max_compute_work_group_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &max_compute_work_group_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &max_compute_work_group_size[2]);
    std::cout << "Maximum compute work group size (X, Y, Z): "
              << max_compute_work_group_size[0] << ", "
              << max_compute_work_group_size[1] << ", "
              << max_compute_work_group_size[2] << std::endl;
    */
}

void display() {

    deltaTime = calculateDeltaTime();
    calculateFPS();

    if (!pressR) {
        computeShader->use();
        glBindImageTexture(
            0, velocity_tbo, 0, GL_FALSE, 0,GL_READ_WRITE, GL_RGBA32F
        );
        glBindImageTexture(
            1, position_tbo, 0, GL_FALSE, 0,GL_READ_WRITE, GL_RGBA32F
        );
        float dt = deltaTime * speed;
        computeShader->setFloat("dt", dt);
        computeShader->setInt("attractorSize", attractorSize); 
        computeShader->setVec3("origin", origin);

        glDispatchCompute(particle_count / 128 + 1, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    } 

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    view = camera->getViewMatrix();
    projection = glm::perspective(glm::radians(60.0f), (float)gWidth/gHeight, 0.1f, 100.0f);

    glm::mat4 mvp = projection * view * model;
    //mvp = glm::mat4(1.0f);

    particleShader->use();
    particleShader->setMat4("mvp", mvp);
    particleShader->setInt("pointSize", pointSize);

    glBindVertexArray(render_vao);
    glDisable(GL_DEPTH_TEST);
    // glDisable(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDrawArrays(GL_POINTS, 0, particle_count);

    if (pressT) {
        renderAllTexts();
    }
}

void initBuffersAndTextures() {
    glGenBuffers(2, buffers);

    position_buffer = buffers[0];
    velocity_buffer = buffers[1];

    // Init position buffer
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, particle_count * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);

    glm::vec4* positions = (glm::vec4*) glMapNamedBufferRange(
        position_buffer, 
        0, 
        particle_count * sizeof(glm::vec4),
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
    );

    for (int i = 0; i < particle_count; ++i) {
        float x = distPos(rng);
        float y = distPos(rng);
        float z = 0.0f;
        float w = distAge(rng);
        positions[i] = glm::vec4(x, y, z, w);
    }
    glUnmapNamedBuffer(position_buffer);

    // Init velocity buffer
    glBindBuffer(GL_ARRAY_BUFFER, velocity_buffer);
    glBufferData(GL_ARRAY_BUFFER, particle_count * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);

    glm::vec4* velocities = (glm::vec4*) glMapBufferRange(
        GL_ARRAY_BUFFER,
        0,
        particle_count * sizeof(glm::vec4),
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
    );

    for (int i = 0; i < particle_count; ++i) {
        float x = distVel(rng);
        float y = distVel(rng);
        velocities[i] = glm::vec4(glm::vec3(x,y,0.0f), 0.0f);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);


    // Init Textures
    glGenTextures(1, &position_tbo);
    glBindTexture(GL_TEXTURE_BUFFER, position_tbo);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, position_buffer);

    glGenTextures(1, &velocity_tbo);
    glBindTexture(GL_TEXTURE_BUFFER, velocity_tbo);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, velocity_buffer);

    // Bind positions into render_vao
    glGenVertexArrays(1, &render_vao);
    glBindVertexArray(render_vao);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);


    glGenBuffers(1, &attractor_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, attractor_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 64, NULL, GL_DYNAMIC_DRAW); 
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, attractor_ubo); 

    std::vector<glm::vec4> attractors(64, glm::vec4(0.0f));

    // Define attractors with position and mass
    attractors[0] = glm::vec4(0.0f, 0.0f, 0.0f, 0.1f);     // Attractor on Origin
    attractors[1] = glm::vec4(5.0f, 5.0f, 0.0f, 30.0f);    // Attractor
    attractors[2] = glm::vec4(-5.0f, -5.0f, 0.0f, 30.0f);  // Attractor

    glBindBuffer(GL_UNIFORM_BUFFER, attractor_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4) * attractors.size(), attractors.data());

}

void renderAllTexts() {
    ostringstream oss;
    string s;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,gWidth,gHeight);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec3 color = glm::vec3(1.0f, 0.2f, 0.2f);


    renderText("Attractor Count:", 20, 130, 1, color);
    renderText(to_string(attractorSize), 365, 130, 1, color);

    renderText("Mass:", 20, 80, 1, color);
    renderText(to_string(mass), 150, 80, 1, color);

    if (pressG) bottomLeft = "Origin";
    else bottomLeft = "Attractor";
    renderText(bottomLeft, 20, 30, 1, color);
  
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void calculateFPS() {
    fps++;
    fpsDisplayTime += deltaTime;

    if (fpsDisplayTime >= 1.0f) {
        //bottomLeft = to_string(fps);
        cout << fps << endl;
        fpsDisplayTime = 0;
        fps = 0;
    } 
}


float calculateDeltaTime(){
    currentFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    return deltaTime;
}

void switchFullScreen(GLFWwindow* window) {
    if (!pressF) {
        glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        GLFWmonitor*   mon  = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(mon);
        glfwSetWindowMonitor(window,
                             mon,
                             0, 0,
                             mode->width, mode->height,
                             mode->refreshRate);
    }
    else {
        glfwSetWindowMonitor(window,
                             nullptr,
                             windowedPosX, windowedPosY,
                             windowedWidth, windowedHeight,
                             0);
    }
}


void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        pressR = !pressR;
    }   
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        pressT = !pressT;
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        pressG = !pressG;
    }      
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        switchFullScreen(window);
        pressF = !pressF;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        if (speed <= 8.0f) speed*=2;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        if (speed >= 0.05f) speed/=2;
    }
    //bottomLeft = to_string(speed);
    
}


static void scroll_callback(GLFWwindow* window, double xoff, double yoff) {
    if (yoff > 0) mass = std::min(mass+10, 100);
    else if (yoff < 0) mass = std::max(mass-10,  10);
    bottomLeft = to_string(mass);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!pressG) {
            if (attractorSize < 12) {
                int camZ = camera->getCamPosition().z;
                float xPos = 2 * camZ * x / gWidth - camZ;
                float yPos = -(2 * (camZ*gHeight/gWidth) * y / gHeight - (camZ*gHeight/gWidth));
                cout << "xPos: " << xPos << "     yPos: " << yPos << endl;
    
                glm::vec4 newAttractor = glm::vec4(xPos, yPos, 0.0f, mass);
    
                GLintptr byteOffset = sizeof(glm::vec4) * attractorSize;
                glBindBuffer(GL_UNIFORM_BUFFER, attractor_ubo);
                glBufferSubData(
                  GL_UNIFORM_BUFFER,
                  byteOffset,
                  sizeof(glm::vec4),
                  glm::value_ptr(newAttractor)
                );
                attractorSize++;
            }
        }
        else {
            int camZ = camera->getCamPosition().z;
            float xPos = 2 * camZ * x / gWidth - camZ;
            float yPos = -(2 * (camZ*gHeight/gWidth) * y / gHeight - (camZ*gHeight/gWidth));
            origin.x = xPos;
            origin.y = yPos;
            cout << "xPos: " << xPos << "     yPos: " << yPos << endl;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (attractorSize > 0) attractorSize--;
    }
}

void reshape(GLFWwindow* window, int width, int height) {
    gWidth  = width;
    gHeight = height;
    glViewport(0, 0, width, height);

   

    textShader->use();
    glm::mat4 proj = glm::ortho(0.0f,
                                static_cast<float>(width),
                                0.0f,
                                static_cast<float>(height));
    glUniformMatrix4fv(glGetUniformLocation(textShader->ID, "projection"),
                       1, GL_FALSE, glm::value_ptr(proj));

}
  
