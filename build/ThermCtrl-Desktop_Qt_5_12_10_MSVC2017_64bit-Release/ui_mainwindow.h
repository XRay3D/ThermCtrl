/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "chartview.h"
#include "doublespinbox.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *grbxConnection;
    QHBoxLayout *horizontalLayout;
    QComboBox *cmbxPort;
    QComboBox *cmbxDevice;
    QLabel *label;
    QSpinBox *sbxAddress;
    QPushButton *pbtnFind;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *grbxMan;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer_2;
    DoubleSpinBox *dsbxSetPoint;
    QPushButton *pbtnManStart;
    QPushButton *pbtnManReadTemp;
    QDoubleSpinBox *dsbxReadTemp;
    QPushButton *pbtnManStop;
    QGroupBox *grbxAuto;
    QGridLayout *gridLayout_2;
    QSpinBox *sbxPoints;
    QTableView *twPoints;
    QLabel *label_2;
    QPushButton *pbtnAutoStartStop;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;
    ChartView *chartView;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(656, 523);
        QFont font;
        font.setPointSize(10);
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(6, 6, 0, 6);
        grbxConnection = new QGroupBox(layoutWidget);
        grbxConnection->setObjectName(QString::fromUtf8("grbxConnection"));
        horizontalLayout = new QHBoxLayout(grbxConnection);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cmbxPort = new QComboBox(grbxConnection);
        cmbxPort->setObjectName(QString::fromUtf8("cmbxPort"));

        horizontalLayout->addWidget(cmbxPort);

        cmbxDevice = new QComboBox(grbxConnection);
        cmbxDevice->setObjectName(QString::fromUtf8("cmbxDevice"));

        horizontalLayout->addWidget(cmbxDevice);

        label = new QLabel(grbxConnection);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(label);

        sbxAddress = new QSpinBox(grbxConnection);
        sbxAddress->setObjectName(QString::fromUtf8("sbxAddress"));
        sbxAddress->setMinimumSize(QSize(50, 0));
        sbxAddress->setAlignment(Qt::AlignCenter);
        sbxAddress->setButtonSymbols(QAbstractSpinBox::NoButtons);
        sbxAddress->setMinimum(1);
        sbxAddress->setMaximum(255);
        sbxAddress->setSingleStep(1);

        horizontalLayout->addWidget(sbxAddress);

        pbtnFind = new QPushButton(grbxConnection);
        pbtnFind->setObjectName(QString::fromUtf8("pbtnFind"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/edit-find.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbtnFind->setIcon(icon);

        horizontalLayout->addWidget(pbtnFind);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        verticalLayout->addWidget(grbxConnection);

        grbxMan = new QGroupBox(layoutWidget);
        grbxMan->setObjectName(QString::fromUtf8("grbxMan"));
        grbxMan->setEnabled(false);
        gridLayout = new QGridLayout(grbxMan);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 3, 1, 1);

        dsbxSetPoint = new DoubleSpinBox(grbxMan);
        dsbxSetPoint->setObjectName(QString::fromUtf8("dsbxSetPoint"));
        dsbxSetPoint->setAlignment(Qt::AlignCenter);
        dsbxSetPoint->setButtonSymbols(QAbstractSpinBox::NoButtons);
        dsbxSetPoint->setDecimals(3);
        dsbxSetPoint->setMinimum(-999.000000000000000);
        dsbxSetPoint->setMaximum(999.000000000000000);

        gridLayout->addWidget(dsbxSetPoint, 0, 1, 1, 1);

        pbtnManStart = new QPushButton(grbxMan);
        pbtnManStart->setObjectName(QString::fromUtf8("pbtnManStart"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/res/media-playback-start.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbtnManStart->setIcon(icon1);
        pbtnManStart->setCheckable(false);

        gridLayout->addWidget(pbtnManStart, 0, 0, 1, 1);

        pbtnManReadTemp = new QPushButton(grbxMan);
        pbtnManReadTemp->setObjectName(QString::fromUtf8("pbtnManReadTemp"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/res/media-record.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbtnManReadTemp->setIcon(icon2);

        gridLayout->addWidget(pbtnManReadTemp, 1, 0, 1, 1);

        dsbxReadTemp = new QDoubleSpinBox(grbxMan);
        dsbxReadTemp->setObjectName(QString::fromUtf8("dsbxReadTemp"));
        dsbxReadTemp->setAlignment(Qt::AlignCenter);
        dsbxReadTemp->setReadOnly(true);
        dsbxReadTemp->setButtonSymbols(QAbstractSpinBox::NoButtons);
        dsbxReadTemp->setDecimals(3);
        dsbxReadTemp->setMinimum(-999.000000000000000);
        dsbxReadTemp->setMaximum(999.000000000000000);

        gridLayout->addWidget(dsbxReadTemp, 1, 1, 1, 1);

        pbtnManStop = new QPushButton(grbxMan);
        pbtnManStop->setObjectName(QString::fromUtf8("pbtnManStop"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/res/media-playback-stop.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbtnManStop->setIcon(icon3);

        gridLayout->addWidget(pbtnManStop, 0, 2, 1, 1);


        verticalLayout->addWidget(grbxMan);

        grbxAuto = new QGroupBox(layoutWidget);
        grbxAuto->setObjectName(QString::fromUtf8("grbxAuto"));
        grbxAuto->setEnabled(false);
        gridLayout_2 = new QGridLayout(grbxAuto);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        sbxPoints = new QSpinBox(grbxAuto);
        sbxPoints->setObjectName(QString::fromUtf8("sbxPoints"));
        sbxPoints->setMinimumSize(QSize(50, 0));
        sbxPoints->setAlignment(Qt::AlignCenter);
        sbxPoints->setMinimum(1);
        sbxPoints->setMaximum(100);

        gridLayout_2->addWidget(sbxPoints, 0, 1, 1, 1);

        twPoints = new QTableView(grbxAuto);
        twPoints->setObjectName(QString::fromUtf8("twPoints"));

        gridLayout_2->addWidget(twPoints, 1, 0, 1, 4);

        label_2 = new QLabel(grbxAuto);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        pbtnAutoStartStop = new QPushButton(grbxAuto);
        pbtnAutoStartStop->setObjectName(QString::fromUtf8("pbtnAutoStartStop"));
        pbtnAutoStartStop->setIcon(icon1);
        pbtnAutoStartStop->setCheckable(true);

        gridLayout_2->addWidget(pbtnAutoStartStop, 0, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 2, 2, 1, 1);


        verticalLayout->addWidget(grbxAuto);

        splitter->addWidget(layoutWidget);
        chartView = new ChartView(splitter);
        chartView->setObjectName(QString::fromUtf8("chartView"));
        splitter->addWidget(chartView);

        verticalLayout_2->addWidget(splitter);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 656, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        grbxConnection->setTitle(QApplication::translate("MainWindow", "\320\237\320\276\320\270\321\201\320\272 \321\202\320\265\321\200\320\274\320\276\320\272\320\260\320\274\320\265\321\200\321\213", nullptr));
        label->setText(QApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201:", nullptr));
        pbtnFind->setText(QApplication::translate("MainWindow", "\320\237\320\276\320\270\321\201\320\272", nullptr));
        grbxMan->setTitle(QApplication::translate("MainWindow", "\320\240\321\203\321\207\320\275\320\276\320\265 \321\203\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\320\265", nullptr));
        dsbxSetPoint->setSuffix(QApplication::translate("MainWindow", "\302\272C", nullptr));
        pbtnManStart->setText(QApplication::translate("MainWindow", "\320\241\321\202\320\260\321\200\321\202 \320\275\320\260 \321\203\321\201\321\202\320\260\320\262\320\272\321\203", nullptr));
        pbtnManReadTemp->setText(QApplication::translate("MainWindow", "\320\247\321\202\320\265\320\275\320\270\320\265 \321\202\320\265\320\274\320\277\320\265\321\200\320\260\321\202\321\203\321\200\321\213", nullptr));
        dsbxReadTemp->setSuffix(QApplication::translate("MainWindow", "\302\272C", nullptr));
        pbtnManStop->setText(QApplication::translate("MainWindow", "\320\241\321\202\320\276\320\277", nullptr));
        grbxAuto->setTitle(QApplication::translate("MainWindow", "\320\220\320\262\321\202\320\276\320\274\320\260\321\202\320\270\321\207\320\265\321\201\320\272\320\276\320\265 \321\203\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\320\265", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "\320\242\320\276\321\207\320\272\320\270:", nullptr));
        pbtnAutoStartStop->setText(QApplication::translate("MainWindow", "\320\241\321\202\320\260\321\200\321\202", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
