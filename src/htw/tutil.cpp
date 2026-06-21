#include "tutil.h"
#include <fstream>
#include <iostream>


void init_log()
{
    std::cout <<  htw::logdir;
    // htw::log = new std::ofstream(htw::logdir);
}