#include "chip8.h"
#include <assert.h>


Chip8::Chip8(){}

void Chip8::CPUReset()
{
    m_AddressI = 0;
    m_ProgramCounter = 0x200;
    memset(m_Registers,0,sizeof(m_Registers));

    // Load the game
    File *in;
    in = fopen("roms/Pong (1 player).ch8", "rb");
    fread(&m_GameMemory[0x200], 1, 3584, in);
    fclose(in);
}                           

WORD Chip8::GetNextOpcode()
{
    WORD res = 0;
    res = m_GameMemory[m_ProgramCounter];
    // res <<=8;
    // res |= m_GameMemory[m_ProgramCounter+1];
    res = m_GameMemory[m_ProgramCounter] <<8 | m_GameMemory[m_ProgramCounter+1]; // grabs opcode from 2 bytes and merges them
    m_ProgramCounter +=2;
    return res;
}

void Chip8::ExecuteNextOpcode()
{
    WORD opcode = GetNextOpcode();
    // switch(opcode & 0xF000)
    // {
    //     case 0x1000: Opcode1NNN(opcode); break;
    //     case 0x0000: //break down further
    //     {
    //         switch(opcode & 0x00F)
    //         {
    //             case 0x0000: Opcode00E0(); break; //clear screen
    //             case 0x000E: Opcode00EE(); break; //return from subroutine
    //         }
    //     }
    //     break;
    //     default : break;
    // }
    switch(opcode & 0xF000)
    {
        case 0x0000: DecodeOpcode00(opcode); break;
        case 0x1000: Opcode1NNN(opcode); break;
        case 0x2000: Opcode2NNN(opcode); break;
        case 0x3000: Opcode3XNN(opcode); break;
        case 0x4000: Opcode4XNN(opcode); break;
        case 0x5000: Opcode5XY0(opcode); break;
        case 0x6000: Opcode6XNN(opcode); break;
        case 0x7000: Opcode7XNN(opcode); break;
        case 0x8000: DecodeOpcode8(opcode); break;
        case 0x9000: Opcode9XY0(opcode); break;
        case 0xA000: OpcodeANNN(opcode); break;
        case 0xB000: OpcodeBNNN(opcode); break;
        case 0xC000: OpcodeCXNN(opcode); break;
        case 0xD000: OpcodeDXYN(opcode); break;
        case 0xE000: DecodeOpcodeE(opcode); break;
        case 0xF000: DecodeOpcodeF(opcode); break;
        default : break;
    }
}


void Chip8::DecodeOpcode00(WORD opcode){
    switch(opcode & 0xF)
    {
        case 0x0: Opcode00E0; break;
        case 0xE: Opcode00EE; break;
        default: break;
    }
}

void Chip8::DecodeOpcode8(WORD opcode)
{
    switch (opcode & 0xF)
    {
        case 0x0: Opcode8XY0(opcode); break;
        case 0x1: Opcode8XY1(opcode); break;
        case 0x2: Opcode8XY2(opcode); break;
        case 0x3: Opcode8XY3(opcode); break;
        case 0x4: Opcode8XY4(opcode); break;
        case 0x5: Opcode8XY5(opcode); break;
        case 0x6: Opcode8XY6(opcode); break;
        case 0x7: Opcode8XY7(opcode); break;
        case 0xE: Opcode8XYE(opcode); break;
        default: break;
    }
}

void Chip8::DecodeOpcodeE(WORD opcode)
{
    switch(opcode & 0xF)
    {
        case 0xE: OpcodeEX9E(opcode); break;
        case 0x1: OpcodeEXA1(opcode); break;
        default: break;
    }
}

void Chip8::DecodeOpcodeF(WORD opcode)
{
    switch(opcode & 0xFF)
    {
        case 0x07: OpcodeFX07(opcode); break;
        case 0x0A: OpcodeFX0A(opcode); break;
        case 0x15: OpcodeFX15(opcode); break;
        case 0x18: OpcodeFX18(opcode); break;
        case 0x1E: OpcodeFX1E(opcode); break;
        case 0x29: OpcodeFX29(opcode); break;
        case 0x33: OpcodeFX33(opcode); break;
        case 0x55: OpcodeFX55(opcode); break;
        case 0x65: OpcodeFX65(opcode); break;
        default: break;
    }
}

// Clear the screen
void Chip8::Opcode00E0 ()
{

}

// Returns from subroutine
void Chip8::Opcode00EE	()
{

}

// Jump to address at NNN
void Chip8::Opcode1NNN(WORD opcode)
{
    m_ProgramCounter = opcode & 0x0FFF; // only interested in NNN of opcode 1NNN
}

// Calls subroutine at NNN
void Chip8::Opcode2NNN(WORD opcode)
{
    m_Stack.push_back(m_ProgramCounter);
    m_ProgramCounter = opcode & 0x0FFF;
}

// Skip instruction if Vx == NN 
void Chip8::Opcode3XNN(WORD opcode)
{
    if(m_Registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
    {
        m_ProgramCounter += 2;
    }
}

// Skip instruction if Vx != NN
void Chip8::Opcode4XNN(WORD opcode)
{
    if(m_Registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
    {
        m_ProgramCounter += 2;
    }
}

// Skip instruction if Vx == Vy
void Chip8::Opcode5XY0(WORD opcode)
{
    if(m_Registers[(opcode & 0x0F00) >> 8] == m_Registers[(opcode & 0x00F0) >> 4])
    {
        m_ProgramCounter += 2;
    }
}

// Set Vx as NN
void Chip8::Opcode6XNN(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
}

// Adds NN to Vx
void Chip8::Opcode7XNN(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
}

// Set Vx as Vy
void Chip8::Opcode8XY0(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] = m_Registers[(opcode & 0x00F0) >>4];
}

// Apply Vx OR Vy to Vx
void Chip8::Opcode8XY1(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] |= m_Registers[(opcode & 0x00F0) >>4];
}

