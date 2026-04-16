#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct PretendYoureInFranciade : AutoAssemblerCodeHolder_Base
{
    PretendYoureInFranciade();
};
PretendYoureInFranciade::PretendYoureInFranciade()
{
    DEFINE_ADDR(whenCheckingIfPlayerIsInFranciade, 0x141D242B0);
    whenCheckingIfPlayerIsInFranciade = {
        "B0 01"                // - mov al,01
        "C3"                   // - ret
    };
}
