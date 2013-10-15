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

#include "applicationmanager.h"
#include "application.h"


Application::Application(MDesktopEntry *desktopEntry, QObject *parent) :
    QObject(parent) {
    qDebug() << Q_FUNC_INFO << "Application::Application() desktopEntry{ path=" << desktopEntry->path() << " fileName=" << desktopEntry->fileName() << "}";
    desktopEntry_ = desktopEntry;
}

Application::Application(QObject *parent) :
    QObject(parent)
{
    desktopEntry_ = NULL;
}

Application::Application(const Application &application) :
    QObject(application.parent())
{
    desktopEntry_ = application.desktopEntry_;
}

QString Application::filePath() const
{
    return desktopEntry_->fileName();
}

QString Application::appIcon() const
{
    return  desktopEntry_->icon();
}

QString Application::exec() const
{
    return  desktopEntry_->exec();
}

MDesktopEntry* Application::getDesktopEntry()
{
    return  desktopEntry_;
}
