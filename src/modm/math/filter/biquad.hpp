/*
 * Copyright (c) 2025, Hannes Ellinger
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BIQUAD_HPP
#define MODM_BIQUAD_HPP

namespace modm
{
	namespace filter
    {

        /**             
            xn        |\ B0                    yn
            O----+----| >----(+)-----------+----O
                 |    |/      |            |
              [xn z1]         |         [yn z1]
                 |    |\ B1   |  -A1 /|    |
                 +----| >----(+)----< |----+
                 |    |/      |      \|    |
              [xn z2]         |         [yn z2]
                 |    |\ B2   |  -A2 /|    |
                 +----| >----(+)----< |----+
                      |/             \|
        */
        

        template<typename T, T B0, T B1, T B2, T A1, T A2>
        class Biquad {
        public:

            Biquad() : xnz1(0), xnz2(0), ynz1(0), ynz2(0) {};
            virtual ~Biquad() {};

            /**
            * \brief	Reset all values
            */
            inline void
            reset()
            {
                xnz1 = 0;
                xnz2 = 0;
                ynz1 = 0;
                ynz2 = 0;
            };

            inline void
            update(const T& input)
            {
                // Input
                T xn = input;

                // Output
                yn = B0*xn + B1*xnz1 + B2*xnz2- A1*ynz1 - A2*ynz2;

                // Buffers
                xnz2 = xnz1;
                xnz1 = xn;
                ynz2 = ynz1;
                ynz1 = yn;
            }

            inline const T&
            getValue() const
            {
                return yn;
            }

        private:
            T yn;
            T xnz1, xnz2, ynz1, ynz2;
        };

    }
}

#endif // MODM_BIQUAD_HPP
