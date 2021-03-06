/***************************************************************************
**
** Copyright (C) 2012 Jolla Ltd.
** Contact: Robin Burchell <robin.burchell@jollamobile.com>
**
** This file is part of lipstick.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QCoreApplication>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDBusError>
#include <QDBusServiceWatcher>
#include <mremoteaction.h>
#include <sys/statfs.h>
#include "categorydefinitionstore.h"
#include "notificationmanageradaptor.h"
#include "notificationmanager.h"

#include <iostream>
using namespace std;

// Define this if you'd like to see debug messages from the notification manager
#ifdef DEBUG_NOTIFICATIONS
#define NOTIFICATIONS_DEBUG(things) qDebug() << Q_FUNC_INFO << things
#else
#define NOTIFICATIONS_DEBUG(things)
#endif

//! The category definitions directory
static const char *CATEGORY_DEFINITION_FILE_DIRECTORY = "/usr/share/lipstick/notificationcategories";

//! The number configuration files to load into the event type store.
static const uint MAX_CATEGORY_DEFINITION_FILES = 100;

//! Path of the configuration directory relative to the home directory
static const char *CONFIG_PATH = "/.config/lipstick";

//! Minimum amount of disk space needed for the notification database in kilobytes
static const uint MINIMUM_FREE_SPACE_NEEDED_IN_KB = 1024;

const char *NotificationManager::HINT_URGENCY = "urgency";
const char *NotificationManager::HINT_CATEGORY = "category";
const char *NotificationManager::HINT_DESKTOP_ENTRY = "desktop-entry";
const char *NotificationManager::HINT_IMAGE_DATA = "image_data";
const char *NotificationManager::HINT_SOUND_FILE = "sound-file";
const char *NotificationManager::HINT_SUPPRESS_SOUND = "suppress-sound";
const char *NotificationManager::HINT_X = "x";
const char *NotificationManager::HINT_Y = "y";
const char *NotificationManager::HINT_ICON = "x-nemo-icon";
const char *NotificationManager::HINT_ITEM_COUNT = "x-nemo-item-count";
const char *NotificationManager::HINT_PRIORITY = "x-nemo-priority";
const char *NotificationManager::HINT_TIMESTAMP = "x-nemo-timestamp";
const char *NotificationManager::HINT_PREVIEW_ICON = "x-nemo-preview-icon";
const char *NotificationManager::HINT_PREVIEW_BODY = "x-nemo-preview-body";
const char *NotificationManager::HINT_PREVIEW_SUMMARY = "x-nemo-preview-summary";
const char *NotificationManager::HINT_REMOTE_ACTION_PREFIX = "x-nemo-remote-action-";
const char *NotificationManager::HINT_USER_REMOVABLE = "x-nemo-user-removable";
const char *NotificationManager::HINT_USER_CLOSEABLE = "x-nemo-user-closeable";
const char *NotificationManager::HINT_FEEDBACK = "x-nemo-feedback";
const char *NotificationManager::HINT_HIDDEN = "x-nemo-hidden";

NotificationManager *NotificationManager::instance_ = 0;

NotificationManager *NotificationManager::instance()
{
    if (instance_ == 0) {
        instance_ = new NotificationManager(qApp);
    }
    return instance_;
}

NotificationManager::NotificationManager(QObject *parent) :
    QObject(parent),
    previousNotificationID(0),
    categoryDefinitionStore(new CategoryDefinitionStore(CATEGORY_DEFINITION_FILE_DIRECTORY, MAX_CATEGORY_DEFINITION_FILES, this)),
    database(new QSqlDatabase),
    committed(true)
{

    cout << "NotificationManager::NotificationManager()" << endl;
    qDBusRegisterMetaType<QVariantHash>();
    qDBusRegisterMetaType<Notification>();
    qDBusRegisterMetaType<QList<Notification> >();

    adaptor = new NotificationManagerAdaptor(this);
    QString serviceName("org.freedesktop.Notifications");
    QString objectName("/org/freedesktop/Notifications");
    QDBusConnection sessionBus = QDBusConnection::sessionBus();

    watcher = new QDBusServiceWatcher(serviceName, sessionBus,
                    QDBusServiceWatcher::WatchForUnregistration
                            | QDBusServiceWatcher::WatchForRegistration
                            | QDBusServiceWatcher::WatchForOwnerChange);
    connect(watcher, SIGNAL(serviceUnregistered(const QString)), this, SLOT(serviceUnregistered(QString)));
    connect(watcher, SIGNAL(serviceRegistered(QString)), this, SLOT(serviceRegistered(QString)));
    connect(watcher, SIGNAL(serviceOwnerChanged(QString,QString,QString)), this, SLOT(serviceOwnerChange(QString,QString,QString)));

    bool serviceOK = sessionBus.registerService(serviceName);
    bool objectOK = sessionBus.registerObject(objectName, this);

    cout << "DBUS SERVICES REGISTERED "
         << " service=" << (serviceOK?"OK":"KO")
         << " object=" << (objectOK?"OK":"KO")
         << " baseService=" << QDBusConnection::sessionBus().baseService().toUtf8().data()
         << " name=" << QDBusConnection::sessionBus().name().toUtf8().data()
         << " id=" << QDBusConnection::sessionBus().localMachineId().toULong()
         << " ********************************************" << endl;
    cout << "ERROR: name=" << sessionBus.lastError().name().toUtf8().data() << " msg=" << sessionBus.lastError().message().toUtf8().data() << endl;

    connect(categoryDefinitionStore, SIGNAL(categoryDefinitionUninstalled(QString)), this, SLOT(removeNotificationsWithCategory(QString)));
    connect(categoryDefinitionStore, SIGNAL(categoryDefinitionModified(QString)), this, SLOT(updateNotificationsWithCategory(QString)));

    // Commit the modifications to the database 10 seconds after the last modification so that writing to disk doesn't affect user experience
    databaseCommitTimer.setInterval(10000);
    databaseCommitTimer.setSingleShot(true);
    connect(&databaseCommitTimer, SIGNAL(timeout()), this, SLOT(commit()));

    restoreNotifications();

    cout << "NotificationManager::NotificationManager() END count=" << this->calls.size() << endl;



}


void NotificationManager::serviceUnregistered(QString service)
{
    cout << "NotificationManager::serviceUnregistered: " << service.toUtf8().data() << endl;
}

void NotificationManager::serviceRegistered(QString service)
{
    cout << "NotificationManager::serviceRegistered: " << service.toUtf8().data() << endl;
}
void NotificationManager::serviceOwnerChange(QString service, QString oldOwner, QString newOwner) {
    cout << "NotificationManager::serviceOwnerChange: "
         << " service=" << service.toUtf8().data()
         << " oldOwner=" << oldOwner.toUtf8().data()
         << " newOwner=" << newOwner.toUtf8().data() << endl;
}

NotificationManager::~NotificationManager()
{
    database->commit();
    delete database;
}

Notification *NotificationManager::notification(uint id) const
{
    return notifications.value(id);
}

QList<uint> NotificationManager::notificationIds() const
{
    return notifications.keys();
}

QStringList NotificationManager::GetCapabilities()
{
    return QStringList() << "body" << "actions" << HINT_ICON << HINT_ITEM_COUNT << HINT_TIMESTAMP << HINT_PREVIEW_ICON << HINT_PREVIEW_BODY << HINT_PREVIEW_SUMMARY << "x-nemo-remote-actions" << HINT_USER_REMOVABLE << "x-nemo-get-notifications";
}

uint NotificationManager::Notify(const QString &appName, uint replacesId, const QString &appIcon, const QString &summary, const QString &body, const QStringList &actions, const QVariantHash &originalHints, int expireTimeout)
{
    uint id = replacesId != 0 ? replacesId : nextAvailableNotificationID();

    if (replacesId == 0 || notifications.contains(id)) {
        // Apply a category definition, if any, to the hints
        QVariantHash hints(originalHints);
        applyCategoryDefinition(hints);

        // Ensure the hints contain a timestamp
        addTimestamp(hints);

        if (replacesId == 0) {
            // Create a new notification
            Notification *notification = new Notification(appName, id, appIcon, summary, body, actions, hints, expireTimeout, this);
            connect(notification, SIGNAL(actionInvoked(QString)), this, SLOT(invokeAction(QString)));
            notifications.insert(id, notification);
        } else {
            // Only replace an existing notification if it really exists
            Notification *notification = notifications.value(id);
            notification->setAppName(appName);
            notification->setAppIcon(appIcon);
            notification->setSummary(summary);
            notification->setBody(body);
            notification->setActions(actions);
            notification->setHints(hints);
            notification->setExpireTimeout(expireTimeout);

            // Delete the existing notification from the database
            execSQL(QString("DELETE FROM notifications WHERE id=?"), QVariantList() << id);
            execSQL(QString("DELETE FROM actions WHERE id=?"), QVariantList() << id);
            execSQL(QString("DELETE FROM hints WHERE id=?"), QVariantList() << id);
        }

        // Add the notification, its actions and its hints to the database
        execSQL("INSERT INTO notifications VALUES (?, ?, ?, ?, ?, ?)", QVariantList() << id << appName << appIcon << summary << body << expireTimeout);
        foreach (const QString &action, actions) {
            execSQL("INSERT INTO actions VALUES (?, ?)", QVariantList() << id << action);
        }
        foreach (const QString &hint, hints.keys()) {
            execSQL("INSERT INTO hints VALUES (?, ?, ?)", QVariantList() << id << hint << hints.value(hint));
        }

        NOTIFICATIONS_DEBUG("NOTIFY:" << appName << appIcon << summary << body << actions << hints << expireTimeout << "->" << id);
        emit notificationModified(id);
    } else {
        // Return the ID 0 when trying to update a notification which doesn't exist
        id = 0;
    }

    return id;
}

void NotificationManager::notifyCall(const QString &from){

    cout << "NotificationManager::NotifyCall from=" << from.toUtf8().data() << endl;

    // Create a new call
    Call *call = new Call(from, this);
    //connect(call, SIGNAL(actionInvoked(QString)), this, SLOT(invokeAction(QString)));
    calls.append(call);

    // Add the call to the database
    execSQL("INSERT INTO calls VALUES (?)", QVariantList() << from);


    cout << "NotificationManager::NotifyCall OK" << endl;

    NOTIFICATIONS_DEBUG("NotifyCall:" << from);
    emit callsModified();

}

void NotificationManager::notifySMS(const QString &from){

    cout << "NotificationManager::NotifySMS from=" << from.toUtf8().data() << endl;

    // Create a new sms
    SMS *sms = new SMS(from, this);
    //connect(sms, SIGNAL(actionInvoked(QString)), this, SLOT(invokeAction(QString)));
    SMSs.append(sms);

    // Add the sms to the database
    execSQL("INSERT INTO SMSs VALUES (?)", QVariantList() << from);

    cout << "NotificationManager::NotifySMS OK" << endl;
    NOTIFICATIONS_DEBUG("NotifySMS:" << from);
    emit smsModified();

}


void NotificationManager::CloseNotification(uint id, NotificationClosedReason closeReason)
{
    if (notifications.contains(id)) {
        emit NotificationClosed(id, closeReason);

        // Remove the notification, its actions and its hints from database
        execSQL(QString("DELETE FROM notifications WHERE id=?"), QVariantList() << id);
        execSQL(QString("DELETE FROM actions WHERE id=?"), QVariantList() << id);
        execSQL(QString("DELETE FROM hints WHERE id=?"), QVariantList() << id);

        NOTIFICATIONS_DEBUG("REMOVE:" << id);
        emit notificationRemoved(id);

        // Mark the notification to be destroyed
        removedNotifications.insert(notifications.take(id));
    }
}

QString NotificationManager::GetServerInformation(QString &name, QString &vendor, QString &version)
{
    name = qApp->applicationName();
    vendor = "Nemo Mobile";
    version = qApp->applicationVersion();
    return QString();
}

QList<Notification> NotificationManager::GetNotifications(const QString &appName)
{
    QList<Notification> notificationList;
    foreach (uint id, notifications.keys()) {
        Notification *notification = notifications.value(id);
        if (notification->appName() == appName) {
            notificationList.append(*notification);
        }
    }
    return notificationList;
}

uint NotificationManager::nextAvailableNotificationID()
{
    bool idIncreased = false;

    // Try to find an unused ID. Increase the ID at least once but only up to 2^32-1 times.
    for (uint i = 0; i < UINT32_MAX && (!idIncreased || notifications.contains(previousNotificationID)); i++, idIncreased = true) {
        previousNotificationID++;

        if (previousNotificationID == 0) {
            // 0 is not a valid ID so skip it
            previousNotificationID = 1;
        }
    }

    return previousNotificationID;
}

void NotificationManager::removeNotificationsWithCategory(const QString &category)
{
    foreach(uint id, notifications.keys()) {
        if (notifications[id]->hints().value("category").toString() == category) {
            CloseNotification(id);
        }
    }
}

void NotificationManager::updateNotificationsWithCategory(const QString &category)
{
    foreach(uint id, notifications.keys()) {
        if (notifications[id]->hints().value("category").toString() == category) {
            Notify(notifications[id]->appName(), id, notifications[id]->appIcon(), notifications[id]->summary(), notifications[id]->body(), notifications[id]->actions(), notifications[id]->hints(), notifications[id]->expireTimeout());
        }
    }
}

void NotificationManager::applyCategoryDefinition(QVariantHash &hints)
{
    QString category = hints.value(HINT_CATEGORY).toString();
    if (!category.isEmpty()) {
        foreach (const QString &key, categoryDefinitionStore->allKeys(category)) {
            hints.insert(key, categoryDefinitionStore->value(category, key));
        }
    }
}

void NotificationManager::addTimestamp(QVariantHash &hints)
{
    if (hints.value(HINT_TIMESTAMP).toString().isEmpty()) {
        hints.insert(HINT_TIMESTAMP, QDateTime::currentDateTimeUtc());
    }
}

void NotificationManager::restoreNotifications()
{
    cout << "NotificationManager::restoreNotifications()" << endl;
    if (connectToDatabase()) {
        if (checkTableValidity()) {
            fetchData();
        } else {
            database->close();
        }
    }
}

bool NotificationManager::connectToDatabase()
{
    QString configPath = QDir::homePath() + QString(CONFIG_PATH);
    if (!QDir::root().exists(configPath)) {
        QDir::root().mkpath(configPath);
    }

    QString databaseName = configPath + "/notifications.db";
    *database = QSqlDatabase::addDatabase("QSQLITE", metaObject()->className());
    database->setDatabaseName(databaseName);
    bool success = checkForDiskSpace(configPath, MINIMUM_FREE_SPACE_NEEDED_IN_KB);
    if (success) {
        success = database->open();
        if (!success) {
            NOTIFICATIONS_DEBUG(database->lastError().driverText() << databaseName << database->lastError().databaseText());

            // If opening the database fails, try to recreate the database
            removeDatabaseFile(databaseName);
            success = database->open();
            NOTIFICATIONS_DEBUG("Unable to open database file. Recreating. Success: " << success);
        }
    } else {
        NOTIFICATIONS_DEBUG("Not enough free disk space available. Unable to open database.");
    }

    if (success) {
        // Set up the database mode to write-ahead locking to improve performance
        QSqlQuery(*database).exec("PRAGMA journal_mode=WAL");
    }

    return success;
}

bool NotificationManager::checkForDiskSpace(const QString &path, unsigned long freeSpaceNeeded)
{
    struct statfs st;
    bool spaceAvailable = false;
    if (statfs(path.toUtf8().data(), &st) != -1) {
        unsigned long freeSpaceInKb = (st.f_bsize * st.f_bavail) / 1024;
        if (freeSpaceInKb > freeSpaceNeeded) {
            spaceAvailable = true;
        }
    }
    return spaceAvailable;
}

void NotificationManager::removeDatabaseFile(const QString &path)
{
    // Remove also -shm and -wal files created when journal-mode=WAL is being used
    QDir::root().remove(path + "-shm");
    QDir::root().remove(path + "-wal");
    QDir::root().remove(path);
}

bool NotificationManager::checkTableValidity()
{

    cout << "NotificationManager::checkTableValidity()" << endl;

    bool result = true;
    bool recreateNotificationsTable = false;
    bool recreateActionsTable = false;
    bool recreateHintsTable = false;
    bool recreateCallsTable = false;
    bool recreateSMSTable = false;


    {
        // Check that the notifications table schema is as expected
        QSqlTableModel notificationsTableModel(0, *database);
        notificationsTableModel.setTable("notifications");
        recreateNotificationsTable = (notificationsTableModel.fieldIndex("id") == -1 ||
                                      notificationsTableModel.fieldIndex("app_name") == -1 ||
                                      notificationsTableModel.fieldIndex("app_icon") == -1 ||
                                      notificationsTableModel.fieldIndex("summary") == -1 ||
                                      notificationsTableModel.fieldIndex("body") == -1 ||
                                      notificationsTableModel.fieldIndex("expire_timeout") == -1);

        // Check that the actions table schema is as expected
        QSqlTableModel actionsTableModel(0, *database);
        actionsTableModel.setTable("actions");
        recreateActionsTable = (actionsTableModel.fieldIndex("id") == -1 ||
                                actionsTableModel.fieldIndex("action") == -1);

        // Check that the hints table schema is as expected
        QSqlTableModel hintsTableModel(0, *database);
        hintsTableModel.setTable("hints");
        recreateHintsTable = (hintsTableModel.fieldIndex("id") == -1 ||
                              hintsTableModel.fieldIndex("hint") == -1 ||
                              hintsTableModel.fieldIndex("value") == -1);

        // Check that the calls table schema is as expected
        QSqlTableModel callsTableModel(0, *database);
        callsTableModel.setTable("calls");
        recreateCallsTable = (callsTableModel.fieldIndex("origin") == -1);
        cout << "NotificationManager::checkTableValidity() recreateCallsTable=" << recreateCallsTable  << endl;

        // Check that the calls table schema is as expected
        QSqlTableModel smsTableModel(0, *database);
        smsTableModel.setTable("SMSs");
        recreateSMSTable = (smsTableModel.fieldIndex("origin") == -1);


    }

    if (recreateNotificationsTable) {
        result &= recreateTable("notifications", "id INTEGER PRIMARY KEY, app_name TEXT, app_icon TEXT, summary TEXT, body TEXT, expire_timeout INTEGER");
    }

    if (recreateActionsTable) {
        result &= recreateTable("actions", "id INTEGER, action TEXT, PRIMARY KEY(id, action)");
    }

    if (recreateHintsTable) {
        result &= recreateTable("hints", "id INTEGER, hint TEXT, value TEXT, PRIMARY KEY(id, hint)");
    }

    if(recreateCallsTable) {
        result &= recreateTable("calls", "origin TEXT");
    }

    if(recreateSMSTable) {
        result &= recreateTable("SMSs", "origin TEXT");
    }

    return result;
}

bool NotificationManager::recreateTable(const QString &tableName, const QString &definition)
{
    cout << "NotificationManager::recreateTable" << endl;
    bool result = false;

    if (database->isOpen()) {
        QSqlQuery(*database).exec("DROP TABLE " + tableName);
        result = QSqlQuery(*database).exec("CREATE TABLE " + tableName + " (" + definition + ")");
    }

    cout << "NotificationManager::recreateTable result=" << result << endl;
    return result;
}

void NotificationManager::fetchData()
{
    // Gather actions for each notification
    QSqlQuery actionsQuery("SELECT * FROM actions", *database);
    QSqlRecord actionsRecord = actionsQuery.record();
    int actionsTableIdFieldIndex = actionsRecord.indexOf("id");
    int actionsTableActionFieldIndex = actionsRecord.indexOf("action");
    QHash<uint, QStringList> actions;
    while (actionsQuery.next()) {
        uint id = actionsQuery.value(actionsTableIdFieldIndex).toUInt();
        actions[id].append(actionsQuery.value(actionsTableActionFieldIndex).toString());
    }

    // Gather hints for each notification
    QSqlQuery hintsQuery("SELECT * FROM hints", *database);
    QSqlRecord hintsRecord = hintsQuery.record();
    int hintsTableIdFieldIndex = hintsRecord.indexOf("id");
    int hintsTableHintFieldIndex = hintsRecord.indexOf("hint");
    int hintsTableValueFieldIndex = hintsRecord.indexOf("value");
    QHash<uint, QVariantHash> hints;
    while (hintsQuery.next()) {
        uint id = hintsQuery.value(hintsTableIdFieldIndex).toUInt();
        hints[id].insert(hintsQuery.value(hintsTableHintFieldIndex).toString(), hintsQuery.value(hintsTableValueFieldIndex));
    }

    // Create the notifications
    QSqlQuery notificationsQuery("SELECT * FROM notifications", *database);
    QSqlRecord notificationsRecord = notificationsQuery.record();
    int notificationsTableIdFieldIndex = notificationsRecord.indexOf("id");
    int notificationsTableAppNameFieldIndex = notificationsRecord.indexOf("app_name");
    int notificationsTableAppIconFieldIndex = notificationsRecord.indexOf("app_icon");
    int notificationsTableSummaryFieldIndex = notificationsRecord.indexOf("summary");
    int notificationsTableBodyFieldIndex = notificationsRecord.indexOf("body");
    int notificationsTableExpireTimeoutFieldIndex = notificationsRecord.indexOf("expire_timeout");
    while (notificationsQuery.next()) {
        uint id = notificationsQuery.value(notificationsTableIdFieldIndex).toUInt();
        QString appName = notificationsQuery.value(notificationsTableAppNameFieldIndex).toString();
        QString appIcon = notificationsQuery.value(notificationsTableAppIconFieldIndex).toString();
        QString summary = notificationsQuery.value(notificationsTableSummaryFieldIndex).toString();
        QString body = notificationsQuery.value(notificationsTableBodyFieldIndex).toString();
        int expireTimeout = notificationsQuery.value(notificationsTableExpireTimeoutFieldIndex).toInt();
        Notification *notification = new Notification(appName, id, appIcon, summary, body, actions[id], hints[id], expireTimeout, this);
        connect(notification, SIGNAL(actionInvoked(QString)), this, SLOT(invokeAction(QString)));
        notifications.insert(id, notification);

        NOTIFICATIONS_DEBUG("RESTORED:" << appName << appIcon << summary << body << actions[id] << hints[id] << expireTimeout << "->" << id);
        emit notificationModified(id);

        if (id > previousNotificationID) {
            // Use the highest notification ID found as the previous notification ID
            previousNotificationID = id;
        }
    }

    QSqlQuery callsQuery("SELECT * FROM calls", *database);
    QSqlRecord callsRecord = callsQuery.record();
    int callsTableOriginFieldIndex = callsRecord.indexOf("origin");

    while (callsQuery.next()) {
        QString origin = callsQuery.value(callsTableOriginFieldIndex).toString();
        Call *call = new Call(origin, this);
        calls.append(call);
        NOTIFICATIONS_DEBUG("CALL RESTORED:" << origin << "->" << id);
    }
    cout << "NotificationManager::fetchData() calls: " << calls.size() << endl;
    emit callsModified();

    QSqlQuery smsQuery("SELECT * FROM SMSs", *database);
    QSqlRecord smsRecord = smsQuery.record();
    int smsTableOriginFieldIndex = smsRecord.indexOf("origin");

    while (smsQuery.next()) {
        QString origin = smsQuery.value(smsTableOriginFieldIndex).toString();
        SMS *sms = new SMS(origin, this);
        SMSs.append(sms);
        NOTIFICATIONS_DEBUG("SMS RESTORED:" << origin << "->" << id);
    }

    cout << "NotificationManager::fetchData() SMSs: " << SMSs.size() << endl;

    emit smsModified();


}

void NotificationManager::commit()
{
    // Any aditional rules about when database commits are allowed can be added here
    if (!committed) {
        database->commit();
        committed = true;
    }

    qDeleteAll(removedNotifications);
    removedNotifications.clear();
}

void NotificationManager::execSQL(const QString &command, const QVariantList &args)
{
    if (!database->isOpen()) {
        return;
    }

    if (committed) {
        committed = false;
        database->transaction();
    }

    QSqlQuery query(*database);
    query.prepare(command);

    foreach(const QVariant &arg, args) {
        query.addBindValue(arg);
    }

    query.exec();

    if (query.lastError().isValid()) {
        NOTIFICATIONS_DEBUG(command << args << query.lastError());
    }

    databaseCommitTimer.start();
}

void NotificationManager::invokeAction(const QString &action)
{
    Notification *notification = qobject_cast<Notification *>(sender());
    if (notification != 0) {
        uint id = notifications.key(notification, 0);
        if (id > 0) {
            QString remoteAction = notification->hints().value(QString(HINT_REMOTE_ACTION_PREFIX) + action).toString();
            if (!remoteAction.isEmpty()) {
                NOTIFICATIONS_DEBUG("INVOKE REMOTE ACTION:" << action << id);

                // If a remote action has been defined for the given action, trigger it
                MRemoteAction(remoteAction).trigger();
            }

            for (int actionIndex = 0; actionIndex < notification->actions().count() / 2; actionIndex++) {
                // Actions are sent over as a list of pairs. Each even element in the list (starting at index 0) represents the identifier for the action. Each odd element in the list is the localized string that will be displayed to the user.
                if (notification->actions().at(actionIndex * 2) == action) {
                    NOTIFICATIONS_DEBUG("INVOKE ACTION:" << action << id);

                    emit ActionInvoked(id, action);
                }
            }

            removeNotificationIfUserRemovable(id);
        }
    }
}

void NotificationManager::removeNotificationIfUserRemovable(uint id)
{
    Notification *notification = notifications[id];
    QVariant userRemovable = notification->hints().value(HINT_USER_REMOVABLE);
    if (!userRemovable.isValid() || userRemovable.toBool()) {
        // The notification should be removed if user removability is not defined (defaults to true) or is set to true
        QVariant userCloseable = notification->hints().value(HINT_USER_CLOSEABLE);
        if (!userCloseable.isValid() || userCloseable.toBool()) {
            // The notification should be closed if user closeability is not defined (defaults to true) or is set to true
            CloseNotification(id, NotificationDismissedByUser);
        } else {
            // Uncloseable notifications should be only removed
            emit notificationRemoved(id);

            // Mark the notification as hidden
            execSQL("INSERT INTO hints VALUES (?, ?, ?)", QVariantList() << id << HINT_HIDDEN << true);
        }
    }
}

void NotificationManager::removeUserRemovableNotifications()
{
    foreach(uint id, notifications.keys()) {
        removeNotificationIfUserRemovable(id);
    }
}


QList<Call*> NotificationManager::getCalls()
{
    QList<Call*> result(calls);
    return result;
}

QList<SMS*> NotificationManager::getSMSs()
{
    QList<SMS*> result(SMSs);
    return result;
}

void NotificationManager::clearCalls()
{
    calls.clear();
    execSQL(QString("DELETE FROM calls"), QVariantList());
    emit callsModified();
}

void NotificationManager::clearSMSs()
{
    SMSs.clear();
    execSQL(QString("DELETE FROM SMSs"), QVariantList());
    emit smsModified();
}

void NotificationManager::reloadFeedTemplates()
{
    emit feedTemplatesModified();
}
