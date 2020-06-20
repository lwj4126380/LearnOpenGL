#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "openglwidget.h"
#include <QVBoxLayout>

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

