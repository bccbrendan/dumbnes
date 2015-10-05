#ifndef __OP_RESULT_H
#define __OP_RESULT_H

#include "dumbnes_includes.h"
#include "opcodes/opcodes.h"
#include "opcodes/op_info.h"

namespace dumbnes { namespace cpu6502 {

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

    OpResult(uint16_t current_pc, dumbnes::opcodes::OpInfo const& op_info);
};



}} // namespace dumbnes::cpu6502

#endif /* __OP_RESULT_H */
