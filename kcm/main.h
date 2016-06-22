/**
 *  Copyright (C) 2015 Leslie Zhai <xiang.zhai@i-soft.com.cn>
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

#ifndef MAIN_H
#define MAIN_H

#include <kcmodule.h>

/* QtAccountsService */
#include <QtAccountsService/AccountsManager>

#include <PolkitQt1/Gui/ActionButton>

#include "faceiconbutton.h"
#include "pwdedit.h"

#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

class QObject;

class KCMUserAccount : public KCModule
{
	Q_OBJECT

public:
	explicit KCMUserAccount(QWidget *parent, const QVariantList & list = QVariantList());
	~KCMUserAccount();

	void load();
    void save();

private slots:
    void slotItemClicked(QListWidgetItem *item);
    void slotFaceIconClicked(QString filePath);
    void slotFaceIconPressed(QPoint pos);
    void slotPasswordEditPressed();
    void slotAddBtnClicked();
    void slotRemoveBtnClicked();
    void slotUnlockBtnClicked();
    void actionActivated();
    void slotAutoLoginClicked();

private:
    QtAccountsService::AccountsManager *_accountManager = nullptr;
    QtAccountsService::UserAccount *_loginUser = nullptr;
    QtAccountsService::UserAccount *_currentUser = nullptr;
    bool _unlocked = true;
    QListWidget *_accountList = nullptr;
    PolkitQt1::Gui::ActionButton *_actionBtn = nullptr;
    QPushButton *_addBtn = nullptr;
    QPushButton *_removeBtn = nullptr;
    FaceIconButton *_currentFaceIcon = nullptr;
    QLineEdit *_currentUserName = nullptr;
    QLineEdit *_currentRealName = nullptr;
    QLineEdit *_currentEmail = nullptr;
    QCheckBox *_currentAccountType = nullptr;
    QLabel *_currentLanguage = nullptr;
    PwdEdit *_passwdEdit = nullptr;
    QCheckBox *_autoLoginBox = nullptr;
    QCheckBox *_userLockedBox = nullptr;
    QString _editUserName = "";
    QString _editIconFilePath = "";
    bool _isAutoLoginChecked = false;
    bool _needUnlocked = false;
    QMetaObject::Connection _userFoundConnection;
    QMetaObject::Connection _currentAccountTypeConnection;
    QMetaObject::Connection _userLockedBoxConnection;

    void _unlockUi();
    bool _isOtherAutoLogin(uid_t uid);
    void setPasswordEdit(bool setAccType);
};

#endif // MAIN_H
