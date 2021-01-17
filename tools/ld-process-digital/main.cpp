/************************************************************************

    main.cpp

    ld-process-digital - Digital signal processing
    Copyright (C) 2021 Simon Inns

    This file is part of ld-decode-tools.

    ld-process-digital is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

************************************************************************/

#include <QCoreApplication>
#include <QDebug>
#include <QtGlobal>
#include <QCommandLineParser>
#include <QThread>
#include <QFile>
#include <QFileInfo>

#include "logging.h"
#include "lddecodemetadata.h"
#include "sourcevideo.h"

#include "digitalprocess.h"

int main(int argc, char *argv[])
{
    // Install the local debug message handler
    setDebug(true);
    qInstallMessageHandler(debugOutputHandler);

    QCoreApplication a(argc, argv);

    // Set application name and version
    QCoreApplication::setApplicationName("ld-process-digital");
    QCoreApplication::setApplicationVersion(QString("Branch: %1 / Commit: %2").arg(APP_BRANCH, APP_COMMIT));
    QCoreApplication::setOrganizationDomain("domesday86.com");

    // Set up the command line parser ---------------------------------------------------------------------------------
    QCommandLineParser parser;
    parser.setApplicationDescription(
                "ld-process-digital - Digital signal processing\n"
                "\n"
                "(c)2021 Simon Inns\n"
                "GPLv3 Open-Source - github: https://github.com/happycube/ld-decode");
    parser.addHelpOption();
    parser.addVersionOption();

    // Add the standard debug options --debug and --quiet
    addStandardDebugOptions(parser);

    // Option to select the number of threads (-t)
    QCommandLineOption threadsOption(QStringList() << "t" << "threads",
                                        QCoreApplication::translate("main", "Specify the number of concurrent threads (default is the number of logical CPUs)"),
                                        QCoreApplication::translate("main", "number"));
    parser.addOption(threadsOption);

    // Positional argument to specify input digital file
    parser.addPositionalArgument("input", QCoreApplication::translate("main", "Specify input digital file"));

    // Positional argument to specify output digital file
    parser.addPositionalArgument("output", QCoreApplication::translate("main", "Specify output digital file"));

    // Process the command line options and arguments given by the user
    parser.process(a);

    // Standard logging options
    processStandardDebugOptions(parser);

    // Get the arguments from the parser
    QString inputFilename;
    QString outputFilename;
    QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.count() == 2) {
        inputFilename = positionalArguments.at(0);
        outputFilename = positionalArguments.at(1);
    } else {
        // Quit with error
        qCritical("You must specify an input and output digital file");
        return -1;
    }

    // Perform the processing
    qInfo() << "Beginning sample to EFM signal processing...";

    DigitalProcess digitalProcess;
    if (!digitalProcess.process(inputFilename, outputFilename)) return 1;

    // Quit with success
    return 0;
}
