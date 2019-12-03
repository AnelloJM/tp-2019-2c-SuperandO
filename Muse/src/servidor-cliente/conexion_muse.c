#include "conexion_muse.h"


uint32_t iniciar_servidor(char *ip,uint32_t puerto)  //estaria bueno que el logger no se maneje aca
{

  serv.sin_family = AF_INET;
  serv.sin_port = htons(puerto); //Define the port at which the server will listen for connections.
  serv.sin_addr.s_addr = INADDR_ANY;
  fd = socket(AF_INET, SOCK_STREAM, 0); //This will create a new socket and also return the identifier of the socket into fd.
  // To handle errors, you can add an if condition that checks whether fd is greater than 0. If it isn't, prompt an error


  bind(fd, (struct sockaddr *)&serv, sizeof(serv)); //assigns the address specified by serv to the socket
  listen(fd,5); //Listen for client connections. Maximum 5 connections will be permitted.
  //Now we start handling the connections.
  printf("Servidor levantado en la direccion %s:%d\n",ip,puerto);
  while(conn = accept(fd, (struct sockaddr *)NULL, NULL)) {
      int pid;
      if((pid = fork()) == 0) {
            while (recv(conn, message, 100, 0)>0) {
              printf("Message Received: %s\n", message);
                  //An extra breaking condition can be added here (to terminate the child process)

              }
            exit(0);
      }

    }
    return 0;
}

uint32_t conectarse_a_servidor(char *ip,uint32_t puerto)
{
  fd = socket(AF_INET, SOCK_STREAM, 0);
  serv.sin_family = AF_INET;
  serv.sin_port = htons(puerto);

  inet_pton(AF_INET, ip, &serv.sin_addr); //This binds the client to localhost
  connect(fd, (struct sockaddr *)&serv, sizeof(serv)); //This connects the client to the server.

  return fd;
}

void enviar_cod_op(uint32_t destino,uint32_t codigo,uint32_t tamanio_enviar)
{
  void * buffer = malloc(8);
  estructura_codigo estructura_envio;
  memcpy(buffer,&(estructura_envio.codigo_op),codigo);
  memcpy(buffer+4,&(estructura_envio.tamanio_buffer),tamanio_enviar);
  send(destino,buffer,8,0);
  free(buffer);
}

void recibir_mensaje(uint32_t destinatario,uint32_t tamanio_recibir)
{
  void *buffer = malloc(4+tamanio_recibir);
  recv(destinatario,buffer,4+tamanio_recibir,0);
  estructura_recibir *paquete = malloc(sizeof(estructura_recibir));
  memcpy(&(paquete->operacion),buffer,4);
  memcpy(&(paquete->valor),buffer+4,tamanio_recibir);
    
    //tratar_operacion(struct estructura_recibir)
  free(buffer);
}

void recibir_primer_mensaje(uint32_t destinatario)
{
  void *buffer = malloc(4);
  recv(destinatario,buffer,4,0);

  recibir_mensaje(destinatario,buffer);

  free(buffer);

}

void enviar_primer_mensaje(uint32_t destino,uint32_t tamanio_enviar)
{
  void *buffer = malloc(4);
  memcpy(buffer,tamanio_enviar,4);
  send(destino,buffer,4,0);
  free(buffer);
}

void enviar_mensaje(uint32_t destino,uint32_t codigo_op,char parametros)
{
  size_t largo_de_parametros = sizeof(parametros);
  void *buffer = malloc(8+largo_de_parametros);
  memcpy(buffer,codigo_op,4);
  memcpy(buffer+4,largo_de_parametros,4);
  memcpy(buffer+8,parametros,largo_de_parametros);
  send(destino,buffer,8+largo_de_parametros,0);
  printf("operacion enviada!\n");
  free(buffer);
}



int main(int argc, char const *argv[]) {
  printf("asdasdas\n");
  return 0;
}
