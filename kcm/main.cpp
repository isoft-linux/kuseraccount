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

#include "main.h"
#include "faceiconpopup.h"
#include "chgpwddlg.h"
#include "adduserdlg.h"
#include "removeuserdlg.h"

#include <kdialog.h>
#include <kaboutdata.h>
#include <KToolInvocation>

#include <KPluginFactory>
#include <KPluginLoader>

#include <PolkitQt1/Authority>
#include <PolkitQt1/Subject>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QIcon>
#include <QMessageBox>

static const int opIconSize = 32;
static const int ctrlMaxWidth = 180;
static const QString unlockActionId = "org.freedesktop.accounts.user-administration";
static const QString lockedPixmap = ":/action-locked-default.png";
static const QString unLockedPixmap = ":/action-unlocked.png";

K_PLUGIN_FACTORY(Factory,
        registerPlugin<KCMUserAccount>();
        )
K_EXPORT_PLUGIN(Factory("useraccount"))

KCMUserAccount::KCMUserAccount(QWidget *parent, const QVariantList &)
  : KCModule(parent)
{
	setButtons(Help | Apply);

    QHBoxLayout *topLayout = new QHBoxLayout(this);
    topLayout->setSpacing(KDialog::spacingHint());
    topLayout->setMargin(0);

    QVBoxLayout *vbox = new QVBoxLayout;
    _accountList = new QListWidget;
    _accountList->setIconSize(QSize(faceIconSize, faceIconSize));
    _accountList->setMaximumWidth(206);
    connect(_accountList, SIGNAL(itemClicked(QListWidgetItem*)), 
            this, SLOT(slotItemClicked(QListWidgetItem*)));
    vbox->addWidget(_accountList);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setAlignment(Qt::AlignRight);
#if 0
    QPushButton *unlockBtn = new QPushButton;
    unlockBtn->setMaximumWidth(opIconSize);
    unlockBtn->setMaximumHeight(opIconSize);
    _actionBtn = new PolkitQt1::Gui::ActionButton(
        unlockBtn, unlockActionId, this);
    _actionBtn->setIcon(QPixmap(lockedPixmap));
    connect(_actionBtn, SIGNAL(clicked(QAbstractButton *, bool)),
            _actionBtn, SLOT(activate()));
    connect(_actionBtn, SIGNAL(authorized()), this, SLOT(actionActivated()));
    hbox->addWidget(unlockBtn);
#endif
    _addBtn = new QPushButton;
    _addBtn->setEnabled(_unlocked);
    _addBtn->setMaximumWidth(opIconSize);
    _addBtn->setMaximumHeight(opIconSize);
    _addBtn->setIcon(QIcon::fromTheme("list-add"));
    connect(_addBtn, SIGNAL(clicked()), this, SLOT(slotAddBtnClicked()));
    hbox->addWidget(_addBtn);
    _removeBtn = new QPushButton;
    _removeBtn->setEnabled(false);
    _removeBtn->setMaximumWidth(opIconSize);
    _removeBtn->setMaximumHeight(opIconSize);
    _removeBtn->setIcon(QIcon::fromTheme("list-remove"));
    connect(_removeBtn, SIGNAL(clicked()), this, SLOT(slotRemoveBtnClicked()));
    hbox->addWidget(_removeBtn);
    vbox->addLayout(hbox);
    topLayout->addLayout(vbox);

    topLayout->addSpacing(22);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setHorizontalSpacing(10);
    formLayout->setVerticalSpacing(10);
    topLayout->addLayout(formLayout);

    QLabel *subTitle = new QLabel(i18n("User Information Configuration"));
    subTitle->setStyleSheet("QLabel { color: #cccccc; }");
    formLayout->addRow(subTitle);

    _currentFaceIcon = new FaceIconButton;
    _currentFaceIcon->setMinimumWidth(60);
    _currentFaceIcon->setMinimumHeight(60);
    _currentFaceIcon->setIconSize(QSize(faceIconSize, faceIconSize));
    connect(_currentFaceIcon, SIGNAL(pressed(QPoint)), 
            this, SLOT(slotFaceIconPressed(QPoint)));
    _currentUserName = new QLineEdit("UserName");
    _currentUserName->setMaximumWidth(ctrlMaxWidth);
    _currentUserName->setEnabled(false);
    formLayout->addRow(_currentFaceIcon, _currentUserName);

    _currentRealName = new QLineEdit("RealName");
    _currentRealName->setMaximumWidth(ctrlMaxWidth);
    formLayout->addRow(i18n("Real Name:"), _currentRealName);

    _currentEmail = new QLineEdit("Email");
    _currentEmail->setMaximumWidth(ctrlMaxWidth);
    //formLayout->addRow(i18n("Email Address:"), _currentEmail);

    _passwdEdit = new PwdEdit("password");
    _passwdEdit->setMaximumWidth(ctrlMaxWidth);
    _passwdEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    connect(_passwdEdit, SIGNAL(pressed()), this, SLOT(slotPasswordEditPressed()));
    formLayout->addRow(i18n("Password:"), _passwdEdit);

    subTitle = new QLabel(i18n("Advanced Information Configuration"));
    subTitle->setStyleSheet("QLabel { color: #cccccc; }");
    formLayout->addRow(subTitle);

    _currentAccountType = new QCheckBox;
    formLayout->addRow(i18n("Administrator:"), _currentAccountType);
    _currentAccountType->setToolTip(i18n("It need to reset password when \nuser account type is changed to administrator."));

    _autoLoginBox = new QCheckBox;
    _autoLoginBox->setEnabled(_unlocked);
    formLayout->addRow(i18n("Automatic Login:"), _autoLoginBox);

    _userLockedBox = new QCheckBox;
    formLayout->addRow(i18n("Login Not Permitted"), _userLockedBox);

    QLabel *autostart = new QLabel(i18n("<a href=\"#\">Autostart application configuration</a>"));
    connect(autostart, &QLabel::linkActivated, [this]() {
        KToolInvocation::kdeinitExec(QString("kcmshell5"),
                                     QStringList() << QString("autostart"));
    });
    formLayout->addRow(autostart);

    KAboutData *about = new KAboutData("kcm_useraccount", 
        i18n("Password & User Information"), 
        QString(PROJECT_VERSION), i18n("Password & User Information"), 
        KAboutLicense::GPL_V2, i18n("(C) 2015, Leslie Zhai"));

    about->addAuthor(i18n("Leslie Zhai"), i18n("Developer"), "xiang.zhai@i-soft.com.cn");
    about->addAuthor(i18n("fujiang"), i18n("Developer"), "fujiang.zhu@i-soft.com.cn");
    setAboutData(about);

	setQuickHelp(i18n("<qt>Here you can change your personal information, which "
			"will be used, for instance, in mail programs and word processors. You can "
			"change your login password by clicking <em>Change Password...</em>.</qt>") );

    _accountManager = new QtAccountsService::AccountsManager;
    connect(_accountManager, &QtAccountsService::AccountsManager::userDeleted,
            this, &KCMUserAccount::load);
    connect(_accountManager, &QtAccountsService::AccountsManager::userAdded,
            this, [=](QtAccountsService::UserAccount *) {
                load();

                _accountManager->listCachedUsersAsync();
                connect(_accountManager,
                        &QtAccountsService::AccountsManager::listCachedUsersFinished,
                        this,
                        [=](QtAccountsService::UserAccountList userList) {
                            for (QtAccountsService::UserAccount *user : userList) {
                                QDir homeDir(user->homeDirectory());
                                if (!homeDir.exists())
                                    continue;

                                disconnect(user, SIGNAL(accountChanged()), 
                                           this, SLOT(load()));
                                connect(user, SIGNAL(accountChanged()), 
                                        this, SLOT(load()));
                            }
                        });
            });

    _loginUser = new QtAccountsService::UserAccount;

    _accountManager->listCachedUsersAsync();
    connect(_accountManager, 
            &QtAccountsService::AccountsManager::listCachedUsersFinished,
            this, [=](QtAccountsService::UserAccountList userList) {
                for (QtAccountsService::UserAccount *user : userList) {
                    QDir homeDir(user->homeDirectory());
                    if (!homeDir.exists())
                        continue;

                    connect(user, SIGNAL(accountChanged()), this, SLOT(load()));
                }
            });
}

