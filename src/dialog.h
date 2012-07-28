#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>


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
QT_END_NAMESPACE


class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0, Qt::WFlags f = 0);
    virtual ~Dialog();

private:
    void createMenu();
    void createConfigGroup();
    void createCheckGroup();
    void createDeviceGroup();
    void createFrequencyGroup();
    void createButtonGroup();
    void createSamplingRate();

    enum { NumButtons = 7 };

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

    QCheckBox *check[NumButtons];
    QComboBox *device[NumButtons];
    QSpinBox  *frequency[NumButtons];

    QComboBox *samplingRate;
};

#endif
