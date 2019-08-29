//Chip 8 CPU/Interpretor Class, handles intruction emulation
#include "CHIP_8.h"



static const unsigned char fontset[80] =  {
        0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
        0x20, 0x60, 0x20, 0x20, 0x70,		// 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
        0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
        0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
        0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
        0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
        0xF0, 0x80, 0xF0, 0x80, 0x80		// F

     };


CHIP_8::CHIP_8(){

}

void CHIP_8::init(){
	
	//Reset
	sp = 0;
	pc = 0x200;
	opcode = 0;
	I = 0;
	//Clear memory
	for(int i = 0; i < 4096; i++){
		memory[i] = 0;
	}
	//Clear stack
	for(int i = 0; i < 0xF; i++){
		stack[i] = 0;
	}
	//Clear V registers
    for(int i = 0; i < 0xF; i++){
		V[i] = 0;
	}
	//CLR GFX
    for(int x = 0; x < 64; x++){
        for(int y = 0; y < 32; y++){
           gfx[x][y] = 0;
        }
	}
    for(int i = 0; i < 16; i++){
        keypad[i] = 0;
    }
	//Clear Timers
    delay_timer = 0;
	sound_timer = 0;
	
    //Load Fontset
    for(int i = 0; i < 80; i++){
        memory[i] = fontset[i];
    }
    drawFlag = false;
	
}
	


