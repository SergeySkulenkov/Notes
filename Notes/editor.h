#ifndef EDITOR_H
#define EDITOR_H

#include <QFrame>
#include <QTextBrowser>

#include "checker.h"
#include "highlighter.h"
#include "codeeditor.h"
#include "database.h"

namespace Ui {
class Editor;
}

class Editor : public QFrame
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();
    void setData(const Note& note);
    void setSaveButtonIcon(const QString& path);
    void setBoldButtonIcon(const QString& path);
    void setItalicButtonIcon(const QString& path);
    void setUnderlineButtonIcon(const QString& path);
    void setStrikethroughButtonIcon(const QString& path);
    void setTagsIcon(const QString& path);

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
