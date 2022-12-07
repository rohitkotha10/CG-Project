#include "importer.h"
#include "framebuffer.h"

using namespace CG;
using namespace std;

bool firstMouse = true;
int press = 0;

int screen_width = 1920;
int screen_height = 1080;
bool isFullScreen = false;
float screen_aspect = (float)screen_width / (float)screen_height;
float fov = 45.0f;

float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;

float pitch = 0;
float yaw = -90;

float deltaTime;
float lastFrame;

glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::normalize(cameraTarget - cameraPos);          // also facing the scene
glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));  // camera is in left hand system
glm::vec3 cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) press = 1;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
};

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
};

void cursor_callback(GLFWwindow* window, double xdpos, double ydpos) {
    float xpos = (float)xdpos;
    float ypos = (float)ydpos;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);
    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

int main() {
    GLFWwindow* window = NULL;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(
        screen_width, screen_height, "CG Project", isFullScreen ? glfwGetPrimaryMonitor() : NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VENDOR) << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    std::cout << glGetString(GL_RENDERER) << std::endl << std::endl;

    Program myProgram;
    Program myProgramDepth;
    Program myProgramTest;
    Program myProgramLight;

    Framebuffer depthfbo;

    Importer ourModel;
    Importer cube;
    Mesh myPlane;
    Texture wood;

    glm::vec3 lightPos;

    // shader compilation
    myProgram.create("src/shaders/vs.shader", "src/shaders/fs.shader");
    myProgramDepth.create("src/shaders/vsDepth.shader", "src/shaders/fsDepth.shader");
    myProgramTest.create("src/shaders/vsTest.shader", "src/shaders/fsTest.shader");
    myProgramLight.create("src/shaders/vsLight.shader", "src/shaders/fsLight.shader");

    // startup processes
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyboard_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    wood.loadTexture("media/wood.png", "diffuse");
    myPlane.createPlaneTexture(glm::vec3(0.0f, -0.5f, 0.0f), 20.0f, wood, 10.0f);
    ourModel.loadModel("media/backpack/backpack.obj");
    cube.loadModel("media/cube/cube.obj");

    depthfbo.create(screen_width, screen_height);

    lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        glBindFramebuffer(GL_FRAMEBUFFER, depthfbo.id);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);

        float currentFrame = (float)currentTime;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        myProgramDepth.use();
        float near_plane = 1.0f, far_plane = 7.5f;
        glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 view = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), worldUp);
        glm::mat4 lightSpace = projection * view;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.3f));
        model = glm::translate(model, glm::vec3(0.0f, 0.3f, 0.0f));

        myProgramDepth.setMat4("projection_matrix", projection);
        myProgramDepth.setMat4("view_matrix", view);
        myProgramDepth.setMat4("model_matrix", model);

        ourModel.draw(myProgramDepth);

        model = glm::mat4(1.0f);
        myProgramDepth.setMat4("model_matrix", model);
        myPlane.draw(myProgramDepth);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myProgram.use();

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthfbo.texture);
        myProgram.setInt("depthMap", 2);

        myProgram.setVec3("viewPos", cameraPos);
        myProgram.setVec3("lightPos", lightPos);
        myProgram.setMat4("lightSpace", lightSpace);

        projection = glm::perspective(glm::radians(fov), screen_aspect, 0.1f, 100.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, worldUp);

        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.3f));
        model = glm::translate(model, glm::vec3(0.0f, 0.3f, 0.0f));

        myProgram.setMat4("projection_matrix", projection);
        myProgram.setMat4("view_matrix", view);
        myProgram.setMat4("model_matrix", model);

        ourModel.draw(myProgram);

        model = glm::mat4(1.0f);
        myProgram.setMat4("model_matrix", model);
        myPlane.draw(myProgram);

        // depthfbo.drawfbo(myProgramTest);

        myProgramLight.use();

        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.1f));
        model = glm::translate(model, 10.0f * lightPos);
        myProgramLight.setMat4("projection_matrix", projection);
        myProgramLight.setMat4("view_matrix", view);
        myProgramLight.setMat4("model_matrix", model);

        cube.draw(myProgramLight);

        // process inputs
        const float cameraSpeed = 2.5f * deltaTime;  // adjust accordingly

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraRight;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += cameraSpeed * cameraRight;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraUp;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cameraPos += cameraSpeed * cameraUp;

        // roll, q and e
        float rollSpeed = 50.0f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            worldUp = glm::mat3(glm::rotate(glm::mat4(1.0f), -glm::radians(rollSpeed), cameraFront)) * cameraUp;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            worldUp = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(rollSpeed), cameraFront)) * cameraUp;

        cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

        glfwPollEvents();
        glfwSwapInterval(1);
        glfwSwapBuffers(window);
    }

    // shutdown
    myProgram.shutdown();
    myProgramDepth.shutdown();
    myProgramTest.shutdown();
    myProgramLight.shutdown();
    myPlane.shutdown();
    ourModel.shutdown();
    cube.shutdown();
    depthfbo.shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();
}
