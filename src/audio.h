#ifndef AUDIO_H
#define AUDIO_H

#include <QIODevice>
#include <QQueue>

class ExtractorAudio : public QIODevice
{
    Q_OBJECT
public:
    ExtractorAudio(QObject *parent, unsigned char *bufferQs1r, qint64 seq);
    ~ExtractorAudio();

    void start();
    void stop();
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;

private:
    unsigned char *bufferQs1r;
    QQueue<qint16> queueAudio;
    qint64 seq;
    qint64 sinIndex; // Used for debug

private slots:
    void enqueueBuffer(int);
};
#endif
