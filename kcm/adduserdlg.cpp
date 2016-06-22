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

#include "adduserdlg.h"
#include "faceiconpopup.h"

#include <unistd.h>
#include <QMessageBox>
#include <KLocalizedString>
#include <QtAccountsService/AccountsManager>
#include <QtAccountsService/UserAccount>
#include <QDebug>

const int minPasswordLength = 6;

bool _isAutoLoginChecked = false;

#include <pwquality.h>

static pwquality_settings_t *
get_pwq (void)
{
        static pwquality_settings_t *settings = NULL;

        if (settings == NULL) {
                char *err = NULL;
                settings = pwquality_default_settings ();
                pwquality_set_int_value (settings, PWQ_SETTING_MAX_SEQUENCE, 4);
                if (pwquality_read_config (settings, NULL,(void **) &err) < 0) {
                        printf ("failed to read pwquality configuration: %s\n", err);
                }
        }

        return settings;
}

QString AddUserDlg:: pwdErrorStr (int errorNo,int *value)
{
        *value = 0;
        switch (errorNo) {
        case PWQ_ERROR_SAME_PASSWORD:
                return i18n("It needs to be different from the old one.");
        case PWQ_ERROR_CASE_CHANGES_ONLY:
                return i18n("Try changing some letters and numbers.");
        case PWQ_ERROR_TOO_SIMILAR:
                return i18n("Try changing the password a bit more.");
        case PWQ_ERROR_USER_CHECK:
                return i18n("A password without your user name would be better.");
        case PWQ_ERROR_GECOS_CHECK:
                return i18n("Try to avoid using your name in the password.");

        case PWQ_ERROR_ROTATED:
                return i18n("Try changing the password a bit more.");
        case PWQ_ERROR_CRACKLIB_CHECK:
        case PWQ_ERROR_PALINDROME:
        case PWQ_ERROR_BAD_WORDS:
        case PWQ_ERROR_MIN_LENGTH:
                *value = 5;
                return i18n(" ");
        case PWQ_ERROR_MIN_DIGITS:
                return i18n("Try to use more numbers.");
        case PWQ_ERROR_MIN_UPPERS:
                return i18n("Try to use more uppercase letters.");
        case PWQ_ERROR_MIN_LOWERS:
                return i18n("Try to use more lowercase letters.");
        case PWQ_ERROR_MIN_OTHERS:
                return i18n("Try to use more special characters, like punctuation.");
        case PWQ_ERROR_MIN_CLASSES:
                return i18n("Try to use a mixture of letters, numbers.");
        case PWQ_ERROR_MAX_CONSECUTIVE:
                return i18n("Try to avoid repeating the same character.");
        case PWQ_ERROR_MAX_CLASS_REPEAT:
                return i18n("Try to mix up letters, numbers and punctuation.");
        case PWQ_ERROR_MAX_SEQUENCE:
                return i18n("Try to avoid sequences like 1234 or abcd.");
        case PWQ_ERROR_EMPTY_PASSWORD:
                return i18n("Mix uppercase and lowercase and use a number.");
        default:
                return i18n("Good password! ");
        }
}


AddUserDlg::AddUserDlg(QtAccountsService::AccountsManager *am, 
                       QWidget *parent, 
                       Qt::WindowFlags f)
  : QDialog(parent, f),
    _am(am)
{
    setWindowModality(Qt::WindowModal);

    Dlg = new QWidget;
    ui.setupUi(Dlg);

    QHBoxLayout *topLayout = new QHBoxLayout(this);
    topLayout->setMargin(0);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(Dlg);
    topLayout->addLayout(vbox);

    this->setWindowTitle(i18n("Add User"));

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::gray);
    ui.infoLabel->setPalette(pe);

    QObject::connect(ui.cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(ui.addBtn, SIGNAL(clicked()), this, SLOT(slotAddUser()));

    _currentFaceIcon = ui.faceIconPushButton;
    connect(ui.faceIconPushButton, SIGNAL(pressed(QPoint)),
            this, SLOT(slotFaceIconPressed(QPoint)));

    connect(ui.PwdEdit, SIGNAL(editingFinished()),
            this, SLOT(slotCheckPwd()));
    connect(ui.verifyPwdEdit, SIGNAL(editingFinished()),
            this, SLOT(slotCheckPwd()));

    connect(ui.PwdEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotCheckPwdMatch(const QString &)));
    connect(ui.verifyPwdEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotCheckPwdMatch(const QString &)));

    connect(ui.autoLoginCheckBox, SIGNAL(clicked()),
            this, SLOT(slotAutoLoginClicked()));

    connect(ui.disLoginCheckBox, SIGNAL(clicked()),
            this, SLOT(slotDisLoginClicked()));

    setFixedSize(400, 450);
}

