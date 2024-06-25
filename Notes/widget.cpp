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
#include "editor.h"
#include <QBoxLayout>
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

    ui->splitter->setStretchFactor(0,0);
    ui->splitter->setStretchFactor(1,0);
    ui->splitter->setStretchFactor(2,1);

    editor = new Editor;

    QBoxLayout* box = new QBoxLayout(QBoxLayout::TopToBottom);
    ui->editorWidget->setLayout(box);
    box->addWidget(editor);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        isRelease = 0;
        mousePreviousPosition = event->pos();
        qDebug () << mousePreviousPosition;
        mouseClickType = checkCollision(event->screenPos());
        if(!isMaximized())
            changeCursor();
    }
}
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        isRelease = 1;
        mouseClickType = MouseType::None;
        changeCursor();
    }
}
void Widget::mouseMoveEvent(QMouseEvent* event )
{
    QPointF position = event->screenPos();
    if(!isMaximized() and isRelease)
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
    case MouseType::TopLeft:
        if (!isMaximized()) {
             int deltaX = mapToGlobal(event->pos()).x()-x();
             int deltaY = event->y() - mousePreviousPosition.y();
             setGeometry(mapToGlobal(event->pos()).x(), y() + deltaY, width() - deltaX, height() - deltaY);
             mousePreviousPosition = event->pos();
        }
        break;
    case MouseType::TopRight:
        if (!isMaximized()) {
             int deltaX = event->x() - mousePreviousPosition.x();
             int deltaY = event->y() - mousePreviousPosition.y();
             setGeometry(x(), y() + deltaY, width() + deltaX, height() - deltaY);
             mousePreviousPosition = event->pos();
        }
        break;

    case MouseType::BottomLeft:
        if (!isMaximized()) {
             int deltaX = mapToGlobal(event->pos()).x()-x();
             int deltaY = event->y() - mousePreviousPosition.y();
             setGeometry(mapToGlobal(event->pos()).x(), y(), width() - deltaX, height() + deltaY);
             mousePreviousPosition = event->pos();
        }
        break;
    case MouseType::BottomRight:
        if (!isMaximized()) {
             int deltaX = event->x() - mousePreviousPosition.x();
             int deltaY = event->y() - mousePreviousPosition.y();
             setGeometry(x(), y(), width() + deltaX, height() + deltaY);
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

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << "DoubleClick";
    maxButtonSlot();
}

MouseType Widget::checkCollision(const QPointF &mousePos)
{
    float winX      = this->x();
    float winY      = this->y();
    float winWidth  = this->width();
    float winHeight = this->height();

    QRectF rectTop    = {winX + 20, winY,  winWidth  - 40, 9};
    QRectF rectBottom = {winX + 20, winY + winHeight - 9,  winWidth - 40, 9};
    QRectF rectLeft   = {winX, winY + 20,  9,  winHeight - 40};
    QRectF rectRight  = {winX + winWidth - 9,  winY + 20,  9, winHeight - 40};
    QRectF rectTopLeft  = {winX,  winY,  15, 15};
    QRectF rectTopRight  = {winX + winWidth - 15,  winY,  15, 15};
    QRectF rectBottomLeft = {winX,  winY + winHeight - 15,  15, 15};
    QRectF rectBottomRight = {winX + winWidth - 15,  winY + winHeight - 15,  15,15};
    QRectF rectMainTop= {winX + 10, winY + 10, winWidth -  20, 30};

    if(rectTop.contains(mousePos)){
        return MouseType::Top;
    }else if(rectBottom.contains(mousePos)){
        return MouseType::Bottom;
    }else if(rectLeft.contains(mousePos)){
        return MouseType::Left;
    }else if(rectRight.contains(mousePos)){
        return MouseType::Right;
    }else if(rectTopLeft.contains(mousePos)){
        return MouseType::TopLeft;
    }else if(rectTopRight.contains(mousePos)){
        return MouseType::TopRight;
    }else if(rectBottomLeft.contains(mousePos)){
        return MouseType::BottomLeft;
    }else if(rectBottomRight.contains(mousePos)){
        return MouseType::BottomRight;
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
    case MouseType::TopLeft:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case MouseType::TopRight:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case MouseType::BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case MouseType::BottomRight:
        setCursor(Qt::SizeFDiagCursor);
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

//Файл
void Widget::openSlot()
{

}

void Widget::exportSlot()
{

}

void Widget::exitSlot()
{
    close();
}
//Заметки
void Widget::newSlot()
{
}

void Widget::saveCopySlot()
{

}

void Widget::saveAsSlot()
{

}

void Widget::clearSlot()
{

}

void Widget::deleteSlot()
{

}
//Редактировать
void Widget::cancelSlot()
{

}

void Widget::undoSlot()
{

}

void Widget::copySlot()
{

}

void Widget::pasteSlot()
{

}

void Widget::cutSlot()
{

}

void Widget::settingsSlot()
{

}
//Помощь
void Widget::helpSlot()
{

}

void Widget::aboutProgSlot()
{

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

    QAction* openAction = new QAction("Открыть");
    QAction* exportAction = new QAction("Экспорт");
    QAction* exitAction = new QAction("Выход");

    menuFile->addAction(openAction);
    menuFile->addAction(exportAction);
    menuFile->addAction(exitAction);

    connect(openAction, &QAction::triggered, this, &Widget::openSlot);
    connect(exportAction, &QAction::triggered, this, &Widget::exportSlot);
    connect(exitAction, &QAction::triggered, this, &Widget::exitSlot);

    QMenu* menuNotes = new QMenu("&Заметки");

    menuBar->addMenu(menuNotes);

    QAction* newAction = new QAction("Новый");
    QAction* saveCopyAction = new QAction("Сохранить копию");
    QAction* saveAsAction = new QAction("Сохранить как");
    QAction* clearAction = new QAction("Очистить");
    QAction* deleteAction = new QAction("Удалить");

    menuNotes->addAction(newAction);
    menuNotes->addAction(saveCopyAction);
    menuNotes->addAction(saveAsAction);
    menuNotes->addAction(clearAction);
    menuNotes->addAction(saveAsAction);

    connect(newAction, &QAction::triggered, this, &Widget::newSlot);
    connect(saveCopyAction, &QAction::triggered, this, &Widget::saveCopySlot);
    connect(saveAsAction, &QAction::triggered, this, &Widget::saveAsSlot);
    connect(clearAction, &QAction::triggered, this, &Widget::clearSlot);
    connect(deleteAction, &QAction::triggered, this, &Widget::deleteSlot);

    QMenu* menuEdit = new QMenu("&Редактировать");

    menuBar->addMenu(menuEdit);

    QAction* cancelAction = new QAction("Отменить");
    QAction* undoAction = new QAction("Вернуть");
    QAction* copyAction = new QAction("Копировать");
    QAction* pasteAction = new QAction("Вставить");
    QAction* cutAction = new QAction("Вырезать");
    QAction* settingsAction = new QAction("Настройки");

    menuEdit->addAction(cancelAction);
    menuEdit->addAction(undoAction);
    menuEdit->addAction(copyAction);
    menuEdit->addAction(pasteAction);
    menuEdit->addAction(cutAction);
    menuEdit->addAction(settingsAction);

    connect(cancelAction, &QAction::triggered, this, &Widget::cancelSlot);
    connect(undoAction, &QAction::triggered, this, &Widget::undoSlot);
    connect(copyAction, &QAction::triggered, this, &Widget::copySlot);
    connect(pasteAction, &QAction::triggered, this, &Widget::pasteSlot);
    connect(cutAction, &QAction::triggered, this, &Widget::cutSlot);
    connect(settingsAction, &QAction::triggered, this, &Widget::settingsSlot);

    QMenu* menuHelp = new QMenu("&Помощь");

    menuBar->addMenu(menuHelp);

    QAction* helpAction = new QAction("Справка");
    QAction* aboutProgAction = new QAction("О программе");

    menuHelp->addAction(helpAction);
    menuHelp->addAction(aboutProgAction);

    connect(helpAction, &QAction::triggered, this, &Widget::helpSlot);
    connect(aboutProgAction, &QAction::triggered, this, &Widget::aboutProgSlot);
}


