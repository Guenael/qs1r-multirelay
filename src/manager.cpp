/*
 * 7 channels audio relay, version 0.1a
 *
 * QS1R hardware & software interface : Phil a N8VB
 * FPGA verilog & misc for 7 channels : Alex, VE3NEA
 * This tiny application : Guenael, VA2GKA
 *
 * Licence:
 *  - My code : BSD (copyleft)
 *  - Other : Check for each authors/files
 *
 * Todo:
 * - Clean exit
 * - Bug fix sampling rate diff. 48kHz
 * - Save settings
 * - Clock correction
 * - Statusbar update (qs1r init status etc...)
 * - Additionnal checks
 * - Full static build
 * - Quit button
 *
 */

#include <QtGui>
#include "manager.h"
#include <windows.h>


Manager::Manager(QWidget *parent, Qt::WFlags f)
    : QDialog(parent, f)
    , m_extractorAudio()
    , m_audioOutput()
{
    mainLayout = new QVBoxLayout;

    createMenu();
    createSamplingRate();
    createCheckGroup();
    createDeviceGroup();
    createFrequencyGroup();
    createStatusGroup();
    createConfigGroup();
    createButtonGroup();
    createDebugBox();

    setFixedSize (450, 400);
    setWindowTitle(tr("QS1R 7 channels audio relay"));
    setLayout (mainLayout);
}


Manager::~Manager() { }


void Manager::systemStart() {
    startStopButton->setDisabled(false); // Lock the interface button

    // QS1R startup
    m_extractorExchange = new ExtractorExchange(this);
    m_extractorExchange->init();

    // Settings for the local audio outputs
    QAudioFormat settings;
    settings.setCodec("audio/pcm");
    settings.setFrequency(samplingRate->itemData(samplingRate->currentIndex()).value<int>()); // BUG > 48k
    settings.setChannels(2);
    settings.setSampleSize(16);
    settings.setSampleType(QAudioFormat::SignedInt);
    settings.setByteOrder(QAudioFormat::LittleEndian);

    // Set the ouput sampling rate (48000 96000 192000) on the QS1R // FIXME : bug
    m_extractorExchange->SetSR(samplingRate->currentIndex());

    for (int i = 0; i < NumButtons; ++i) {
        // Lock the selected values on the GUI
        check[i]->setDisabled(true);
        device[i]->setDisabled(true);
        frequency[i]->setDisabled(true);
        samplingRate->setDisabled(true);

        // Setup the frequency for the 7 bands
        m_extractorExchange->SetLO(i,frequency[i]->value());

        // Create & start each selected audio output
        if (check[i]->isChecked()) {
            deviceInfo[i] = new QAudioDeviceInfo();
            *deviceInfo[i] = device[i]->itemData(device[i]->currentIndex()).value<QAudioDeviceInfo>();
            if (!deviceInfo[i]->isNull()) {
                if (deviceInfo[i]->isFormatSupported(settings)) {
                    status[i]->setText("Success");

                    m_audioOutput[i] = new QAudioOutput(*deviceInfo[i], settings, this);
                    //connect(m_audioOutput[i], SIGNAL(notify()), SLOT(notified()));
                    //connect(m_audioOutput[i], SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));

                    m_extractorAudio[i] = new ExtractorAudio(this, m_extractorExchange->bufferQs1r, i);
                    connect(m_extractorExchange, SIGNAL(bufferUpdate(int)), m_extractorAudio[i], SLOT(enqueueBuffer(int)));
                }
                else {
                    status[i]->setText("Unsuppored");
                }
            }
            else {
                status[i]->setText("No Device");
            }
        }
        else {
            status[i]->setText("Disabled");
        }


    }

    /* Start processing */
    m_extractorExchange->start(QThread::TimeCriticalPriority); // QS1R get data in a separate thread
    Sleep(1000); // FIXME : Dirty hack to load the buffer...

    for (int i = 0; i < NumButtons; ++i) {
        if (check[i]->isChecked()) {
            m_extractorAudio[i]->start();
            m_audioOutput[i]->start(m_extractorAudio[i]); // Pull method
        }
    }

    // Start/Stop button switch
    startStopButton->setDisabled(false);
    startStopButton->setText(tr("Stop"));
    startStopButton->disconnect(this);
    connect(startStopButton, SIGNAL(clicked()), SLOT(systemStop()));
}


void Manager::systemStop() {
    delete m_extractorExchange; // FIXME Fix the exit/clean

    for (int i = 0; i < NumButtons; ++i) {
        if (check[i]->isChecked()) {
            delete deviceInfo[i];
            m_audioOutput[i]->stop();
            m_audioOutput[i]->disconnect(this);
            delete m_audioOutput[i];
            m_extractorAudio[i]->stop();
            delete m_extractorAudio[i];
        }

        // Unlock the GUI selections
        check[i]->setDisabled(false);
        device[i]->setDisabled(false);
        frequency[i]->setDisabled(false);
        samplingRate->setDisabled(false);
   }

   startStopButton->setText(tr("Start"));
   startStopButton->disconnect(this);
   connect(startStopButton, SIGNAL(clicked()), SLOT(systemStart()));
}


void Manager::notified() {
    for (int i = 0; i < NumButtons; ++i) {
        if (check[i]->isChecked()) {
            qWarning() << "bytesFree = " << m_audioOutput[i]->bytesFree()
                       << ", " << "elapsedUSecs = " << m_audioOutput[i]->elapsedUSecs()
                       << ", " << "processedUSecs = " << m_audioOutput[i]->processedUSecs();
        }
    }
}


void Manager::stateChanged(QAudio::State state) {
    qWarning() << "state = " << state;
}


