#ifndef __OP_INFO_H
#define __OP_INFO_H

#include "dumbnes_includes.h"
#include "opcodes/opcodes.h"

namespace dumbnes { namespace opcodes {

using Opcode = uint8_t;

/* Holds the unchanging data about an opcode
 * such as how many cycles it takes to execute
 * and how many bytes it and its operands are
 */
class OpInfo
{
public:
    const dumbnes::opcodes::Mnemonic mnemonic;
    const dumbnes::opcodes::OpMode mode;
    const size_t length;
    const size_t time;
    const bool add_1_for_branch_taken;
    const bool add_1_for_page_crossing;
    
    OpInfo(
        dumbnes::opcodes::Mnemonic mnemonic,
        dumbnes::opcodes::OpMode mode,
        size_t length,
        size_t time,
        bool add_1_for_branch_taken = false,
        bool add_1_for_page_crossing = false
    ):mnemonic(mnemonic)
     ,mode(mode)
     ,length(length)
     ,time(time)
     ,add_1_for_branch_taken(add_1_for_branch_taken)
     ,add_1_for_page_crossing(add_1_for_page_crossing)
    {}

    static const OpInfo& Decode(Opcode opcode);
};


}} // namespace dumbnes::opcodes

std::ostream& operator<<(std::ostream& os, dumbnes::opcodes::OpInfo const& op);

#endif /* __OP_INFO_H */
