#ifndef EDITOR_H
#define EDITOR_H

#include <QFrame>
#include <QTextBrowser>

#include "checker.h"
#include "highlighter.h"
#include "codeeditor.h"

namespace Ui {
class Editor;
}

class Editor : public QFrame
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

private slots:
    void testEditor();

private:
    Ui::Editor *ui;

    BodyTextEdit*    textEditor;
    QTextBrowser*    textBrowser;
    CodeEditor*      codeEditor;
    HtmlHighLighter* highlighter;

    void setupEditors();
};

#endif // EDITOR_H
