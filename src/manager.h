#ifndef MANAGER_H
#define MANAGER_H

#include <QAudioOutput>
#include <QDialog>

#include "audio.h"
#include "qs1r.h"


QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QAction;
class QLabel;
class QMenu;
class QMenuBar;
class QDialogButtonBox;
class QGroupBox;
class QCheckBox;
class QComboBox;
class QSpinBox;
class QPushButton;
class QFrame;
class QStatusBar;
class QLineEdit;
QT_END_NAMESPACE



class Manager : public QDialog
{
    Q_OBJECT
public:
    Manager(QWidget *parent = 0, Qt::WFlags f = 0);
    virtual ~Manager();

    char *tmpBufferQs1r;

private:
    void createMenu();
    void createConfigGroup();
    void createCheckGroup();
    void createDeviceGroup();
    void createFrequencyGroup();
    void createButtonGroup();
    void createSamplingRate();
    void createStatusGroup();
    void createDebugBox();

    enum { NumButtons = 7 };

    ExtractorExchange  *m_extractorExchange;

    ExtractorAudio     *m_extractorAudio[NumButtons];
    QAudioDeviceInfo   *deviceInfo[NumButtons];
    QAudioOutput       *m_audioOutput[NumButtons];

    // GUI
    QVBoxLayout *mainLayout;

    QLabel      *textSamplingRate;
    QMenuBar    *menuBar;
    QMenu       *fileMenu;
    QAction     *exitAction;
    QPushButton *startStopButton;
    QPushButton *quitButton;

    QFrame *configGroup;
    QFrame *checkGroup;
    QFrame *deviceGroup;
    QFrame *frequencyGroup;
    QFrame *buttonGroup;
    QFrame *statusGroup;

    QCheckBox *check[NumButtons];
    QComboBox *device[NumButtons];
    QSpinBox  *frequency[NumButtons];
    QLineEdit *status[NumButtons];

    QComboBox *samplingRate;

    QLineEdit *globalStatus;


private slots:
    void systemStart();
    void systemStop();
    void notified();
    void toggleSuspendResume();
    void stateChanged(QAudio::State state);
};
#endif
