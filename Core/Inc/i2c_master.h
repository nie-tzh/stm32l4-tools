#ifndef __I2C_MASTER_H__
#define __I2C_MASTER_H__

void MX_I2C1_Init(unsigned int rate);
void MX_I2C2_Init(unsigned int rate);
void shell_test_i2c_detect(void *arg);
void shell_test_i2c_rate_change(void *arg);
void shell_test_i2c_send_message(void *arg);

#endif