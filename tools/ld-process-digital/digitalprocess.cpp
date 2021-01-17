#include "digitalprocess.h"

DigitalProcess::DigitalProcess()
{

}

bool DigitalProcess::process(QString inputFilename, QString outputFilename)
{
    // Open the input file
    if (!openInputFile(inputFilename)) {
        qCritical("Could not open input file!");
        return false;
    }

    // Open the output file
    if (!openOutputFile(outputFilename)) {
        qCritical("Could not open output file!");
        return false;
    }

    QByteArray inputSampleData;
    QByteArray outputEfmData;
    bool finished = false;
    do {
        qint32 bufferSizeInBytes = (1024 * 1024); // 1 MiB
        inputSampleData.resize(bufferSizeInBytes);

        // Fill the input buffer with data
        qint64 receivedBytes = 0;
        qint32 totalReceivedBytes = 0;
        do {
            // In practice as long as the file descriptor is blocking this will read everything in one chunk...
            receivedBytes = inputFileHandle->read(reinterpret_cast<char *>(inputSampleData.data() + totalReceivedBytes),
                                                 bufferSizeInBytes - totalReceivedBytes);
            if (receivedBytes > 0) totalReceivedBytes += receivedBytes;
        } while (receivedBytes > 0 && totalReceivedBytes < bufferSizeInBytes);

        inputSampleData.resize(totalReceivedBytes);

        if (inputSampleData.size() > 0) {
            // Perform zero cross detection
            QVector<double> zeroCrossingDeltas = zeroCrossDetector.process(inputSampleData);

            // Use zero-cross detection and a PLL to get the T values from the EFM signal
            outputEfmData = pll.process(zeroCrossingDeltas);

            // Save the resulting T values as a byte stream to the output file
            if (!outputFileHandle->write(reinterpret_cast<char *>(outputEfmData.data()), outputEfmData.size())) {
                // File write failed
                qCritical("Could not write to output file!");
                closeOutputFile();
                return false;
            }
        }
    } while (inputSampleData.size() > 0 && !finished);

    // Close the open files
    closeOutputFile();
    closeInputFile();

    qInfo() << "Processing complete";

    return true;
}

// Method to open the input file for reading
bool DigitalProcess::openInputFile(QString inputFilename)
{
    // Open the input file for reading
    inputFileHandle = new QFile(inputFilename);
    if (!inputFileHandle->open(QIODevice::ReadOnly)) {
        // Failed to open input file
        qDebug() << "Could not open " << inputFilename << "as input file";
        return false;
    }
    qDebug() << "Input file is" << inputFilename;

    // Exit with success
    return true;
}

// Method to close the input file
void DigitalProcess::closeInputFile(void)
{
    // Is an input file open?
    if (inputFileHandle != nullptr) {
        inputFileHandle->close();
    }

    // Clear the file handle pointer
    delete inputFileHandle;
    inputFileHandle = nullptr;
}

// Method to open the output file for writing
bool DigitalProcess::openOutputFile(QString outputFileName)
{
    // Open the output file for writing
    outputFileHandle = new QFile(outputFileName);
    if (!outputFileHandle->open(QIODevice::WriteOnly)) {
        // Failed to open output file
        qDebug() << "Could not open " << outputFileName << "as output file";
        return false;
    }
    qDebug() << "LdsProcess::openOutputFile(): Output file is" << outputFileName;

    // Exit with success
    return true;
}

// Method to close the output file
void DigitalProcess::closeOutputFile(void)
{
    // Is an output file open?
    if (outputFileHandle != nullptr) {
        outputFileHandle->close();
    }

    // Clear the file handle pointer
    delete outputFileHandle;
    outputFileHandle = nullptr;
}
