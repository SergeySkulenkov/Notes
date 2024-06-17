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

    textEditor->setPlainText("Текстовый редактор");
    textBrowser->setText("<p>Просмотр</p>");
    codeEditor->setPlainText("Редактор кода");
}
