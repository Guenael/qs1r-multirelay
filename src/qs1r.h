#ifndef QS1R_H
#define QS1R_H

#include <QThread>

class ExtractorExchange : public QThread
{
    Q_OBJECT
public:
    ExtractorExchange(QObject *parent);
    ~ExtractorExchange();

    void init();
    void run();
    void SetLO(int channel, long freq);
    void SetSR(int value);

    unsigned char *bufferQs1r;

private:
    int sizeRead;

signals:
     void bufferUpdate(int size);

};
#endif
