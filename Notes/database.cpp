#include "database.h"
static int localeCompare( void* /*arg*/, int len1, const void* data1,  int len2, const void* data2 )
{
    QString string1 = QString::fromUtf8((char*)data1,len1).toUpper();
    QString string2 = QString::fromUtf8((char*)data2,len2).toUpper();
    return QString::localeAwareCompare( string1, string2 );
}

static void upper(sqlite3_context *context,  int argc, sqlite3_value **argv){
    if( argc != 1 ) return;
    switch(sqlite3_value_type(argv[0]))
    {
    case SQLITE_NULL:
    {
        sqlite3_result_text( context, "NULL", 4, SQLITE_STATIC );
        break;
    }
    case SQLITE_TEXT:
    {
        QString str(reinterpret_cast<const char*>(sqlite3_value_text(argv[0])));
        str = str.toUpper();
        const char* cstr = static_cast<char*>( str.toUtf8().data());
        sqlite3_result_text(context, cstr, str.toUtf8().size() , SQLITE_TRANSIENT);
        break;
    }
    default:
        sqlite3_result_text( context, "NULL", 4, SQLITE_STATIC );
        break;
    }
}

DataBase::DataBase(QObject *parent) :QObject(parent)
{

}

bool DataBase::connectDb()
{
    if(!QFile(fname).exists()){
        qDebug() << fname << " не существует";
        return restoreDb();
    }else{
        return openDb();
    }
}

QVector<Notepad> DataBase::getNotepads()
{
    QVector <Notepad> vector;
    const char* sql = "SELECT * FROM notepads ORDER BY name";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql ,-1, &stmt, NULL);
    if( rc != SQLITE_OK ){
        qDebug() << "Не удалось выполнить запрос к базе данных " << sql;
        return vector;
    }

    while(sqlite3_step(stmt) == SQLITE_ROW )
    {
        Notepad notepad {sqlite3_column_int(stmt,0), reinterpret_cast<const char*> (sqlite3_column_text(stmt,1))};
        vector.append(notepad);
    }
    return vector;
}

Notes DataBase::getNotes(int notepadId)
{
    Notes vector;
    QString sql = "SELECT id, title, date_create, status, checked "
                  "FROM notes WHERE notepad_id = "+ QString::number(notepadId) +" ORDER BY title";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.toStdString().c_str() ,-1, &stmt, NULL);
    if( rc != SQLITE_OK ){
        qDebug() << "Не удалось выполнить запрос к базе данных " << sql;
        return vector;
    }
    while(sqlite3_step(stmt) == SQLITE_ROW )
    {
        Note note;
        note.id = sqlite3_column_int(stmt,0);
        note.title = reinterpret_cast<const char*> (sqlite3_column_text(stmt,1));
        note.dateCreate = QDateTime::fromString( reinterpret_cast<const char*> (sqlite3_column_text(stmt,2)),"yyyy-MM-dd hh:mm:ss");
        note.status = sqlite3_column_int(stmt,3);
        if(note.status == 2)
            note.checked = sqlite3_column_int(stmt,4);
        vector.append(note);
    }
    return vector;
}

Note DataBase::getNote(int noteId)
{
    Note note;
    QString sql = "SELECT id, notepad_id, title, content, keywords, date_create, date_update, status, checked "
                  "FROM notes WHERE id = "+ QString::number(noteId) +" ORDER BY title";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.toStdString().c_str() ,-1, &stmt, NULL);
    if( rc != SQLITE_OK ){
        qDebug() << "Не удалось выполнить запрос к базе данных " << sql;
        return note;
    }

    while(sqlite3_step(stmt) == SQLITE_ROW )
    {
        note.id = sqlite3_column_int(stmt,0);
        note.notepad_id = sqlite3_column_int(stmt,1);
        note.title = reinterpret_cast<const char*> (sqlite3_column_text(stmt,2));
        note.content = reinterpret_cast<const char*> (sqlite3_column_text(stmt,3));
        note.keywords = reinterpret_cast<const char*> (sqlite3_column_text(stmt,4));
        note.dateCreate = QDateTime::fromString( reinterpret_cast<const char*> (sqlite3_column_text(stmt,5)),"yyyy-MM-dd hh:mm:ss");
        if(sqlite3_column_type(stmt,6) != SQLITE_NULL){
            note.dateUpdate = QDateTime::fromString( reinterpret_cast<const char*> (sqlite3_column_text(stmt,6)),"yyyy-MM-dd hh:mm:ss");
        }
        note.status = sqlite3_column_int(stmt,7);
        if(note.status == 2)
            note.checked = sqlite3_column_int(stmt,8);
    }
    return note;
}

