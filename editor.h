#ifndef EDITOR_H
#define EDITOR_H

#include <QFrame>

namespace Ui {
class Editor;
}

class Editor : public QFrame
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

private:
    Ui::Editor *ui;
};

#endif // EDITOR_H
