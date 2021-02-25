#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QApplication::setOrganizationName("XrSoft");
    QApplication::setApplicationName("ThermCtrl");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    MainWindow w;
    w.show();

    return app.exec();
}
