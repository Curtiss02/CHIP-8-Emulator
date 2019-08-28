#include <QCoreApplication>
#include "CHIP_8.h"
CHIP_8 myChip;
int main(int argc, char *argv[])
{
    myChip.init();
    myChip.loadROM("C:\\Users\\Curtiss-PC\\Documents\\CHIP-8\\CHIP-8-Emulator\\src\\CHIP_8_Emulator\\ROM\\particles.ch8");
    QCoreApplication a(argc, argv);

    return a.exec();
}
