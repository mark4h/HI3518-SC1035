#ifdef SC1035_SENSOR_CTRL_H
#define SC1035_SENSOR_CTRL_H

void sensor_init();
int sensor_read_register(int addr);
int sensor_write_register(int addr, int data);
static void delay_ms(int ms);

#endif