#pragma once
#include <string>
#include <vector>


typedef unsigned char BYTE; 
typedef unsigned short int WORD;


class Chip8
{
public:
    Chip8();
    ~Chip8();

    void ExecuteNextOpcode();
private:

    void CPUReset();
    WORD GetNextOpcode();

    void Opcode00EE	();
    void Opcode00E0 ();
    void Opcode1NNN	( WORD opcode ) ;
    void Opcode2NNN	( WORD opcode ) ;
    void Opcode3XNN	( WORD opcode ) ;
    void Opcode4XNN	( WORD opcode ) ;
    void Opcode5XY0	( WORD opcode ) ;
    void Opcode6XNN	( WORD opcode ) ;
    void Opcode7XNN	( WORD opcode ) ;
    void Opcode8XY0	( WORD opcode ) ;
    void Opcode8XY1	( WORD opcode ) ;
    void Opcode8XY2	( WORD opcode ) ;
    void Opcode8XY3	( WORD opcode ) ;
    void Opcode8XY4	( WORD opcode ) ;
    void Opcode8XY5	( WORD opcode ) ;
    void Opcode8XY6	( WORD opcode ) ;
    void Opcode8XY7	( WORD opcode ) ;
    void Opcode8XYE	( WORD opcode ) ;
    void Opcode9XY0	( WORD opcode ) ;
    void OpcodeANNN	( WORD opcode ) ;
    void OpcodeBNNN	( WORD opcode ) ;
    void OpcodeCXNN	( WORD opcode ) ;
    void OpcodeDXYN	( WORD opcode ) ;
    void OpcodeEX9E	( WORD opcode ) ;
    void OpcodeEXA1	( WORD opcode ) ;
    void OpcodeFX07	( WORD opcode ) ;
    void OpcodeFX0A	( WORD opcode ) ;
    void OpcodeFX15	( WORD opcode ) ;
    void OpcodeFX18	( WORD opcode ) ;
    void OpcodeFX1E	( WORD opcode ) ;
    void OpcodeFX29	( WORD opcode ) ;
    void OpcodeFX33	( WORD opcode ) ;
    void OpcodeFX55	( WORD opcode ) ;
    void OpcodeFX65	( WORD opcode ) ;

    void DecodeOpcode00(WORD opcode);
    void DecodeOpcode8(WORD opcode);
    void DecodeOpcodeE(WORD opcode);
    void DecodeOpcodeF(WORD opcode);
    
public:
    BYTE m_ScreenData[320][640][3]; 

private:
    BYTE m_GameMemory[0xFFF] ; // 0xFFF bytes of memory
    BYTE m_Registers[16] ; // 16 registers, 1 byte each
    WORD m_AddressI ; // the 16-bit address register I
    WORD m_ProgramCounter ; // the 16-bit program counter
    std::vector<WORD> m_Stack; // the 16-bit stack

};