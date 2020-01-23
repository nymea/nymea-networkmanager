/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU General Public License as published by the Free Software
* Foundation, GNU version 3. This project is distributed in the hope that it
* will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QSettings>
#include <QStandardPaths>
#include <QFileInfo>
#include <QMetaEnum>

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

    // Default configuration:
    Core::Mode mode = Core::ModeOffline;
    int timeout = 60;
    int buttonGpio = -1;
    QString advertiseName = "BT WLAN setup";
    QString platformName = "nymea-box";

    Application application(argc, argv);
    application.setApplicationName("nymea-networkmanager");
    application.setOrganizationName("nymea");
    application.setApplicationVersion(VERSION_STRING);

    // Command line parser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(QString("\nThis daemon allows to configure a wifi network using a bluetooth low energy connection.\n\n"
                                             "Copyright %1 2018-2019 Simon Stürz <simon.stuerz@nymea.io>\n\n"
                                             "Modes: \n"
                                             "  - offline  This mode starts the bluetooth server once the device is offline\n"
                                             "             and not connected to any LAN network.\n"
                                             "  - once     This mode starts the bluetooth server only if no network configuration exists.\n"
                                             "             Once a network connection exists the server will never start again.\n"
                                             "  - button   This mode enables the bluetooth server when a GPIO button has been pressed for\n"
                                             "             the configured timeout periode.\n"
                                             "  - always   This mode enables the bluetooth server as long the application is running.\n"
                                             "  - start    This mode starts the bluetooth server for 3 minutes on start and shuts down after a connection.\n\n").arg(QChar(0xA9)));

    QCommandLineOption debugOption(QStringList() << "d" << "debug", "Enable more debug output.");
    parser.addOption(debugOption);

    QCommandLineOption advertiseNameOption(QStringList() << "a" << "advertise-name", QString("The name of the bluetooth server. Default \"%1\".").arg(advertiseName), "NAME");
    advertiseNameOption.setDefaultValue(advertiseName);
    parser.addOption(advertiseNameOption);

    QCommandLineOption platformNameOption(QStringList() << "p" << "platform-name", QString("The name of the platform this daemon is running. Default \"%1\".").arg(platformName), "NAME");
    platformNameOption.setDefaultValue(platformName);
    parser.addOption(platformNameOption);

    QCommandLineOption gpioOption(QStringList() << "g" << "gpio", QString("The GPIO sysfs number for the button GPIO. This parameter is only needed for the \"button\" mode."), "GPIO");
    platformNameOption.setDefaultValue("-1");
    parser.addOption(gpioOption);

    QCommandLineOption timeoutOption(QStringList() << "t" << "timeout", QString("The timeout of the bluetooth server. Minimum value is 10. Default \"%1\".").arg(timeout), "SECONDS");
    timeoutOption.setDefaultValue(QString::number(timeout));
    parser.addOption(timeoutOption);

    QCommandLineOption modeOption(QStringList() << "m" << "mode", "Run the daemon in a specific mode (offline, once, always, button, start). Default is \"offline\".", "MODE");
    parser.addOption(modeOption);

    parser.process(application);

    // Enable debug categories
    s_loggingFilters.insert("Application", true);
    s_loggingFilters.insert("NymeaService", parser.isSet(debugOption));
    s_loggingFilters.insert("NetworkManager", parser.isSet(debugOption) );
    s_loggingFilters.insert("NetworkManagerBluetoothServer", parser.isSet(debugOption));

    QLoggingCategory::installFilter(loggingCategoryFilter);

    bool timeoutValueOk = true;
    bool gpioValueOk = true;

    // Now read the cofig file, overriding defaults
    QStringList configLocations;
    configLocations << QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
    configLocations << "/etc";
    QString fileName = "/nymea/nymea-networkmanager.conf";
    foreach (const QString &configLocation, configLocations) {
        if (QFileInfo::exists(configLocation + fileName)) {
            qCDebug(dcApplication) << "Using configuration file from:" << configLocation + fileName;
            QSettings settings(configLocation + fileName, QSettings::IniFormat);

            if (settings.contains("Mode")) {
                if (settings.value("Mode").toString().toLower() == "offline") {
                    mode = Core::ModeOffline;
                } else if (settings.value("Mode").toString().toLower() == "always") {
                    mode = Core::ModeAlways;
                } else if (settings.value("Mode").toString().toLower() == "start") {
                    mode = Core::ModeStart;
                } else if (settings.value("Mode").toString().toLower() == "once") {
                    mode = Core::ModeOnce;
                } else if (settings.value("Mode").toString().toLower() == "button") {
                    mode = Core::ModeButton;
                } else {
                    qCWarning(dcApplication()).noquote() << QString("The config file's mode \"%1\" does not match the allowed modes.").arg(settings.value("Mode").toString());
                }
            }
            if (settings.contains("ButtonGpio")) {
                buttonGpio = settings.value("ButtonGpio", -1).toInt(&gpioValueOk);
            }
            if (settings.contains("Timeout")) {
                timeout = settings.value("Timeout").toInt(&timeoutValueOk);
            }
            if (settings.contains("AdvertiseName")) {
                advertiseName = settings.value("AdvertiseName").toString();
            }
            if (settings.contains("PlatformName")) {
                platformName = settings.value("PlatformName").toString();
            }
            break;
        }
    }

    // Now parse command line
    if (parser.isSet(modeOption)) {
        if (parser.value(modeOption).toLower() == "offline") {
            mode = Core::ModeOffline;
        } else if (parser.value(modeOption).toLower() == "always") {
            mode = Core::ModeAlways;
        } else if (parser.value(modeOption).toLower() == "start") {
            mode = Core::ModeStart;
        } else if (parser.value(modeOption).toLower() == "once") {
            mode = Core::ModeOnce;
        } else if (parser.value(modeOption).toLower() == "button") {
            mode = Core::ModeButton;

        }  else {
            qCWarning(dcApplication()).noquote() << QString("The given mode \"%1\" does not match the allowed modes.").arg(parser.value(modeOption));
            parser.showHelp(1);
        }
    }
    if (parser.isSet(advertiseNameOption)) {
        advertiseName = parser.value(advertiseNameOption);
    }
    if (parser.isSet(platformNameOption)) {
        platformName = parser.value(platformNameOption);
    }
    if (parser.isSet(timeoutOption)) {
        timeout = parser.value(timeoutOption).toInt(&timeoutValueOk);
    }
    if (parser.isSet(gpioOption)) {
        buttonGpio = parser.value(gpioOption).toInt(&gpioValueOk);
    }

    // All parsed. Validate input:
    if (!timeoutValueOk) {
        qCCritical(dcApplication()) << QString("Invalid timeout value passed: \"%1\". Please pass an integer >= 10").arg(parser.value(timeoutOption));
        return(1);
    }
    if (!gpioValueOk) {
        qCCritical(dcApplication()) << QString("Invalid GPIO number value passed: \"%1\". Please pass an integer > 0").arg(parser.value(gpioOption));
        return(1);
    }
    if (timeout < 10) {
        qCCritical(dcApplication()) << QString("Invalid timeout value passed: \"%1\". The minimal timeout is 10 [s].").arg(parser.value(timeoutOption));
        return(1);
    }

    if (mode == Core::ModeButton && buttonGpio <= 0) {
        qCCritical(dcApplication()) << "Button mode selected but no valid GPIO passed.";
        return(1);
    }

    qCDebug(dcApplication()) << "=====================================";
    qCDebug(dcApplication()) << "Starting nymea-networkmanager" << application.applicationVersion();
    qCDebug(dcApplication()) << "=====================================";
    qCDebug(dcApplication()) << "Advertising name:" << advertiseName;
    qCDebug(dcApplication()) << "Platform name:" << platformName;
    qCDebug(dcApplication()) << "Mode:" << mode;
    qCDebug(dcApplication()) << "Timeout:" << timeout;
    if (mode == Core::ModeButton)
        qCDebug(dcApplication()) << "Button GPIO:" << buttonGpio;

    // Start core
    Core::instance()->setMode(mode);
    Core::instance()->setAdvertisingTimeout(timeout);
    Core::instance()->setAdvertiseName(advertiseName);
    Core::instance()->setPlatformName(platformName);
    Core::instance()->setButtonGpio(buttonGpio);

    Core::instance()->run();

    return application.exec();
}
