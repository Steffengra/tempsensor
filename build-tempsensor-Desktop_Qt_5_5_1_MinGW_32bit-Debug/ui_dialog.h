/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QPushButton *frequencyPushButton;
    QLabel *frequencyLabel;
    QLineEdit *frequencyLineEdit;
    QLabel *label_6;
    QLabel *label_100;
    QPushButton *LEDPushButton;
    QLineEdit *lineEdit_6;
    QLineEdit *lineEdit_100;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(377, 195);
        frequencyPushButton = new QPushButton(Dialog);
        frequencyPushButton->setObjectName(QStringLiteral("frequencyPushButton"));
        frequencyPushButton->setGeometry(QRect(140, 40, 75, 31));
        frequencyLabel = new QLabel(Dialog);
        frequencyLabel->setObjectName(QStringLiteral("frequencyLabel"));
        frequencyLabel->setGeometry(QRect(10, 20, 101, 16));
        frequencyLineEdit = new QLineEdit(Dialog);
        frequencyLineEdit->setObjectName(QStringLiteral("frequencyLineEdit"));
        frequencyLineEdit->setGeometry(QRect(10, 50, 113, 20));
        label_6 = new QLabel(Dialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 110, 161, 16));
        label_100 = new QLabel(Dialog);
        label_100->setObjectName(QStringLiteral("label_100"));
        label_100->setGeometry(QRect(10, 140, 171, 16));
        LEDPushButton = new QPushButton(Dialog);
        LEDPushButton->setObjectName(QStringLiteral("LEDPushButton"));
        LEDPushButton->setGeometry(QRect(260, 40, 75, 23));
        lineEdit_6 = new QLineEdit(Dialog);
        lineEdit_6->setObjectName(QStringLiteral("lineEdit_6"));
        lineEdit_6->setGeometry(QRect(190, 110, 113, 20));
        lineEdit_100 = new QLineEdit(Dialog);
        lineEdit_100->setObjectName(QStringLiteral("lineEdit_100"));
        lineEdit_100->setGeometry(QRect(190, 140, 113, 20));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0));
        frequencyPushButton->setText(QApplication::translate("Dialog", "Ok", 0));
        frequencyLabel->setText(QApplication::translate("Dialog", "Change Frequency", 0));
        label_6->setText(QApplication::translate("Dialog", "Durchschnitt letzte 6 Messungen", 0));
        label_100->setText(QApplication::translate("Dialog", "Durchschnitt letzte 100 Messungen", 0));
        LEDPushButton->setText(QApplication::translate("Dialog", "LED", 0));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
