#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <GL/gl.h>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;
    GLuint VAO = 0;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    void InitShader();
    void InitVBO();
    void Draw();
    void ShaderLog(GLuint shader);
    void ProgramLog(GLuint program);
    void checkOpenGLerror();

    void ReleaseShader();
    void ReleaseVBO();
    void ReleaseVAO();

    GLuint Program = 0;
    GLint  Attrib_vertex = -1;
    GLuint VBO = 0;
};
#endif // GLWIDGET_H
