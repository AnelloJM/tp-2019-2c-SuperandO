#include "muse_init.h"

/**
     * Inicializa la biblioteca de MUSE.
     * @param id El Process (o Thread) ID para identificar el caller en MUSE.
     * @param ip IP en dot-notation de MUSE.
     * @param puerto Puerto de conexión a MUSE.
     * @return Si pasa un error, retorna -1. Si se inicializó correctamente, retorna 0.
     * @see Para obtener el id de un proceso pueden usar getpid() de la lib POSIX (unistd.h)
     * @note Debido a la naturaleza centralizada de MUSE, esta función deberá definir
     *  el ID del proceso/hilo según "IP-ID".
     */




int main(int argc, char const *argv[]) {

  int id,puerto;
  char* ip;



  if(argc < 2)
  {
    printf("Faltan parámetros ..\n");
    printf("Indique el archivo de configuracion para\n");
    printf("leer los valores\n" );
    printf("USO: <programa> <archivo_de_configuracion.config>\n");
    exit(1);
  }

  else
  {
    g_config = config_create(argv[1]);
    id = leer_id(g_config);
    ip = leer_ip(g_config);
    puerto = leer_puerto(g_config);

  }

  printf("\n [+]La configuracion es la siguiente \n");
  printf("ID ->  %d\n",id );
  printf("IP ->  %s\n",ip );
  printf("Puerto ->  %d\n",puerto );
  printf("--------------\n");

  printf("\n\n::::::::INICIAMOS EL SERVIDOR::::::::\n");

  muse_init(id,ip,puerto);

  return 0;
}


int leer_id(void *archivo)
{
  int id;
  id = config_get_int_value(archivo,"ID");

  //falta loggear los valores

  return id;

}

char* leer_ip(void *archivo)
{
  char *ip;
  ip = config_get_string_value(archivo,"IP");

    //falta loggear los valores

  return ip;
}

int leer_puerto(void *archivo)
{
  int puerto;
  puerto = config_get_int_value(archivo,"PUERTO");

  //falta loggear los valores
  return puerto;
}


int muse_init(int id, char* ip, int puerto)
{
    iniciar_servidor(ip,puerto);

    return 0;
}
