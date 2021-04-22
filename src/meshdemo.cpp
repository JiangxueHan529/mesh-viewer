#include "AGL.h"
#include "AGLM.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include "mesh.h"
#include "osutils.h"

using namespace std;
using namespace glm;
using namespace agl;

// globals
Mesh theModel;
int theCurrentModel = 0;
vector<string> theModelNames;

// OpenGL IDs
GLuint theVboPosId;
GLuint theVboNormalId;
GLuint theElementbuffer;


float dist = 3.0f;
float Azimuth = 0.0f;
float Elevation = 0.0f;
float startx = 0.0f;
float starty = 0.0f;
bool shiftDown = false;
bool mouseDown = false;
glm::mat4 _transform;

static void LoadModel(int modelId)
{
    assert(modelId >= 0 && modelId < theModelNames.size());
    theModel.loadPLY(theModelNames[theCurrentModel]);
    vec3 min_bound = theModel.getMinBounds();
    vec3 max_bound = theModel.getMaxBounds();

    glm::vec3 center = (min_bound + max_bound) / 2.0f;
    float longest;
    if ((max_bound[0] - min_bound[0]) > (max_bound[1] - min_bound[1]) && (max_bound[0] - min_bound[0]) > (max_bound[2] - min_bound[2])) {
        longest = max_bound[0] - min_bound[0];
    }
    else if ((max_bound[1] - min_bound[1]) > (max_bound[0] - min_bound[0]) && (max_bound[1] - min_bound[1]) > (max_bound[2] - min_bound[2])) {
        longest = max_bound[1] - min_bound[1];
    }
    else {
        longest = max_bound[2] - min_bound[2];
    }
    float ratio = 2.0f / longest;
    glm::mat4 move = glm::translate(glm::mat4(1.0f), -center);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(ratio, ratio, ratio));
    //glm::vec4 vector(1.f, 1.f, 1.f, 1.f);
    _transform = scale * move;
    glBindBuffer(GL_ARRAY_BUFFER, theVboPosId);
    glBufferData(GL_ARRAY_BUFFER, theModel.numVertices() * 3 * sizeof(float), theModel.positions(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, theVboNormalId);
    glBufferData(GL_ARRAY_BUFFER, theModel.numVertices() * 3 * sizeof(float), theModel.normals(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theElementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, theModel.numTriangles() * 3 * sizeof(unsigned int), theModel.indices(), GL_DYNAMIC_DRAW);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == 'P')
    {
        if (--theCurrentModel < 0)
        {
            theCurrentModel = theModelNames.size() - 1;
        }
        cout << "Current file: " << theModelNames[theCurrentModel] << endl;
        LoadModel(theCurrentModel);
    }
    else if (key == 'N')
    {
        theCurrentModel = (theCurrentModel + 1) % theModelNames.size();
        cout << "Current file: " << theModelNames[theCurrentModel] << endl;
        LoadModel(theCurrentModel);
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Prevent a divide by zero
    if (height == 0) height = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, width, height);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    startx = xpos;
    starty = ypos;

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        mouseDown = true;
        int keyPress = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        if (keyPress == GLFW_PRESS) {
            shiftDown = true;
        }

    }
    else if (state == GLFW_RELEASE)
    {
        mouseDown = false;
        shiftDown = false;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (shiftDown == false && mouseDown == true) {
        Azimuth += (xpos - startx) * 0.15f;
        Elevation += (ypos - starty) * 0.15f;
    }
    else if (mouseDown == true) {
        float delta = 0.15f * (ypos - starty);
        dist += delta;
    }
    startx = xpos;
    starty = ypos;
}

static void PrintShaderErrors(GLuint id, const std::string label)
{
    std::cerr << label << " failed\n";
    GLint logLen;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 0)
    {
        char* log = (char*)malloc(logLen);
        GLsizei written;
        glGetShaderInfoLog(id, logLen, &written, log);
        std::cerr << "Shader log: " << log << std::endl;
        free(log);
    }
}

static std::string LoadShaderFromFile(const std::string& fileName)
{
    std::ifstream file(fileName);
    if (!file)
    {
        std::cout << "Cannot load file: " << fileName << std::endl;
        return "";
    }

    std::stringstream code;
    code << file.rdbuf();
    file.close();

    return code.str();
}

static void LoadModels(const std::string& dir)
{
    std::vector<std::string> filenames = GetFilenamesInDir(dir, "ply");
    for (int i = 0; i < filenames.size(); i++)
    {
        std::string filename = filenames[i];
        theModelNames.push_back(dir + filename);
    }
}

