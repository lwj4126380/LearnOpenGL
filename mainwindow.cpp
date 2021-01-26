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
#include <QOpenGLFramebufferObjectFormat>

GLTest::GLTest()
{
    surface = new QOffscreenSurface;
    surface->create();

    label = new QLabel();
    label->resize(2000, 1400);
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
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    auto m_vertex = new QOpenGLVertexArrayObject;
    m_vertex->create();
    m_vertex->bind();

    auto m_vertexBuffer = new QOpenGLBuffer;
    m_vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer->create();
    m_vertexBuffer->bind();
    float vertices[] =
    {
        -1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f
    };

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
                                               out vec4 mainPosition;

                                               uniform mat4 model;
                                               uniform mat4 projection;

                                               void main()
                                               {
                                                   TexCoords = vertex.zw;
                                                   mainPosition = vec4(vertex.xy, 0.0, 1.0);
                                                   gl_Position = model * mainPosition;
                                               }
                                               )");

    b = m_fragmentShader->compileSourceCode(R"(
                                            #version 330 core
                                            in vec2 TexCoords;
                                            in vec4 mainPosition;
                                            out vec4 color;

                                            uniform sampler2D image;
                                            uniform sampler2D maskImage;
                                            uniform vec2 leftTop;
                                            uniform vec2 rightBottom;
                                            void main()
                                            {
                                                vec4 imageColor = texture(image, TexCoords);
                                                if (mainPosition.x >= leftTop.x && mainPosition.y <= leftTop.y && mainPosition.x <= rightBottom.x && mainPosition.y >= rightBottom.y) {
                                                    //color = vec4(1.0, 0.0, 0.0, 1.0);
                                                    vec2 maskCoords = vec2((mainPosition.x - leftTop.x) / (rightBottom.x - leftTop.x), (mainPosition.y - leftTop.y) / (rightBottom.y - leftTop.y));
                                                    vec4 maskColor = texture(maskImage, maskCoords);
                                                    vec4 outputColor;
                                                    float a = maskColor.a + imageColor.a * (1.0 - maskColor.a);
                                                    float alphaDivisor = a + step(a, 0.0);
                                                    outputColor.r = (maskColor.r * maskColor.a + imageColor.r * imageColor.a * (1.0 - maskColor.a))/alphaDivisor;
                                                    outputColor.g = (maskColor.g * maskColor.a + imageColor.g * imageColor.a * (1.0 - maskColor.a))/alphaDivisor;
                                                    outputColor.b = (maskColor.b * maskColor.a + imageColor.b * imageColor.a * (1.0 - maskColor.a))/alphaDivisor;
                                                    outputColor.a = a;
                                                    color = outputColor;
                                                }
                                                else {
                                                    color = imageColor;
                                                }
                                            }
                                            )");
    auto m_shaderProgram = new QOpenGLShaderProgram;
    m_shaderProgram->addShader(m_vertexShader);
    m_shaderProgram->addShader(m_fragmentShader);
    m_shaderProgram->link();

    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("image", 0);
    m_shaderProgram->setUniformValue("maskImage", 1);

    auto m_texture = new QOpenGLTexture(QImage("E:/test1.jpg"));
    auto strawberry = new QOpenGLTexture(QImage("C:/Users/luweijia.YUPAOPAO/Desktop/strawberry2.png"));

    auto fbo = new QOpenGLFramebufferObject(m_texture->width(), m_texture->height());

    glActiveTexture(GL_TEXTURE0);
    m_texture->bind();
    glActiveTexture(GL_TEXTURE1);
    strawberry->bind();

    b = fbo->bind();
    glViewport(0, 0, fbo->width(), fbo->height());
    {
        m_vertex->bind();
        m_shaderProgram->bind();
        {
            QMatrix4x4 model;
            model.setToIdentity();
//            model.scale(1, -1);
//            model.scale(-1, 1);

            qDebug() << "AAAA " << strawberry->width() << strawberry->height();
            m_shaderProgram->setUniformValue("leftTop", QVector2D(-0.5, -0.5));
            m_shaderProgram->setUniformValue("rightBottom", QVector2D(-0.5+(float)strawberry->width()/fbo->width()*2, -0.5-(float)strawberry->height()/fbo->height()*2));
            m_shaderProgram->setUniformValue("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

//        {
//            strawberry->bind();
//            QMatrix4x4 model;
//            model.setToIdentity();
//            model.scale(180.0/fbo->width(), 180.0/fbo->height());
//            model.translate(200.0/180.0, 0); // 偏移值是相对自身大小的百分比
//            m_shaderProgram->setUniformValue("model", model);
//            glDrawArrays(GL_TRIANGLES, 0, 6);
//            strawberry->release();
//        }

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

