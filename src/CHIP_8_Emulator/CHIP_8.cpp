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
	//Clear Timers
    delay_timer = 0;
	sound_timer = 0;
	
    //Load Fontset
    for(int i = 0; i < 80; i++){
        memory[i] = fontset[i];
    }
	
	
}
	


void CHIP_8::emulateCycle(){
	
	//Get opcode
	//Instruction are 16bit, OR to combine
    opcode = (memory[pc] << 8) | (memory[pc+1]);
	//Decode Opcode
	switch(opcode & 0xF000){
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
                    //SHIFT RIGHT - if LSB of Vx = 1, set Vf = 1, then >> Vx idk why Vy is needed
                    if((V[(opcode & 0x0F00) >>8] & 1) == 1){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >>8] = V[(opcode & 0x0F00) >>8] >> 1;
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
                    //SHIFT LEFT - if MSB of Vx = 1, VF = 1, else 0. Then, Vx = Vx << 2, 0x40 is MSB bit mask
                    if(((V[(opcode & 0x0F00) >> 8] & 0x40) >> 7) == 1){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >>8] = V[(opcode & 0x0F00) >>8] << 1;
                    pc += 2;
                    break;
				
			}
			break;
		case 0x9000:
            //Skip not equal - SKip if Vx != Vy
            if(V[(opcode & 0x0F00) >>8] != V[(opcode & 0x00F0) >> 4]){
                pc += 2;
            }
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

        /*
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
            unsigned int bytes; //Do this

            pc += 2;;
            break;
        }
		case 0xE000:
			break;
		case 0xF000:
			break;
		case 0x0000:
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
