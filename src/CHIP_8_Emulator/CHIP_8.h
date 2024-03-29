#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

class CHIP_8{
	private:

	//Delcare variables for CPU
	
	//General purpose registers - 8 bit
	unsigned char V[16];
	//Intstruction OpCode
	unsigned short opcode;
	//Memory - 8 bit 4K
	unsigned char memory[4096];
    //I Register - stores memory locations
	unsigned short I;
	//Program counter
	unsigned short pc;
	//Graphics pixel state
    unsigned char gfx[64][32];
	//Timers
	unsigned char delay_timer;
	unsigned char sound_timer;
	//Stack and Stack pointer
	unsigned char sp;
	unsigned short stack[16];
	//Keypad inputs
    unsigned char keypad[16];


	public:

    //Functions
	CHIP_8();
    void init();
	void emulateCycle();
    void loadROM(std::string filename);
    void printMemory();
    void setKeypad(unsigned char KeyNum, unsigned char KeyVal);
    void decrementTimers();
    void getSoundTimer();
    unsigned char** getGFX();
    unsigned int getGFX_X_Y(int x, int y);
    void dumpGFX();
    bool compatabilityFlag;
    bool drawFlag;

};
