#include <QtGui>
#include "dialog.h"

/* Todo
  Label status
  Main non reseizable
  + QStatusBar
*/


Dialog::Dialog(QWidget *parent, Qt::WFlags f) {
    mainLayout = new QVBoxLayout;

    createMenu();
    createSamplingRate();
    createCheckGroup();
    createDeviceGroup();
    createFrequencyGroup();
    createConfigGroup();

    QLineEdit *myTest = new QLineEdit();
    myTest->setText("bla");
    myTest->setReadOnly(true);
    myTest->setEnabled(false);

    mainLayout->addWidget(myTest);

    createButtonGroup();
    setLayout (mainLayout);

    //setFixedSize (350, 400);
    setWindowTitle(tr("QS1R 7 channel audio relay"));
}


Dialog::~Dialog() {}


void Dialog::createMenu() {
    menuBar = new QMenuBar;

    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
    mainLayout->setMenuBar(menuBar);
}


void Dialog::createConfigGroup() {
    configGroup = new QFrame();
    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(checkGroup);
    layout->addWidget(deviceGroup);
    layout->addWidget(frequencyGroup);
    configGroup->setLayout(layout);

    mainLayout->addWidget(configGroup);
}


void Dialog::createCheckGroup() {
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
    checkGroup->setLayout(layout);
}


void Dialog::createDeviceGroup() {
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
}


void Dialog::createFrequencyGroup() {
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
    frequency[0]->setValue(1812000);
    frequency[1]->setValue(3524000);
    frequency[2]->setValue(7024000);
    frequency[3]->setValue(10124000);
    frequency[4]->setValue(14024000);
    frequency[5]->setValue(18092000);
    frequency[6]->setValue(21024000);
    frequencyGroup->setLayout(layout);
}


void Dialog::createSamplingRate() {
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


void Dialog::createButtonGroup() {
    buttonGroup = new QFrame();
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignLeft);

    startStopButton = new QPushButton(tr("Start"));
    startStopButton->setFixedWidth(50);

    quitButton = new QPushButton(tr("Quit"));
    quitButton->setFixedWidth(50);

    layout->addWidget(startStopButton);
    layout->addWidget(quitButton);
    buttonGroup->setLayout(layout);

    mainLayout->addWidget(buttonGroup);

    connect(startStopButton, SIGNAL(accepted()), this, SLOT(accept()));
    connect(quitButton, SIGNAL(rejected()), this, SLOT(reject()));
}
