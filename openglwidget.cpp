#include "openglwidget.h"
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

OpenGLWidget::OpenGLWidget()
{

}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    m_vertex = new QOpenGLVertexArrayObject;
    m_vertex->create();
    m_vertex->bind();

    m_vertexBuffer = new QOpenGLBuffer;
    m_vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer->create();
    m_vertexBuffer->bind();
    float vertices[6][2] =
    {
    { -0.90f, -0.90f }, // Triangle 1
    { +0.85f, -0.90f },
    { -0.90f, +0.85f },

    { +0.90f, -0.85f }, // Triangle 2
    { +0.90f, +0.90f },
    { -0.85f, +0.90f }
    };
    m_vertexBuffer->allocate(sizeof(vertices));
    m_vertexBuffer->write(0, vertices, sizeof (vertices));
    const int position = 0;
    glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 2*sizeof (float), (void *)0);
    glEnableVertexAttribArray(position);
    m_vertexBuffer->release();
    m_vertex->release();

    m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    bool b = m_vertexShader->compileSourceCode(R"(
                                    #version 330 core
                                    layout (location = 0) in vec2 aPos;

                                    void main()
                                    {
                                      gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);
                                    }
                                    )");

    b = m_fragmentShader->compileSourceCode(R"(
                                    #version 330 core
                                    out vec4 FragColor;

                                    void main()
                                    {
                                        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
                                    }
                                    )");
    m_shaderProgram = new QOpenGLShaderProgram;
    m_shaderProgram->addShader(m_vertexShader);
    m_shaderProgram->addShader(m_fragmentShader);
    m_shaderProgram->link();
}

void OpenGLWidget::resizeGL(int w, int h)
{

}

void OpenGLWidget::paintGL()
{
    m_vertex->bind();
    m_shaderProgram->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_vertex->release();
    m_shaderProgram->release();
}
