#ifndef SUSE2_H
#define SUSE2_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include "/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Conexiones/Conexiones.h"
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Serializacion/serializacion.h"
#include <commons/collections/queue.h>
#include <pthread.h>
#include "/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Lista/lista.h"
#include <time.h>
#include <semaphore.h>
#include "SerializacionSUSE/SerializacionSUSE.h"

//ESTRUCTURAS
t_log* logger;
t_config* archivoConfig;
t_list * cola_new;
t_list * cola_blocked;
t_list * cola_exit;
t_list * semaforos;
t_list * lista_programas;

typedef struct {
	int pid;
	int tid;
	int estimacionAnterior;
	float rafagasEjecutadas;
	float rafagasEstimadas;
	char * razon_bloqueado;
	bool finalizado;
	//Tiempo entre la creacion y la toma de la metrica
	int tiempoEjecucionInicial;
	int tiempoEjecucion;
	//Tiempo que pasa en el estado READY
	int tiempoEsperaInicial; /*CUANDO LO MANDE A READY TENGO QUE TOMAR EL TIEMPO DE ESE INSTANTE, NO OLVIDARSE (CUANDO PASA DE NEW A READY)*/
	int tiempoEsperaFinal;
	int tiempo_espera;
	//Tiempo que pasa en el estado EXEC
	int tiempoUsoCPUInicial;
	int tiempoUsoCPUFinal;
	int tiempoUsoCPU;
	//Tiempo ejecucion del hilo dividido el total de tiempo de ejecucion de todos los hilos del proceso
	float porcentajeTiempoEjecucion;
} t_hilo;

typedef struct {
	char* semID;
	int semInit;
	int semActual;
	int semMax;
	t_list* hilosEnEspera;
} t_semaforo;

typedef struct {
	int pid;
	t_list * hilos;
	t_list * cola_ready;
	t_list * cola_exec;
} t_programa;

//VARIABLES
char* suse_ip;
char* suse_port;
int metrics_timer;
int max_multiprog;
char** sems_ids_suse;
char** sem_init_suse;
char** sem_max_suse;
double alpha_sjf;
int socket_suse;
int socket_cliente;
int pidMAX = 1;
int tidMAX = 1;

//FUNCIONES
void crearLogger();
void leerArchivoDeConfiguracion();
void setearValores(t_config* archivoConfig);
void inicializarEstructuras();
void inicializarSemaforos();
int posicionFinalDoblePuntero(char **puntero);
void liberarDoblePuntero(char **puntero);
bool comparadorPrograma(int unPid, t_programa* unPrograma);

//FUNCIONES DE SUSE
int suse_create(int pid);
int suse_schedule_next(int pid);
int suse_wait(int pid, char* semaforoID);
int suse_signal(int pid, char* semaforoID);
int suse_join(int pid, int tid);
int suse_close(int pid, int tid);
//FUNCION MAGICA
void* atenderCliente(int socket_cliente); //Mi funcion magica

#endif