KCMUserAccount::~KCMUserAccount()
{
    if (_accountList) {
        while (_accountList->count()) {
            _accountList->takeItem(0);
        }
        delete _accountList;
        _accountList = nullptr;
    }

    if (_currentFaceIcon) {
        delete _currentFaceIcon;
        _currentFaceIcon = nullptr;
    }

    if (_currentUserName) {
        delete _currentUserName;
        _currentUserName = nullptr;
    }

    if (_currentRealName) {
        delete _currentRealName;
        _currentRealName = nullptr;
    }

    if (_currentEmail) {
        delete _currentEmail;
        _currentEmail = nullptr;
    }

    if (_passwdEdit) {
        delete _passwdEdit;
        _passwdEdit = nullptr;
    }

    if (_currentAccountType) {
        delete _currentAccountType;
        _currentAccountType = nullptr;
    }

    if (_accountManager) {
        delete _accountManager;
        _accountManager = nullptr;
    }

    if (_currentUser) {
        delete _currentUser;
        _currentUser = nullptr;
    }

    if (_loginUser) {
        delete _loginUser;
        _loginUser = nullptr;
    }
}

void KCMUserAccount::_unlockUi() 
{
    _actionBtn->setIcon(_unlocked ?
                        QPixmap(unLockedPixmap) :
                        QPixmap(lockedPixmap));
    _addBtn->setEnabled(_unlocked);
    _autoLoginBox->setEnabled(_unlocked);
    load();
}

