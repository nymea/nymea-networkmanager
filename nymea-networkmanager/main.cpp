/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 * Copyright (C) 2018 Simon Stürz <simon.stuerz@guh.io>                          *
 *                                                                               *
 * This file is part of nymea-networkmanager.                                    *
 *                                                                               *
 * nymea-networkmanager is free software: you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License,                *
 * or (at your option) any later version.                                        *
 *                                                                               *
 * nymea-networkmanager is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  *
 * GNU General Public License for more details.                                  *
 *                                                                               *
 * You should have received a copy of the GNU General Public License along       *
 * with nymea-networkmanager. If not, see <http://www.gnu.org/licenses/>.        *
 *                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "core.h"
#include "application.h"
#include "loggingcategories.h"

static const char *const normal = "\033[0m";
static const char *const warning = "\e[33m";
static const char *const error = "\e[31m";

static QHash<QString, bool> s_loggingFilters;

static void loggingCategoryFilter(QLoggingCategory *category)
{
    // If this is a known category
    if (s_loggingFilters.contains(category->categoryName())) {
        category->setEnabled(QtDebugMsg, s_loggingFilters.value(category->categoryName()));
        category->setEnabled(QtWarningMsg, true);
        category->setEnabled(QtCriticalMsg, true);
        category->setEnabled(QtFatalMsg, true);
    } else {
        //Disable default debug messages, print only >= warnings
        category->setEnabled(QtDebugMsg, false);
        category->setEnabled(QtWarningMsg, true);
        category->setEnabled(QtCriticalMsg, true);
        category->setEnabled(QtFatalMsg, true);
    }
}

static void consoleLogHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    switch (type) {
    case QtInfoMsg:
        fprintf(stdout, " I | %s: %s\n", context.category, message.toUtf8().data());
        break;
    case QtDebugMsg:
        fprintf(stdout, " I | %s: %s\n", context.category, message.toUtf8().data());
        break;
    case QtWarningMsg:
        fprintf(stdout, "%s W | %s: %s%s\n", warning, context.category, message.toUtf8().data(), normal);
        break;
    case QtCriticalMsg:
        fprintf(stdout, "%s C | %s: %s%s\n", error, context.category, message.toUtf8().data(), normal);
        break;
    case QtFatalMsg:
        fprintf(stdout, "%s F | %s: %s%s\n", error, context.category, message.toUtf8().data(), normal);
        break;
    }
    fflush(stdout);
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(consoleLogHandler);

    Application application(argc, argv);
    application.setApplicationName("nymea-networkmanager");
    application.setOrganizationName("nymea");
    application.setApplicationVersion("0.0.2");

    // Command line parser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(QString("\nThis daemon allows to configure a wifi network using a bluetooth low energy connection.\n\nCopyright %1 2018 Simon Stürz <simon.stuerz@guh.io>").arg(QChar(0xA9)));

    QCommandLineOption debugOption(QStringList() << "d" << "debug", "Enable more debug output.");
    parser.addOption(debugOption);

    QCommandLineOption advertiseNameOption(QStringList() << "a" << "advertise-name", "The name of the bluetooth server. Default \"nymea\".", "NAME");
    advertiseNameOption.setDefaultValue("nymea");
    parser.addOption(advertiseNameOption);

    QCommandLineOption platformNameOption(QStringList() << "p" << "platform-name", "The name of the platform this daemon is running. Default \"nymea-box\".", "NAME");
    platformNameOption.setDefaultValue("nymea-box");
    parser.addOption(platformNameOption);

    parser.process(application);

    // Enable debug categories
    s_loggingFilters.insert("Application", true);
    s_loggingFilters.insert("BluetoothServer", parser.isSet(debugOption));
    s_loggingFilters.insert("NetworkManager", parser.isSet(debugOption) );
    s_loggingFilters.insert("NymeaService", parser.isSet(debugOption));

    QLoggingCategory::installFilter(loggingCategoryFilter);

    qCDebug(dcApplication()) << "=====================================";
    qCDebug(dcApplication()) << "Starting nymea-networkmanager" << application.applicationVersion();
    qCDebug(dcApplication()) << "=====================================";
    qCDebug(dcApplication()) << "Advertising name:" << parser.value(advertiseNameOption);
    qCDebug(dcApplication()) << "Platform name:" << parser.value(platformNameOption);

    // Start core
    Core::instance()->setAdvertiseName(parser.value(advertiseNameOption));
    Core::instance()->setPlatformName(parser.value(platformNameOption));
    Core::instance()->run();

    return application.exec();
}
