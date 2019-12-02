#include "conexion_muse.h"
#include "conexion_muse.c"


int main()
{
  printf("nos conectamos al servidor..\n" );

  uint32_t serverMuse = conectarse_a_servidor("127.0.0.1",8003);
  enviar_mensaje(serverMuse);
  return 0;
}