// Apply Vx AND Vy to Vx
void Chip8::Opcode8XY2(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] &= m_Registers[(opcode & 0x00F0) >>4];
}

// Apply Vx XOR Vy to Vx
void Chip8::Opcode8XY3(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] ^= m_Registers[(opcode & 0x00F0) >>4];
}

// Apply Vx +=Vy
// Vf is overflow flag
void Chip8::Opcode8XY4(WORD opcode)
{
    m_Registers[0xF] = 0;
    int val = m_Registers[(opcode & 0x0F00) >>8] + m_Registers[(opcode & 0x00F0) >> 4];
    if(val > 255)
        m_Registers[0xF] = 1;

    m_Registers[(opcode & 0x0F00) >>8] += m_Registers[(opcode & 0x00F0) >> 4];
}

// Apply Vx -=Vy
// Vf is 0 when underflow
void Chip8::Opcode8XY5(WORD opcode)
{
    m_Registers[0xF] = 1;
    if(m_Registers[(opcode & 0x0F00) >>8] < m_Registers[(opcode & 0x00F0) >> 4])
        m_Registers[0xF] = 0;
    
    m_Registers[(opcode & 0x0F00) >>8] -= m_Registers[(opcode & 0x00F0) >> 4];
}

// Apply >> 1
// Vf contains lost bit
void Chip8::Opcode8XY6(WORD opcode)
{
    m_Registers[0xF] = m_Registers[((opcode & 0x0F00)>>8)] & 0x1;
    m_Registers[(opcode & 0x0F00)>>8] >>=1
}

// Apply Vx = Vy-Vx
// Vf is 0 when underflow
void Chip8::Opcode8XY7(WORD opcode)
{
    m_Registers[0xF] = 1;
    if(m_Registers[(opcode & 0x00F0) >> 4] < m_Registers[(opcode & 0x0F00) >>8])
        m_Registers[0xF] = 0;
    m_Registers[(opcode & 0x0F00) >>8] = m_Registers[(opcode & 0x00F0) >>4] - m_Registers[(opcode & 0x0F00) >>8];
}

// Apply << 1
// Vf contains lost bit
void Chip8::Opcode8XYE(WORD opcode)
{
    m_Registers[0xF] = m_Registers[((opcode & 0x0F00)>>8)] >> 7;
    m_Registers[(opcode & 0x0F00)>>8] <<=1;
}

// Skip instruction if Vx != Vy
void Chip8::Opcode9XY0(WORD opcode)
{
    if(m_Registers[(opcode & 0x0F00) >> 8] != m_Registers[(opcode & 0x00F0) >> 4])
    {
        m_ProgramCounter += 2;
    }
}

// Set I to NNN
void Chip8::OpcodeANNN(WORD opcode)
{
    m_AddressI = opcode & 0x0FFF;
}

// Set PC to NNN + V0
void Chip8::OpcodeBNNN(WORD opcode)
{
    m_ProgramCounter = (opcode & 0x0FFF) + m_Registers[0];
}

// Set Vx to rand & NN
void Chip8::OpcodeCXNN(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] = (rand() % 255) & (opcode & 0x00FF);
}

// Draw sprite at Vx,Vy
// Vf is 1 if any screen pixels are flipped from set to unset
void Chip8::OpcodeDXYN(WORD opcode)
{
    
}

// Skip instruction if key in Vx is pressed
void Chip8::OpcodeEX9E(WORD opcode)
{
    
}

// Skip instruction if key in Vx is not pressed
void Chip8::OpcodeEXA1(WORD opcode)
{

}

// Set Vx to delay timer
void Chip8::OpcodeFX07(WORD opcode)
{

}

// key press is stored ni Vx
void Chip8::OpcodeFX0A(WORD opcode)
{

}

// Set delay timer to Vx
void Chip8::OpcodeFX15(WORD opcode)
{

}

// Set sound timer to Vx
void Chip8::OpcodeFX18(WORD opcode)
{

}

// I += Vx
void Chip8::OpcodeFX1E(WORD opcode)
{
    m_AddressI += m_Registers[(opcode & 0x0F00) >> 8];
}

// Set I to location of sprite in Vx
void Chip8::OpcodeFX29(WORD opcode)
{
    m_AddressI = m_Registers[(opcode & 0x0F00) >> 8] * 5;
}

// Stores binary coded decimal rep of Vx
void Chip8::OpcodeFX33(WORD opcode)
{
    int value = m_Registers[(opcode & 0x0F00) >> 8];
    int hundreds = value / 100;
    int tens = (value / 10) % 10;
    int units = value % 10;

    m_GameMemory[m_AddressI] = hundreds;
    m_GameMemory[m_AddressI+1] = tens;
    m_GameMemory[m_AddressI+2] = units;
}

// Stores V0 -> Vx in memory starting at I
void Chip8::OpcodeFX55(WORD opcode)
{
    for(int i=0; i<= (opcode & 0x0F00) >> 8; i++)
    {
        m_GameMemory[m_AddressI+i] = m_Registers[i];
    }
    m_AddressI = m_AddressI+ (opcode & 0x0F00) >>8 +1;
}

// Fills V0->Vx from memory starting at I
void Chip8::OpcodeFX65(WORD opcode)
{
    for(int i=0; i<= (opcode & 0x0F00) >> 8; i++)
    {
        m_Registers[m_AddressI+i] = m_GameMemory[i];
    }
    m_AddressI = m_AddressI+ (opcode & 0x0F00) >>8 +1;
}