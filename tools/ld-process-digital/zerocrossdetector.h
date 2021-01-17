/************************************************************************

    zerocrossdetector.h

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

#ifndef ZEROCROSSDETECTOR_H
#define ZEROCROSSDETECTOR_H

#include <QCoreApplication>
#include <QDebug>

class ZeroCrossDetector
{
public:
    ZeroCrossDetector();

    QVector<double> process(QByteArray buffer);

private:
    // ZC detector state
    qint16 zcPreviousInput;
    qreal delta;

    QVector<double> zeroCrossings;
};

#endif // ZEROCROSSDETECTOR_H
