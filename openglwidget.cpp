#include "openglwidget.h"
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>

OpenGLWidget::OpenGLWidget()
{

}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    m_vertex = new QOpenGLVertexArrayObject;
    m_vertex->create();
    m_vertex->bind();

    m_vertexBuffer = new QOpenGLBuffer;
    m_vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer->create();
    m_vertexBuffer->bind();
    float vertices[] =
    {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
//    float vertices[] =
//    {
//        0.0f, 0.0f, 0.0f, 1.0f,
//        1.0f, 1.0f, 1.0f, 0.0f,
//        0.0f, 1.0f, 0.0f, 0.0f,

//        0.0f, 0.0f, 0.0f, 1.0f,
//        1.0f, 1.0f, 1.0f, 0.0f,
//        1.0f, 0.0f, 1.0f, 1.0f
//    };
    m_vertexBuffer->allocate(sizeof(vertices));
    m_vertexBuffer->write(0, vertices, sizeof (vertices));
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof (float), (void *)0);
    glEnableVertexAttribArray(0);
    m_vertexBuffer->release();
    m_vertex->release();

    m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    bool b = m_vertexShader->compileSourceCode(R"(
                                               #version 330 core
                                               layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

                                               out vec2 TexCoords;

                                               uniform mat4 model;
                                               uniform mat4 projection;

                                               void main()
                                               {
                                                   TexCoords = vertex.zw;
                                                   gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
                                               }
                                               )");

    b = m_fragmentShader->compileSourceCode(R"(
                                            #version 330 core
                                            in vec2 TexCoords;
                                            out vec4 color;

                                            uniform sampler2D image;
                                            void main()
                                            {
                                                color = texture(image, TexCoords);
                                            }
                                            )");
    m_shaderProgram = new QOpenGLShaderProgram;
    m_shaderProgram->addShader(m_vertexShader);
    m_shaderProgram->addShader(m_fragmentShader);
    m_shaderProgram->link();

    m_framebufferObject = new QOpenGLFramebufferObject(800, 800);
    m_texture = new QOpenGLTexture(QImage("E:/test.jpg"));
    m_texture2 = new QOpenGLTexture(QImage("E:/test.png"));
}

void OpenGLWidget::resizeGL(int w, int h)
{
    Q_UNUSED(w)
    Q_UNUSED(h)
    m_shaderProgram->bind();
    QMatrix4x4 projection;
    projection.ortho(this->rect());
    m_shaderProgram->setUniformValue("projection", projection);
}

void OpenGLWidget::paintGL()
{
//    m_framebufferObject->bind();
//    glViewport(0, 0, m_framebufferObject->width(), m_framebufferObject->height());
    m_vertex->bind();
    m_shaderProgram->bind();
    {
        m_texture->bind();
        QMatrix4x4 model;
        model.setToIdentity();
        //model.translate(100, 0);
//        model.translate(m_texture->width()*0.5f, m_texture->height()*0.5f);
//        model.rotate(180, QVector3D(0, 0, 1.0));
//        model.translate(-m_texture->width()*0.5f, -m_texture->height()*0.5f);
        model.scale(QVector3D(m_texture->width(), m_texture->height(), 1.0));
        m_shaderProgram->setUniformValue("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_texture->release();
    }

    {
        m_texture2->bind();
        QMatrix4x4 model;
        model.translate(400, 100);
        model.rotate(0, QVector3D(0, 0, 1.0));
        model.scale(QVector3D(m_texture2->width(), m_texture2->height(), 1.0));
        m_shaderProgram->setUniformValue("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_texture2->release();
    }
    m_vertex->release();
    m_shaderProgram->release();

//    auto image = m_framebufferObject->toImage();
//    image.save("E:/ccccc.png");
}
