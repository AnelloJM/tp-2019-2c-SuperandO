#include <ComunParaTodos/conexiones.h>
#include <ComunParaTodos/configs.h>
#include <ComunParaTodos/logs.h>
#include <stdbool.h>

/*FUNCIONES*/

void main();
void setearValores();

/*VARIABLES*/
char* listen_ip;
int listen_port;
int memory_size;
int page_size;
int swap_size;

int socket_servidor;
int socket_cliente;
