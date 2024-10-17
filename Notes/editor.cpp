#include "editor.h"
#include "ui_editor.h"

Editor::Editor(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::Editor)
{
    ui->setupUi(this);
    setupEditors();
    connect(ui->saveButton, &QPushButton::clicked, this, &Editor::testEditor);
}

Editor::~Editor()
{
    delete ui;
}

void Editor::setData(const Note& note)
{
    QString months[] = {"января",  "февраля", "марта",
                        "апреля",  "мая",     "июня",
                        "июля",    "августа", "сентября",
                        "октября", "ноября",  "декабря"
    };
    textBrowser->setText(note.content);
    textEditor->setText(note.content);
    codeEditor->setPlainText(textEditor->toHtml());
    ui->titleLabel->setText(note.title);

    if(note.dateUpdate.isValid()){
        int index = note.dateUpdate.date().month()-1;
        ui->dateLabel->setText(note.dateUpdate.toString("d "+months[index]+" yyyy"));
        ui->dateLabel->setToolTip("Дата создания: " + note.dateCreate.toString("d.MM.yyyy hh:mm:ss")
                                  +"\nДата изменения: " + note.dateUpdate.toString("dd.MM.yyyy hh:mm:ss") );
    }else{
        int index = note.dateCreate.date().month()-1;
        ui->dateLabel->setText(note.dateCreate.toString("dd "+months[index]+" yyyy"));
        ui->dateLabel->setToolTip("Дата создания: " + note.dateCreate.toString("dd.MM.yyyy hh:mm:ss"));
    }
    if(note.keywords.length() > 0){
        QString keywords = note.keywords;
        keywords.remove(0,1);
        keywords.remove(keywords.length()-1,1);
        ui->keywordsEdit->setText(keywords);
    }else{
        ui->keywordsEdit->setText(note.keywords);
    }

}

void Editor::setSaveButtonIcon(const QString& path)
{
    ui->saveButton->setIcon(QIcon(path));
}

void Editor::setBoldButtonIcon(const QString &path)
{
    ui->boldButton->setIcon(QIcon(path));
}
void Editor::setItalicButtonIcon(const QString &path)
{
    ui->italicButton->setIcon(QIcon(path));
}
void Editor::setUnderlineButtonIcon(const QString &path)
{
    ui->underlineButton->setIcon(QIcon(path));
}
void Editor::setStrikethroughButtonIcon(const QString &path)
{
    ui->strikeButton->setIcon(QIcon(path));
}

void Editor::setTagsIcon(const QString &path)
{
    ui->tagsLabel->setPixmap(QPixmap(path));

}


void Editor::testEditor()
{
    static int index = 1;
    switch(index){
    case 0:
        ui->stackedWidget->setCurrentWidget(textEditor);
        break;
    case 1:
        ui->stackedWidget->setCurrentWidget(textBrowser);
        break;
    case 2:
        ui->stackedWidget->setCurrentWidget(codeEditor);
        break;
    }

    //ui->stackedWidget->setCurrentIndex(index);
    index ++;
    if(index==3)
        index = 0;



}

void Editor::setupEditors()
{
    textEditor  = new BodyTextEdit;
    textBrowser = new QTextBrowser;
    codeEditor  = new CodeEditor;
    highlighter = new HtmlHighLighter(codeEditor->document());

    ui->stackedWidget->addWidget(textEditor);
    ui->stackedWidget->addWidget(textBrowser);
    ui->stackedWidget->addWidget(codeEditor);
    ui->stackedWidget->setCurrentWidget(textEditor);

    textEditor->setStyleSheet("QFrame{border:none; font-size:12px;}");
    textBrowser->setStyleSheet("QFrame{border:none; font-size:12px;}");
    codeEditor->setStyleSheet("QFrame{border:none; font-size:12px;}");

}
