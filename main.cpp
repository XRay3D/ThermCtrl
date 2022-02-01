#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    const char* file = context.file ? context.file : "";
    const char* function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        MainWindow::teLog->setTextColor(QColor(0, 0, 0));
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        MainWindow::teLog->setTextColor(QColor(0, 128, 0));
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        MainWindow::teLog->setTextColor(QColor(0, 0, 255));
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        MainWindow::teLog->setTextColor(QColor(128, 0, 0));
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        MainWindow::teLog->setTextColor(QColor(128, 0, 0));
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
//    if (MainWindow::teLog && localMsg.size())
//        MainWindow::teLog->append(localMsg.trimmed());
}

int main(int argc, char* argv[]) {
    qInstallMessageHandler(myMessageOutput);
    QApplication app(argc, argv);

    QApplication::setOrganizationName("XrSoft");
    QApplication::setApplicationName("ThermCtrl");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    MainWindow w;
    w.show();

    return app.exec();
}
