#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_
#include <stdlib.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <netinet/in.h>


//////////////////////////////////////////
//           Comunicacion Base          //
//////////////////////////////////////////

typedef struct {
	char * tipoMensaje;
	uint32_t quienEnvia; //FD del prog
	uint32_t tamanioMensaje;
}__attribute__((packed)) Header;

typedef struct {
	Header header;
	void* mensaje;
}__attribute__((packed)) Paquete;

//////////////////////////////////////////
//          Estructuras Utiles          //
//////////////////////////////////////////


typedef struct paqueteSemaforo{
	int tid;
	char* sem_name;
}paqueteSemaforo;

typedef struct {
	int socket;
	char* ID;
	t_list* clave;
	bool bloqueado;
	char* razon_bloqueo;//clave que lo bloqueo o "consola"
	int rafagas_ejecutadas;
	float rafagas_estimadas;
	int response_ratio;
	int tiempo_espera;
}__attribute__((packed)) t_ESIPlanificador;

typedef struct {
	int socket;
	char* nombre;
	bool estado_de_conexion;
	//bool flagEL;
	//t_list* claves;
}__attribute__((packed)) conexion_programa;

typedef struct {
//	t_Instancia* dato;
	int tamanio;
}__attribute__((packed)) t_Instancia_con_tamanio;

typedef struct {
	char* clave;
	int index;
	int entradasOcupadas;
	int tamanio;
}__attribute__((packed)) t_AlmacenamientoEntradaAdministrativa;

//////////////////////////////////////////
//              Funciones               //
//////////////////////////////////////////

bool EnviarPaquete(int socketCliente, Paquete* paquete);
/*
char* getNombreDelProceso(proceso proceso);

bool EnviarHandshake(int socketFD, proceso quienEnvia);

bool EnviarDatosTipo(int socketFD, proceso quienEnvia, void* datos, int tamDatos, t_protocolo tipoMensaje);


int RecibirDatos(void* paquete, int socketFD, uint32_t cantARecibir);

int RecibirPaqueteServidor(int socketFD, proceso quienEnvia, Paquete* paquete); //Responde al recibir un Handshake
*/
int RecibirPaqueteCliente(int socketFD, Paquete* paquete); //No responde los Handshakes


#endif /* SERIALIZACION_H_ */
