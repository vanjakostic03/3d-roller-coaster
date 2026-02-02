// Vanja Kostic SV29/2022

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include "../header/Util.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <cmath>
#include <iostream>

// ================= STRUCTURES   =================
struct Passenger {
    float offsetX, offsetY;
    bool beltOn;
    bool isSick;
    bool active;            //flag da li je izasao iz vozila
};

// ================= GLOBAL VARIABLES   =================
enum CarState { MOVING, SLOWING_DOWN, RETURNING, STOPPED, WAITING };

CarState carState = STOPPED;
float waitTimer = 0.0f;
float carPos = 0.0f;
float carSpeed = 0.1f;
float gravityFactor = 9.7f;
const int maxSeats = 8;
float carX = 0.0f;
float carY = 0.0f;
bool allowBoarding = true;

std::vector<Passenger> passengers;

//==========================PASSENGERS FUNCTIONS======================
bool allGone() {
    bool gone = true;
    for (Passenger& p : passengers) {
        if (p.active) {
            gone = false;
            break;
        }
    }
    return gone;
}

//void generateRails(const std::vector<std::vector<float>>& keyPoints, std::vector<float>& railVertices, float trackWidth = 0.05f)
//{
//    railVertices.clear();
//
//    size_t n = keyPoints.size();
//    if (n < 2) return;
//
//    for (int i = 0; i < n; ++i) {
//        std::vector<float> P0 = keyPoints[(i - 1 + n) % n];
//        std::vector<float> P1 = keyPoints[i];
//        std::vector<float> P2 = keyPoints[(i + 1) % n];
//        std::vector<float> P3 = keyPoints[(i + 2) % n];
//
//        float dx = P2[0] - P1[0];
//        float dy = P2[1] - P1[1];
//        float dz = P2[2] - P1[2];
//        float distance = sqrt(dx * dx + dy * dy + dz * dz);
//        int numPoints = static_cast<int>(distance * 50);
//        if (numPoints < 20) numPoints = 20;
//
//        // Debug boja na osnovu ID (poslednji element u P1)
//        int id = static_cast<int>(P1[3]);
//
//        float r = (id % 3 == 0) ? 1.0f : 0.0f;
//        float g = (id % 3 == 1) ? 1.0f : 0.0f;
//        float b = (id % 3 == 2) ? 1.0f : 0.0f;
//        
//        if (id % 9 == 0) {
//            r = 1.0f;
//            g = 0.0f;
//            b = 0.0f;
//        }
//        else if (id % 9 == 1) { //narandzasta
//            r = 1.0f;
//            g = 0.5f;
//            b = 0.0f;
//        } else if (id % 9 == 2) {//zuta
//            r = 1.0f;
//            g = 1.0f;
//            b = 0.0f;
//        } else if (id % 9 == 3) {   //neka fluroscenta
//            r = 0.5f;
//            g = 1.0f;
//            b = 0.0f;
//        }else if (id % 9== 4) {
//            r = 0.0f;
//            g = 1.0f;
//            b = 0.0f;
//        }else if (id % 9 == 5) {        //tirkizno zelena
//            r = 0.0f;
//            g = 1.0f;
//            b = 0.5f;
//        }else if (id % 9 == 6) {
//            r = 0.0f;
//            g = 1.0f;
//            b = 1.0f;
//        }else if (id % 9 == 7) {
//            r = 0.0f;
//            g = 0.0f;
//            b = 1.0f;
//        }else if (id % 9 == 8) {
//            r = 1.0f;
//            g = 0.0f;
//            b = 1.0f;
//        }
//
//        for (int j = 0; j <= numPoints; ++j) {
//            float t = j / (float)numPoints;
//            float t2 = t * t;
//            float t3 = t2 * t;
//
//            float x = 0.5f * ((2.0f * P1[0]) + (-P0[0] + P2[0]) * t + (2.0f * P0[0] - 5.0f * P1[0] + 4.0f * P2[0] - P3[0]) * t2 + (-P0[0] + 3.0f * P1[0] - 3.0f * P2[0] + P3[0]) * t3);
//            float y = 0.5f * ((2.0f * P1[1]) + (-P0[1] + P2[1]) * t + (2.0f * P0[1] - 5.0f * P1[1] + 4.0f * P2[1] - P3[1]) * t2 + (-P0[1] + 3.0f * P1[1] - 3.0f * P2[1] + P3[1]) * t3);
//            float z = 0.5f * ((2.0f * P1[2]) + (-P0[2] + P2[2]) * t + (2.0f * P0[2] - 5.0f * P1[2] + 4.0f * P2[2] - P3[2]) * t2 + (-P0[2] + 3.0f * P1[2] - 3.0f * P2[2] + P3[2]) * t3);
//
//            glm::vec3 tangent = glm::normalize(glm::vec3(
//                0.5f * (-P0[0] + P2[0] + (-5 * P1[0] + 4 * P2[0] - P3[0]) * 2 * t + (3 * P1[0] - 3 * P2[0] + P3[0]) * 3 * t2),
//                0.5f * (-P0[1] + P2[1] + (-5 * P1[1] + 4 * P2[1] - P3[1]) * 2 * t + (3 * P1[1] - 3 * P2[1] + P3[1]) * 3 * t2),
//                0.5f * (-P0[2] + P2[2] + (-5 * P1[2] + 4 * P2[2] - P3[2]) * 2 * t + (3 * P1[2] - 3 * P2[2] + P3[2]) * 3 * t2)
//            ));
//
//            glm::vec3 up(0, 1, 0);
//            glm::vec3 side = glm::normalize(glm::cross(tangent, up));
//
//            glm::vec3 left = glm::vec3(x, y, z) - side * trackWidth;
//            glm::vec3 right = glm::vec3(x, y, z) + side * trackWidth;
//
//            railVertices.push_back(left.x); railVertices.push_back(left.y); railVertices.push_back(left.z);
//            railVertices.push_back(r); railVertices.push_back(g); railVertices.push_back(b);
//
//            railVertices.push_back(right.x); railVertices.push_back(right.y); railVertices.push_back(right.z);
//            railVertices.push_back(r); railVertices.push_back(g); railVertices.push_back(b);
//        }
//    }
//}


