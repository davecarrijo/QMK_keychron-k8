#pragma once

#include <stdint.h>
#include <stdbool.h>

void SN32F26x_init(void);
void SN32F26x_set_value(int index, uint8_t value);
void SN32F26x_set_value_all(uint8_t value);
void matrix_scan_keys(matrix_row_t raw_matrix[], uint8_t current);