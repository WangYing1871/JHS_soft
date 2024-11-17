/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_JHS_GUI
{
public:
    QAction *actionquit;
    QAction *actionabout;
    QAction *actionaboutQt;
    QWidget *centralwidget;
    QPushButton *Test_Button;
    QGroupBox *groupBox_2;
    QTextBrowser *textBrowser;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *Open;
    QPushButton *unpack;
    QPushButton *ana;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGroupBox *groupBox;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QCustomPlot *rms_qcpt;
    QCustomPlot *mean_qcpt;
    QWidget *tab_2;
    QGroupBox *groupBox_3;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_2;
    QCustomPlot *wavey_qcpt;
    QCustomPlot *wavex_qcpt;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_2;
    QFrame *line;
    QLabel *label_3;
    QComboBox *comboBox;
    QWidget *tab_3;
    QGroupBox *groupBox_4;
    QWidget *gridLayoutWidget_3;
    QGridLayout *gridLayout_3;
    QCustomPlot *widget_3;
    QCustomPlot *mean_qcpt_3;
    QMenuBar *menubar;
    QMenu *menu2;
    QMenu *menuAbourt;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *JHS_GUI)
    {
        if (JHS_GUI->objectName().isEmpty())
            JHS_GUI->setObjectName("JHS_GUI");
        JHS_GUI->resize(1583, 883);
        actionquit = new QAction(JHS_GUI);
        actionquit->setObjectName("actionquit");
        actionabout = new QAction(JHS_GUI);
        actionabout->setObjectName("actionabout");
        actionaboutQt = new QAction(JHS_GUI);
        actionaboutQt->setObjectName("actionaboutQt");
        centralwidget = new QWidget(JHS_GUI);
        centralwidget->setObjectName("centralwidget");
        Test_Button = new QPushButton(centralwidget);
        Test_Button->setObjectName("Test_Button");
        Test_Button->setGeometry(QRect(1060, 10, 171, 25));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(1280, 30, 291, 791));
        textBrowser = new QTextBrowser(groupBox_2);
        textBrowser->setObjectName("textBrowser");
        textBrowser->setGeometry(QRect(10, 30, 271, 751));
        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(50, 50, 1221, 51));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(horizontalLayoutWidget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(horizontalLayoutWidget);
        lineEdit->setObjectName("lineEdit");

        horizontalLayout->addWidget(lineEdit);

        Open = new QPushButton(horizontalLayoutWidget);
        Open->setObjectName("Open");

        horizontalLayout->addWidget(Open);

        unpack = new QPushButton(horizontalLayoutWidget);
        unpack->setObjectName("unpack");

        horizontalLayout->addWidget(unpack);

        ana = new QPushButton(horizontalLayoutWidget);
        ana->setObjectName("ana");

        horizontalLayout->addWidget(ana);

        pushButton = new QPushButton(horizontalLayoutWidget);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(10, 160, 1261, 661));
        tab = new QWidget();
        tab->setObjectName("tab");
        groupBox = new QGroupBox(tab);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(20, 10, 1231, 611));
        gridLayoutWidget = new QWidget(groupBox);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(10, 30, 1211, 571));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        rms_qcpt = new QCustomPlot(gridLayoutWidget);
        rms_qcpt->setObjectName("rms_qcpt");

        gridLayout->addWidget(rms_qcpt, 1, 0, 1, 1);

        mean_qcpt = new QCustomPlot(gridLayoutWidget);
        mean_qcpt->setObjectName("mean_qcpt");

        gridLayout->addWidget(mean_qcpt, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        groupBox_3 = new QGroupBox(tab_2);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(10, 10, 1241, 611));
        gridLayoutWidget_2 = new QWidget(groupBox_3);
        gridLayoutWidget_2->setObjectName("gridLayoutWidget_2");
        gridLayoutWidget_2->setGeometry(QRect(10, 70, 1221, 531));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        wavey_qcpt = new QCustomPlot(gridLayoutWidget_2);
        wavey_qcpt->setObjectName("wavey_qcpt");

        gridLayout_2->addWidget(wavey_qcpt, 1, 0, 1, 1);

        wavex_qcpt = new QCustomPlot(gridLayoutWidget_2);
        wavex_qcpt->setObjectName("wavex_qcpt");

        gridLayout_2->addWidget(wavex_qcpt, 0, 0, 1, 1);

        horizontalLayoutWidget_2 = new QWidget(groupBox_3);
        horizontalLayoutWidget_2->setObjectName("horizontalLayoutWidget_2");
        horizontalLayoutWidget_2->setGeometry(QRect(10, 30, 1221, 31));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(horizontalLayoutWidget_2);
        label_2->setObjectName("label_2");

        horizontalLayout_2->addWidget(label_2);

        lineEdit_2 = new QLineEdit(horizontalLayoutWidget_2);
        lineEdit_2->setObjectName("lineEdit_2");

        horizontalLayout_2->addWidget(lineEdit_2);

        pushButton_2 = new QPushButton(horizontalLayoutWidget_2);
        pushButton_2->setObjectName("pushButton_2");

        horizontalLayout_2->addWidget(pushButton_2);

        line = new QFrame(horizontalLayoutWidget_2);
        line->setObjectName("line");
        line->setFrameShape(QFrame::Shape::VLine);
        line->setFrameShadow(QFrame::Shadow::Sunken);

        horizontalLayout_2->addWidget(line);

        label_3 = new QLabel(horizontalLayoutWidget_2);
        label_3->setObjectName("label_3");

        horizontalLayout_2->addWidget(label_3);

        comboBox = new QComboBox(horizontalLayoutWidget_2);
        comboBox->setObjectName("comboBox");

        horizontalLayout_2->addWidget(comboBox);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName("tab_3");
        groupBox_4 = new QGroupBox(tab_3);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setGeometry(QRect(10, 10, 571, 351));
        gridLayoutWidget_3 = new QWidget(groupBox_4);
        gridLayoutWidget_3->setObjectName("gridLayoutWidget_3");
        gridLayoutWidget_3->setGeometry(QRect(10, 30, 551, 311));
        gridLayout_3 = new QGridLayout(gridLayoutWidget_3);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        widget_3 = new QCustomPlot(gridLayoutWidget_3);
        widget_3->setObjectName("widget_3");

        gridLayout_3->addWidget(widget_3, 1, 0, 1, 1);

        mean_qcpt_3 = new QCustomPlot(gridLayoutWidget_3);
        mean_qcpt_3->setObjectName("mean_qcpt_3");

        gridLayout_3->addWidget(mean_qcpt_3, 0, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());
        JHS_GUI->setCentralWidget(centralwidget);
        menubar = new QMenuBar(JHS_GUI);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1583, 22));
        menu2 = new QMenu(menubar);
        menu2->setObjectName("menu2");
        menuAbourt = new QMenu(menubar);
        menuAbourt->setObjectName("menuAbourt");
        JHS_GUI->setMenuBar(menubar);
        statusbar = new QStatusBar(JHS_GUI);
        statusbar->setObjectName("statusbar");
        JHS_GUI->setStatusBar(statusbar);

        menubar->addAction(menu2->menuAction());
        menubar->addAction(menuAbourt->menuAction());
        menu2->addSeparator();
        menu2->addAction(actionquit);
        menuAbourt->addAction(actionabout);
        menuAbourt->addAction(actionaboutQt);

        retranslateUi(JHS_GUI);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(JHS_GUI);
    } // setupUi

    void retranslateUi(QMainWindow *JHS_GUI)
    {
        JHS_GUI->setWindowTitle(QCoreApplication::translate("JHS_GUI", "MainWindow", nullptr));
        actionquit->setText(QCoreApplication::translate("JHS_GUI", "&Quit", nullptr));
        actionabout->setText(QCoreApplication::translate("JHS_GUI", "about", nullptr));
        actionaboutQt->setText(QCoreApplication::translate("JHS_GUI", "aboutQt", nullptr));
        Test_Button->setText(QCoreApplication::translate("JHS_GUI", "This a test Button", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("JHS_GUI", "GroupBox", nullptr));
        label->setText(QCoreApplication::translate("JHS_GUI", "Filename", nullptr));
        Open->setText(QCoreApplication::translate("JHS_GUI", "&Choose", nullptr));
        unpack->setText(QCoreApplication::translate("JHS_GUI", "&Unpack", nullptr));
        ana->setText(QCoreApplication::translate("JHS_GUI", "&Ana", nullptr));
        pushButton->setText(QCoreApplication::translate("JHS_GUI", "D&raw W&ave", nullptr));
        groupBox->setTitle(QCoreApplication::translate("JHS_GUI", "prestal", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("JHS_GUI", "Tab 1", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("JHS_GUI", "Wave", nullptr));
        label_2->setText(QCoreApplication::translate("JHS_GUI", "Event ID", nullptr));
        pushButton_2->setText(QCoreApplication::translate("JHS_GUI", "Choose", nullptr));
        label_3->setText(QCoreApplication::translate("JHS_GUI", "Channel", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("JHS_GUI", "Tab 2", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("JHS_GUI", "prestal", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("JHS_GUI", "Page", nullptr));
        menu2->setTitle(QCoreApplication::translate("JHS_GUI", "F&ile", nullptr));
        menuAbourt->setTitle(QCoreApplication::translate("JHS_GUI", "&Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class JHS_GUI: public Ui_JHS_GUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
