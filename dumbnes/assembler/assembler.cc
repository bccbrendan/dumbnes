#include "assembler.h"

using namespace dumbnes::assembler;

std::map<std::string, uint16_t>
ParseLabels(std::vector<std::string> const& assembly_lines)
{
    std::map<std::string, uint16_t> lables;
    throw std::logic_error("TODO implement!");
    return lables;
}

void
AssembleLine(std::string const& asm_line,
             uint8_t *bytes,
             uint8_t &num_bytes,
             std::map<std::string, uint16_t> const& lables)
{
    throw std::logic_error("TODO implement!");
}



