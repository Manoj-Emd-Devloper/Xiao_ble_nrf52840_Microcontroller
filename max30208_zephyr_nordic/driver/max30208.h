#ifndef _MAX30208_H
#define _MAX30208_H

#define DT_DRV_COMPAT maxim_max30208

/* Includes */
#include <zephyr/drivers/i2c.h>

#define MAX30208_DEBUG 1
// #define USE_DO_WHILE 1

// Temperature Register
#define ALARM_HIGH_MSB 0x10
#define ALARM_HIGH_LSB 0x11
#define ALARM_LOW_MSB 0x12
#define ALARM_LOW_LSB 0x13
#define TEMP_SET 0x14

#define FIFO_DATA 0x08
#define FIFO_CONFIGURATION1 0x09
#define FIFO_CONFIGURATION2 0x0A
#define FIFO_OVERFLOW_COUNTER 0x06
#define FIFO_DATA_COUNTER 0x07

#define DELAY 2000

// Interrupt config
#define INTERRUPT_CONF 0x01
#define NORMAL_MODE 0x00

// Part ID Register
#define PART_ID_ADDR 0xFF
#define PART_IDENTIFIER_VAL 0x30

// SYSTEM reset
#define MAX30208_SYSTEM_REG 0x0C

// Status reg
#define STATUS_REG 0x00

#define GPIO_SETUP 0x20
#define GPIO_CONTROL 0x21

// max30208 functions
int read_part_id(const struct i2c_dt_spec *spec, uint8_t *data);
int8_t read_status(const struct i2c_dt_spec *spec, uint8_t *data);
int max30208_setmode(const struct i2c_dt_spec *spec);
int reset_max30208(const struct i2c_dt_spec *spec);
int32_t max30208_get_Status(const struct i2c_dt_spec *spec, uint8_t *value);

int32_t max30208_get_FIFOConfig1(const struct i2c_dt_spec *spec, uint8_t *value);
int32_t max30208_set_FIFOConfig1(const struct i2c_dt_spec *spec, uint8_t config);
int32_t max30208_set_FIFOConfig2(const struct i2c_dt_spec *spec, uint8_t config);
int max30208_get_FifoData(const struct i2c_dt_spec *spec, uint8_t value[2]);
int32_t max30208_get_Overflow(const struct i2c_dt_spec *spec, uint8_t *value);
int32_t max30208_get_DataCounter(const struct i2c_dt_spec *spec, uint8_t *value);

int max30208_init(const struct i2c_dt_spec *spec);

int max30208_takeDataMeasurement(const struct i2c_dt_spec *spec);
int8_t MAX30208_Trigger_Conversion(const struct i2c_dt_spec *spec);
int read_temp(const struct i2c_dt_spec *spec, float *Temp);
int32_t MAX30208_GetRawData(const struct i2c_dt_spec *spec, uint16_t *data);
uint16_t Get_Skin_Temperature(void);

uint8_t read_register(const struct i2c_dt_spec *spec, uint8_t reg, uint8_t *val, uint8_t rd_len);
uint8_t write_register(const struct i2c_dt_spec *spec, uint8_t *reg, uint8_t wd_len);

float max30208_toCelsius(uint16_t rawTemp);
float max30208_toFahrenheit(float temperatureC);

int32_t max30208_set_GPIOControl(const struct i2c_dt_spec *spec);
int32_t max30208_configure_gpio(const struct i2c_dt_spec *spec);
int32_t max30208_enable_interrupts(const struct i2c_dt_spec *spec);
#endif