void CHIP_8::emulateCycle(){
	
	//Get opcode
	//Instruction are 16bit, OR to combine
    opcode = (memory[pc] << 8) | (memory[pc+1]);
    //std::cout << "Opcode: " << opcode << std::endl;
    drawFlag = false;
	//Decode Opcode
	switch(opcode & 0xF000){
        case 0x0000:
            switch(opcode & 0x00FF){
                // Clear Screen
                case(0x00E0):
                    for(int x = 0; x < 64; x++){
                        for(int y = 0; y < 32; y++){
                           gfx[x][y] = 0;
                        }
                    }
                    pc += 2;
                    drawFlag = true;
                    break;
                case(0x00EE):
                    sp -= 1;
                    pc = stack[sp];
                    pc += 2;
                    break;
            }

            break;
		case 0x1000:
			//JMP - Sets PC to mem adress epcfified at 0x1nnn
			pc = opcode & 0x0FFF;
			break;
		case 0x2000:
			//SUB ROUTINE - jumps to adress and stores return adress on stack
			stack[sp] = pc;
			sp++;
            pc = opcode & 0x0FFF;
			break;
		case 0x3000:
			//SKIp if Equal 0x3xkk - skips intruction if v[x] == kk
            if(V[(opcode&0x0F00) >>  8] == (opcode&0x00FF)){
				pc += 2;
			}
            pc += 2;
			break;
		case 0x4000:
			//Skip not equal 0x4xkk - skips intruction if v[x] != kk
            if(V[(opcode&0x0F00)>>  8] != (opcode&0x00FF)){
				pc += 2;
			}
            pc += 2;
			break;
		case 0x5000:
			//Skip equal register 0x5xy0 - skips the next intruction if register v[x] and v[y] are equal
			if(V[(opcode&0x0F00) >>  8] == V[(opcode&0x00F0) >> 4]){
                pc +=2;
            }
            pc += 2;
			break;
		case 0x6000:
			//LOAD BYTE 0x6xkk - sets V[x] = kk
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
			break;
		case 0x7000:
			//ADD 0x7xkk  - V[x] =  V[x] + kk
			V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
			break;
		case 0x8000:
			switch(opcode & 0x000F){
                case(0x000):
                    //SET Vx = Vy - 0x8xy0
                    V[(opcode & 0x0F00) >>8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
				case(0x0001):
                    //OR - Vx = Vx OR Vy - 0x8xy1
                    V[(opcode & 0x0F00) >>8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
				case(0x0002):
                    //AND
                    V[(opcode & 0x0F00) >>8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
				case(0x0003):
                    //XOR
                    V[(opcode & 0x0F00) >>8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
				case(0x0004):
                    //ADD and and store carry in V[F]
                    //Checks if calculation will overflow
                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
				case(0x0005):
                    //SUBTRACt - if Vx > Vy - VF = 1, ese 0 then Vx = Vx - Vy
                    if(V[(opcode & 0x0F00) >>8] > V[(opcode & 0x00F0) >> 4]){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >>8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
				case(0x0006):
                if(!compatabilityFlag){
                    //SHIFT RIGHT - if LSB of Vx = 1, set Vf = 1, then >> Vx idk why Vy is needed
                    if((V[(opcode & 0x00F0) >> 4] & 1) == 1){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >>8] = V[(opcode & 0x00F0) >> 4]  >> 1;

                     }
                else{
                    if((V[(opcode & 0x0F00) >> 8] & 1) == 1){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >>8] = V[(opcode & 0x0F00) >> 8]  >> 1;
                }
                    pc += 2;
                    break;
				case(0x0007):
                    //SUB N - Vx = Vy - Vx if Vy > Vx, VF = 1, else 0
                    if(V[(opcode & 0x0F00) >>8] < V[(opcode & 0x00F0) >> 4]){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >>8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case(0x000E):
                    //SHIFT LEFT - if MSB of Vx = 1, VF = 1, else 0. Then, Vx = Vx << 2, 0x80 is MSB bit mask
                    if(!compatabilityFlag){
                        if(((V[(opcode & 0x00F0) >> 4] & 0x80) >> 7) == 1){
                            V[0xF] = 1;
                        }
                        else{
                            V[0xF] = 0;
                        }
                        V[(opcode & 0x0F00) >>8] = V[(opcode & 0x00F0) >> 4] << 1;
                    }
                    else{
                        if(((V[(opcode & 0x0F00) >> 8] & 0x80) >> 7) == 1){
                            V[0xF] = 1;
                        }
                        else{
                            V[0xF] = 0;
                        }
                        V[(opcode & 0x0F00) >>8] = V[(opcode & 0x0F00) >> 8] << 1;
                    }
                    pc += 2;
                    break;
				
			}
			break;
		case 0x9000:
            //Skip not equal - SKip if Vx != Vy
            if(V[(opcode & 0x0F00) >>8] != V[(opcode & 0x00F0) >> 4]){
                pc += 2;
            }
            pc+=2;
			break;
		case 0xA000:
            //LD I 0xAnnn - set REG I to nnn
            I = opcode & 0x0FFF;
            pc += 2;
			break;
		case 0xB000:
            //JP nnn + v0 - Jump to location at nnn + v0
            pc = (opcode & 0x0FFF) + V[0];
			break;
		case 0xC000:
            //RAND - random number between
            V[(opcode & 0x0F00) >>8] = (rand() % 256) & (opcode & 0x00FF);
            pc += 2;
			break;
        case 0xD000:
        {

        /*  Dxyn
         * The interpreter reads n bytes from memory, starting at the address stored in I.
         * These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
         * Sprites are XORed onto the existing screen.
         *  If this causes any pixels to be erased, VF is set to 1,
         * otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of the display,
         * it wraps around to the opposite side of the screen.
         * See instruction 8xy3 for more information on XOR, and section 2.4, Display,
         * for more information on the Chip-8 screen and sprites.
         */
            unsigned int x = V[(opcode & 0x0F00) >>8];
            unsigned int y = V[(opcode & 0x00F0) >>4];
            unsigned int nBytes = opcode & 0x000F;
            //If initial coord is off screen, wrap around
            x = x % 64;
            y = y % 32;

            //VF is set if any pixels are erased - init to 0
            V[0xF] = 0;
            for(unsigned int i = 0; i < nBytes; i++){
                //Grab each byte/row
                unsigned char currentByte = memory[I + i];

                //Dont draw off screen
                if((y + i) > 31){
                    continue;
                }

                //Loop through each pixel in row
                for(unsigned int j = 0; j < 8; j++){
                    //Bit mask for current pixel
                    unsigned char bitmask = (0x80) >> j;
                    unsigned char bit = (bitmask & currentByte) != 0;
                    if(bit != 0){
                        //IF partially off screen - cut off
                        if((x + j) > 63){
                            continue;
                        }
                        //if any bits cleared by draw, set V[F] to 1;
                        if(gfx[x+j][y+i] != 0){
                            V[0xF] = 1;
                        }
                        gfx[x+j][y+i] ^= bit;

                    }

                }
            }
            drawFlag = true;
            pc += 2;
            break;
        }
		case 0xE000:
            switch(opcode & 0x00FF){
                case(0x009E):
                if(keypad[V[(opcode & 0x0F00) >>8]] == 1){
                    pc += 2;
                }
                pc +=2;
                break;

                case(0x00A1):
                if(keypad[V[(opcode & 0x0F00) >>8]] == 0){
                    pc += 2;
                }
                pc += 2;
                break;

            }
        break;
		case 0xF000:
            switch(opcode & 0x00FF){
                case(0x0007):
                    V[(opcode & 0x0F00) >>8] = delay_timer;
                    pc += 2;
                    break;
                case(0x000A):
                //Blocking nput - get input and store value/keynum in VX Register
                    for(unsigned int i = 0; i < 16; i++){
                        if(keypad[i] == 1){
                            V[(opcode & 0x0F00) >>8] = i;
                            pc += 2;
                            break;
                        }
                    }
                    //Wont increment pc unless some key is pressed - therefor blocking
                    break;
                case(0x0015):
                    delay_timer = V[(opcode & 0x0F00) >>8];
                    pc += 2;
                    break;
                case(0x0018):
                    sound_timer = V[(opcode & 0x0F00) >>8];
                    pc += 2;
                    break;
                case(0x001E):
                    I += V[(opcode & 0x0F00) >>8];
                    pc += 2;
                    break;
                case(0x0029):
                    I = V[(opcode & 0x0F00) >>8] * 0x5;
                    pc += 2;
                    break;
                case(0x0033):{
                    unsigned int value = V[(opcode & 0x0F00) >>8];
                    unsigned char ones = value % 10;
                    value = value / 10;
                    unsigned char tens = value % 10;
                    unsigned char hundreds = value / 10;
                    memory[I] = hundreds;
                    memory[I+1] = tens;
                    memory[I+2] = ones;
                    pc += 2;
                    break;
                }

                case(0x0055):
                    for(int i = 0; i < ((opcode & 0x0F00) >> 8); i++){
                        memory[I+i] = V[i];
                    }
                    pc += 2;
                    break;
                case(0x0065):
                    for(int i = 0; i < ((opcode & 0x0F00) >> 8); i++){
                        V[i] = memory[I+i];
                    }
                    pc += 2;


            }
            break;
		default:
            std::cout << "ERROR: Unknown Opcode" << std::endl;
	}	
  }
void CHIP_8::loadROM(std::string filename){
    std::ifstream romFile;
    char* buffer;
    std::streampos size;
    romFile.open(filename,std::ios::in | std::ios::binary | std::ios::ate);
    if(romFile.is_open()){
        //Get size of ROM
        size = romFile.tellg();
        //Allocate buffer
        buffer = new char [size];
        romFile.seekg (0, std::ios::beg);
        romFile.read(buffer, size);
        romFile.close();
        std::cout << "Success loading ROM!" << std::endl;
        std::cout << "Dumping ROM to Console:" << std::endl;
        for(int i = 0; i < size; i++){
            //TODO : LOAD rum into MEME starting at 0x200
            memory[0x200+i] = buffer[i];
            std::cout << std::hex << int(buffer[i] & 0xff);
            std::cout << " ";
        }
        std::cout << std::endl;
        delete[] buffer;
    }
    else{
        std::cout << "Error loading ROM." << std::endl;
    }

}
void CHIP_8::decrementTimers(){
    if(sound_timer > 0){
        sound_timer -= 1;
    }
    if(delay_timer > 0){
        delay_timer -= 1;
    }
}


//Prints the contents of the memory to the console
void CHIP_8::printMemory(){
    std::cout << "Dumping Memory";
    for(int i = 0; i < 4096; i++){
        if(i % 16 == 0){
            std::cout << std::endl;
        }
        std::cout << std::hex << int(memory[i] & 0xff);
        std::cout << " ";
    }
}
void CHIP_8::setKeypad(unsigned char KeyNum, unsigned char KeyVal){
    keypad[KeyNum] = KeyVal;
    std::cout << std::hex << "Key " << int(KeyNum) << "set to " << int(KeyVal) << std::endl;
}

unsigned char** CHIP_8::getGFX(){
    unsigned char ** graphics = 0;
    graphics = new unsigned char*[64];
    for(int i = 0; i < 64; i++){
        graphics[i] = new unsigned char[32];
        for(int j = 0; j < 32; j++){
            graphics[i][j] = gfx[i][j];
        }

    }
    return graphics;
}

unsigned int CHIP_8::getGFX_X_Y(int x, int y){
    return gfx[x][y];
}

void CHIP_8::dumpGFX(){
    std::cout << "GFX MEM:\n" ;
    for(int x = 0; x < 64; x++){
        for(int y = 0; y < 32; y++){
           std::cout << int(gfx[x][y]);
        }
        std::cout << std::endl;
    }
}

