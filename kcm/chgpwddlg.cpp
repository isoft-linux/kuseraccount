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
#include <errno.h>
#include "chgpwddlg.h"
#include <shadow.h>
#include <unistd.h>
#include <QMessageBox>
#include <kwallet.h>
#include <pwquality.h>

const int minPasswordLength = 6;

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

QString ChgPwdDlg:: pwdErrorStr (int errorNo,int *value)
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


ChgPwdDlg::ChgPwdDlg(QtAccountsService::UserAccount *ua,
                     QWidget *parent,bool setAccType ,
                     Qt::WindowFlags f)
  : QDialog(parent, f),
    _ua(ua),
    _setAccType(setAccType)
{
    setWindowModality(Qt::WindowModal);

    Dlg = new QWidget;
    ui.setupUi(Dlg);

    QHBoxLayout *topLayout = new QHBoxLayout(this);
    topLayout->setMargin(0);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(Dlg);
    topLayout->addLayout(vbox);

    this->setWindowTitle(i18n("Change Password"));

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::gray);
    ui.infoLabel->setPalette(pe);

    QObject::connect(ui.cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(ui.changeBtn, SIGNAL(clicked()), this, SLOT(slotChangePwd()));

    connect(ui.newPwdEdit, SIGNAL(editingFinished()),
            this, SLOT(slotCheckPwd()));
    connect(ui.verPwdEdit, SIGNAL(editingFinished()),
            this, SLOT(slotCheckPwd()));

    connect(ui.newPwdEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotCheckPwdMatch(const QString &)));
    connect(ui.verPwdEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotCheckPwdMatch(const QString &)));

    setFixedSize(370, 250);
}

ChgPwdDlg::~ChgPwdDlg() 
{
    delete Dlg;
}

static bool checkPwd(const char *src)
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
bool ChgPwdDlg::slotCheckPwdMatch(const QString chgStr)
{
    if (chgStr.isEmpty()) {
        return true;
    }
    if( ui.verPwdEdit->text() !=  ui.newPwdEdit->text() ){
        ui.infoLabel->setText(i18n("Passwords do not match."));
        return false;
    } else {
        ui.infoLabel->setText("");
        return true;
    }
}

bool ChgPwdDlg::slotCheckPwd()
{
    ui.infoLabel->setText("");

    if (_ua->accountType() ==  QtAccountsService::UserAccount::AdministratorAccountType || _setAccType) {
        QString strPwd = ui.newPwdEdit->text();

        if( ui.newPwdEdit->text().length() < minPasswordLength ) {
            ui.infoLabel->setText(i18n("The password must be at least 6 characters ."));
            ui.newPwdEdit->selectAll();
            ui.pwdProgressBar->setValue(0);
            return false;
         }

        if (!checkPwd(qPrintable(strPwd))) {
            ui.infoLabel->setText(i18n("Try to mix up alphabets and numbers."));
            ui.newPwdEdit->selectAll();
            return false;
        }

        if( ui.verPwdEdit->text().length() < minPasswordLength ) {
            ui.infoLabel->setText(i18n("The verify password must be at least 6 characters ."));
            ui.verPwdEdit->selectAll();
            return false;
        }

        strPwd = ui.verPwdEdit->text();
        if (!checkPwd(qPrintable(strPwd))) {
            ui.infoLabel->setText(i18n("Try to mix up alphabets and numbers."));
            ui.verPwdEdit->selectAll();
            return false;
        }


    }

    if (true) {
        char pwdStr[512] ="";
        int    pwdErrValue = 0;
        pwdStr[0] = '\0';
        snprintf(pwdStr,sizeof(pwdStr),"%s",qPrintable(ui.newPwdEdit->text()) );
        void *auxerror;
        int pwdValue = pwquality_check (get_pwq (),
                            pwdStr, NULL, NULL,&auxerror);

        if( pwdValue < 0 ) {
            pwdErrValue = 0;
            QString  setStr = pwdErrorStr(pwdValue,&pwdErrValue);
            if( pwdErrValue > 0 ) {
                int i = ui.newPwdEdit->text().length() ;
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
                ui.newPwdEdit->selectAll();
                ui.pwdProgressBar->setValue(0);
                return false;
            }
        }
        else {
            ui.pwdProgressBar->setValue(pwdValue);
        }
    }

    if( ui.verPwdEdit->text() !=  ui.newPwdEdit->text() ){
        ui.infoLabel->setText(i18n("Passwords do not match."));
        return false;
    }

    return true;
}

void ChgPwdDlg::slotChangePwd()
{
    if (!ui.newPwdEdit || !ui.verPwdEdit) {
        return;
    }

    if (!_ua) {
        close();
        return;
    }

    if (!slotCheckPwd()) {
        return ;
    }

    if (ui.newPwdEdit->text().isEmpty() || ui.verPwdEdit->text().isEmpty())
        return;

    QString newStr = ui.newPwdEdit->text();
    QString verStr = ui.verPwdEdit->text();

    if (newStr == verStr) {
        qsrand(time(NULL));
        int n = qrand();
        char salt[8] = "";
        snprintf(salt, sizeof(salt) - 1, "$6$%02d", n);
        char *crystr = crypt(qPrintable(newStr), salt);
        if (crystr == NULL) {
            QMessageBox::warning(this, "warning",
                i18n("fail to crypt password, please try again."));
            close();
        }
        _ua->setPassword(crystr);
        if (_setAccType) {
            _ua->setAccountType((QtAccountsService::UserAccount::AccountType)_setAccType);
            usleep(500);
            _ua->setAccountType((QtAccountsService::UserAccount::AccountType)_setAccType);
        }
        // TODO:
        //KWallet::Wallet::changePassword(_ua->userName(), effectiveWinId());
        //KWallet::Wallet::requestChangePassword(0);
        close();
    } else {
        QMessageBox::warning(this, "warning", 
            i18n("password are not the same, please input again."));
    }

    return;
}
