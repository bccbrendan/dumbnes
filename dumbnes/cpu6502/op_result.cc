#include "cpu6502/op_result.h"

using namespace dumbnes::cpu6502;
using namespace dumbnes::opcodes;

OpResult::OpResult(uint16_t current_pc, OpInfo const& op_info) :
    branch_taken(false)
    , page_crossed(false)
    , cycles_run(op_info.time)
    , next_pc(current_pc + op_info.length)
{}