AddUserDlg::~AddUserDlg() 
{
    if (Dlg) {
        delete Dlg;
    }
}

void AddUserDlg::slotDisLoginClicked()
{
    if(! ui.disLoginCheckBox->isChecked()) {
        return;
    }
    if( ui.disLoginCheckBox->isChecked()) {
       ui.autoLoginCheckBox->setChecked(false);
    }
    return;
}

void AddUserDlg::slotAutoLoginClicked()
{
    if(! ui.autoLoginCheckBox->isChecked()) {
        _isAutoLoginChecked = false;
        return;
    }

    QMessageBox msgBox(
        QMessageBox::Question,
        i18n("Override Automatic Login?"),
        i18n("Do you really want to override other`s Automaic Login?"),
        QMessageBox::Cancel | QMessageBox::Ok);

    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Ok) {
        _isAutoLoginChecked = true;

        if (ui.disLoginCheckBox->isChecked())
            ui.disLoginCheckBox->setChecked(false);
    }
    else {
        ui.autoLoginCheckBox->setChecked(false);
    }

}


static bool checkPwdSpelling(const char *src)
{
    //char otherStr[] = "~!@#$%^&*()_+{}|:\"<>?`-=[]\\;',./ ";
    bool isAlpha  = false, isDig = false;
    int  len = (int)strlen(src);

    if (src == NULL || src[0] == 0)
        return false;

    for (int i =0; i < len; i++) {
        if (isalpha(src[i]) ) {
            isAlpha = true;
        } else if (isdigit(src[i]) ) {
            isDig = true;
        }
    }

    return (isAlpha && isDig);

}

bool AddUserDlg::slotCheckPwdMatch(const QString chgStr)
{
    if (chgStr.isEmpty()) {
        return true;
    }
    if( ui.PwdEdit->text() !=  ui.verifyPwdEdit->text() ){
        ui.infoLabel->setText(i18n("Passwords do not match."));
        return false;
    } else {
        ui.infoLabel->setText("");
        return true;
    }
}


bool AddUserDlg::checkPwd()
{
    if( ui.PwdEdit->text().length() < minPasswordLength ) {
        ui.infoLabel->setText(i18n("The password must be at least 6 characters ."));
        ui.PwdEdit->selectAll();
        ui.pwdProgressBar->setValue(0);
        return false;
     }

    if( ui.verifyPwdEdit->text().length() < minPasswordLength ) {
        ui.infoLabel->setText(i18n("The verify password must be at least 6 characters ."));
        ui.verifyPwdEdit->selectAll();
        return false;
    }

    QString strPwd = ui.PwdEdit->text();
    if (!checkPwdSpelling(qPrintable(strPwd))) {
        ui.infoLabel->setText(i18n("Try to mix up alphabets and numbers."));
        ui.PwdEdit->selectAll();
        return false;
    }

    strPwd = ui.verifyPwdEdit->text();
    if (!checkPwdSpelling(qPrintable(strPwd))) {
        ui.infoLabel->setText(i18n("Try to mix up alphabets and numbers."));
        ui.verifyPwdEdit->selectAll();
        return false;
    }

    return true;

}

