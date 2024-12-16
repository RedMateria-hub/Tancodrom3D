

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <string>


#include "shader_utils.h"


const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;


bool isNight = false;
float timeOfDay = 0.0f;


void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        isNight = !isNight;
    }
}


void renderScene(GLuint shaderProgram, bool nightMode) {
 
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "isNight"), nightMode);


    glBegin(GL_TRIANGLES);
    glColor3f(0.3f, 0.5f, 0.1f);
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(1.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();


    glPushMatrix();
    glTranslatef(0.2f, 0.1f, 0.3f);
    glColor3f(0.7f, 0.0f, 0.0f);
    glutSolidCube(0.1f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.3f, 0.4f, -0.2f);
    glColor3f(0.0f, 0.0f, 0.7f);
    glutSolidSphere(0.05f, 20, 20); 
    glPopMatrix();
}

int main() {
  
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }


    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Battlefield Simulator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        glfwTerminate();
        return -1;
    }


    glEnable(GL_DEPTH_TEST);

    GLuint shaderProgram = loadShaders("vertex_shader.glsl", "fragment_shader.glsl");


    while (!glfwWindowShouldClose(window)) {

        processInput(window);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderScene(shaderProgram, isNight);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
