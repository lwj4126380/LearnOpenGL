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
#include <QTimer>
#include <QDateTime>
#include <QtMath>

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

#define G_VALUE 1000
#define STRAWBERRY_TIME 4

void calcPosition(int &width, int &height)
{
    int stepx = 1920 / 5;
    int stepy = 1080 / 3;
    int x_r = 1 % 5;
    int y_r = 1 / 5;

    width = (x_r < 2 ? (x_r - 2.5) * stepx : (x_r - 1.5) * stepx);
    height = 1080 - (y_r + 0.5) * stepy;
}

QImage testImage;

GLTest::GLTest()
{
    testImage = QImage("E:/test1.jpg");
    surface = new QOffscreenSurface;
    surface->create();

    label = new QLabel();
    label->resize(2000, 1400);
    label->show();

    qint64 m_gameStartTime = QDateTime::currentMSecsSinceEpoch();
    moveToThread(this);

//    QTimer *timer = new QTimer;
//    timer->moveToThread(qApp->thread());
//    connect(timer, &QTimer::timeout, timer, [=](){
//        int s, h;
//        calcPosition(s, h);
//        qreal deltaTime = (QDateTime::currentMSecsSinceEpoch() -
//                   m_gameStartTime) /
//                  1000.;
//        qreal gvalue =
//            8 * h / (STRAWBERRY_TIME * STRAWBERRY_TIME);
//        int s1 = s / qSqrt(8 * h / gvalue) * deltaTime;
//        int h1 = qSqrt(2 * gvalue * h) * deltaTime -
//             0.5 * gvalue * deltaTime * deltaTime;
//        qDebug() << "AAAA " << h1;
//        QPoint center =
//            QPoint(s1 + 1920 / 2 -
//                       180 / 2,
//                   h1+180);

//        //qDebug() << "AAAAAAA " << center.x() << center.y();
//        QMetaObject::invokeMethod(this, "test", Qt::BlockingQueuedConnection,
//                                  Q_ARG(const QImage &, testImage),
//                                  Q_ARG(bool, true),
//                                  Q_ARG(int, 0),
//                                  Q_ARG(int, center.x()),
//                                  Q_ARG(int, center.y()));
//    });
//    timer->start(30);
}

GLTest::~GLTest()
{
    surface->deleteLater();

}

void GLTest::run()
{
    qDebug() << "BBBBBBB " << QThread::currentThreadId();
    initOpenGLContext();
    initShader();
    initVertexData();
    initTexture();

    test(QImage("E:/test1.jpg"), true, 0, 480, 480);




    //exec();

    ctx->makeCurrent(surface);
    delete m_fbo;
    if (m_backgroundTexture)
    {
        m_backgroundTexture->destroy();
        delete m_backgroundTexture;
    }
    m_strawberryTexture->destroy();
    delete m_strawberryTexture;
    m_bombTexture->destroy();
    delete m_bombTexture;

    delete m_shader;
    delete m_vao;
    delete ctx;
    surface->deleteLater();
}

void GLTest::test(const QImage &image, bool drawMask, int maskType, int x, int y)
{
    ctx->makeCurrent(surface);
    bool sizeChanged = (image.width() != m_fboWidth || image.height() != m_fboHeight);
    if (sizeChanged)
    {
        m_fboWidth = image.width();
        m_fboHeight = image.height();
    }

    if (!m_fbo || sizeChanged)
    {
        if(m_fbo)
            delete m_fbo;

        m_fboWidth = image.width();
        m_fboHeight = image.height();
        QOpenGLFramebufferObjectFormat format;
        format.setInternalTextureFormat(GL_RGBA);
        m_fbo = new QOpenGLFramebufferObject(m_fboWidth, m_fboHeight, format);
    }

    if (!m_backgroundTexture || sizeChanged)
    {
        if (m_backgroundTexture)
        {
            m_backgroundTexture->destroy();
            delete m_backgroundTexture;
        }

        m_backgroundTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        m_backgroundTexture->setSize(m_fboWidth, m_fboHeight);
        m_backgroundTexture->setFormat(QOpenGLTexture::RGBA8_UNorm);
        m_backgroundTexture->allocateStorage();
    }

    QImage ii = image.convertToFormat(QImage::Format_RGBA8888);
    m_backgroundTexture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, ii.constBits());

    m_fbo->bind();

    glActiveTexture(GL_TEXTURE0);
    m_backgroundTexture->bind();
    glActiveTexture(GL_TEXTURE1);
    m_strawberryTexture->bind();

    glViewport(0, 0, m_fboWidth, m_fboHeight);
    {
        m_vao->bind();
        m_shader->bind();
        {
            QMatrix4x4 model;
            model.setToIdentity();
            model.scale(1, -1);
//            model.scale(-1, 1);

            float maskX = 2. * x / m_fboWidth - 1;
            float maskY = 2. * y / m_fboHeight - 1;

            m_shader->setUniformValue("leftTop", QVector2D(maskX, maskY));
            m_shader->setUniformValue("maskSize", QVector2D((float)m_strawberryTexture->width()/m_fboWidth*2, -(float)m_strawberryTexture->height()/m_fboHeight*2));
            m_shader->setUniformValue("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }



        m_vao->release();
        m_shader->release();
    }

    auto fi = m_fbo->texture();
    m_fbo->release();
    QImage image1 = m_fbo->toImage();
    QMetaObject::invokeMethod(label, [=](){
        label->setPixmap(QPixmap::fromImage(image1));
    });

    ctx->doneCurrent();
}

void GLTest::initShader()
{
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
                                            uniform vec2 maskSize;
                                            void main()
                                            {
                                                vec4 imageColor = texture(image, TexCoords);
                                                //vec2 v-mirror = vec2(leftTop.x, leftTop.)
                                                if (mainPosition.x >= leftTop.x && mainPosition.y <= leftTop.y && mainPosition.x <= leftTop.x + maskSize.x && mainPosition.y >= leftTop.y + maskSize.y) {
                                                    //color = vec4(1.0, 0.0, 0.0, 1.0);
                                                    vec2 maskCoords = vec2((mainPosition.x - leftTop.x) / maskSize.x, (mainPosition.y - leftTop.y) / maskSize.y);
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
    m_shader = new QOpenGLShaderProgram;
    m_shader->addShader(m_vertexShader);
    m_shader->addShader(m_fragmentShader);
    m_shader->link();

    m_shader->bind();
    m_shader->setUniformValue("image", 0);
    m_shader->setUniformValue("maskImage", 1);
    m_shader->release();
}

void GLTest::initOpenGLContext()
{
    ctx = new QOpenGLContext;
    ctx->create();
    ctx->makeCurrent(surface);
    initializeOpenGLFunctions();

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLTest::initVertexData()
{
    m_vao = new QOpenGLVertexArrayObject;
    m_vao->create();
    m_vao->bind();

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
    m_vao->release();
}

void GLTest::initTexture()
{
    m_strawberryTexture = new QOpenGLTexture(QImage("C:/Users/luweijia.YUPAOPAO/Desktop/strawberry2.png"));
    m_bombTexture = new QOpenGLTexture(QImage("C:/Users/luweijia.YUPAOPAO/Desktop/bomb2.png"));
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

