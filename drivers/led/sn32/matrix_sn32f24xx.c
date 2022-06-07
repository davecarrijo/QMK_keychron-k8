/*
Copyright 2021 Dimitris Mantzouranis <d3xter93@gmail.com>
Copyright 2021 Adam Honse <calcprogrammer1@gmail.com>
Copyright 2011 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Ported to QMK by Stephen Peery <https://github.com/smp4488/>
*/

#include <stdint.h>
#include <stdbool.h>
#include "ch.h"
#include "hal.h"

#include "wait.h"
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "quantum.h"
#include "sn32f24xb.h"

#if defined(OPTICAL_MATRIX)
#ifndef PRESSED_KEY_PIN_STATE
#    define PRESSED_KEY_PIN_STATE 1
#endif
#endif

#ifndef PRESSED_KEY_PIN_STATE
#    define PRESSED_KEY_PIN_STATE 0
#endif

#ifndef MATRIX_KEY_SAMPLE_DELAY
#    define MATRIX_KEY_SAMPLE_DELAY 100
#endif

#if defined(MATRIX_KEY_SAMPLE_DELAY)
void sample_delay(void){
    //should give 2000/48000000Mhz = 42us delay
    //we want 42 micro thus to get value
    for (int i = 0; i < MATRIX_KEY_SAMPLE_DELAY; ++i) {
        __asm__ volatile("" ::: "memory");
    }
    //wait_us(73); //does not work
}
#endif

static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
matrix_row_t raw_matrix[MATRIX_ROWS]; //raw values
matrix_row_t last_matrix[MATRIX_ROWS] = {0};  // raw values
matrix_row_t matrix[MATRIX_ROWS]; //debounced values

static bool matrix_changed = false;

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }

__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }

__attribute__((weak)) void matrix_init_user(void) {}

__attribute__((weak)) void matrix_scan_user(void) {}

inline matrix_row_t matrix_get_row(uint8_t row) { return matrix[row]; }

void matrix_print(void) {}

static void init_pins(void) {
#if(DIODE_DIRECTION == ROW2COL)
    //  Unselect ROWs
    for (uint8_t x = 0; x < MATRIX_ROWS; x++) {
        setPinInputHigh(row_pins[x]);
    }
#elif(DIODE_DIRECTION == COL2ROW)
    // Unselect COLs
    for (uint8_t x = 0; x < MATRIX_COLS; x++) {
        setPinInputHigh(col_pins[x]);
    }
#else
#error DIODE_DIRECTION must be one of COL2ROW or ROW2COL!
#endif
}

void matrix_scan_select(uint8_t current){ 
#if(DIODE_DIRECTION == ROW2COL)
    // Select COL
    setPinOutput(col_pins[current]);
    writePinLow(col_pins[current]);
#elif(DIODE_DIRECTION == COL2ROW)
    // Select ROW
    setPinOutput(row_pins[current]);
    writePinLow(row_pins[current]);
#endif
}

void matrix_scan_unselect(uint8_t current) {
#if(DIODE_DIRECTION == ROW2COL)
    // Select COL
    setPinInputHigh(col_pins[current]);
#elif(DIODE_DIRECTION == COL2ROW)
    // Select ROW
    setPinInputHigh(row_pins[current]);
#endif
}

void matrix_init(void) {
    // initialize key pins
    init_pins();

    // initialize matrix state: all keys off
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        raw_matrix[i] = 0;
        matrix[i]     = 0;
    }

    debounce_init(MATRIX_ROWS);

    matrix_init_quantum();
}

uint8_t matrix_scan(void) {
    matrix_changed = false;
    for (uint8_t current_col = 0; current_col < MATRIX_COLS; current_col++) {
        for (uint8_t row_index = 0; row_index < MATRIX_ROWS; row_index++) {
            // Determine if the matrix changed state
            if ((last_matrix[row_index] != raw_matrix[row_index])) {
                matrix_changed         = true;
                last_matrix[row_index] = raw_matrix[row_index];
            }
        }
    }

    debounce(raw_matrix, matrix, MATRIX_ROWS, matrix_changed);

    matrix_scan_quantum();

    return matrix_changed;
}

void matrix_scan_keys(matrix_row_t raw_matrix[], uint8_t current){
        #if(DIODE_DIRECTION == ROW2COL)
            // Read the key matrix rows on col
            uint8_t col_index = current;
            // Enable the column
            matrix_scan_select(col_index);
            sample_delay();
            for (uint8_t row_index = 0; row_index < MATRIX_ROWS; row_index++) {
                // Check row pin state
                if (readPin(row_pins[row_index]) == PRESSED_KEY_PIN_STATE) {
                    // Pin LO, set col bit
                    raw_matrix[row_index] |= (MATRIX_ROW_SHIFTER << col_index);
                } else {
                    // Pin HI, clear col bit
                    raw_matrix[row_index] &= ~(MATRIX_ROW_SHIFTER << col_index);
                }
            }
            // Disable the column
            matrix_scan_unselect(col_index);
            //see https://github.com/SonixQMK/qmk_firmware/issues/157
            sample_delay();
        #elif(DIODE_DIRECTION == COL2ROW)
            // Read the key matrix cols on row
            uint8_t row_index = current;
            // Enable the row
            matrix_scan_select(row_index);
            sample_delay();
            for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++) {
                // Check row pin state
                if (readPin(col_pins[col_index]) == PRESSED_KEY_PIN_STATE) {
                    // Pin LO, set col bit
                    raw_matrix[row_index] |= (MATRIX_ROW_SHIFTER << col_index);
                } else {
                    // Pin HI, clear col bit
                    raw_matrix[row_index] &= ~(MATRIX_ROW_SHIFTER << col_index);
                }
            }
            // Disable the row
            matrix_scan_unselect(row_index);
            //see https://github.com/SonixQMK/qmk_firmware/issues/157
            sample_delay();
        #endif
}
