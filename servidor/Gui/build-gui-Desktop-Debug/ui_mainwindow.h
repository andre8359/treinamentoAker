/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *lb_root_dir;
    QLineEdit *txt_root_dir;
    QLabel *lb_port;
    QLineEdit *txt_port;
    QLabel *lb_speed_limit;
    QHBoxLayout *horizontalLayout;
    QLineEdit *txt_speed_limit;
    QComboBox *cb_speed_limit;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *bt_apply;
    QPushButton *bt_cancel;
    QPushButton *bt_exit;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(400, 264);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        formLayoutWidget = new QWidget(centralWidget);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(29, 59, 341, 121));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout->setContentsMargins(0, 0, 0, 0);
        lb_root_dir = new QLabel(formLayoutWidget);
        lb_root_dir->setObjectName(QString::fromUtf8("lb_root_dir"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lb_root_dir);

        txt_root_dir = new QLineEdit(formLayoutWidget);
        txt_root_dir->setObjectName(QString::fromUtf8("txt_root_dir"));

        formLayout->setWidget(0, QFormLayout::FieldRole, txt_root_dir);

        lb_port = new QLabel(formLayoutWidget);
        lb_port->setObjectName(QString::fromUtf8("lb_port"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lb_port);

        txt_port = new QLineEdit(formLayoutWidget);
        txt_port->setObjectName(QString::fromUtf8("txt_port"));
        txt_port->setMaxLength(5);

        formLayout->setWidget(1, QFormLayout::FieldRole, txt_port);

        lb_speed_limit = new QLabel(formLayoutWidget);
        lb_speed_limit->setObjectName(QString::fromUtf8("lb_speed_limit"));

        formLayout->setWidget(2, QFormLayout::LabelRole, lb_speed_limit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        txt_speed_limit = new QLineEdit(formLayoutWidget);
        txt_speed_limit->setObjectName(QString::fromUtf8("txt_speed_limit"));

        horizontalLayout->addWidget(txt_speed_limit);

        cb_speed_limit = new QComboBox(formLayoutWidget);
        cb_speed_limit->setObjectName(QString::fromUtf8("cb_speed_limit"));

        horizontalLayout->addWidget(cb_speed_limit);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout);

        horizontalLayoutWidget_2 = new QWidget(centralWidget);
        horizontalLayoutWidget_2->setObjectName(QString::fromUtf8("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(100, 190, 296, 31));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        bt_apply = new QPushButton(horizontalLayoutWidget_2);
        bt_apply->setObjectName(QString::fromUtf8("bt_apply"));

        horizontalLayout_2->addWidget(bt_apply);

        bt_cancel = new QPushButton(horizontalLayoutWidget_2);
        bt_cancel->setObjectName(QString::fromUtf8("bt_cancel"));

        horizontalLayout_2->addWidget(bt_cancel);

        bt_exit = new QPushButton(horizontalLayoutWidget_2);
        bt_exit->setObjectName(QString::fromUtf8("bt_exit"));

        horizontalLayout_2->addWidget(bt_exit);

        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Configura Servidor", 0, QApplication::UnicodeUTF8));
        lb_root_dir->setText(QApplication::translate("MainWindow", "Diret\303\263rio Raiz:", 0, QApplication::UnicodeUTF8));
        lb_port->setText(QApplication::translate("MainWindow", "Porta :", 0, QApplication::UnicodeUTF8));
        txt_port->setInputMask(QApplication::translate("MainWindow", "00009; ", 0, QApplication::UnicodeUTF8));
        txt_port->setText(QString());
        lb_speed_limit->setText(QApplication::translate("MainWindow", "Limite de Vel. :", 0, QApplication::UnicodeUTF8));
        txt_speed_limit->setInputMask(QApplication::translate("MainWindow", "000000000; ", 0, QApplication::UnicodeUTF8));
        cb_speed_limit->clear();
        cb_speed_limit->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "b/s", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Kb/s", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Mb/s", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Gb/s", 0, QApplication::UnicodeUTF8)
        );
        bt_apply->setText(QApplication::translate("MainWindow", "Aplicar", 0, QApplication::UnicodeUTF8));
        bt_cancel->setText(QApplication::translate("MainWindow", "Cancelar", 0, QApplication::UnicodeUTF8));
        bt_exit->setText(QApplication::translate("MainWindow", "Sair", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
