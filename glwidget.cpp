#include "glwidget.h"

#include <iostream>
#include <vector>

struct Vertex { GLfloat x; GLfloat y; };

static const char* VertexShaderSource = R"(
#version 330 core
in vec2 coord;
void main() {
    gl_Position = vec4(coord, 0.0, 1.0);
}
)";

static const char* FragShaderSource = R"(
#version 330 core
out vec4 color;
void main() {
    color = vec4(0, 1, 0, 1);
}
)";

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

GLWidget::~GLWidget()
{
    makeCurrent();
    ReleaseShader();
    ReleaseVBO();
    ReleaseVAO();
    doneCurrent();
}

void GLWidget::ReleaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);
}

void GLWidget::ReleaseShader()
{
    glUseProgram(0);
    glDeleteProgram(Program);
}

void GLWidget::ReleaseVAO()
{
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        checkOpenGLerror();
    }
}

void GLWidget::ShaderLog(GLuint shader)
{
    GLint infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1)
    {
        GLint charsWritten = 0;
        std::vector<char> infoLog(infologLen);
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
        std::cout << "InfoLog: " << infoLog.data() << std::endl;
    }

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        std::cerr << "Shader compilation failed" << std::endl;
    }
}

void GLWidget::ProgramLog(GLuint program)
{
    GLint logLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 1)
    {
        std::vector<char> log(logLen);
        glGetProgramInfoLog(program, logLen, nullptr, log.data());
        std::cerr << "Program log: " << log.data() << std::endl;
    }
}

void GLWidget::checkOpenGLerror()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: 0x" << std::hex << err << std::dec << std::endl;
    }
}

void GLWidget::InitShader()
{
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    glCompileShader(vShader);
    std::cout << "vertex shader " << std::endl;
    ShaderLog(vShader);

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    glCompileShader(fShader);
    std::cout << "fragment shader " << std::endl;
    ShaderLog(fShader);

    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);
    glLinkProgram(Program);
    ProgramLog(Program);

    GLint link_ok = 0;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        std::cerr << "error attach shaders" << std::endl;
        checkOpenGLerror();
        return;
    }

    const char* attr_name = "coord";
    Attrib_vertex = glGetAttribLocation(Program, attr_name);
    if (Attrib_vertex == -1)
    {
        std::cerr << "could not bind attrib " << attr_name << std::endl;
        checkOpenGLerror();
        return;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    checkOpenGLerror();
}

void GLWidget::InitVBO()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    Vertex triangle[3] = {
        { -1.0f, -1.0f },
        {  0.0f,  1.0f },
        {  1.0f, -1.0f }
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkOpenGLerror();
}

void GLWidget::initializeGL()
{

    if (!initializeOpenGLFunctions())
    {
        std::cerr << "initializeOpenGLFunctions() failed" << std::endl;
        return;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    InitShader();
    InitVBO();
}

void GLWidget::Draw()
{
    if (Program == 0 || Attrib_vertex == -1)
    {
        return;
    }

    glBindVertexArray(VAO);
    glUseProgram(Program);
    glEnableVertexAttribArray(Attrib_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(Attrib_vertex);
    glUseProgram(0);
    checkOpenGLerror();
}

void GLWidget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Draw();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    checkOpenGLerror();
}