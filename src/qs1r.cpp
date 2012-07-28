#include "qs1r.h"
#include "qs_io_libusb.h"
#include <QDebug>

#define QS1R_FIRMWARE_NAME "QS1R_7CH.hex"
#define QS1R_FPGA_NAME "QS1R_7CH.rbf"


const int Blockz = 512; // Fixed number of buffers for QS1R hardware
const int BlockzSize = Blockz * 8 * sizeof(qint32) * 2; // 8 channels transmitteds, 32 bits per channel, IQ = x2


ExtractorExchange::ExtractorExchange(QObject *parent) {
    bufferQs1r = new unsigned char[BlockzSize];
}


void ExtractorExchange::init() {
    qs1rio_init();

    qWarning() << "Detecting device...";
    if (findQsDevice( ) == -1) { // Try to initialize board
        qWarning() << "Could not find QS1R! Make sure board is connected and powered up.";
        return;  // Board is probably not connected?
    }
    qWarning() << "Device found!";

    std::string path = "";
    std::string path_filename = "";

    if ( readFwSn() != ID_FWWR ) {
        qWarning() << "Loading firmware.";
        path_filename.append( path );
        path_filename.append( QS1R_FIRMWARE_NAME );
        if ( loadFirmware( path_filename.c_str( ) ) == -1) {
            qWarning() << "Could not load QS1R firmware!";
            return;
        }

        qWarning() << "Firmware loaded... reinit...";
        Sleep(10000);   // delay for reenumeration

        if ( findQsDevice( ) == -1 ) { // try to initialize board again
            qWarning() << "Could not find QS1R! ";
            return;  // board is probably not connected or firmware not loaded?
        }
    }
    qWarning() << "Found QS1R...";
    Sleep(500);

    if ( readMultibusInt( MB_ID_REG ) != ID_1RXWR ) { // is FPGA loaded with proper file?
        qWarning() << "Loading QS1R FPGA... Please wait...";
        path_filename.clear( );
        path_filename.append( path );
        path_filename.append( QS1R_FPGA_NAME );
        if ( loadFpga( path_filename.c_str( ) ) == -1 ) {
            qWarning() << "Could not load QS1R Fpga!";
            return;  // Can't find rbf file?
        }
    }

    qWarning() << "QS1R FPGA is loaded...";
    Sleep(500);
}


ExtractorExchange::~ExtractorExchange() {
    qs1rio_close();
}


void ExtractorExchange::run() {
    do {
        sizeRead = readEP6(bufferQs1r, BlockzSize);
        emit bufferUpdate(sizeRead);
    }
    while (sizeRead); // FIXME check if something goes wrong...
}


void ExtractorExchange::SetLO(int channel, long freq) {
    int frequency = (int)((double)freq / (double)125e6 * (double)4294967296 * (double)1.00001084816); // FIXME : Float value in the GUI interface
    writeMultibusInt(channel, frequency);
    Sleep( 10 );
}


void ExtractorExchange::SetSR(int value) {
    writeMultibusInt(10, value); // FIXME 7
    Sleep( 10 );
}
