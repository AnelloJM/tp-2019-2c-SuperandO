#include "conexion_muse.h"
#include "conexion_muse.c"


int main()
{
  printf("Levantamos servidor..\n" );

  iniciar_servidor("127.0.0.1",8003);


  return 0;
}
