/* Copyright 2020 Adam Honse <calcprogrammer1@gmail.com>
 * Copyright 2020 Dimitris Mantzouranis <d3xter93@gmail.com>
 * Copyright 2022 Harrison Chan (Xelus)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "optical_iso.h"

#define XX NO_LED

__attribute__ ((weak))
led_config_t g_led_config = { {
  {  0, XX,  1, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,  13, 14, 15},
  { 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,  30, 31, 32},
  { 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, XX,  46, 47, 48},
  { 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,  XX, XX, XX},
  { 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, XX, 75,  XX, 76, XX},
  { 77, 78, 79, XX, XX, XX, 80, XX, XX, XX, 81, 82, 83, 84,  85, 86, 87}
}, {
    {0  ,  0}, {26 ,  0}, {38 ,  0}, {51 ,  0}, {64 ,  0}, {83 ,  0}, {96 ,  0}, {109,  0}, {122,  0}, {141,  0}, {154,  0}, {166,  0}, {179,  0},            {198,  0}, {211,  0}, {224,  0},
    {0  , 15}, {13 , 15}, {26 , 15}, {38 , 15}, {51 , 15}, {64 , 15}, {77 , 15}, {90 , 15}, {102, 15}, {115, 15}, {128, 15}, {141, 15}, {154, 15}, {173, 15}, {198, 15}, {211, 15}, {224, 15},
    {3  , 27}, {19 , 27}, {32 , 27}, {45 , 27}, {58 , 27}, {70 , 27}, {83 , 27}, {96 , 27}, {109, 27}, {122, 27}, {134, 27}, {147, 27}, {160, 27},            {198, 27}, {211, 27}, {224, 27},
    {5  , 40}, {22 , 40}, {35 , 40}, {48 , 40}, {61 , 40}, {74 , 40}, {86 , 40}, {99 , 40}, {112, 40}, {125, 40}, {138, 40}, {150, 40}, {163, 40}, {178, 34},
    {2  , 52}, {16 , 52}, {29 , 52}, {42 , 52}, {54 , 52}, {67 , 52}, {80 , 52}, {93 , 52}, {106, 52}, {118, 52}, {131, 52}, {144, 52},            {168, 52},            {211, 52},
    {2  , 64}, {18 , 64}, {34 , 64},                                  {82 , 64},                                  {130, 64}, {146, 64}, {162, 64}, {178, 64}, {198, 64}, {211, 64}, {224, 64}
}, {
   1, 4, 4, 4, 4, 1, 1, 1, 1, 4, 4, 4, 4,    1, 1, 1,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1,
   1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    1, 1, 1,
   1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1,
   1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    1,    1,
   1, 1, 1,          4,          1, 1, 1, 1, 1, 1, 1
} };