//==========================RAILS GENERATION==========================
void generateRails(const std::vector<std::vector<float>>& keyPoints, std::vector<float>& railVertices, float trackWidth = 0.05f)
{
    railVertices.clear();
    float r = 0.8f, g = 0.2f, b = 0.0f;

    size_t n = keyPoints.size();
    if (n < 2) return;

    float halfWidth = trackWidth;
    float halfHeight = 0.05f;

    std::vector<glm::vec3> leftVerts;
    std::vector<glm::vec3> rightVerts;

    for (int i = 0; i < n; ++i) {
        std::vector<float> P0 = keyPoints[(i - 1 + n) % n];
        std::vector<float> P1 = keyPoints[i];
        std::vector<float> P2 = keyPoints[(i + 1) % n];
        std::vector<float> P3 = keyPoints[(i + 2) % n];

        float dx = P2[0] - P1[0];
        float dy = P2[1] - P1[1];
        float dz = P2[2] - P1[2];
        float distance = sqrt(dx * dx + dy * dy + dz * dz);
        int numPoints = static_cast<int>(distance * 50);
        if (numPoints < 20) numPoints = 20;

        for (int j = 0; j <= numPoints; ++j) {
            float t = j / (float)numPoints;
            float t2 = t * t;
            float t3 = t2 * t;

            float x = 0.5f * ((2.0f * P1[0]) + (-P0[0] + P2[0]) * t + (2.0f * P0[0] - 5.0f * P1[0] + 4.0f * P2[0] - P3[0]) * t2 + (-P0[0] + 3.0f * P1[0] - 3.0f * P2[0] + P3[0]) * t3);
            float y = 0.5f * ((2.0f * P1[1]) + (-P0[1] + P2[1]) * t + (2.0f * P0[1] - 5.0f * P1[1] + 4.0f * P2[1] - P3[1]) * t2 + (-P0[1] + 3.0f * P1[1] - 3.0f * P2[1] + P3[1]) * t3);
            float z = 0.5f * ((2.0f * P1[2]) + (-P0[2] + P2[2]) * t + (2.0f * P0[2] - 5.0f * P1[2] + 4.0f * P2[2] - P3[2]) * t2 + (-P0[2] + 3.0f * P1[2] - 3.0f * P2[2] + P3[2]) * t3);

            // tangenta spline-a (približno)
            glm::vec3 tangent = glm::normalize(glm::vec3(
                0.5f * (-P0[0] + P2[0] + (-5 * P1[0] + 4 * P2[0] - P3[0]) * 2 * t + (3 * P1[0] - 3 * P2[0] + P3[0]) * 3 * t2),
                0.5f * (-P0[1] + P2[1] + (-5 * P1[1] + 4 * P2[1] - P3[1]) * 2 * t + (3 * P1[1] - 3 * P2[1] + P3[1]) * 3 * t2),
                0.5f * (-P0[2] + P2[2] + (-5 * P1[2] + 4 * P2[2] - P3[2]) * 2 * t + (3 * P1[2] - 3 * P2[2] + P3[2]) * 3 * t2)
            ));

            glm::vec3 up(0, 0, 1);
            glm::vec3 side = glm::normalize(glm::cross(tangent, up));

            glm::vec3 left = glm::vec3(x, y, z) - side * halfWidth;
            glm::vec3 right = glm::vec3(x, y, z) + side * halfWidth;

            leftVerts.push_back(left + up * halfHeight);
            leftVerts.push_back(left - up * halfHeight);
            rightVerts.push_back(right + up * halfHeight);
            rightVerts.push_back(right - up * halfHeight);


           /* glm::vec3 up(0, 0, 2);
            glm::vec3 side = glm::normalize(glm::cross(tangent, up));

            glm::vec3 left = glm::vec3(x, y, z) - side * trackWidth;
            glm::vec3 right = glm::vec3(x, y, z) + side * trackWidth;

            railVertices.push_back(left.x); railVertices.push_back(left.y); railVertices.push_back(left.z);
            railVertices.push_back(r); railVertices.push_back(g); railVertices.push_back(b);

            railVertices.push_back(right.x); railVertices.push_back(right.y); railVertices.push_back(right.z);
            railVertices.push_back(r); railVertices.push_back(g); railVertices.push_back(b);*/




            /*glm::vec3 up(0, 0, 1);
            glm::vec3 side = glm::normalize(glm::cross(tangent, up));

            float halfWidth = trackWidth;
            float halfHeight = 0.05f;

            glm::vec3 leftTop = glm::vec3(x, y, z) + up * halfHeight - side * trackWidth;
            glm::vec3 rightTop = glm::vec3(x, y, z) + up * halfHeight + side * trackWidth;
            glm::vec3 leftBottom = glm::vec3(x, y, z) - up * halfHeight - side * trackWidth;
            glm::vec3 rightBottom = glm::vec3(x, y, z) - up * halfHeight + side * trackWidth;*/
        }
    }
    for (size_t i = 0; i < leftVerts.size(); i += 2) {
        railVertices.push_back(leftVerts[i].x); railVertices.push_back(leftVerts[i].y); railVertices.push_back(leftVerts[i].z);
        railVertices.push_back(r); railVertices.push_back(g); railVertices.push_back(b);

        railVertices.push_back(rightVerts[i].x); railVertices.push_back(rightVerts[i].y); railVertices.push_back(rightVerts[i].z);
        railVertices.push_back(r); railVertices.push_back(g); railVertices.push_back(b);

        railVertices.push_back(leftVerts[i + 1].x); railVertices.push_back(leftVerts[i + 1].y); railVertices.push_back(leftVerts[i + 1].z);
        railVertices.push_back(r); railVertices.push_back(g); railVertices.push_back(b);

        railVertices.push_back(rightVerts[i + 1].x); railVertices.push_back(rightVerts[i + 1].y); railVertices.push_back(rightVerts[i + 1].z);
        railVertices.push_back(r); railVertices.push_back(g); railVertices.push_back(b);
    }
}

