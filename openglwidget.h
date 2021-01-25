#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class QOpenGLVertexArrayObject;
class QOpenGLBuffer;
class QOpenGLShader;
class QOpenGLShaderProgram;
class QOpenGLFramebufferObject;
class QOpenGLTexture;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    OpenGLWidget();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    QOpenGLVertexArrayObject *m_vertex;
    QOpenGLBuffer *m_vertexBuffer;
    QOpenGLShader *m_vertexShader;
    QOpenGLShader *m_fragmentShader;
    QOpenGLShaderProgram *m_shaderProgram;
    QOpenGLFramebufferObject *m_framebufferObject;
    QOpenGLTexture *m_texture;
    QOpenGLTexture *m_texture2;
};

#endif // OPENGLWIDGET_H
