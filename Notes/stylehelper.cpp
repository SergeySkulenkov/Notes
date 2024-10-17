#include "stylehelper.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

QSize StyleHelper::winIconSize = {16,16};
QSize StyleHelper::winBtnSize = {30,26};

StyleHelper::StyleHelper()
{
    appTheme = new AppTheme;
}

StyleHelper::~StyleHelper()
{
    delete appTheme;
}

bool StyleHelper::setAppTheme(const QString& filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug() << "not open file "<< filePath;
        return false;
    }
    QByteArray data = file.readAll();
    //qDebug() << data;
    file.close();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(!doc.isObject()){
        qDebug() << "JsonError: " << error.errorString() << error.offset << error.error;
        return false;
    }
    QJsonObject root = doc.object();
    appTheme->name = root["name"].toString();

    if(root.value("windowTitle").isObject()){
        QJsonObject windowTitle = root.value("windowTitle").toObject();
        appTheme->windowIcoPath = windowTitle.value("icon").toString();
        appTheme->windowTitleQSS += "QWidget{background-color:"+windowTitle.value("background-color").toString()+"}"
                                 +  "QWidget#menuWidget{min-width:250px;}";
    }
    if(root.value("mainMenu").isObject()){
        QJsonObject mainMenu = root.value("mainMenu").toObject();
        appTheme->windowTitleQSS += "QMenuBar{}"
                                    "QMenuBar::item{color:"+mainMenu.value("color").toString()+";"
                                     "padding:"+mainMenu.value("item-padding").toString()+"}"
                                    "QMenuBar::item:selected{background:"
                                    +mainMenu.value("item-selected-background-color").toString()+";}"
                                    "QMenuBar::item:pressed{background:"
                                    +mainMenu.value("item-pressed-background-color").toString()+";}";
    }
    if(root.value("windowButtons").isObject()){
        QJsonObject windowButtons = root.value("windowButtons").toObject();
        if(windowButtons.value("minimaze-button").isObject()){
            QJsonObject objBtn = windowButtons.value("minimaze-button").toObject();
            appTheme->minimazeBtnQSS = getWindowButtonQSS(objBtn,"minWindowButton");
        }
        if(windowButtons.value("maximaze-button").isObject()){
            QJsonObject objBtn = windowButtons.value("maximaze-button").toObject();
            appTheme->maximazeBtnQSS = getWindowButtonQSS(objBtn,"maxWindowButton");
        }
        if(windowButtons.value("normal-button").isObject()){
            QJsonObject objBtn = windowButtons.value("normal-button").toObject();
            appTheme->normalBtnQSS = getWindowButtonQSS(objBtn,"maxWindowButton");
        }
        if(windowButtons.value("close-button").isObject()){
            QJsonObject objBtn = windowButtons.value("close-button").toObject();
            appTheme->closeBtnQSS = getWindowButtonQSS(objBtn,"closeWindowButton");
        }
    }

    if(root.value("leftColumn").isObject()){
        QJsonObject leftColumn = root.value("leftColumn").toObject();
        appTheme->leftColumn += "QWidget{background:" + leftColumn.value("background").toString()+";}"
                                "QScrollArea{border:none;}";
        if(leftColumn.value("add-button").isObject()){
            QJsonObject objBtn = leftColumn.value("add-button").toObject();
            appTheme->leftColumn += getWindowButtonQSS(objBtn,"leftAddButton");
        }
        if(leftColumn.value("add-combobox").isObject()){
            QJsonObject objCombo = leftColumn.value("add-combobox").toObject();
            appTheme->leftColumn += "QComboBox{"+jsonToQSS(objCombo) + "}";
        }
        if(leftColumn.value("add-combobox-drop-down").isObject()){
            QJsonObject objCombo = leftColumn.value("add-combobox-drop-down").toObject();
            appTheme->leftColumn += "QComboBox::drop-down{"+jsonToQSS(objCombo) + "}";
        }
        if(leftColumn.value("add-combobox-item-view").isObject()){
            QJsonObject objCombo = leftColumn.value("add-combobox-item-view").toObject();
            appTheme->leftColumn += "QComboBox QAbstractItemView{"+jsonToQSS(objCombo) + "}";
        }
        if(leftColumn.value("add-combobox-item").isObject()){
            QJsonObject objCombo = leftColumn.value("add-combobox-item").toObject();
            appTheme->leftColumn += "QComboBox QAbstractItemView::item{"+jsonToQSS(objCombo) + "}";
        }
        if(leftColumn.value("toolbox-page").isObject()){
            QJsonObject objPage = leftColumn.value("toolbox-page").toObject();
            appTheme->leftPage += "QWidget{"+jsonToQSS(objPage) + "}";
        }
        if(leftColumn.value("toolbox-tab").isObject()){
            QJsonObject leftTabTitle = leftColumn.value("toolbox-tab").toObject();
            appTheme->leftTabTitle += "QToolBox::tab{"+jsonToQSS(leftTabTitle) + "}";
        }
        if(leftColumn.value("toolbox-tab-selected").isObject()){
            QJsonObject leftTabSelectedTitle = leftColumn.value("toolbox-tab-selected").toObject();
            appTheme->leftTabTitle += "QToolBox::tab:selected{"+jsonToQSS(leftTabSelectedTitle) + "}";
        }
        appTheme->tabIcon[Tab::NOTEPADS] = leftColumn.value("toolbox-tab-notepads-icon").toString();
        appTheme->tabIcon[Tab::TASKS]    = leftColumn.value("toolbox-tab-tasks-icon").toString();
        appTheme->tabIcon[Tab::NOTES]    = leftColumn.value("toolbox-tab-notes-icon").toString();
        appTheme->tabHeight              = leftColumn.value("toolbox-tab-height").toInt();

    }
    if(root.value("editor").isObject()){
        QJsonObject jObj;
        if(root.value("editor").toObject().value("title").isObject()){
            jObj = root.value("editor").toObject().value("title").toObject();
            appTheme->editorQSS += "QLineEdit#titleLabel{"+jsonToQSS(jObj) + "} ";
        }
        if(root.value("editor").toObject().value("title-focus").isObject()){
            jObj = root.value("editor").toObject().value("title-focus").toObject();
            appTheme->editorQSS += "QLineEdit#titleLabel::focus{"+jsonToQSS(jObj) + "} ";
        }
        if(root.value("editor").toObject().value("buttons-bar").isObject()){
            jObj = root.value("editor").toObject().value("buttons-bar").toObject();
            appTheme->editorQSS += "QWidget#buttonsBarWidget{"+jsonToQSS(jObj) + "} ";
        }
        if(root.value("editor").toObject().value("tags").isObject()){
            jObj = root.value("editor").toObject().value("tags").toObject();
            appTheme->editorQSS += "QLineEdit#keywordsEdit{"+jsonToQSS(jObj) + "} ";
        }
        if(root.value("editor").toObject().value("buttons").isObject()){
            jObj = root.value("editor").toObject().value("buttons").toObject();
            if(jObj.value("normal-button").isObject()){
                QJsonObject objBtn = jObj.value("normal-button").toObject();
                appTheme->editorQSS += "QWidget#buttonsBarWidget QPushButton{"+jsonToQSS(objBtn) + "}";
            }
            if(jObj.value("hover-button").isObject()){
                QJsonObject objBtn = jObj.value("hover-button").toObject();
                appTheme->editorQSS += "QWidget#buttonsBarWidget QPushButton:hover{"+jsonToQSS(objBtn) + "}";
            }
            appTheme->editorButtonIcon[EditorStyle::SAVE_BTN] = jObj.value("save-button-icon").toString();
            appTheme->editorButtonIcon[EditorStyle::BOLD_BTN] = jObj.value("bold-button-icon").toString();
            appTheme->editorButtonIcon[EditorStyle::ITALIC_BTN] = jObj.value("italic-button-icon").toString();
            appTheme->editorButtonIcon[EditorStyle::UNDERLINE_BTN] = jObj.value("underline-button-icon").toString();
            appTheme->editorButtonIcon[EditorStyle::STRIKETHROUGH_BTN] = jObj.value("strikethrough-button-icon").toString();
            appTheme->editorButtonIcon[EditorStyle::TAGS_ICO] = jObj.value("tags-icon").toString();
            appTheme->editorButtonIcon[EditorStyle::PREV_BTN] = jObj.value("prev-button-icon").toString();
            appTheme->editorButtonIcon[EditorStyle::NEXT_BTN] = jObj.value("next-button-icon").toString();
        }

    }



    return true;
}

