#ifndef STYLEHELPER_H
#define STYLEHELPER_H
#include <QString>
#include <QJsonObject>
#include <QSize>
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
    QString getWindowButtonQSS(const QJsonObject& obj, const QString& name);

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

    };
    AppTheme* appTheme;

    QString jsonToQSS(const QJsonObject& obj);



};

#endif // STYLEHELPER_H
