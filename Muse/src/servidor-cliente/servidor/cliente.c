#include "cliente.h"


int main()
{
  server = conectarse_a_servidor("127.0.0.1",5555);
  muse_alloc(15);
  //muse_mensaje("holaa");

  return 0;
}


uint32_t muse_alloc(uint32_t tam)
{
  Paquete *paquete = malloc(sizeof(Paquete));
  paquete->size_next = sizeof(tam);
  paquete->valor = tam;
  paquete->op = 0;


  enviar_paquete_v2(server,paquete);

  return 0;
}

void muse_mensaje(void * mensaje)
{
  Paquete *paquete = malloc(sizeof(Paquete));
  paquete->op = 0;
  memcpy(&(paquete->valor),mensaje,strlen(mensaje));
  paquete->size_next=strlen(mensaje);
  enviar_paquete_v2(server,paquete);
}