//========================= DRAW FUNCTIONS =====================================
void drawRails(unsigned int colorShader, int numPoints, unsigned int VAOrails) {
    glUseProgram(colorShader);
    glBindVertexArray(VAOrails);
    glLineWidth(40.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numPoints);
}

//========================= BUFFERS ========================================
void formVAOs(float* railVertices, size_t railSize, unsigned int& VAOrails) {
    unsigned int VBOrails;
    glGenVertexArrays(1, &VAOrails);
    glGenBuffers(1, &VBOrails);

    glBindVertexArray(VAOrails);
    glBindBuffer(GL_ARRAY_BUFFER, VBOrails);
    glBufferData(GL_ARRAY_BUFFER, railSize, railVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

// ================= CAMERA ORBIT =================
glm::vec3 camPos;
glm::vec3 camFront;
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f); // Z je gore

glm::vec3 coasterCenter = glm::vec3(0.0f, 0.0f, 0.0f);
float camRadius = 8.0f;
float camAngle = 0.0f;
float camHeight = 3.0f;
float camSpeed = 1.5f;

void processCameraOrbit(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camAngle -= camSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camAngle += camSpeed * deltaTime;

    // Vertikalno pomeranje kamere (W/S) po Y osi
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camHeight += camSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camHeight -= camSpeed * deltaTime;

    if (camHeight < 0.5f) camHeight = 0.5f;  // minimalna visina
    if (camHeight > 10.0f) camHeight = 10.0f; // opcionalno max visina

    // Postavi novu poziciju kamere oko centra rolerkostera
    camPos.x = coasterCenter.x + camRadius * sin(camAngle);
    camPos.z = coasterCenter.z + camRadius * cos(camAngle);
    camPos.y = camHeight; // Y je visina

    // Kamera uvek gleda ka centru rolerkostera
    camFront = glm::normalize(coasterCenter - camPos);
}

// ================= MAIN =================
int main()
{
    if (!glfwInit()) {
        std::cout << "GLFW nije ucitan!\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int screenWidth = mode->width;
    int screenHeight = mode->height;

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Window", NULL, NULL);
    if (!window) {
        std::cout << "Prozor nije kreiran!\n";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW nije ucitan!\n";
        return 3;
    }

    unsigned int shaderProgram = createShader("basic.vert", "basic.frag");
    unsigned int colorShader = createShader("color.vert", "color.frag");

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), screenWidth / (float)screenHeight, 0.1f, 100.0f);

    std::vector<std::vector<float>> keyPoints = {
        {-3.0f, -2.75f, 2.75f},
    {-5.0f, -2.75f, -2.00f},
    {-4.5f, 1.25f, -2.00f},
    {-1.25f, -2.17f, -1.0f},
    {0.1f, 2.5f, 1.75f},
    {1.5f, -1.5f, -1.25f},
    {2.0f, 0.7f, -3.25f},
    {4.75f, -2.75f, -2.75f},
    {3.0f, -2.75f, 2.75f}
    };

    /*std::vector<std::vector<float>> keyPoints = {
    {-3.0f, -2.75f, 2.75f, 0},
    {-5.0f, -2.75f, -2.00f, 1},
    {-4.5f, 1.25f, -2.00f, 2},
    {-1.25f, -2.17f, -1.0f, 3},
    {0.1f, 2.5f, 1.75f, 4},
    {1.5f, -1.5f, -1.25f, 5},
    {2.0f, 0.7f, -3.25f, 6},
    {4.75f, -2.75f, -2.75f, 7},
    {3.0f, -2.75f, 2.75f, 8}
    };*/


    std::vector<float> railVertices;
    generateRails(keyPoints, railVertices);

    unsigned int VAOrails;
    formVAOs(railVertices.data(), railVertices.size() * sizeof(float), VAOrails);

    glClearColor(0.12f, 0.8f, 1.0f, 1.0f);
    float lastFrame = 0.0f;
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        double startTime = glfwGetTime();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        double currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processCameraOrbit(window, deltaTime);

        glm::mat4 view = glm::lookAt(camPos, coasterCenter, camUp);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int railPoints = railVertices.size() / 6;

        glUseProgram(colorShader);
        GLuint modelLoc = glGetUniformLocation(colorShader, "uM");
        GLuint viewLoc = glGetUniformLocation(colorShader, "uV");
        GLuint projLoc = glGetUniformLocation(colorShader, "uP");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAOrails);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, railPoints);

        while (glfwGetTime() - startTime < 1.0 / 75) {}
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
