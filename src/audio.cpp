#include <QtCore/qmath.h>
#include <QtCore/qendian.h>
#include <QDebug>

#include "audio.h"


ExtractorAudio::ExtractorAudio(QObject *parent, unsigned char *lBufferQs1r, qint64 lSeq): QIODevice(parent) {
    seq = lSeq;
    bufferQs1r = lBufferQs1r;
}

ExtractorAudio::~ExtractorAudio() { }


void ExtractorAudio::start() {
    open(QIODevice::ReadOnly);
}


void ExtractorAudio::stop() {
    close();
}


qint64 ExtractorAudio::readData(char *data, qint64 len) {
    qint16 *ptrAudio  = reinterpret_cast<qint16 *>(data);

    for(int i=0; i<(len / sizeof(qint16)); i += sizeof(qint16)) {
        ptrAudio[i+1] = queueAudio.dequeue();
        ptrAudio[i]   = queueAudio.dequeue();
    }
    return len;
}


qint64 ExtractorAudio::writeData(const char *data, qint64 len) {
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}


qint64 ExtractorAudio::bytesAvailable() const {
    return QIODevice::bytesAvailable();
}


void ExtractorAudio::enqueueBuffer(int size) {
    quint32 *ptrQs1r  = reinterpret_cast<quint32 *>(bufferQs1r);

    if (queueAudio.size() < 524288) { // FIXME : Dirty hack for buffer growing...
        for(int i=0; i<(int)(size / (8 * sizeof(qint32) * 2)); i++) {
            queueAudio.enqueue(qToLittleEndian<qint16>( ptrQs1r[i*16+(2*seq)] >> 16) );
            queueAudio.enqueue(qToLittleEndian<qint16>( ptrQs1r[i*16+(2*seq)+1] >> 16) );

            // DEBUG : Generate IQ tones for debuggin buffers
            //queueAudio.enqueue(qToLittleEndian<qint16>(qCos(2 * M_PI * 0.01 * sinIndex * (seq+1))*5000));
            //queueAudio.enqueue(qToLittleEndian<qint16>(qSin(2 * M_PI * 0.01 * sinIndex * (seq+1))*5000));
            //sinIndex++;
        }
    }
}
