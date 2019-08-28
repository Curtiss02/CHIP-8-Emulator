#include <QCoreApplication>
#include <iostream>
#include "CHIP_8.h"
CHIP_8 myChip;

void drawGraphics();

int main(int argc, char *argv[])
{
    myChip.init();
    myChip.loadROM("C:\\Users\\Curtiss-PC\\Documents\\CHIP-8\\CHIP-8-Emulator\\src\\CHIP_8_Emulator\\ROM\\particles.ch8");
    myChip.printMemory();
    QCoreApplication a(argc, argv);
    for (;;) {
        myChip.emulateCycle();
        drawGraphics();
        getchar();

    }

    return a.exec();
}

void drawGraphics(){

}
