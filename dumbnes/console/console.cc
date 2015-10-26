#include "console.h"
#include "dumbnes_config.h"
#include "dumbnes_includes.h"

namespace dumbnes
{
namespace console
{

using dumbnes::memory::IMemory;
using namespace std;
using namespace dumbnes::cpu6502;

void Console::ConsoleThread(void)
{
    PrintHeader(cout);
    while (!console_thread_kill_)
    {
        char *command = rl_gets();
        if (nullptr != command)
        {
            auto tokens = Tokenize(command);
            if (tokens.size() > 0)
            {
                auto lookup = handlers_.find(tokens[0]);
                if (lookup == handlers_.end())
                {
                    cout << "Unknown command: " << tokens[0] << endl;
                }
                else
                {
                    auto handler = lookup->second;
                    handler(tokens, std::cout);
                }
            }
        }
    }
}


Console::Console(shared_ptr<dumbnes::cpu6502::Nes6502> cpu,
                 shared_ptr<dumbnes::memory::IMemory> memory,
                 shared_ptr<dumbnes::ppu::Ppu> ppu)
    : console_thread_()
    , console_thread_kill_(false)
    , cpu_(cpu)
    , memory_(memory)
    , ppu_(ppu)
    , handlers_()
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    handlers_["?"] = std::bind(&Console::HandleHelp, this, _1, _2);
    handlers_["quit"] = std::bind(&Console::HandleQuit, this, _1, _2);
    handlers_["registers"] = std::bind(&Console::HandleRegisters, this, _1, _2);
    handlers_["step"] = std::bind(&Console::HandleStep, this, _1, _2);
    handlers_["setreg"] = std::bind(&Console::HandleSetReg, this, _1, _2);
    handlers_["memdump"] = std::bind(&Console::HandleMemdump, this, _1, _2);
    handlers_["memset"] = std::bind(&Console::HandleMemset, this, _1, _2);
}

    
void Console::StartPrompt(void)
{
    console_thread_ = thread(&Console::ConsoleThread, this);
}

bool Console::PromptRunning(void) const
{
    // the console might terminate if the user runs 'quit'
    // In this case, 'quit' signals console_thread_kill_,
    // and the application must close (only then does
    // the thread actually get killed - in the ~Console dtor).
    return !console_thread_kill_;
}

Console::~Console(void)
{
    console_thread_kill_ = true;
    if (console_thread_.joinable())
    {
        console_thread_.join();
    }
}

void Console::PrintHeader(std::ostream &out) const
{
    out << "Dumbnes Version " << Dumbnes_VERSION_MAJOR
        << "." << Dumbnes_VERSION_MINOR << endl;
    std::vector<std::string> empty;
    HandleHelp(empty, out);
}


void Console::HandleHelp(std::vector<std::string> const& tokens, std::ostream& out) const
{
    out << "Dumbnes Console" << endl;
    out << "For help with a specific command, type '<command> ?'" << endl;
    for (auto command_handler : handlers_)
    {
        out << command_handler.first << endl;
    }
    out << endl;
}

void Console::HandleQuit(std::vector<std::string> const& tokens, std::ostream& out)
{
    out << "Terminating console thread" << endl;
    console_thread_kill_ = true;
}

void Console::HandleRegisters(std::vector<std::string> const& tokens, std::ostream& out) const
{
    out << hex << setfill('0')
        << "PC : 0x" << setw(4) << (int)(cpu_->PC()) << endl
        << "SP : 0x" << setw(2) << (int)(cpu_->SP()) << endl
        << "A  : 0x" << setw(2) << (int)(cpu_->A()) << endl
        << "X  : 0x" << setw(2) << (int)(cpu_->X()) << endl
        << "Y  : 0x" << setw(2) << (int)(cpu_->Y()) << endl
        << "SR : 0x" << setw(2) << (int)(cpu_->SR()) << endl
        << "+--------------------------------" << endl
        << "| N | V |   | B | D | I | Z | C |" << endl
        << "| " << setw(1) << cpu_->GetStatus(SR_S) << " "
        << "| " << setw(1) << cpu_->GetStatus(SR_V) << " "
        << "|   "
        << "| " << setw(1) << cpu_->GetStatus(SR_B) << " "
        << "| " << setw(1) << cpu_->GetStatus(SR_D) << " "
        << "| " << setw(1) << cpu_->GetStatus(SR_I) << " "
        << "| " << setw(1) << cpu_->GetStatus(SR_Z) << " "
        << "| " << setw(1) << cpu_->GetStatus(SR_C) << " |" << endl
        << "+-------------------------------+" << endl
        << dec;
}
  
void Console::HandleStep(std::vector<std::string> const& tokens, std::ostream& out) const
{
    cpu_->Step();
    out << "PC : 0x" << cpu_->PC() << endl;
}

void Console::HandleSetReg(std::vector<std::string> const& tokens, std::ostream& out) const
{
    if (tokens.size() < 3)
    {
        out << "usage error: \"setreg <regname> <newvalue>\"" << endl;
        return;
    }
    auto regname = tokens[1];
    std::transform(regname.begin(), regname.end(), regname.begin(), ::toupper);
    auto new_val = std::stoi(tokens[2], nullptr, 16);
    if (regname == "PC")
    {
        cpu_->PC(new_val & 0xFFFF);
    }
    if (regname == "A")
    {
        cpu_->A(new_val & 0xFF);
    }
    if (regname == "X")
    {
        cpu_->X(new_val & 0xFF);
    }
    if (regname == "Y")
    {
        cpu_->Y(new_val & 0xFF);
    }
    if (regname == "SR")
    {
        cpu_->SR(new_val & 0xFF);
    }
    if (regname == "SP")
    {
        cpu_->SP(new_val & 0xFF);
    }
}

void Console::HandleMemdump(std::vector<std::string> const& tokens, std::ostream& out) const
{
    if (tokens.size() > 1 && tokens[1] == "?")
    {
        out << "memdump 0x12    # print data at address 0x12" << endl;
        out << "memset 0x10--1f # print data at 0x10-0x1f" << endl;
        out << "memset 0x10++10 # print data at 0x10-0x1f" << endl;
        return;
    }

    if (tokens.size() < 2)
    {
        out << "usage error: \"memdump <address or range>\"" << endl;
        return;
    }
    int res = 0;
    uint16_t start_addr = 0;
    uint16_t num_bytes = 0;
    res = dumbnes::cpu6502::ParseAddressAndLength(
            tokens[1], start_addr, num_bytes);
    if (res != 0)
    {
        out << "usage error: \"memdump address_range\"" << endl;
        return;
    }
    const size_t bytes_per_line = 8;
    for (size_t i = 0; i < num_bytes; /*update in body*/)
    {
        size_t bytes_on_line = min(bytes_per_line, num_bytes - i);
        out << hex << setw(4) << setfill('0') << (start_addr + i);
        out << " ";
        for (size_t b = 0; b < bytes_on_line; ++b)
        {
            out << " " << setw(2) << (int)(memory_->R(start_addr + i));
            i++;
        }
        out << dec << endl;
    }
}

void Console::HandleMemset(std::vector<std::string> const& tokens, std::ostream& out) const
{
    if (tokens.size() > 1 && tokens[1] == "?")
    {
        out << "memset 0x12 22     # set data at 0x12 to 0x22" << endl;
        out << "memset 0x10--1f 21 # set data at 0x10-0x1f to 0x22." << endl;
        out << "memset 0x10++10 a8 # set data at 0x10-0x1f to 0xa8." << endl;
        return;
    }
    if (tokens.size() < 3)
    {
        out << "usage error: \"memset <address or range> <newvalue>\"" << endl;
        return;
    }

    int res = 0;
    uint16_t start_addr = 0;
    uint16_t num_bytes = 0;
    if (0 != dumbnes::cpu6502::ParseAddressAndLength(
            tokens[1], start_addr, num_bytes))
    {
        out << "usage error: \"memset <address or range> <newvalue>\"" << endl;
        return;
    }
    uint8_t value = uint8_t(0xFF & stoul(tokens[2], nullptr, 16));
    for (uint16_t i = 0; i < num_bytes; ++i)
    {
        memory_->W(start_addr + i, value);
    }
}

std::vector<std::string> Tokenize(std::string const& line)
{
    istringstream linestream(line);
    string buff;
    vector<string> tokens;
    while (getline(linestream, buff, ' '))
    {
        if (buff.size() > 0)
        {
            tokens.push_back(buff);
        }
    }
    return tokens;
}


/* Read a string, and return a pointer to it.  Returns NULL on EOF.
 * proudly stolen from http://web.mit.edu/gnu/doc/html/rlman_2.html
 * modified slighty*/
char *
rl_gets ()
{
    /* A static variable for holding the line. */
    static char *line_read = (char *)NULL;
    /* If the buffer has already been allocated, return the memory
    to the free pool. */
    if (line_read)
    {
        free (line_read);
        line_read = (char *)NULL;
    }

    /* Get a line from the user. */
    line_read = readline (">");

    /* If the line has any text in it, save it on the history. */
    if (line_read && *line_read)
    {
        add_history (line_read);
    }

    return (line_read);
}

}
}

