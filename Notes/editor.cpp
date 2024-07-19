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
    textBrowser->setText(note.content);
    textEditor->setText(note.content);
    codeEditor->setPlainText(textEditor->toHtml());
    ui->titleLabel->setText(note.title);
    if(note.dateUpdate.isValid()){
        ui->dateLabel->setText(note.dateUpdate.toString("dd.MM.yyyy"));
        ui->dateLabel->setToolTip("Дата создания: " + note.dateCreate.toString("dd.MM.yyyy hh:mm:ss")
                                  +"\nДата изменения: " + note.dateUpdate.toString("dd.MM.yyyy hh:mm:ss") );
    }else{
        ui->dateLabel->setText(note.dateCreate.toString("dd.MM.yyyy"));
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

}