bool DataBase::openDb()
{
    if(sqlite3_open(fname.toStdString().c_str(), &db) == SQLITE_OK){
        qDebug() << "Соединение с базой данных установленно.";
        sqlite3_create_function(db, "upper", 1, SQLITE_UTF8, NULL, &upper, NULL, NULL);
        sqlite3_create_collation(db, "NOCASECOLLATE", SQLITE_UTF8, nullptr, &localeCompare );
        return true;
    }else{
        qDebug() << "Не удалось установить соединение с базой данных";
        return false;
    }
}

bool DataBase::restoreDb()
{
    if(!openDb())
        return false;
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString sql[] = {"CREATE TABLE notepads ("
                     "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "name       TEXT NOT NULL COLLATE NOCASECOLLATE"
                     ");",
                     "CREATE TABLE notes ("
                     "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "notepad_id INTEGER NOT NULL,"
                     "title       TEXT NOT NULL COLLATE NOCASECOLLATE,"
                     "content     TEXT NOT NULL COLLATE NOCASECOLLATE,"
                     "keywords    TEXT NOT NULL COLLATE NOCASECOLLATE,"
                     "date_create TEXT NOT NULL,"
                     "date_update TEXT,"
                     "status      INTEGER NOT NULL,"
                     "checked     INTEGER"
                     ");",
                     "INSERT INTO notepads (name) "
                     "VALUES ('Первый блокнот');",
                             "INSERT INTO notes (notepad_id, title, content, keywords, date_create, status) "
                     "VALUES ((SELECT MAX(id) FROM notepads),'Первая заметка','<p>Привет, Мир!</p>', ',старт,','"+date+"',1);",
                     "INSERT INTO notes (notepad_id, title, content, keywords, date_create, status) "
                     "VALUES ((SELECT MAX(id) FROM notepads),'Вторая заметка','<p>Текст второй заметки.</p>', ',вторая, начало', '"+date+"',1);",
                    "INSERT INTO notepads (name) "
                    "VALUES ('Ещё один блокнот');",
                    "INSERT INTO notes (notepad_id, title, content, keywords, date_create, status) "
                    "VALUES ((SELECT MAX(id) FROM notepads),'Заметка из второго блокнота','<p>Текст заметки (второй блокнот).</p>', ',вторая, начало','"+date+"',1);"
                     };
    // QString sql[] = {
    //     "CREATE TABLE notepads ("
    //     "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
    //     "name       TEXT NOT NULL"
    //     ");",
    //     "CREATE TABLE notes ("
    //     "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
    //     "notepad_id INTEGER NOT NULL,"
    //     "title       TEXT NOT NULL,"
    //     "content     TEXT NOT NULL,"
    //     "keywords    TEXT NOT NULL,"
    //     "date_create TEXT NOT NULL,"
    //     "date_update TEXT,"
    //     "status      INTEGER NOT NULL,"
    //     "checked     INTEGER"
    //     ");",
    //     "INSERT INTO notepads (name) "
    //     "VALUES ('Первый блокнот');",
    //     "INSERT INTO notes (notepad_id, title, content, keywords, date_create, status) "
    //     "VALUES ((SELECT MAX(id) FROM notepads),'Первая заметка','<p>Привет, Мир!</p>', ',старт,','"+date+"',1);",
    //     "INSERT INTO notes (notepad_id, title, content, keywords, date_create, status) "
    //     "VALUES ((SELECT MAX(id) FROM notepads),'Вторая заметка','<p>Текст второй заметки.</p>', ',вторая, начало','"+date+"',1);",
    //     "INSERT INTO notepads (name) "
    //     "VALUES ('Ещё один блокнот');",
    //     "INSERT INTO notes (notepad_id, title, content, keywords, date_create, status) "
    //     "VALUES ((SELECT MAX(id) FROM notepads),'Заметка из второго блокнота','<p>Текст заметки (второй блокнот).</p>', ',вторая, начало','"+date+"',1);"
    // };

    char *zErrMsg = 0;
    for(QString& query: sql){
        if(sqlite3_exec(db, query.toStdString().c_str(), nullptr, 0, &zErrMsg) == SQLITE_OK){
            qDebug() << "Запрос " << query << " выполнен успешно.";
        }else{
            qDebug() << "Ошибка! Запрос " << query << " Не выполнен!";
            qDebug() << "Текст сообщения об ошибке: " << zErrMsg;
            return false;
        }
    }
    qDebug() << "База данных создана.";
    return true;
}

void DataBase::closeDb()
{
    sqlite3_close(db);
}
