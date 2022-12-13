#include "importer.h"
#include "framebuffer.h"

using namespace std;
using namespace CG;

/**
 * @brief so that cursor operates properly
 */
bool firstMouse = true;
int press = 0;

int screen_width = 1920;
int screen_height = 1080;
bool isFullScreen = false;
float screen_aspect = (float)screen_width / (float)screen_height;
/**
 * @brief for zooming in and out
 */
float fov = 45.0f;

/**
 * @brief set cursor x position in center
 */
float lastX = screen_width / 2.0f;
/**
 * @brief set cursor y position in center
 */
float lastY = screen_height / 2.0f;

/**
 * @brief initial pitch of camera
 */
float pitch = 0;
/**
 * @brief initial yaw of camera
 */
float yaw = -90;

/**
 * @brief time between frames
 */
float deltaTime;
/**
 * @brief store last frame time for computing delta time
 */
float lastFrame;

glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
/**
 * @brief camera position in world
 */
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
/**
 * @brief must change yaw and pitch accordingly when setting the camera target
 */
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
/**
 * @brief the front direction of camera
 * the camera is in left hand coordinate sytem
 * this is because increasing z value should move you closer to the world
 */
glm::vec3 cameraFront = glm::normalize(cameraTarget - cameraPos);
/**
 * @brief right direction of camera
 */
glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
/**
 * @brief up direction of camera
 */
glm::vec3 cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));

/**
 * @brief updated the values whenever they are pressed
 * @param window
 * @param button
 * @param action
 * @param mods
 */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) press = 1;
}

/**
 * @brief change viewport when window resizes
 * @param window
 * @param width
 * @param height
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
};

/**
 * @brief closes the window when escape is pressed
 * @param window
 * @param key
 * @param scancode
 * @param action
 * @param mods
 */
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
};

/**
 * @brief simulate the yaw and pitches here
 * @param window
 * @param xdpos
 * @param ydpos
 */
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

    // mouse moving horizontally gives you yaw
    // mouse moving vertically gives you pitch
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

/**
 * @brief simulate the zoom function here by changing fov
 * @param window
 * @param xoffset
 * @param yoffset
 */
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

    /**
     * @brief the shaders for final scene
     */
    Program myProgram;
    /**
     * @brief shaders for creating depth map
     */
    Program myProgramDepth;
    /**
     * @brief shaders for testing the framebuffer texture quad
     */
    Program myProgramTest;
    /**
     * @brief shaders for drawing the cube light
     */
    Program myProgramLight;

    /**
     * @brief my depth framebuffer texture
     */
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

        // first construct the depth map from light position
        glBindFramebuffer(GL_FRAMEBUFFER, depthfbo.id);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);

        float currentFrame = (float)currentTime;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        myProgramDepth.use();
        float near_plane = 1.0f, far_plane = 7.5f;
        // orthographic projection used to preserve angles in depth map
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

        // now draw the scene from the camera point of view and generate shadows
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myProgram.use();

        // bind the depth map texture
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

        // showcase the light
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

        // simulate pan here
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraRight;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += cameraSpeed * cameraRight;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraUp;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cameraPos += cameraSpeed * cameraUp;

        // simulate roll, q and e
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
