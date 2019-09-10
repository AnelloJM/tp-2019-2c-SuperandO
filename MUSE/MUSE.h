//Librerias
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

//Variables
char* listen_IP;
int listen_port;
int memory_size;
int page_size;
int swap_size;

//Funciones
void sigchld_handler(int s);
void levantarServidor();
void crearLogger(char* file, char *program_name, bool is_active_console, t_log_level level);
void leerConfig(char* configPath, t_log* logger);
void leerArchivoConfiguracion(char* configPath, t_log* logger);
void setearValores(t_config* archivoConfiguracion);
