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

#ifndef __ADDUSERDLG_H__
#define __ADDUSERDLG_H__

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QProgressBar>
#include <QtAccountsService/AccountsManager>
#include <QtAccountsService/UserAccount>
#include <KLocalizedString>
#include "faceiconbutton.h"
#include "faceiconpopup.h"

typedef QtAccountsService::UserAccount UserAccount;

class Ui_Dialog
{
public:
        QLineEdit *userNameEdit;
        QLabel *userNameLabel;
        QLabel *headLable;
        QPushButton *cancelBtn;
        QPushButton *addBtn;
        QLineEdit *PwdEdit;
        QLabel *passwordLabel;
        QLineEdit *verifyPwdEdit;
        QLabel *verifyPwdLabel;
        QLabel *AccTypeLable;
        QLabel *autoLoginLabel;
        QCheckBox *autoLoginCheckBox;
        QCheckBox *canLoginCheckBox;
        QCheckBox *disLoginCheckBox;
        QLabel *canLoginLabel;
        QLabel *realNameLabel;
        QLineEdit *realNameEdit;
        QLabel *infoLabel;
        QProgressBar *pwdProgressBar;

        FaceIconButton *faceIconPushButton;

    void setupUi(QWidget *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));

                Dialog->resize(400, 450);
                userNameEdit = new QLineEdit(Dialog);
                userNameEdit->setObjectName(QStringLiteral("userNameEdit"));
                userNameEdit->setGeometry(QRect(150, 80, 111, 21));
                userNameEdit->setContextMenuPolicy(Qt::NoContextMenu);
                userNameEdit->setAutoFillBackground(true);
                userNameEdit->setMaxLength(31);
                userNameEdit->setEchoMode(QLineEdit::Normal);
                QRegExp regx("^[a-zA-Z][A-Za-z0-9_-]+$");
                QValidator *validator = new QRegExpValidator(regx, userNameEdit );
                userNameEdit->setValidator( validator );

                userNameLabel = new QLabel(Dialog);
                userNameLabel->setObjectName(QStringLiteral("userNameLabel"));
                userNameLabel->setGeometry(QRect(10, 80, 131, 20));
                userNameLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
                headLable = new QLabel(Dialog);
                headLable->setObjectName(QStringLiteral("headLable"));
                headLable->setGeometry(QRect(165, 20, 125, 23));
                QFont font;
                font.setPointSize(12);
                font.setBold(true);
                font.setWeight(75);
                headLable->setFont(font);
                headLable->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
                cancelBtn = new QPushButton(Dialog);
                cancelBtn->setObjectName(QStringLiteral("cancelBtn"));
                cancelBtn->setGeometry(QRect(240, 394, 75, 27));
                addBtn = new QPushButton(Dialog);
                addBtn->setObjectName(QStringLiteral("addBtn"));
                addBtn->setGeometry(QRect(80, 394, 75, 27));
                PwdEdit = new QLineEdit(Dialog);
                PwdEdit->setObjectName(QStringLiteral("PwdEdit"));
                PwdEdit->setGeometry(QRect(150, 160, 171, 21));
                PwdEdit->setMaxLength(64);
                PwdEdit->setEchoMode(QLineEdit::Password);
                passwordLabel = new QLabel(Dialog);
                passwordLabel->setObjectName(QStringLiteral("passwordLabel"));
                passwordLabel->setGeometry(QRect(10, 160, 131, 20));
                passwordLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
                verifyPwdEdit = new QLineEdit(Dialog);
                verifyPwdEdit->setObjectName(QStringLiteral("verifyPwdEdit"));
                verifyPwdEdit->setGeometry(QRect(150, 200, 171, 21));
                verifyPwdEdit->setMaxLength(64);
                verifyPwdEdit->setEchoMode(QLineEdit::Password);
                verifyPwdLabel = new QLabel(Dialog);
                verifyPwdLabel->setObjectName(QStringLiteral("verifyPwdLabel"));
                verifyPwdLabel->setGeometry(QRect(10, 200, 131, 20));
                verifyPwdLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
                AccTypeLable = new QLabel(Dialog);
                AccTypeLable->setObjectName(QStringLiteral("AccTypeLable"));
                AccTypeLable->setGeometry(QRect(10, 240, 131, 20));
                AccTypeLable->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
                autoLoginLabel = new QLabel(Dialog);
                autoLoginLabel->setObjectName(QStringLiteral("autoLoginLabel"));
                autoLoginLabel->setGeometry(QRect(10, 280, 131, 20));
                autoLoginLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
                autoLoginCheckBox = new QCheckBox(Dialog);
                autoLoginCheckBox->setObjectName(QStringLiteral("autoLoginCheckBox"));
                autoLoginCheckBox->setGeometry(QRect(150, 280, 51, 21));
                canLoginCheckBox = new QCheckBox(Dialog);
                canLoginCheckBox->setObjectName(QStringLiteral("canLoginCheckBox"));
                canLoginCheckBox->setGeometry(QRect(150, 240, 41, 21));
                disLoginCheckBox = new QCheckBox(Dialog);
                disLoginCheckBox->setObjectName(QStringLiteral("disLoginCheckBox"));
                disLoginCheckBox->setGeometry(QRect(150, 320, 51, 21));
                canLoginLabel = new QLabel(Dialog);
                canLoginLabel->setObjectName(QStringLiteral("canLoginLabel"));
                canLoginLabel->setGeometry(QRect(10, 320, 131, 20));
                canLoginLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
                faceIconPushButton = new FaceIconButton(Dialog);
                faceIconPushButton->setObjectName(QStringLiteral("faceIconPushButton"));
                faceIconPushButton->setGeometry(QRect(275, 70, 46, 46));

                faceIconPushButton->setMinimumWidth(faceIconSize);
                faceIconPushButton->setMinimumHeight(faceIconSize);
                faceIconPushButton->setIconSize(QSize(faceIconSize, faceIconSize));

                realNameLabel = new QLabel(Dialog);
                realNameLabel->setObjectName(QStringLiteral("realNameLabel"));
                realNameLabel->setGeometry(QRect(10, 120, 131, 20));
                realNameLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
                realNameEdit = new QLineEdit(Dialog);
                realNameEdit->setObjectName(QStringLiteral("realNameEdit"));
                realNameEdit->setGeometry(QRect(150, 120, 171, 21));
                realNameEdit->setContextMenuPolicy(Qt::NoContextMenu);
                realNameEdit->setAutoFillBackground(true);
                realNameEdit->setMaxLength(64);
                realNameEdit->setEchoMode(QLineEdit::Normal);
                QWidget::setTabOrder(userNameEdit, faceIconPushButton);
                QWidget::setTabOrder(faceIconPushButton, realNameEdit);
                QWidget::setTabOrder(realNameEdit, PwdEdit);
                QWidget::setTabOrder(PwdEdit, verifyPwdEdit);
                QWidget::setTabOrder(verifyPwdEdit, canLoginCheckBox);
                QWidget::setTabOrder(canLoginCheckBox, autoLoginCheckBox);
                QWidget::setTabOrder(autoLoginCheckBox, disLoginCheckBox);
                QWidget::setTabOrder(disLoginCheckBox, addBtn);
                QWidget::setTabOrder(addBtn, cancelBtn);
                infoLabel = new QLabel(Dialog);
                infoLabel->setObjectName(QStringLiteral("label"));
                infoLabel->setGeometry(QRect(80, 360, 301, 21));

                pwdProgressBar = new QProgressBar(Dialog);
                pwdProgressBar->setObjectName(QStringLiteral("pwdProgressBar"));
                pwdProgressBar->setGeometry(QRect(150, 180, 171, 20));
                pwdProgressBar->setValue(0);
                pwdProgressBar->setTextVisible(false);

            retranslateUi(Dialog);
            //Dialog->setStyleSheet("QLineEdit{ background-color: rgb(202, 202, 202);border:1px solid #c0fff5;}");
            QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QWidget *Dialog)
    {
        Q_UNUSED(Dialog);

            userNameLabel->setText(i18n("Account Name:"));
            headLable->setText(i18n("Add User"));
            cancelBtn->setText(i18n("Cancel"));
            addBtn->setText(i18n("Add"));
            passwordLabel->setText(i18n("Password:"));
            verifyPwdLabel->setText(i18n("Verify Password:"));
            AccTypeLable->setText(i18n("Administrator:"));
            autoLoginLabel->setText(i18n( "Automatic Login:"));
            autoLoginCheckBox->setText(QString());
            canLoginCheckBox->setText(QString());
            disLoginCheckBox->setText(QString());
            canLoginLabel->setText(i18n("Login Not Permitted:"));
            faceIconPushButton->setText(QString());
            realNameLabel->setText(i18n( "Display Name:"));

    #ifndef QT_NO_TOOLTIP
            userNameEdit->setToolTip(i18n("The name to login."));
            realNameEdit->setToolTip(i18n("The display name."));
            faceIconPushButton->setToolTip(i18n("Set an icon for the account."));
            PwdEdit->setToolTip(i18n("The password must be at least 6 characters and is case sensitive."));
            verifyPwdEdit->setToolTip(i18n("The password must be at least 6 characters and is case sensitive."));
            disLoginCheckBox->setToolTip(i18n("The account is not permitted to login."));
            autoLoginCheckBox->setToolTip(i18n("The account can automatic login."));
            canLoginCheckBox->setToolTip(i18n("The account type is adminstator."));
            pwdProgressBar->setToolTip(i18n("The password quality indicator."));
    #endif // QT_NO_TOOLTIP

    }

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

class AddUserDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddUserDlg(QtAccountsService::AccountsManager *am, 
                        QWidget *parent = NULL, 
                        Qt::WindowFlags f = Qt::Dialog);
    ~AddUserDlg();

Q_SIGNALS:
    void finished();

private slots:
    void slotAddUser();
    void slotFaceIconClicked(QString filePath);
    void slotFaceIconPressed(QPoint pos);
    void slotUserAdded(UserAccount* ua);
    void slotCheckPwd();
    bool slotCheckPwdMatch(const QString chgStr);
    void slotAutoLoginClicked();
    void slotDisLoginClicked();
private:
    QtAccountsService::AccountsManager *_am;
    Ui::Dialog ui;
    QWidget *Dlg =  NULL;
    FaceIconButton *_currentFaceIcon;
    QString iconFilePath;
    QString pwdErrorStr (int errorNo,int *value);
    bool checkPwd();
};

#endif /* __ADDUSERDLG_H__ */
