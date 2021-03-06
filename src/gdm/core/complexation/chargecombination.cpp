#include "chargecombination.h"

#include <cassert>
#include <util_lowlevel.h>

bool gdm::operator==(ChargeCombination a, ChargeCombination b) noexcept
{
    return a.nucleusCharge == b.nucleusCharge
            && a.ligandCharge == b.ligandCharge;
}

bool gdm::operator!=(ChargeCombination a, ChargeCombination b) noexcept
{
    return !(a == b);
}

gdm::ChargeNumber gdm::charge(ChargeCombination obj, ConstituentType type) noexcept
{
    switch(type) {

    case ConstituentType::Nucleus:
        return obj.nucleusCharge;

    case ConstituentType::Ligand:
        return obj.ligandCharge;
    }

    IMPOSSIBLE_PATH;
}

void gdm::setCharge(ChargeCombination& obj, ConstituentType type, ChargeNumber charge) noexcept
{
    switch(type) {

    case ConstituentType::Nucleus:
        obj.nucleusCharge = charge;
        return;

    case ConstituentType::Ligand:
        obj.ligandCharge = charge;
        return;
    }

    IMPOSSIBLE_PATH;
}

