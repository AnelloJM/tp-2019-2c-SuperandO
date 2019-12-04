#include "conexion_muse.h"
#include "conexion_muse.c"

int main()
{

  pthread_t server;
  uint32_t puerto = 8003;
  printf("Levantamos servidor..\n" );

  pthread_create(&server,NULL, &iniciar_servidor, puerto);
  pthread_join(server,NULL);


  return 0;
}
