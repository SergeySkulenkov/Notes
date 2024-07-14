#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "database.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
class QMouseEvent;
class StyleHelper;
class Editor;
class QTreeWidgetItem;
class QListWidgetItem;

QT_END_NAMESPACE

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
    void maxButtonSlot();

    void exitSlot();
    void exportSlot();
    void openSlot();

    void newNoteSlot();
    void saveCopyNoteSlot();
    void saveAsNoteSlot();

    void changeActivNotepadSlot(QTreeWidgetItem *current, QTreeWidgetItem *prev);
    void changeActivNoteSlot(QListWidgetItem *current, QListWidgetItem *prev);


private:
    Ui::Widget*  ui;
    MouseType    mouseClickType;
    QPoint       mousePreviousPosition;
    StyleHelper* styleHelper;
    Editor*      editor;
    bool isRelease = 1;

    void setWindowProperties();
    void setupMainMenu();
    void setupEditor();
    MouseType checkCollision(const QPointF& mousePos);
    void changeCursor();
    void changeCursor(MouseType type);
    void setTheme();

    void loadData();
    DataBase dataBase;
    void showNotepads();

};
#endif // WIDGET_H