void Manager::toggleSuspendResume() {
    // FIXME Rewrite manager without delete / with status

    for (int i = 0; i < NumButtons; ++i) {
        if (check[i]->isChecked()) {
            if (m_audioOutput[i]->state() == QAudio::SuspendedState) {
                qWarning() << "status: Suspended, resume()";
                m_audioOutput[i]->resume();
                //m_suspendResumeButton->setText(SuspendLabel);
            } else if (m_audioOutput[i]->state() == QAudio::ActiveState) {
                qWarning() << "status: Active, suspend()";
                m_audioOutput[i]->suspend();
                //m_suspendResumeButton->setText(ResumeLabel);
            } else if (m_audioOutput[i]->state() == QAudio::StoppedState) {
                qWarning() << "status: Stopped, resume()";
                m_audioOutput[i]->resume();
                //m_suspendResumeButton->setText(SuspendLabel);
            } else if (m_audioOutput[i]->state() == QAudio::IdleState) {
                qWarning() << "status: IdleState";
            }
        }
    }
}


/*
 * GUI Part
 *
 */

void Manager::createMenu() {
    menuBar = new QMenuBar;

    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
    mainLayout->setMenuBar(menuBar);
}


void Manager::createConfigGroup() {
    configGroup = new QFrame();
    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(checkGroup);
    layout->addWidget(deviceGroup);
    layout->addWidget(frequencyGroup);
    layout->addWidget(statusGroup);
    configGroup->setLayout(layout);

    mainLayout->addWidget(configGroup);
}


void Manager::createCheckGroup() {
    checkGroup = new QFrame();
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *lText = new QLabel();
    lText->setText("Enable :");
    layout->addWidget(lText);

    for (int i = 0; i < NumButtons; ++i) {
        check[i] = new QCheckBox();
        check[i]->setText(tr("Channel %1").arg(i + 1));
        layout->addWidget(check[i]);
        layout->addStretch(1);
    }
    check[0]->setChecked(true);
    check[1]->setChecked(true);
    check[2]->setChecked(true);
    check[3]->setChecked(true);
    check[4]->setChecked(true);
    check[5]->setChecked(true);
    check[6]->setChecked(true);
    checkGroup->setLayout(layout);
}


void Manager::createDeviceGroup() {
    deviceGroup = new QFrame();
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *lText = new QLabel();
    lText->setText("Device :");
    layout->addWidget(lText);

    for (int i = 0; i < NumButtons; ++i) {
        device[i] = new QComboBox();
        layout->addWidget(device[i]);
        layout->addStretch(1);
    }
    deviceGroup->setLayout(layout);

    for (int i = 0; i < NumButtons; ++i) {
        foreach (const QAudioDeviceInfo deviceInfoTmp, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
           device[i]->addItem(deviceInfoTmp.deviceName(), qVariantFromValue(deviceInfoTmp));

        // Default value (incremental)
        device[i]->setCurrentIndex(i);
    }
}


void Manager::createFrequencyGroup() {
    frequencyGroup = new QFrame();
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *lText = new QLabel();
    lText->setText("Frequency :");
    layout->addWidget(lText);

    for (int i = 0; i < NumButtons; ++i) {
        frequency[i] = new QSpinBox();
        frequency[i]->setRange(15000,55000000);
        layout->addWidget(frequency[i]);
        layout->addStretch(1);
    }

    // Default values for usual bands
    frequency[0]->setValue(7000000);
    frequency[1]->setValue(10135000);
    frequency[2]->setValue(14000000);
    frequency[3]->setValue(18068000);
    frequency[4]->setValue(21000000);
    frequency[5]->setValue(24890000);
    frequency[6]->setValue(28000000);
    frequencyGroup->setLayout(layout);
}


void Manager::createStatusGroup() {
    statusGroup = new QFrame();
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *lText = new QLabel();
    lText->setText("Status :");
    layout->addWidget(lText);

    for (int i = 0; i < NumButtons; ++i) {
        status[i] = new QLineEdit();
        status[i]->setText("Waiting");
        status[i]->setReadOnly(true);
        status[i]->setEnabled(false);
        layout->addWidget(status[i]);
        layout->addStretch(1);
    }
    statusGroup->setLayout(layout);
}


void Manager::createSamplingRate() {
    textSamplingRate = new QLabel();
    textSamplingRate->setText("Sampling rate :");
    mainLayout->addWidget(textSamplingRate);

    samplingRate = new QComboBox();
    samplingRate->setMaximumWidth(100);
    samplingRate->addItem("48000", "48000");
    samplingRate->addItem("96000", "96000");
    samplingRate->addItem("192000", "192000");

    mainLayout->addWidget(samplingRate);
}


void Manager::createButtonGroup() {
    buttonGroup = new QFrame();
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignLeft);

    startStopButton = new QPushButton(tr("Start"));
    startStopButton->setFixedWidth(50);

    //quitButton = new QPushButton(tr("Quit"));
    //quitButton->setFixedWidth(50);

    layout->addWidget(startStopButton);
    //layout->addWidget(quitButton);
    buttonGroup->setLayout(layout);

    mainLayout->addWidget(buttonGroup);

    connect(startStopButton, SIGNAL(clicked()), SLOT(systemStart()));
    //connect(quitButton, SIGNAL(rejected()), this, SLOT(reject()));
}


void Manager::createDebugBox() {
    globalStatus = new QLineEdit();
    globalStatus->setText("Waiting");
    globalStatus->setReadOnly(true);
    globalStatus->setEnabled(false);
    mainLayout->addWidget(globalStatus);
}
