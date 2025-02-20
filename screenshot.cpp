#include "screenshot.h"

// #include "screengrabber.h"
//#include "abstractlogger.h"
//#include "src/core/qguiappcurrentscreen.h"
// #include "src/utils/confighandler.h"
// #include "src/utils/filenamehandler.h"
// #include "src/utils/systemnotification.h"
#include <QApplication>
// #include <QDesktopWidget>
#include <QGuiApplication>
#include <QPixmap>
#include <QProcess>
#include <QScreen>

#include "request.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QDir>
#include <QUrl>
#include <QUuid>
Screenshot::Screenshot(QObject *parent)
    : QObject{parent}
{}


void Screenshot::newShot(QPixmap *&res)
{
    // SPDX-License-Identifier: GPL-3.0-or-later
    // SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors




    QDBusInterface screenshotInterface(
        QStringLiteral("org.freedesktop.portal.Desktop"),
        QStringLiteral("/org/freedesktop/portal/desktop"),
        QStringLiteral("org.freedesktop.portal.Screenshot"));
    // unique token
    QString token =
        QUuid::createUuid().toString().remove('-').remove('{').remove('}');
    // qDebug() << token;
    // premake interface
    auto* request = new OrgFreedesktopPortalRequestInterface(
        QStringLiteral("org.freedesktop.portal.Desktop"),
        "/org/freedesktop/portal/desktop/request/" +
            QDBusConnection::sessionBus().baseService().remove(':').replace('.',
                                                                            '_') +
            "/" + token,
        QDBusConnection::sessionBus(),
        this);

    QEventLoop loop;
    const auto gotSignal = [&res, &loop](uint status, const QVariantMap& map) {
        if (status == 0) {
            qDebug() << "get pic" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
            // Parse this as URI to handle unicode properly
            QUrl uri = map.value("uri").toString();
            QString uriString = uri.toLocalFile();
            // qDebug() << uri;
            res = new QPixmap(uriString);
            // res->setDevicePixelRatio(qApp->devicePixelRatio());
            QFile imgFile(uriString);
            imgFile.remove();
        }
        loop.quit();
    };

    // prevent racy situations and listen before calling screenshot
    QMetaObject::Connection conn = QObject::connect(
        request, &org::freedesktop::portal::Request::Response, gotSignal);
    qDebug() << "request pic" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
    screenshotInterface.call(
        QStringLiteral("Screenshot"),
        "",
        QMap<QString, QVariant>({ { "handle_token", QVariant(token) },
                                 { "interactive", QVariant(false) } }));

    loop.exec();
    QObject::disconnect(conn);
    request->Close().waitForFinished();
    request->deleteLater();
    // qDebug() << "hey";
}
