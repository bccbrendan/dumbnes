#ifndef __NES_6502_H
#define __NES_6502_H
/*
 * nes_6502.h
 *
 * class declarations for NES 6502 cpu emulation
 * brendan long
 * Feb 2015
 */
#include "dumbnes_includes.h"

namespace dumbnes
{

class OpInfo;

/* Status Register Codes
bit ->   7 0
+---+---+---+---+---+---+---+---+
| N | V |   | B | D | I | Z | C |  <-- flag, 0/1 = reset/set
+---+---+---+---+---+---+---+---+
*/
enum SrCode
{
    /* CARRY. Set if the add produced a carry,
     * or if the subtraction produced a borrow. 
     * Also holds bits after a logical shift.*/
    C = 0x1,

    /* ZERO.  Set if the result of the last operation
     * (load/inc/dec/add/sub) was zero. */
    Z = 0x2,

    /* IRQ DISABLE.  Set if maskable interrupts are disabled. */
    I = 0x4,

    /* DECIMAL MODE. Set if decimal mode active. */
    D = 0x8,

    /* BRK COMMAND. Set if an interrupt caused by a BRK, 
     * reset if caused by an external interrupt.*/
    B = 0x10,

    /* OVERFLOW. Set if the addition of two like-signed numbers 
     * or the subtraction of two unlike-signed numbers 
     *  produces a result greater than +127 or less than -128.*/
    V = 0x40,

    /* Sign. Set if bit 7 of the accumulator is set. */
    S = 0x80
};

class Nes6502
{
protected:
    std::shared_ptr<IMemory> _memory;
    /* registers */
    uint8_t _regA; /*accumulator*/
    uint8_t _regX; /*index*/
    uint8_t _regY; /*index*/
    uint8_t _regSR; /*status*/
    uint8_t _regSP; /*stack pointer*/
    uint16_t _regPC; /*program counter*/

public:
    Nes6502(const std::shared_ptr<IMemory>& memory);
    ~Nes6502(void);
    void Reset (void);
    int Step(void);
    int Interrupt( /*TODO code?*/);
    inline uint16_t PC(void) const {return _regPC;}
    inline uint16_t A(void) const {return _regA;}

private:
    uint16_t FetchOperand(const OpMode& opmode);
    void ProcessOp(const OpInfo& op);
    void CheckSr_S(uint8_t result);
    void CheckSr_Z(uint8_t result);
};

} // namespace dumbnes

#endif /*__NES_6502_H*/

