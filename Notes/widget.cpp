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
#include <QMessageBox>
#include <QStyledItemDelegate>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowProperties();                                  //Меняем свойства окна
    setupMainMenu();                                        //Устанавливаем главное меню
    styleHelper = new StyleHelper;                          //Объект для работы с внешним видом виджетов
    setTheme();                                             //Настраиваем внешний вид виджетов
    setupEditor();                                          //Устанавливаем виджет редактора
    loadData();                                             //Загрузка данных
    ui->toolBox->setCurrentIndex(0);                        //Первый Блокнот должен быть активным по умолчанию
}

Widget::~Widget()
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        isRelease = 0;
        mousePreviousPosition = event->pos();                  //Запоминаем позицию в момент клика ЛКМ
        mouseClickType = checkCollision(event->screenPos());   //Определяем, где был выполнен клик
        if(!isMaximized())
            changeCursor();                                     //Меняем указатель мыши
    }
}
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        isRelease = 1;
        mouseClickType = MouseType::None;
        changeCursor(MouseType::None);                          //Меняем указатель мыши
    }
}
void Widget::mouseMoveEvent(QMouseEvent* event )
{
    QPointF position = event->screenPos();
    //Меняем внешний вид указателя, если пользователь не выполнил клик по границе окна
    if(!isMaximized() && isRelease){
        changeCursor(checkCollision(position));
        return;
    }

    //Если пользователь пытается изменить границы окна с помощью мыши
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

//Развернуть окно по двойному клику
//Должно быть только виджету "зголовка окна"!
void Widget::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    maxButtonSlot();
}

//Вид курсора мыши за пределами "регионов"
bool Widget::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->leftColumnWidget || target == ui->middleColumnWidget || target == ui->editorWidget) {

        if (event->type() == QEvent::MouseMove) {
            mouseMoveEvent(static_cast<QMouseEvent*>(event));
        }
    }
    return QWidget::eventFilter(target, event);
}

//Проверка попадания указателя мыши в области на границе окна
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

//Смена внешнего вида указателя мыши
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

//Изменяем внешний вид элементов интерфейса в соответствии с настройками
//из файла в формате JSON
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

    ui->leftAddLayout->setMargin(0);
    ui->leftAddLayout->setSpacing(0);
    ui->leftAddButton->setFixedSize(26,26);
    ui->addComboBox->setFixedHeight(26);
    ui->leftColumnWidget->setStyleSheet(styleHelper->getLeftColumnStyle());
    ui->notepadsPage->setStyleSheet(styleHelper->getLeftPageStyle());
    ui->toolBox->setStyleSheet(styleHelper->getLeftTabTitleStyle());
    ui->toolBox->layout()->setSpacing(0);

    //Назначаем разные иконки разным вкладкам ToolBox
    int tabNum = 0;
    foreach( QWidget* w, ui->toolBox->findChildren<QWidget*>() )
    {
        if( w->inherits("QToolBoxButton") )
        {
            tabNum ++;
            QAbstractButton* button = qobject_cast<QAbstractButton*>(w);
            if(tabNum==1){
                button->setIcon(QIcon(styleHelper->getIconPath(Tab::NOTEPADS)));
            }
            if(tabNum==2){
                button->setIcon(QIcon(styleHelper->getIconPath(Tab::TASKS)));
            }
            if(tabNum==3){
                button->setIcon(QIcon(styleHelper->getIconPath(Tab::NOTES)));
            }
            button->setMinimumHeight(styleHelper->getTabHeight());
        }
    }

    ui->LeftScrollAreaWidgetContents->layout()->setContentsMargins(0,0,0,0);
    ui->addComboBox->view()->setItemDelegate(new QStyledItemDelegate(this));

    ui->notepadsTreeWidget->setStyleSheet(styleHelper->getTreeWidgetStyle());

    ui->notepadsTreeWidget->viewport()->setFocusPolicy(Qt::NoFocus);


}

