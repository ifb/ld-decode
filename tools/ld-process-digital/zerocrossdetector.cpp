/************************************************************************

    zerocrossdetector.cpp

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

#include "zerocrossdetector.h"

ZeroCrossDetector::ZeroCrossDetector()
{
    // Default ZC detector state.
    // In order to hold state over buffer read boundaries, we keep
    // global track of the direction and delta information.
    zcPreviousInput = 0;
    delta = 0;

    zeroCrossings.clear();
}

// This method performs interpolated zero-crossing detection and stores the
// result a sample deltas (the number of samples between each
// zero-crossing).  Interpolation of the zero-crossing point provides a
// result with sub-sample resolution.
//
// Since the EFM data is NRZ-I (non-return to zero inverted) the polarity of the input
// signal is not important (only the frequency); therefore we can simply
// store the delta information.  The resulting delta information is fed to the
// phase-locked loop which is responsible for correcting jitter errors from the ZC
// detection process.
QVector<double> ZeroCrossDetector::process(QByteArray buffer)
{
    // Clear the edge buffer
    zeroCrossings.clear();

    // Input data is really qint16 wrapped in a byte array
    const qint16 *inputBuffer = reinterpret_cast<const qint16 *>(buffer.data());

    for (qint32 i = 0; i < (buffer.size() / 2); i++) {
        qint16 vPrev = zcPreviousInput;
        qint16 vCurr = inputBuffer[i];

        // Have we seen a zero-crossing?
        if ((vPrev < 0 && vCurr >= 0) || (vPrev >= 0 && vCurr < 0)) {
            // Interpolate to get the ZC sub-sample position fraction
            qreal prev = static_cast<qreal>(vPrev);
            qreal curr = static_cast<qreal>(vCurr);
            qreal fraction = (-prev) / (curr - prev);

            // Feed the sub-sample accurate result to the output
            zeroCrossings.append(delta + fraction);

            // Offset the next delta by the fractional part of the current result
            // in order to maintain accuracy
            delta = 1.0 - fraction;
        } else {
            // No ZC, increase delta by 1 sample
            delta += 1.0;
        }

        // Keep the previous input (so we can work across buffer boundaries)
        zcPreviousInput = inputBuffer[i];
    }

    return zeroCrossings;
}
