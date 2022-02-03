#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QTranslator>

MainWindow* w;

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    const char* file = context.file ? context.file : "";
    const char* function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        emit w->logColor(QColor(0, 0, 0));
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        emit w->logColor(QColor(0, 128, 0));
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        emit w->logColor(QColor(0, 0, 255));
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        emit w->logColor(QColor(128, 0, 0));
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        emit w->logColor(QColor(128, 0, 0));
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
    if (localMsg.size())
        emit w->log(QString::fromLocal8Bit(localMsg.trimmed()));
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QApplication::setOrganizationName("XrSoft");
    QApplication::setApplicationName("ThermCtrl");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    QFont f;
    f.setPointSize(10);
    QApplication::setFont(f);

    //    QString locale = QLocale::system().name().mid(0, 2);
    //    QDir dir(QApplication::applicationDirPath() + "/translations");
    //    if (dir.exists()) { // Поиск всех файлов в папке "plugins"
    //        qDebug(dir.path().toLocal8Bit().data());
    //        QStringList listFiles(dir.entryList({ "*" + locale + ".qm" }, QDir::Files));
    //        for (const auto& str : listFiles) { // Проход по всем файлам
    //            qDebug(str.toLocal8Bit().data());
    //            QTranslator translator;
    //            translator.load(dir.path() + '/' + str);
    //            app.installTranslator(&translator);
    //        }
    //    }

    MainWindow w;
    w.show();

    ::w = &w;
    qInstallMessageHandler(myMessageOutput);

    return app.exec();
}
