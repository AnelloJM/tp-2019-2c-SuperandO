#ifndef CLIENTE_H_
#define CLIENTE_H_
#include "../conexion_muse.h"
#include "../conexion_muse.c"


uint32_t server;

uint32_t muse_alloc(uint32_t tam);
void muse_mensaje(void * mensaje);
#endif
