#include "conexion_muse.h"


void iniciar_servidor(uint32_t puerto)  //estaria bueno que el logger no se maneje aca
{
  serv.sin_family = AF_INET;
  serv.sin_port = htons(puerto); //Define the port at which the server will listen for connections.
  serv.sin_addr.s_addr = INADDR_ANY;
  fd = socket(AF_INET, SOCK_STREAM, 0); //This will create a new socket and also return the identifier of the socket into fd.
  // To handle errors, you can add an if condition that checks whether fd is greater than 0. If it isn't, prompt an error
  bind(fd, (struct sockaddr *)&serv, sizeof(serv)); //assigns the address specified by serv to the socket
  listen(fd,5); //Listen for client connections. Maximum 5 connections will be permitted.
  //Now we start handling the connections.
  printf("Servidor levantado en el puerto %d\n",puerto);

  conn = accept(fd, (struct sockaddr *)NULL, NULL);
  while(recv(conn,message,100,0)>0)
  {

    //atender_cliente(fd,"hola");

    recibir_mensaje(fd);

    enviar_mensaje(fd,"gola");

  }

    /*
    while (recv(conn, message, 100, 0)>0)
    {
      uint32_t pid;
      if((pid = fork()) == 0)
      {
        printf("Message Received: %s\n", message);
              //tratar_mensaje(message);
              //enviar_mensaje(pid);
        for(int i=0;i<sizeof(message);i++)
            {
              message[i] = '\0';
            }
        enviar_mensaje(pid,"hola");
                  //An extra breaking condition can be added here (to terminate the child process)
            }
      }

*/



  return 0;
}

void atender_cliente(uint32_t pid,char* respuesta)
{
    char message_resp[100];
    strcpy(message_resp,respuesta);
    //tratamos message
    recv(pid, message, 100, 0);
    printf("Respuesta: %s\n",message );
    send(pid, message_resp,100, 0);
}


void enviar_mensaje(uint32_t destino,char mensaje)
{
  strcpy(message,"");
  puts(message);
  printf("Enter a message: ");
    //comando a enviar
  fgets(message, 100, stdin);
  send(destino, message, 100, 0);
//vaciamos buffer


}

void recibir_mensaje(uint32_t destinatario)
{

  printf("Esperando mensaje \n" );
  //strcpy(message,"\0");
  //esperamos respuesta del servidor
  recv(destinatario, message, 100, 0);
  printf("Mensaje : %s\n",message);
}
