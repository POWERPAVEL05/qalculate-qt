#ifndef H_TUTIL
#define H_TUTIL
#include <fstream>
#include <vector>
#include <string>

namespace htw 
{
    const std::vector<std::string> result_v {"Hallo I Bins","Paul","HAhaha noch eine Reihe!"};
    const std::vector<std::string> entry_v {"Dies ist eine Ausdruck","Lalbert","Und noch eine Reihe!"};
    const std::string logdir = "/src/htw/log.txt";

    extern std::ofstream *log;

    void init_log();
}

#endif