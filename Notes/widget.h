#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "database.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
class QMouseEvent;
class QTreeWidgetItem;
class QListWidgetItem;

QT_END_NAMESPACE

class StyleHelper;
class Editor;

//Тип клика мыши. Используется для реализации возможности
//изменение размеров окна с помощью мыши.
//Соответствует региону, в котором был выполнен клик.
enum class MouseType{
    None,
    Top,
    Bottom,
    Left,
    Right,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    MainTop
};

//Пользовательские роди для хранения ID из базы данных
namespace NotesApp{
enum UsersData{
    NotepadId = Qt::UserRole + 1,    //id блокнота
    NoteId                           //id заметки
};
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    bool eventFilter(QObject *target, QEvent *event) ;

private slots:

    void maxButtonSlot();           //Окно на весь экран

    void openSlot();                //Выбор файла базы данных
    void exportSlot();              //Экспорт файла
    void exitSlot();                //Выход

    void newSlot();                 //Новая заметка
    void saveCopySlot();            //Сохранить копию
    void saveAsSlot();              //Сохранить как
    void clearSlot();               //Очистить содержимое заметки
    void deleteSlot();              //Удалить заметку

    void cancelSlot();              //Отменить действие
    void undoSlot();                //Вернуть (отменить отменённое действие)
    void copySlot();                //Копировать
    void pasteSlot();               //Вставить
    void cutSlot();                 //Вырезать
    void settingsSlot();            //Настройки

    void helpSlot();                //Справка
    void aboutProgSlot();           //О программе

    void changeActivNotepadSlot(QTreeWidgetItem *current, QTreeWidgetItem *prev);   //Смена активного блокнота
    void changeActivNoteSlot(QListWidgetItem *current, QListWidgetItem *prev);      //Смена активной заметки

private:
    Ui::Widget*  ui;
    MouseType    mouseClickType;                            //Тик клика мыши в зависимости от её положения
    QPoint       mousePreviousPosition;                     //Предыдущая позиция указателя мыши
    StyleHelper* styleHelper;                               //Класс для работы с внешним видом виджетов
    Editor*      editor;                                    //Виджет визуального редактора
    bool isRelease = 1;                                     //Флаг "отпускания" левой кнопки мыши

    void setWindowProperties();                             //Изменение внешнего вида окна
    void setupMainMenu();                                   //Установка главного меню
    void setupEditor();                                     //Установка виджета визуального редактора
    MouseType checkCollision(const QPointF& mousePos);      //Проверка местоположения указателя мыши
    void changeCursor();                                    //Смена внешнего вида указателя мыши
    void changeCursor(MouseType type);
    void setTheme();                                        //Изменение внешнего вида элементов интерфейса

    void loadData();                                        //Загрузка данных из базы данных
    DataBase dataBase;                                      //Объект для взаимодействия с базой данных
    void showNotepads();                                    //Отображение данных о блокнотах

};
#endif // WIDGET_H
