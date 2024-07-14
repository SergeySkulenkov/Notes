#ifndef DATABASE_H
#define DATABASE_H
#include <QObject>
#include <QFile>
#include <QDate>
#include <QDebug>
#include "sqlite/sqlite3.h"
#include <QPair>
#include <QVector>


struct Note{
    int         id;                 //id заметки
    int         notepad_id;         //id блокнота
    QString     title;              //Заголовок
    QString     content;            //Текст
    QString     keywords;            //Ключевые слова
    QDateTime   dateCreate;         //Дата создания
    QDateTime   dateUpdate;         //Дата изменения
    int         status;             //Статус (заметка, задача)
    bool        checked;            //Выполнена да/нет (для задач)
};

using Notepad = QPair<int, QString>;
using Notes   = QVector<Note>;

class DataBase:public QObject
{
public:
    explicit DataBase(QObject *parent = nullptr);
    bool connectDb();
    QVector<Notepad> getNotepads();
    Notes getNotes(int notepadId);
    Note  getNote(int noteId);

private:

    sqlite3 *db;
    const QString fname =    "notes.db";

    bool openDb();
    bool restoreDb();
    void closeDb();

};

#endif // DATABASE_H
