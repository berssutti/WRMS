#ifndef MY_NVS_H
#define MY_NVS_H

void nvs_init();

void nvs_write(int32_t valor, char *name);

int32_t nvs_read(char *name);

#endif