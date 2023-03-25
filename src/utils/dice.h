//
// Created by henry on 3/24/23.
//

#ifndef JAMEGAM_DICE_H
#define JAMEGAM_DICE_H

#include <hagame/utils/random.h>

template <size_t Sides>
class Dice {
public:

    Dice(int value = 0):
        m_value(value)
        {}

    int roll() {
        m_value = m_random.integer<int>(0, Sides);
        return m_value;
    }

    int value() const { return m_value; }

private:

    int m_value;
    hg::utils::Random m_random;

};

#endif //JAMEGAM_DICE_H
