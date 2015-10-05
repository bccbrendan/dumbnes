#ifndef __ASSEMBLER_H
#define __ASSEMBLER_H

#include "dumbnes_includes.h"

namespace dumbnes { namespace assembler {

std::map<std::string, uint16_t>
ParseLabels(std::vector<std::string> const& assembly_lines);

void
AssembleLine(std::string const& asm_line,
             uint8_t *bytes,
             uint8_t &num_bytes,
             std::map<std::string, uint16_t> const& lables);

}}

#endif /* __ASSEMBLER_H */

