#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "checker.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    BodyTextEdit* editor = new BodyTextEdit;
    setCentralWidget(editor);
}

MainWindow::~MainWindow()
{
    delete ui;
}
