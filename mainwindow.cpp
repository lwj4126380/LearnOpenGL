#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "openglwidget.h"
#include <QVBoxLayout>
#include <QOpenGLFramebufferObject>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

GLTest::GLTest()
{
    surface = new QOffscreenSurface;
    surface->create();

    label = new QLabel();
    label->resize(1280, 800);
    label->show();
}

void GLTest::run()
{
    ctx = new QOpenGLContext;
    ctx->create();
    ctx->makeCurrent(surface);
    initializeOpenGLFunctions();

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);


    auto fbo = new QOpenGLFramebufferObject(640, 480);

    auto m_vertex = new QOpenGLVertexArrayObject;
    m_vertex->create();
    m_vertex->bind();

    auto m_vertexBuffer = new QOpenGLBuffer;
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

    auto m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    auto m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
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
    auto m_shaderProgram = new QOpenGLShaderProgram;
    m_shaderProgram->addShader(m_vertexShader);
    m_shaderProgram->addShader(m_fragmentShader);
    m_shaderProgram->link();

    auto m_texture = new QOpenGLTexture(QImage("E:/test.jpg"));

    b = fbo->bind();
    glViewport(0, 0, fbo->width(), fbo->height());
    {
        m_vertex->bind();
        m_shaderProgram->bind();

        QMatrix4x4 projection;
        projection.ortho(QRect(0, 0, 640, 480));
        m_shaderProgram->setUniformValue("projection", projection);

        {
            m_texture->bind();
            QMatrix4x4 model;
            model.setToIdentity();
            model.translate(0, 0);
    //        model.translate(m_texture->width()*0.5f, m_texture->height()*0.5f);
    //        model.rotate(180, QVector3D(0, 0, 1.0));
    //        model.translate(-m_texture->width()*0.5f, -m_texture->height()*0.5f);
            model.scale(QVector3D(640, 480, 1.0));
            m_shaderProgram->setUniformValue("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            m_texture->release();
        }
        m_vertex->release();
        m_shaderProgram->release();
    }

    auto fi = fbo->texture();
    fbo->release();
    QImage image = fbo->toImage();
    QMetaObject::invokeMethod(label, [=](){
        label->setPixmap(QPixmap::fromImage(image));
    });

    ctx->doneCurrent();
    //exec();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QVBoxLayout *v = new QVBoxLayout;
    ui->centralwidget->setLayout(v);
    v->setSpacing(0);
    v->setMargin(0);
    v->addWidget(new OpenGLWidget);

    GLTest *t = new GLTest;
    t->setParent(this);
    t->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

