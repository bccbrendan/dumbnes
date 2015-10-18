#include "console.h"
#include "dumbnes_config.h"
#include "dumbnes_includes.h"

namespace dumbnes
{
namespace console
{

using dumbnes::memory::IMemory;
using namespace std;

void Console::ConsoleThread(void)
{
    PrintHeader(cout);
    /*
    (*memory_)[0x0] = 0xa9; // LDA 
    (*memory_)[0x1] = 0xc0; // 
    cpu_->Step();
    cout << hex
              << "Cpu state dump: " << endl
              << " PC: 0x" << cpu_->PC() << endl
              << "  A: 0x" << cpu_->A() << endl
              ;
    */
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
}

    
void Console::StartPrompt(void)
{
    console_thread_ = thread(&Console::ConsoleThread, this);
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
    line_read = readline (">t");

    /* If the line has any text in it, save it on the history. */
    if (line_read && *line_read)
    {
        add_history (line_read);
    }

    return (line_read);
}

}
}