void KCMUserAccount::slotUnlockBtnClicked() 
{
    if (_unlocked) {
        _unlocked = false;
        _unlockUi();
        disconnect(_actionBtn, SIGNAL(clicked(QAbstractButton *, bool)),
                   this, SLOT(slotUnlockBtnClicked()));
        connect(_actionBtn, SIGNAL(clicked(QAbstractButton *, bool)),
                _actionBtn, SLOT(activate()));
    }
}

void KCMUserAccount::slotFaceIconClicked(QString filePath) 
{
    if (_currentUser) {
        _currentUser->setIconFileName(filePath);
        _currentFaceIcon->setIcon(FaceIconPopup::faceIcon(filePath));
        _editUserName = _currentUser->userName();
    }
}

void KCMUserAccount::slotFaceIconPressed(QPoint pos)
{
    if (!_currentUser || !_loginUser)
        return;
    
    if (_currentUser->userName() != _loginUser->userName()) {
        actionActivated();

        if (!_unlocked)
            return;
    }

    FaceIconPopup *faceIconPopup = new FaceIconPopup;
    connect(faceIconPopup, SIGNAL(clickFaceIcon(QString)), 
            this, SLOT(slotFaceIconClicked(QString)));
    faceIconPopup->popup(pos);
}

void KCMUserAccount::slotPasswordEditPressed() 
{
    setPasswordEdit(false);
}

void KCMUserAccount::setPasswordEdit(bool setAccType)
{
    ChgPwdDlg *chgPwdDlg = new ChgPwdDlg(_currentUser, this,setAccType);
    chgPwdDlg->show();

    if (chgPwdDlg->exec() == QDialog::Accepted)
        QMessageBox::warning(this, "warning", "info", "okok");

    delete chgPwdDlg;
}
void KCMUserAccount::slotAddBtnClicked()
{
    AddUserDlg *addUserDlg = new AddUserDlg(_accountManager, this);
    addUserDlg->show();
    connect(addUserDlg, SIGNAL(finished()), this, SLOT(load()));
}

void KCMUserAccount::slotRemoveBtnClicked()
{
    char info[512] = "";

    if (!_currentUser || !_accountManager)
        return;

    if (getuid() == _currentUser->userId()) {
        strncpy(info, "You cannot delete your own account.", sizeof(info));
        QMessageBox::warning(this, "warning", info);
        return;
    } else if(0) {
        strncpy(info, "Deleting a user while they are logged in can leave the "
                      "system in an inconsistent state.", 
                sizeof(info));
        QMessageBox::warning(this, "warning", info);
        return;
    } else if(_currentUser->isLocalAccount()) {
        snprintf(info, sizeof(info) - 1, "will delete local account, uid[%d]", 
                 (int)_currentUser->userId());
    } else {
        strncpy(info, "will delete enterprise user account.", sizeof(info));
        QMessageBox::warning(this, "warning", info);
        return;
    }

    RemoveUserDlg *removeUserDlg = new RemoveUserDlg(_accountManager, _currentUser, this);
    if (removeUserDlg->exec() == QDialog::Accepted) {
        QMessageBox::warning(this, "warning", "info", "ok");
    }

    connect(removeUserDlg, SIGNAL(finished()), this, SLOT(load()));

    delete removeUserDlg;

    return;
}