QString StyleHelper::getWindowIconPath()
{
    return appTheme->windowIcoPath;
}

QString StyleHelper::getWindowTitleStyle()
{
    return appTheme->windowTitleQSS;
}

QString StyleHelper::getMinimazeButtonStyle()
{
    return appTheme->minimazeBtnQSS;
}

QString StyleHelper::getMaximazeButtonStyle()
{
    return appTheme->maximazeBtnQSS;
}

QString StyleHelper::getNormalButtonStyle()
{
    return appTheme->normalBtnQSS;
}

QString StyleHelper::getCloseButtonStyle()
{
    return appTheme->closeBtnQSS;
}
QString StyleHelper::getLeftColumnStyle()
{
    return appTheme->leftColumn;
}

QString StyleHelper::getLeftPageStyle()
{
    return appTheme->leftPage;
}

QString StyleHelper::getLeftTabTitleStyle()
{
    return appTheme->leftTabTitle;
}


QString StyleHelper::getWindowButtonQSS(const QJsonObject& obj, const QString& name)
{
    return "QPushButton#"+name+"{"
           +jsonToQSS(obj.value("normal").toObject())+
           "}"
           "QPushButton#"+name+"::hover{"
           +jsonToQSS(obj.value("hover").toObject())+
           "}"
           "QPushButton#"+name+"::pressed{"
           +jsonToQSS(obj.value("pressed").toObject())+
           "}";
}

QString StyleHelper::getIconPath(Tab::TabIcon type)
{
    return appTheme->tabIcon[type];
}

QString StyleHelper::getEditorStyle()
{
    return appTheme->editorQSS;
}

QString StyleHelper::getEditorButtonIconPath(EditorStyle::Buttons type)
{
    return appTheme->editorButtonIcon[type];
}

int StyleHelper::getTabHeight()
{
    return appTheme->tabHeight;
}

QString StyleHelper::getTreeWidgetStyle()
{
    return "QTreeWidget{"
           "border:none;"
           "font-size:14px;"
           "}"
           "QTreeView::item{"
           "font-size:12px;"
           "color:#aaa;"
           "background:transparent;"

           "}"
           "QTreeView::item:selected{"
           "color:#ddd;"
           "}"
           "QTreeView::item:hover, QTreeView::item:hover{"
           "color:#fff;"
           "}";
}

QString StyleHelper::jsonToQSS(const QJsonObject &obj)
{
    QString qss;
    QStringList keys = obj.keys();
    for(QString& key: keys){
        qss += key + ":" + obj.value(key).toString()+";";
    }
    return qss;
}

