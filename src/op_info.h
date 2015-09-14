#ifndef __OP_INFO_H
#define __OP_INFO_H

#include "dumbnes_includes.h"

namespace dumbnes
{
using Opcode = uint8_t;

class OpInfo
{
public:
    dumbnes::Mnemonic mnemonic;
    dumbnes::OpMode mode;
    size_t length;
    size_t time;
    bool add1forPageCrossing;
    bool add1forBranchTaken;
    
    OpInfo(
        dumbnes::Mnemonic mnemonic,
        dumbnes::OpMode mode,
        size_t length,
        size_t time,
        bool add1forPageCrossing = false,
        bool add1forBranchTaken = false
    ):mnemonic(mnemonic)
     ,mode(mode)
     ,length(length)
     ,time(time)
     ,add1forPageCrossing(add1forPageCrossing)
     ,add1forBranchTaken(add1forBranchTaken)
    {}

    static const OpInfo& Decode(Opcode opcode);
};

} // namespace dumbnes

std::ostream& operator<<(std::ostream& os, const dumbnes::OpInfo& op);

#endif /* __OP_INFO_H */