bool KCMUserAccount::_isOtherAutoLogin(uid_t uid) 
{
    if (!_accountManager)
        return false;

    for (QtAccountsService::UserAccount *user : _accountManager->listCachedUsers()) {
        if (user->automaticLogin()) {
            if (user->userId() != uid)
                return true;
        }
    }

    return false;
}

void KCMUserAccount::slotAutoLoginClicked() 
{
    actionActivated();

    int state = _autoLoginBox->checkState();
    if (state == Qt::Checked) {
        if (_userLockedBox->isChecked())
            _userLockedBox->setChecked(false);
    }
    if (state == Qt::Checked && _isAutoLoginChecked == false) {
        if (_isOtherAutoLogin(_currentUser->userId())) {
            QMessageBox msgBox(
                QMessageBox::Question,
                i18n("Override Automatic Login?"),
                i18n("Do you really want to override other`s Automaic Login?"),
                QMessageBox::Cancel | QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Ok) {
                _isAutoLoginChecked = true;
                if (_userLockedBox->isChecked())
                    _userLockedBox->setChecked(false);

                _needUnlocked = true;
                emit changed();
            } else {
                disconnect(_autoLoginBox, SIGNAL(clicked()), this, SLOT(slotAutoLoginClicked()));
                _autoLoginBox->setChecked(false);
                connect(_autoLoginBox, SIGNAL(clicked()), this, SLOT(slotAutoLoginClicked()));
            }
        } else {
            _needUnlocked = true;
            if (_userLockedBox->isChecked())
                _userLockedBox->setChecked(false);

            emit changed();
        }
    }
    if (state == Qt::Unchecked) {
        _isAutoLoginChecked = false;
        _needUnlocked = true;
        emit changed();
    }
}

void KCMUserAccount::slotItemClicked(QListWidgetItem *item) 
{
    if (!_loginUser)
        return;
    
    _isAutoLoginChecked = false;

    QString itemText = item->text();
    
    _accountManager->findUserByNameAsync(itemText);
    disconnect(_userFoundConnection);
    _userFoundConnection = connect(_accountManager, &QtAccountsService::AccountsManager::userFound, [this](QtAccountsService::UserAccount* user) {
        _currentUser = user;

        _currentFaceIcon->setIcon(FaceIconPopup::faceIcon(_currentUser->iconFileName()));
        _currentUserName->setText(_currentUser->userName());
        _currentRealName->setText(_currentUser->displayName());
        _currentEmail->setText(_currentUser->email());

        _currentAccountType->setChecked((int)_currentUser->accountType());
        if (_currentUser->userName() == _loginUser->userName()) {
            _currentRealName->setEnabled(true);
            _currentEmail->setEnabled(true);
            _currentAccountType->setEnabled(false);
            _userLockedBox->setEnabled(false);
            _removeBtn->setEnabled(false);
        } else {
            _currentRealName->setEnabled(_unlocked);
            _currentEmail->setEnabled(_unlocked);
            _currentAccountType->setEnabled(_unlocked);
            _userLockedBox->setEnabled(_unlocked);
            _removeBtn->setEnabled(_unlocked);
        }

        _passwdEdit->setEnabled(_unlocked);

        connect(_currentRealName, SIGNAL(textEdited(const QString &)), this, SLOT(changed()));
        connect(_currentEmail, SIGNAL(textEdited(const QString &)), this, SLOT(changed()));
        disconnect(_currentAccountTypeConnection);
        _currentAccountTypeConnection = connect(_currentAccountType, &QCheckBox::clicked, 
            [this]() {
            if (_currentAccountType->isChecked()) {
                actionActivated();
                setPasswordEdit(true);
            } else {
                _needUnlocked = true;
                actionActivated();
                emit changed();
            }

        });

        _autoLoginBox->setChecked(_currentUser->automaticLogin());
        disconnect(_autoLoginBox, SIGNAL(clicked()), this, SLOT(slotAutoLoginClicked()));
        connect(_autoLoginBox, SIGNAL(clicked()), this, SLOT(slotAutoLoginClicked()));

        _userLockedBox->setChecked(_currentUser->isLocked());
        disconnect(_userLockedBoxConnection);
        _userLockedBoxConnection = connect(_userLockedBox, &QCheckBox::clicked, [this]() {
            if (_userLockedBox->isChecked())
                _autoLoginBox->setChecked(false);

            _needUnlocked = true;
            actionActivated();
            emit changed();
        });
    });
}

