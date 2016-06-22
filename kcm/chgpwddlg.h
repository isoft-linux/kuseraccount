/**
 *  Copyright (C) 2015 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *  Copyright (C) 2015 fjiang <fujiang.zhu@i-soft.com.cn>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#ifndef __CHGPWDDLG_H__
#define __CHGPWDDLG_H__

#include <KLocalizedString>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtAccountsService/AccountsManager>
#include <QtGui>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QDialog>

class Ui_pwdDlg
{
public:
        QLabel *curPwdLabel_2;
        QPushButton *cancelBtn;
        QPushButton *changeBtn;
        QLineEdit *newPwdEdit;
        QLabel *newPwdLabel;
        QLineEdit *verPwdEdit;
        QLabel *verPwdLabel;
        QProgressBar *pwdProgressBar;
        QLabel *infoLabel;
        QLabel *pwdQuaLabel;
        void setupUi(QWidget *Dialog)
            {
                if (Dialog->objectName().isEmpty())
                    Dialog->setObjectName(QStringLiteral("Dialog"));
                Dialog->resize(356, 248);
                curPwdLabel_2 = new QLabel(Dialog);
                curPwdLabel_2->setObjectName(QStringLiteral("curPwdLabel_2"));
                curPwdLabel_2->setGeometry(QRect(110, 5, 160, 23));
                QFont font;
                font.setPointSize(12);
                font.setBold(true);
                font.setWeight(75);
                curPwdLabel_2->setFont(font);
                curPwdLabel_2->setAlignment(Qt::AlignCenter);
                cancelBtn = new QPushButton(Dialog);
                cancelBtn->setObjectName(QStringLiteral("cancelBtn"));
                cancelBtn->setGeometry(QRect(209, 200, 75, 27));
                changeBtn = new QPushButton(Dialog);
                changeBtn->setObjectName(QStringLiteral("changeBtn"));
                changeBtn->setGeometry(QRect(70, 200, 75, 27));
                newPwdEdit = new QLineEdit(Dialog);
                newPwdEdit->setObjectName(QStringLiteral("newPwdEdit"));
                newPwdEdit->setGeometry(QRect(120, 59, 211, 21));
                newPwdEdit->setMaxLength(64);
                newPwdEdit->setEchoMode(QLineEdit::Password);
                newPwdLabel = new QLabel(Dialog);
                newPwdLabel->setObjectName(QStringLiteral("newPwdLabel"));
                newPwdLabel->setGeometry(QRect(20, 60, 99, 21));
                newPwdLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
                verPwdEdit = new QLineEdit(Dialog);
                verPwdEdit->setObjectName(QStringLiteral("verPwdEdit"));
                verPwdEdit->setGeometry(QRect(120, 100, 211, 21));
                verPwdEdit->setMaxLength(64);
                verPwdEdit->setEchoMode(QLineEdit::Password);
                verPwdLabel = new QLabel(Dialog);
                verPwdLabel->setObjectName(QStringLiteral("verPwdLabel"));
                verPwdLabel->setGeometry(QRect(1, 97, 120, 21));
                verPwdLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
                pwdProgressBar = new QProgressBar(Dialog);
                pwdProgressBar->setObjectName(QStringLiteral("pwdProgressBar"));
                pwdProgressBar->setGeometry(QRect(120, 132, 211, 16));
                pwdProgressBar->setValue(0);
                infoLabel = new QLabel(Dialog);
                infoLabel->setObjectName(QStringLiteral("infoLabel"));
                infoLabel->setGeometry(QRect(120, 150, 211, 31));
                infoLabel->setWordWrap(true);
                pwdQuaLabel = new QLabel(Dialog);
                pwdQuaLabel->setObjectName(QStringLiteral("pwdQuaLabel"));
                pwdQuaLabel->setGeometry(QRect(10, 130, 111, 21));
                pwdQuaLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
                QWidget::setTabOrder(newPwdEdit, verPwdEdit);
                QWidget::setTabOrder(verPwdEdit, changeBtn);
                QWidget::setTabOrder(changeBtn, cancelBtn);

            pwdProgressBar->setTextVisible(false);
            retranslateUi(Dialog);
    }

    void retranslateUi(QWidget *dlg)
    {
        Q_UNUSED(dlg);

        newPwdLabel->setText(i18n("New Password"));
        verPwdLabel->setText(i18n("Verified Password"));
        cancelBtn->setText(i18n("cancel"));
        changeBtn->setText(i18n("change"));
        curPwdLabel_2->setText(i18n("change password"));
        pwdProgressBar->setFormat(QString());
        infoLabel->setText(QString());
        pwdQuaLabel->setText(i18n("Password Quality"));

#ifndef QT_NO_TOOLTIP
        pwdProgressBar->setToolTip(i18n("The password quality indicator."));
#endif // QT_NO_TOOLTIP

    }

};

namespace Ui {
    class pwdDlg: public Ui_pwdDlg {};
}

class ChgPwdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ChgPwdDlg(QtAccountsService::UserAccount *ua,
                       QWidget *parent = NULL, bool setAccType = false,
                       Qt::WindowFlags f = Qt::Dialog);

    QWidget *Dlg;

    ~ChgPwdDlg();

private slots:
    void slotChangePwd();
    bool slotCheckPwd();
    bool slotCheckPwdMatch(const QString chgStr);
private:
    QtAccountsService::UserAccount *_ua;
    Ui::pwdDlg ui;
    QString pwdErrorStr (int errorNo,int *value);
    bool _setAccType;
};

#endif /* __CHGPWDDLG_H__ */
