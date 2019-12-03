#include "conexion_muse.h"
#include "conexion_muse.c"

typedef struct
{
  char ip;
  uint32_t puerto;
}Data_servidor;

int main()
{
/*  pthread_t proceso_cliente;
  Data_servidor *server;
  server->ip = "127.0.0.1";
  server->puerto=8003;
*/

  printf("nos conectamos al servidor..\n" );
  conectarse_a_servidor("127.0.0.1",8003);

//  pthread_create(&proceso_cliente,NULL, &conectarse_a_servidor, server);


//  pthread_join(proceso_cliente,NULL);


  return 0;
}
