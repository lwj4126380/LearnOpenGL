#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QOpenGLShaderProgram;
class QOpenGLVertexArrayObject;
class QOpenGLFramebufferObject;
class QOpenGLTexture;

class GLTest : public QThread, protected QOpenGLFunctions
{
Q_OBJECT
public:
    GLTest();
    ~GLTest();
    void run() override;

    Q_INVOKABLE void test(const QImage &image, bool drawMask, int maskType, int x, int y);

private:
    void initShader();
    void initOpenGLContext();
    void initVertexData();
    void initTexture();

private:
    QOffscreenSurface *surface;
    QOpenGLContext *ctx;
    QOpenGLShaderProgram *m_shader = nullptr;
    QOpenGLVertexArrayObject *m_vao = nullptr;
    QOpenGLFramebufferObject *m_fbo = nullptr;
    QOpenGLTexture *m_backgroundTexture = nullptr;
    QOpenGLTexture *m_strawberryTexture = nullptr;
    QOpenGLTexture *m_bombTexture = nullptr;
    int m_fboWidth = 0;
    int m_fboHeight = 0;

    unsigned int tt;

    QLabel *label;

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
