#include "conexion_muse.h"


void enviar_mensaje(uint32_t destino,char mensaje)
{
  printf("Enter a message: ");
    //comando a enviar
  fgets(message, 100, stdin);
  send(destino, message, 100, 0);
//vaciamos buffer


}

void recibir_mensaje(uint32_t destinatario)
{

  printf("Esperando mensaje \n" );
  //esperamos respuesta del servidor
  recv(destinatario, message, 100, 0);
  printf("Mensaje : %s\n",message);
}

void conectarse_a_servidor(char *ip,uint32_t puerto)
{
  fd = socket(AF_INET, SOCK_STREAM, 0);
  serv.sin_family = AF_INET;
  serv.sin_port = htons(puerto);

  inet_pton(AF_INET, ip, &serv.sin_addr); //This binds the client to localhost
  connect(fd, (struct sockaddr *)&serv, sizeof(serv)); //This connects the client to the server.

  enviar_mensaje(fd,"hola");
  
  recibir_mensaje(fd);
}
