#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
class QMouseEvent;
class StyleHelper;
class Editor;
QT_END_NAMESPACE

enum class MouseType{
    None,
    Top,
    Bottom,
    Left,
    Right,
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

private slots:
    void maxButtonSlot();
    void exitSlot();
    void openSlot();

private:
    Ui::Widget*  ui;
    MouseType    mouseClickType;
    QPoint       mousePreviousPosition;
    StyleHelper* styleHelper;
    Editor*      editor;

    void setWindowProperties();
    void setMainMenu();
    MouseType checkCollision(const QPointF& mousePos);
    void changeCursor();
    void changeCursor(MouseType type);
    void setTheme();

};
#endif // WIDGET_H
