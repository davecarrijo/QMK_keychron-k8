#include "led_matrix.h"
#include "sn32f26x.h"

#if !defined(LED_MATRIX_LED_FLUSH_LIMIT)
#    define LED_MATRIX_LED_FLUSH_LIMIT 16
#endif

#if !defined(LED_MATRIX_VAL_STEP)
#    define LED_MATRIX_VAL_STEP 16
#endif

#if !defined(LED_MATRIX_SPD_STEP)
#    define LED_MATRIX_SPD_STEP 16
#endif

/*
    COLS key / led
    SS8050 transistors NPN driven low
    base      - GPIO
    collector - LED Col pins
    emitter   - VDD

    VDD     GPIO
    (E)     (B)
     |  PNP  |
     |_______|
         |
         |
        (C)
        LED

    ROWS LED
    SS8550 transistors PNP driven high
    base      - GPIO
    collector - LED row pins
    emitter   - GND

        LED
        (C)
         |
         |
      _______
     |  NPN  |
     |       |
    (B)     (E)
    GPIO    GND
*/
static uint8_t        chan_col_order[LED_MATRIX_COLS] = {0};  // track the channel col order
static uint8_t        current_row                     = 0;    // LED row scan counter
static uint8_t        row_idx                         = 0;    // key row scan counter
extern matrix_row_t   raw_matrix[MATRIX_ROWS];                // raw values
static const uint32_t periodticks                   = 256;
static const uint32_t freq                          = (LED_MATRIX_LED_FLUSH_LIMIT * LED_MATRIX_VAL_STEP * LED_MATRIX_SPD_STEP * LED_MATRIX_LED_PROCESS_LIMIT);
static const pin_t    led_row_pins[LED_MATRIX_ROWS] = LED_MATRIX_ROW_PINS;  // We expect a R,B,G order here
static const pin_t    led_col_pins[LED_MATRIX_COLS] = LED_MATRIX_COL_PINS;
uint8_t               led_state[DRIVER_LED_TOTAL];  // LED state buffer
bool                  enable_pwm = false;

void led_ch_ctrl(void) {
    /* Enable PWM function, IOs and select the PWM modes for the LED column pins */
    for (uint8_t i = 0; i < LED_MATRIX_COLS; i++) {
        switch (led_col_pins[i]) {
            case A0:
                chan_col_order[i] = 0;
                break;

            case A1:
                chan_col_order[i] = 1;
                break;

            case A2:
                chan_col_order[i] = 2;
                break;

            case A3:
                chan_col_order[i] = 3;
                break;

            case A4:
                chan_col_order[i] = 4;
                break;

            case A5:
                chan_col_order[i] = 5;
                break;

            case A6:
                chan_col_order[i] = 6;
                break;

            case A7:
                chan_col_order[i] = 7;
                break;

            case A8:
                chan_col_order[i] = 8;
                break;

            case A9:
                chan_col_order[i] = 9;
                break;

            case A10:
                chan_col_order[i] = 10;
                break;

            case A11:
                chan_col_order[i] = 11;
                break;

            case A12:
                chan_col_order[i] = 12;
                break;

            case A13:
                chan_col_order[i] = 13;
                break;

            case A14:
                chan_col_order[i] = 14;
                break;

            case A15:
                chan_col_order[i] = 15;
                break;

            case D0:
                chan_col_order[i] = 16;
                break;

            case D1:
                chan_col_order[i] = 17;
                break;

            case D2:
                chan_col_order[i] = 18;
                break;

            case D3:
                chan_col_order[i] = 19;
                break;

            case D5:
                chan_col_order[i] = 21;
                break;

            case D8:
                chan_col_order[i] = 22;
                break;
        }
    }
}

void led_callback(PWMDriver *pwmp);

