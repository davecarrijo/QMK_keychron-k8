#pragma once

#include <stdint.h>
#include <stdbool.h>

void SN32F24xB_init(void);
void SN32F24xB_set_color(int index, uint8_t r, uint8_t g, uint8_t b);
void SN32F24xB_set_color_all(uint8_t r, uint8_t g, uint8_t b);
void matrix_scan_keys(matrix_row_t raw_matrix[], uint8_t current);