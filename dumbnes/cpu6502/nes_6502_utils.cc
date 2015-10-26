 #include "cpu6502/nes_6502_utils.h"

namespace dumbnes { namespace cpu6502 {

using namespace std;    

/*
 * returns 0 on success
 *         1,2 on invalid parse
 */
int ParseAddressAndLength(std::string const& address_arg,
        uint16_t &start_addr,
        uint16_t &num_bytes)
{
    regex re_single ("(?:0x)?([[:xdigit:]]+)");
    regex re_range ("(?:0x)?([[:xdigit:]]+)--(?:0x)?([[:xdigit:]]+)");
    regex re_count ("(?:0x)?([[:xdigit:]]+)\\+\\+([\\d]+)");

    smatch sm;
    if (regex_match(address_arg.cbegin(), address_arg.cend(), sm, re_single))
    {
        start_addr = uint16_t(0xFFFF & stoul(sm[1], nullptr, 16));
        num_bytes = 1;
     }
    else if (regex_match(address_arg.cbegin(), address_arg.cend(), sm, re_range))
    {
        start_addr = uint16_t(0xFFFF & stoul(sm[1], nullptr, 16));
        uint16_t end_addr = uint16_t(0xFFFF & stoul(sm[2], nullptr, 16));
        num_bytes = end_addr - start_addr + 1;
    }
    else if (regex_match(address_arg.cbegin(), address_arg.cend(), sm, re_count))
    {
        start_addr = uint16_t(0xFFFF & stoul(sm[1], nullptr, 16));
        num_bytes = uint16_t(0xFFFF & stoul(sm[2], nullptr, 10));
    }
    else
    {
        return 1; // parse error
    }

    return 0;
}

}}
