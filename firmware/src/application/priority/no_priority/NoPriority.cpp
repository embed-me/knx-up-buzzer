#include "NoPriority.hpp"

using namespace application::priority;
    

bool NoPriority::isGranted(struct drivers::knx::MelodyConfig melodyCfg) const 
{
    return true;
}
