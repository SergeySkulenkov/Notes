#ifndef CHECKER_H
#define CHECKER_H
#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QTextCodec>
#include <QDebug>
#include "hunspell.hxx"
#include <QMenu>
#include <QRegExp>

class SpellingHighlighter : public QSyntaxHighlighter // Класс для подсвечивания текста
{
    Q_OBJECT

public:
    SpellingHighlighter(QTextEdit *parent) :QSyntaxHighlighter(parent) {}

protected:
    virtual void highlightBlock(const QString &text) override;
};

class BodyTextEdit : public QTextEdit // Класс для ввода и отображения текста
{
    Q_OBJECT

public:
    BodyTextEdit(QWidget* parent = 0);


public slots:
    void openCustomMenu(QPoint);
    void correctWord(QAction * act);

protected:
    SpellingHighlighter * m_highLighter;
};


QStringList suggestCorrections(const QString &word, bool &correct, bool getSuggests = false);

#endif // CHECKER_H
