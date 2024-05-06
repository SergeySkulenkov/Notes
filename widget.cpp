#include "widget.h"
#include "./ui_widget.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QMouseEvent>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "stylehelper.h"
#include <QPixmap>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowProperties();
    setMainMenu();
    styleHelper = new StyleHelper;
    setTheme();
    connect(ui->maxWindowButton,   &QPushButton::clicked, this, &Widget::maxButtonSlot);
    connect(ui->closeWindowButton, &QPushButton::clicked, this, &QWidget::close);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        mousePreviousPosition = event->pos();
        mouseClickType = checkCollision(event->screenPos());
        changeCursor();
    }
}
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        mouseClickType = MouseType::None;
        changeCursor();
    }
}
void Widget::mouseMoveEvent(QMouseEvent* event )
{
    QPointF position = event->screenPos();
    changeCursor(checkCollision(position));

    switch (mouseClickType) {
    case MouseType::Top:
        if (!isMaximized()) {
            int deltaY = event->y() - mousePreviousPosition.y();
            setGeometry(x(), y() + deltaY, width(), height() - deltaY);
        }
        break;
    case MouseType::Bottom:
        if(!isMaximized()){
            int deltaY = event->y() - mousePreviousPosition.y();
            setGeometry(x(), y(), width(), height() + deltaY);
            mousePreviousPosition = event->pos();
        }
        break;
    case MouseType::Left:
        if (!isMaximized()) {
            int deltaX = mapToGlobal(event->pos()).x()-x();
            setGeometry(mapToGlobal(event->pos()).x(), y(), width() - deltaX, height());
        }
        break;
    case MouseType::Right:
        if (!isMaximized()) {
            int deltaX = event->x() - mousePreviousPosition.x();
            setGeometry(x(), y(), width() + deltaX, height());
             mousePreviousPosition = event->pos();
        }
        break;
    case MouseType::MainTop:
        if(isMaximized()){
            int newWidth = width()-100;
            int newHeight = height()-100;
            this->layout()->setMargin(9);
            this->showNormal();
            setGeometry(50, 0, newWidth, newHeight);
        }else{
            int deltaX = event->x() - mousePreviousPosition.x();
            int deltaY = event->y() - mousePreviousPosition.y();
            setGeometry(x() + deltaX, y() + deltaY, width(), height());
        }
        break;
    case MouseType::None:
        break;
    }
}

MouseType Widget::checkCollision(const QPointF &mousePos)
{
    float winX      = this->x();
    float winY      = this->y();
    float winWidth  = this->width();
    float winHeight = this->height();

    QRectF rectTop    = {winX + 10, winY,  winWidth  - 20, 9};
    QRectF rectBottom = {winX + 10, winY + winHeight - 9,  winWidth - 20, 9};
    QRectF rectLeft   = {winX, winY + 10,  9,  winHeight - 20};
    QRectF rectRight  = {winX + winWidth - 9,  winY + 10,  9, winHeight - 20};
    QRectF rectMainTop= {winX + 10, winY + 10, winWidth -  20, 30};
    if(rectTop.contains(mousePos)){
        return MouseType::Top;
    }else if(rectBottom.contains(mousePos)){
        return MouseType::Bottom;
    }else if(rectLeft.contains(mousePos)){
        return MouseType::Left;
    }else if(rectRight.contains(mousePos)){
        return MouseType::Right;
    }else if(rectMainTop.contains(mousePos)){
        return MouseType::MainTop;
    }else{
        return MouseType::None;
    }



}

void Widget::changeCursor()
{
    changeCursor(mouseClickType);
}

void Widget::changeCursor(MouseType type)
{
    switch (type) {
    case MouseType::Top:
        setCursor(Qt::SizeVerCursor);
        break;
    case MouseType::Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case MouseType::Left:
        setCursor(Qt::SizeHorCursor);
        break;
    case MouseType::Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case MouseType::MainTop:
        setCursor(QCursor());
        break;
    default:
        setCursor(QCursor());
        break;
    }
}

void Widget::setTheme()
{
    styleHelper->setAppTheme("assets/themes/standart.json");
    ui->iconLabel->setPixmap(QPixmap(styleHelper->getWindowIconPath()));
    ui->iconLabel->setFixedSize(StyleHelper::winIconSize);
    ui->frame->setStyleSheet(styleHelper->getWindowTitleStyle());

    ui->minWindowButton->  setFixedSize(StyleHelper::winBtnSize);
    ui->minWindowButton->  setStyleSheet(styleHelper->getMinimazeButtonStyle());

    ui->maxWindowButton->  setFixedSize(StyleHelper::winBtnSize);
    ui->maxWindowButton->  setStyleSheet(styleHelper->getMaximazeButtonStyle());

    ui->closeWindowButton->setFixedSize(StyleHelper::winBtnSize);
    ui->closeWindowButton->setStyleSheet(styleHelper->getCloseButtonStyle());
}


void Widget::maxButtonSlot()
{
    if(isMaximized()){
        this->layout()->setMargin(9);
        ui->maxWindowButton->  setStyleSheet(styleHelper->getMaximazeButtonStyle());
        showNormal();

    }else{
         this->layout()->setMargin(0);
        ui->maxWindowButton->  setStyleSheet(styleHelper->getNormalButtonStyle());
        showMaximized();

    }
}

void Widget::exitSlot()
{
    close();
}
void Widget::openSlot()
{
    qDebug() << "open slot";
}


void Widget::setWindowProperties()
{
    this->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(9);
    shadow->setOffset(0);
    ui->mainWidget->setGraphicsEffect(shadow);

    this->setMouseTracking(true);
    ui->mainWidget->setMouseTracking(true);
    ui->frame->setMouseTracking(true);
}

void Widget::setMainMenu()
{
    QMenuBar* menuBar = new QMenuBar(ui->menuWidget);
    QMenu* menuFile = new QMenu("&Файл");
    menuBar->addMenu(menuFile);
    QAction* openAction = new QAction("&Открыть");
    QAction* exitAction = new QAction("&Выход");
    menuFile->addAction(openAction);
    menuFile->addAction(exitAction);
    QMenu* menuEdit = new QMenu("&Редактировать");
    menuBar->addMenu(menuEdit);

    connect(openAction, &QAction::triggered, this, &Widget::openSlot);
    connect(exitAction, &QAction::triggered, this, &Widget::exitSlot);
}


