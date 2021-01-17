#ifndef DIGITALPROCESS_H
#define DIGITALPROCESS_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QDebug>

#include "zerocrossdetector.h"
#include "pll.h"

class DigitalProcess
{
public:
    DigitalProcess();

    bool process(QString inputFilename, QString outputFilename);

private:
    QFile *inputFileHandle;
    QFile *outputFileHandle;
    ZeroCrossDetector zeroCrossDetector;
    Pll pll;

    bool openInputFile(QString inputFilename);
    void closeInputFile(void);

    bool openOutputFile(QString outputFileName);
    void closeOutputFile(void);
};

#endif // DIGITALPROCESS_H