/* PWM configuration structure. We use timer CT16B1 with 23 channels. */
static const PWMConfig pwmcfg = {
    freq,         /* PWM clock frequency. */
    periodticks,  /* PWM period (in ticks) 1S (1/10kHz=0.1mS 0.1ms*10000 ticks=1S) */
    led_callback, /* led Callback */
    .channels =
        {
/* Default all channels to disabled - Channels will be configured durring init */
#ifdef ACTIVATE_PWM_CHAN_0
            [0] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [0]  = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_1
            [1] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [1]  = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_2
            [2] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [2]  = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_3
            [3] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [3]  = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_4
            [4] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [4]  = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_5
            [5] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [5]  = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_6
            [6] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [6]  = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_7
            [7] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [7]  = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_8
            [8] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [8]  = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_9
            [9] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [9]  = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_10
            [10] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [10] = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_11
            [11] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [11] = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_12
            [12] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [12] = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_13
            [13] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [13] = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_14
            [14] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [14] = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_15
            [15] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [15] = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_16
            [16] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [16] = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_17
            [17] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [17] = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_18
            [18] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [18] = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_19
            [19] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [19] = {.mode = PWM_OUTPUT_DISABLED},
#endif /* Channel 20 is a dummy channel in 26x .*/
            [20] = {.mode = PWM_OUTPUT_DISABLED},
#ifdef ACTIVATE_PWM_CHAN_21
            [21] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [21] = {.mode = PWM_OUTPUT_DISABLED},
#endif
#ifdef ACTIVATE_PWM_CHAN_22
            [22] = {.mode = PWM_OUTPUT_ACTIVE_LOW},
#else
            [22] = {.mode = PWM_OUTPUT_DISABLED},
#endif
        },
    0 /* HW dependent part.*/
};

void shared_matrix_led_enable(void) { pwmEnablePeriodicNotification(&PWMD1); }

void shared_matrix_led_disable_pwm(void) {
    // Disable PWM outputs on column pins
    for (uint8_t y = 0; y < LED_MATRIX_COLS; y++) {
        pwmDisableChannel(&PWMD1, chan_col_order[y]);
    }
}

void shared_matrix_led_disable_leds(void) {
    // Disable LED outputs on channel pins
    for (uint8_t x = 0; x < LED_MATRIX_ROWS; x++) {
        writePinLow(led_row_pins[x]);
    }
}

void update_pwm_channels(PWMDriver *pwmp) {
    for (uint8_t col_idx = 0; col_idx < LED_MATRIX_COLS; col_idx++) {
#if (DIODE_DIRECTION == ROW2COL)
        // Scan the key matrix column
        matrix_scan_keys(raw_matrix, col_idx);
#endif
        uint8_t led_index = g_led_config.matrix_co[row_idx][col_idx];
        // Check if we need to enable LED output
        if (led_state[led_index] != 0) enable_pwm |= true;
        // Update matching LED channel PWM configuration
        switch (current_row % LED_MATRIX_ROW_CHANNELS) {
            case 0:
                if (enable_pwm) pwmEnableChannelI(pwmp, chan_col_order[col_idx], led_state[led_index]);
                break;
            default:;
        }
    }
}
void led_callback(PWMDriver *pwmp) {
    // Disable the interrupt
    pwmDisablePeriodicNotification(pwmp);
    // Advance to the next LED channel
    current_row++;
    if (current_row >= LED_MATRIX_ROWS) current_row = 0;
    // Advance to the next key matrix row
    if (current_row % LED_MATRIX_ROW_CHANNELS == 0) row_idx++;
    if (row_idx >= LED_MATRIX_ROWS) row_idx = 0;
    chSysLockFromISR();
    // Disable LED output before scanning the key matrix
    shared_matrix_led_disable_leds();
    shared_matrix_led_disable_pwm();
#if (DIODE_DIRECTION == COL2ROW)
    // Scan the key matrix row
    matrix_scan_keys(raw_matrix, row_idx);
#endif
    update_pwm_channels(pwmp);
    if (enable_pwm) writePinHigh(led_row_pins[current_row]);
    chSysUnlockFromISR();
    // Advance the timer to just before the wrap-around, that will start a new PWM cycle
    pwm_lld_change_counter(pwmp, 0xFFFF);
    // Enable the interrupt
    pwmEnablePeriodicNotification(pwmp);
}

void SN32F26x_init(void) {
    for (uint8_t x = 0; x < LED_MATRIX_ROWS; x++) {
        setPinOutput(led_row_pins[x]);
        writePinLow(led_row_pins[x]);
    }
    // Determine which PWM channels we need to control
    led_ch_ctrl();
    pwmStart(&PWMD1, &pwmcfg);
    shared_matrix_led_enable();
}

void SN32F26x_set_value(int index, uint8_t value) { led_state[index] = value; }

void SN32F26x_set_value_all(uint8_t value) {
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        SN32F26x_set_value(i, value);
    }
}