static GLuint LoadShader(const std::string& vertex, const std::string& fragment)
{
    GLint result;
    std::string vertexShader = LoadShaderFromFile(vertex);
    const char* vertexShaderRaw = vertexShader.c_str();
    GLuint vshaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshaderId, 1, &vertexShaderRaw, NULL);
    glCompileShader(vshaderId);
    glGetShaderiv(vshaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        PrintShaderErrors(vshaderId, "Vertex shader");
        return -1;
    }

    std::string fragmentShader = LoadShaderFromFile(fragment);
    const char* fragmentShaderRaw = fragmentShader.c_str();
    GLuint fshaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshaderId, 1, &fragmentShaderRaw, NULL);
    glCompileShader(fshaderId);
    glGetShaderiv(fshaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        PrintShaderErrors(fshaderId, "Fragment shader");
        return -1;
    }

    GLuint shaderId = glCreateProgram();
    glAttachShader(shaderId, vshaderId);
    glAttachShader(shaderId, fshaderId);
    glLinkProgram(shaderId);
    glGetShaderiv(shaderId, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        PrintShaderErrors(shaderId, "Shader link");
        return -1;
    }
    return shaderId;
}


int main(int argc, char** argv)
{
    GLFWwindow* window;

    if (!glfwInit())
    {
        return -1;
    }

    // Explicitly ask for a 4.0 context 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(500, 500, "Mesh Viewer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current 
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

#ifndef APPLE
    if (glewInit() != GLEW_OK)
    {
        return -1;
    }
#endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0, 0, 0, 1);

    glGenBuffers(1, &theVboPosId);
    glGenBuffers(1, &theVboNormalId);
    glGenBuffers(1, &theElementbuffer);


    GLuint vaoId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    glEnableVertexAttribArray(0); // 0 -> Sending VertexPositions to array #0 in the active shader
    glBindBuffer(GL_ARRAY_BUFFER, theVboPosId); // always bind before setting data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

    glEnableVertexAttribArray(1); // 1 -> Sending Normals to array #1 in the active shader
    glBindBuffer(GL_ARRAY_BUFFER, theVboNormalId); // always bind before setting data
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

    LoadModels("../models/");
    LoadModel(0);

    GLuint shaderId = LoadShader("../shaders/toon.vs", "../shaders/toon.fs");
    glUseProgram(shaderId);

    GLuint mvpId = glGetUniformLocation(shaderId, "mvp");
    GLuint mvId = glGetUniformLocation(shaderId, "uMV");
    GLuint nmvId = glGetUniformLocation(shaderId, "uNMV");

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 10.0f);
    //glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -2.0f, 2.0f);
    glm::mat4 camera;

    glClearColor(0, 0, 0, 1);

    // Loop until the user closes the window 
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers
        float x = dist * sin(Azimuth) * cos(Elevation);
        float y = dist * sin(Elevation);
        float z = dist * cos(Azimuth) * cos(Elevation);
        camera = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        // Draw primitive
        glm::mat4 mvp = projection * camera * _transform;
        glm::mat4 mv = camera * _transform;
        glm::mat3 nmv = glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2]));

        glUniformMatrix3fv(nmvId, 1, GL_FALSE, &nmv[0][0]);
        glUniformMatrix4fv(mvId, 1, GL_FALSE, &mv[0][0]);
        glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);
        glUniform3f(glGetUniformLocation(shaderId, "uMaterial.Ks"), 1.0, 1.0, 1.0);
        glUniform3f(glGetUniformLocation(shaderId, "uMaterial.Kd"), 0.4, 0.6, 1.0);
        glUniform3f(glGetUniformLocation(shaderId, "uMaterial.Ka"), 0.1, 0.1, 0.1);
        glUniform1f(glGetUniformLocation(shaderId, "uMaterial.shininess"), 80.0);
        glUniform4f(glGetUniformLocation(shaderId, "uLight.position"), 100.0, 100.0, 100.0, 1.0f);
        glUniform3f(glGetUniformLocation(shaderId, "uLight.color"), 0.7, 0.7, 1.0);
        glUniform4f(glGetUniformLocation(shaderId, "Light.position"), 100.0, 100.0, 100.0, 1.0f);
        glUniform3f(glGetUniformLocation(shaderId, "Light.intensity"), 1, 1, 1);
        glUniform3f(glGetUniformLocation(shaderId, "Kd"), 0.4, 0.6, 1.0);
        glUniform3f(glGetUniformLocation(shaderId, "Ka"), 0.1, 0.1, 0.1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theElementbuffer);
        glDrawElements(GL_TRIANGLES, theModel.numTriangles() * 3, GL_UNSIGNED_INT, (void*)0);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}




