#ifndef __OP_INFO_H
#define __OP_INFO_H

#include "dumbnes_includes.h"
namespace dumbnes
{
using Opcode = uint8_t;

/* Holds the unchanging data about an opcode
 * such as how many cycles it takes to execute
 * and how many bytes it and its operands are
 */
class OpInfo
{
public:
    const dumbnes::Mnemonic mnemonic;
    const dumbnes::OpMode mode;
    const size_t length;
    const size_t time;
    const bool add_1_for_branch_taken;
    const bool add_1_for_page_crossing;
    
    OpInfo(
        dumbnes::Mnemonic mnemonic,
        dumbnes::OpMode mode,
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

/* This class tracks an operation in progress,
 * mainly for branching, since the cycles consumed
 * and and next_pc can't be determined until
 * operands are evaluated, etc.
 */
class OpResult
{
public:
    bool branch_taken;
    bool page_crossed;
    size_t cycles_run;
    uint16_t next_pc;
    OpResult(void):
        branch_taken(false)
        , page_crossed(false)
        , cycles_run(0)
        , next_pc(0)
    {}

    OpResult(uint16_t current_pc, OpInfo const& op_info) :
        branch_taken(false)
        , page_crossed(false)
        , cycles_run(op_info.time)
        , next_pc(current_pc + op_info.length)
    {}
};



} // namespace dumbnes

std::ostream& operator<<(std::ostream& os, const dumbnes::OpInfo& op);

#endif /* __OP_INFO_H */