void AddUserDlg::slotCheckPwd()
{
    ui.infoLabel->setText("");

    if (ui.canLoginCheckBox->isChecked()) {
        if (!checkPwd()) {
            return;
        }

    }

    if (true) {
        char pwdStr[512] ="";
        char userName[128]="";
        if( ui.userNameEdit->text().trimmed().length() < 1 ) {
            ui.infoLabel->setText(i18n("Please input user name."));
            ui.userNameEdit->selectAll();
            return;
        }
        pwdStr[0] = '\0';
        userName[0] = '\0';
        snprintf(pwdStr,sizeof(pwdStr),"%s",qPrintable(ui.PwdEdit->text()) );
        snprintf(userName,sizeof(userName),"%s",qPrintable(ui.userNameEdit->text().trimmed() ) );
        void *auxerror;
        int pwdErrValue = 0;
        int pwdValue = pwquality_check (get_pwq (),
                            pwdStr, NULL, userName,&auxerror);
        if( pwdValue < 0 ) {
            pwdErrValue = 0;
            QString  setStr = pwdErrorStr(pwdValue,&pwdErrValue);
            if( pwdErrValue > 0 ) {
                int i = ui.PwdEdit->text().length();
                if( i < 0 ) {
                    i = 1;
                }
                pwdErrValue = 2*i;
                if( pwdErrValue >100 ) {
                    pwdErrValue = 100;
                }
                ui.pwdProgressBar->setValue(pwdErrValue);
            }
            else {
                ui.infoLabel->setText(setStr);
                ui.PwdEdit->selectAll();
                ui.pwdProgressBar->setValue(0);
                return;
            }
        }
        else {
            ui.pwdProgressBar->setValue(pwdValue);
        }
    }

    if( ui.verifyPwdEdit->text() !=  ui.PwdEdit->text() ){
        ui.infoLabel->setText(i18n("Passwords do not match."));
        return;
    }
}


void AddUserDlg::slotAddUser()
{
    if (!_am) {
        close();
    }
    QString userName = ui.userNameEdit->text().trimmed();

    if (userName.isEmpty()) {
        QMessageBox::warning(this, "warning",
            i18n("Please input user name."));
        return;
    }

    if (ui.canLoginCheckBox->isChecked()) {
        if (!checkPwd()) {
            return;
        }
    }

    if (ui.PwdEdit->text().isEmpty()) {
        QMessageBox::warning(this, "warning",
            i18n("Please input  account password."));
        return;
    }

    if (ui.verifyPwdEdit->text().isEmpty()) {
        QMessageBox::warning(this, "warning",
            i18n("Please input  verify password."));
        return;
    }

    if (ui.PwdEdit->text() != ui.verifyPwdEdit->text()) {
        QMessageBox::warning(this, "warning",
            i18n("Passwords do not match."));
        return;
    }

    QtAccountsService::UserAccount *user = _am->findUserByName(userName);
    if (user) {
        QMessageBox::warning(this, "warning", i18n("User already exists"));
        return;
    }

    QtAccountsService::UserAccount::AccountType accType = ui.canLoginCheckBox->isChecked() ?
                QtAccountsService::UserAccount::AdministratorAccountType :
                QtAccountsService::UserAccount::StandardAccountType;

    bool ret = _am->createUser(userName,ui.realNameEdit->text().trimmed(),accType);
    if (!ret) {
        QMessageBox::warning(this, "warning",i18n("Failed to add user."));
        close();
        return;
    }

    connect(_am, SIGNAL(userAdded(UserAccount *)),
                this, SLOT(slotUserAdded(UserAccount*)));
}

void AddUserDlg::slotUserAdded(UserAccount* ua)
{
    if (!ua) {
        QMessageBox::warning(this, "warning",i18n("can not find user!") );
        close();
        return;
    }

    if (ua->userName() != ui.userNameEdit->text()) {
        close();
        return;
    }

    qsrand(time(NULL));
    int n = qrand();
    char salt[8] = "";
    snprintf(salt, sizeof(salt) - 1, "$6$%02d", n);
    char *crystr = crypt(qPrintable(ui.verifyPwdEdit->text()), salt);
    if (crystr == NULL) {
        QMessageBox::warning(this, "warning",
            i18n("fail to crypt password, please change password manually."));
        close();
        return;
    }
    ua->setPassword(crystr);
    ua->setAutomaticLogin( ui.autoLoginCheckBox->isChecked() ? true : false);
    ua->setLocked(ui.disLoginCheckBox->isChecked() ? true : false);
    if (!iconFilePath.isEmpty()) {
        ua->setIconFileName(iconFilePath);
        iconFilePath = "";
    }

    emit finished();

    close();
}

void AddUserDlg::slotFaceIconClicked(QString filePath)
{
    _currentFaceIcon->setIcon(FaceIconPopup::faceIcon(filePath));
    iconFilePath = filePath;
}

void AddUserDlg::slotFaceIconPressed(QPoint pos)
{

    FaceIconPopup *faceIconPopup = new FaceIconPopup;
    connect(faceIconPopup, SIGNAL(clickFaceIcon(QString)),
            this, SLOT(slotFaceIconClicked(QString)));
    faceIconPopup->popup(pos);
}
