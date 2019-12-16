/*
 * Muse.h
 *
 *  Created on: 16 dic. 2019
 *      Author: utnso
 */

#ifndef MUSE_H_
#define MUSE_H_


#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <Conexiones/Conexiones.h>
#include <SerializacionMuse/Serializacion.h>
#include <pthread.h>


t_log *logger;

void iniciar_logger();

#endif /* MUSE_H_ */