void KCMUserAccount::actionActivated()
{
    PolkitQt1::Authority::Result result;
    PolkitQt1::UnixProcessSubject subject(static_cast<uint>(
        QCoreApplication::applicationPid()));

    // TODO: Use async API instead
    // result = PolkitQt1::Authority::instance()->checkAuthorizationSync(
    PolkitQt1::Authority::instance()->checkAuthorization(
        unlockActionId, subject, PolkitQt1::Authority::AllowUserInteraction);
    Q_UNUSED(result);
#if 0
    if (result == PolkitQt1::Authority::Yes) {
        _unlocked = true;
        disconnect(_actionBtn, SIGNAL(clicked(QAbstractButton *, bool)),
                   _actionBtn, SLOT(activate()));
        connect(_actionBtn, SIGNAL(clicked(QAbstractButton*, bool)),
                this, SLOT(slotUnlockBtnClicked()));
    } else {
        _unlocked = false;
    }

    _unlockUi();
#endif
}

void KCMUserAccount::load()
{
    QtAccountsService::UserAccountList userList = _accountManager->listCachedUsers();
        
    while (_accountList->count())
        _accountList->takeItem(0);

    // My Account
    QListWidgetItem *item = new QListWidgetItem(i18n("My Account"));
    //item->setFlags(!Qt::ItemIsEnabled);
    _accountList->addItem(item);

    QtAccountsService::UserAccount *myUserAccount = new QtAccountsService::UserAccount;
    QString myAccountLoginName = "";
    if (myUserAccount) {
        myAccountLoginName = myUserAccount->userName();

        QString iconFilePath = myUserAccount->iconFileName();
        item = new QListWidgetItem(FaceIconPopup::faceIcon(iconFilePath), 
                                   myAccountLoginName);
        _accountList->addItem(item);
        _accountList->setCurrentItem(item);
        if (_editUserName == myAccountLoginName)
            _accountList->setCurrentItem(item);

        if (_editUserName == "")
            slotItemClicked(item);
    }

    unsigned int otherAccounts = 0;
    for (QtAccountsService::UserAccount *user : userList) {
        if (user->userName() == myAccountLoginName)
            continue;

        QDir homeDir(user->homeDirectory());
        if (!homeDir.exists())
            continue;

        otherAccounts++;
    }

    // Other Accounts
    if (otherAccounts) {
        item = new QListWidgetItem(i18n("Other Accounts"));
        //item->setFlags(!Qt::ItemIsEnabled);
        _accountList->addItem(item);
    }

    for (QtAccountsService::UserAccount *user : userList) {
        if (user->userName() == myAccountLoginName)
            continue;

        QDir homeDir(user->homeDirectory());
        if (!homeDir.exists())
            continue;

        QString iconFilePath = user->iconFileName();
        item = new QListWidgetItem(FaceIconPopup::faceIcon(iconFilePath), 
                                   user->userName());
        _accountList->addItem(item);

        if (_editUserName == user->userName())
            _accountList->setCurrentItem(item);
    }
}

void KCMUserAccount::save() 
{
    if (_currentUser == nullptr)
        return;

    _currentUser->setRealName(_currentRealName->text());
    _currentUser->setEmail(_currentEmail->text());
    if (_needUnlocked) {
        _currentUser->setAccountType((QtAccountsService::UserAccount::AccountType)_currentAccountType->isChecked());
        _currentUser->setAutomaticLogin(_autoLoginBox->isChecked());
        _currentUser->setLocked(_userLockedBox->isChecked());
    }
}

#include "main.moc"
