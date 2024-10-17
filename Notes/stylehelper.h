#ifndef STYLEHELPER_H
#define STYLEHELPER_H
#include <QString>
#include <QJsonObject>
#include <QSize>
namespace Tab{
    enum TabIcon{
        NOTEPADS,
        NOTES,
        TASKS
    };
}
namespace EditorStyle{
    enum Buttons{
        SAVE_BTN,
        BOLD_BTN,
        ITALIC_BTN,
        UNDERLINE_BTN,
        STRIKETHROUGH_BTN,
        TAGS_ICO,
        PREV_BTN,
        NEXT_BTN
    };
}
class StyleHelper
{
public:
    StyleHelper();
    ~StyleHelper();

    bool setAppTheme(const QString& filePath);

    QString getWindowIconPath();
    QString getWindowTitleStyle();
    QString getMinimazeButtonStyle();
    QString getMaximazeButtonStyle();
    QString getNormalButtonStyle();
    QString getCloseButtonStyle();
    QString getLeftColumnStyle();
    QString getLeftPageStyle();
    QString getLeftTabTitleStyle();
    QString getWindowButtonQSS(const QJsonObject& obj, const QString& name);
    QString getIconPath(Tab::TabIcon type);
    QString getEditorStyle();
    QString getEditorButtonIconPath(EditorStyle::Buttons type);
    int     getTabHeight();
    QString getTreeWidgetStyle();
    static QSize winIconSize;
    static QSize winBtnSize;


private:
    struct AppTheme
    {
        QString name;
        QString windowIcoPath;
        QString windowTitleQSS;
        QString minimazeBtnQSS;
        QString maximazeBtnQSS;
        QString normalBtnQSS;
        QString closeBtnQSS;
        QString leftColumn;
        QString leftPage;
        QString leftTabTitle;
        QString tabIcon[3];
        int     tabHeight;
        QString editorQSS;
        QString editorButtonIcon[8];

    };
    AppTheme* appTheme;

    QString jsonToQSS(const QJsonObject& obj);



};

#endif // STYLEHELPER_H
