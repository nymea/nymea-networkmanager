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

#include "application.h"
#include "loggingcategories.h"
#include "core.h"

#include <signal.h>

static bool s_aboutToShutdown = false;

static void catchUnixSignals(const std::vector<int>& quitSignals, const std::vector<int>& ignoreSignals = std::vector<int>())
{
    auto handler = [](int sig) ->void {
        switch (sig) {
        case SIGQUIT:
            qCDebug(dcApplication()) << "Cought SIGQUIT quit signal...";
            break;
        case SIGINT:
            qCDebug(dcApplication()) << "Cought SIGINT quit signal...";
            break;
        case SIGTERM:
            qCDebug(dcApplication()) << "Cought SIGTERM quit signal...";
            break;
        case SIGHUP:
            qCDebug(dcApplication()) << "Cought SIGHUP quit signal...";
            break;
        case SIGSEGV: {
            qCCritical(dcApplication()) << "Cought SIGSEGV signal. Segmentation fault!";
            exit(EXIT_FAILURE);
        }
        default:
            break;
        }

        if (s_aboutToShutdown) {
            qCCritical(dcApplication()) << "Already shutting down.";
            return;
        }

        qCDebug(dcApplication()) << "=====================================";
        qCDebug(dcApplication()) << "Shutting down nymea-networkmanager";
        qCDebug(dcApplication()) << "=====================================";
        s_aboutToShutdown = true;

        Core::instance()->destroy();
        Application::quit();
    };

    // all these signals will be ignored.
    for (int sig : ignoreSignals)
        signal(sig, SIG_IGN);

    for (int sig : quitSignals)
        signal(sig, handler);

}

Application::Application(int &argc, char **argv) :
    QCoreApplication(argc, argv)
{
    catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP, SIGSEGV});
}