void Widget::loadData()
{

    if(!dataBase.connectDb()){
        //Если соединение с базой данных не установлено предупреждаем об этом пользователя
        QMessageBox messageBox(QMessageBox::Warning,
                               "Ошибка подключения к базе данных",
                               "Не удалось подключиться к базе данных.\n"
                               "Сохранение данных не представляется возможным.",
                               QMessageBox::Yes,
                               this);
        messageBox.setButtonText(QMessageBox::Yes, "Понятно");          //Меняем текст кнопки
        messageBox.exec();                                              //Открываем системное окно

        return;
    }

    //Если соединение с базой данных установлено, инициируем получение данных
    showNotepads();                                                                     //Получаем из базы данных и отображаем Блокноты
    ui->notepadsTreeWidget->setCurrentItem(ui->notepadsTreeWidget->topLevelItem(0));    //Первый Блокнот должне быть активным по умолчанию
}

void Widget::showNotepads()
{
    ui->notepadsTreeWidget->clear();

    QVector <Notepad> notepads = dataBase.getNotepads();           //Получаем вектор блокнотов из базы данных
    for(Notepad& np: notepads){
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0,np.second);                                //Название блокнота
        item->setData(0,NotesApp::NotepadId, np.first);            //id блокнота в базе таблице notepads
        ui->notepadsTreeWidget->addTopLevelItem(item);
    }
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

void Widget::changeActivNotepadSlot(QTreeWidgetItem *current, QTreeWidgetItem * prev)
{
    Q_UNUSED(prev);
    if(current == nullptr)
        return;
    Notes notes = dataBase.getNotes(current->data(0,NotesApp::NotepadId).toInt());  //Список заметок из базы данных

    //Построение списка заметок
    ui->notesListWidget->clear();
    for(Note& note:notes){
        QListWidgetItem *item = new QListWidgetItem(note.title);
        item->setData(NotesApp::NoteId, note.id);
        ui->notesListWidget->addItem(item);
    }
    ui->notesListWidget->setCurrentRow(0);                                      //Активная заметка по умолчанию
}

void Widget::changeActivNoteSlot(QListWidgetItem *current, QListWidgetItem * /*prev*/)
{

    if(current == nullptr)
        return;
    Note note = dataBase.getNote(current->data(NotesApp::NoteId).toInt());     //Данные одной заметки из базы данных
    editor->setData(note);                                                     //Загружаем данные в визуальный редактор
}


void Widget::setWindowProperties()
{
    this->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(9);
    shadow->setOffset(0);
    ui->mainWidget->setGraphicsEffect(shadow);

    connect(ui->maxWindowButton,   &QPushButton::clicked, this, &Widget::maxButtonSlot);
    connect(ui->closeWindowButton, &QPushButton::clicked, this, &QWidget::close);

    connect(ui->notepadsTreeWidget, &QTreeWidget::currentItemChanged, this, &Widget::changeActivNotepadSlot);
    connect(ui->notesListWidget, &QListWidget::currentItemChanged, this, &Widget::changeActivNoteSlot);

    this->setMouseTracking(true);
    ui->mainWidget->setMouseTracking(true);
    ui->frame->setMouseTracking(true);

    ui->leftColumnWidget->installEventFilter(this);
    ui->leftColumnWidget->setMouseTracking(true);

    ui->middleColumnWidget->installEventFilter(this);
    ui->middleColumnWidget->setMouseTracking(true);

    ui->editorWidget->installEventFilter(this);
    ui->editorWidget->setMouseTracking(true);

    QList<int> splitSize {200,200,600};
    ui->splitter->setSizes(splitSize);
    ui->notepadsTreeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->splitter->setStretchFactor(0,0);
    ui->splitter->setStretchFactor(1,0);
    ui->splitter->setStretchFactor(2,1);
}

void Widget::setupMainMenu()
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

void Widget::setupEditor()
{
    editor = new Editor;

    QBoxLayout* box = new QBoxLayout(QBoxLayout::TopToBottom);
    ui->editorWidget->setLayout(box);
    box->addWidget(editor);
}


