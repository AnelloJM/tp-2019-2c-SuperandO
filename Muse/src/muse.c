//  MUSE :gestor de memoria central
#include "muse.h"


int main()
{

  iniciarLogger();


  //reservamos la UPCM y aniadimos tabla de frames
//->  UPCM = malloc(memory_size * sizeof(int));
  tabla_de_frames = list_create();
  Frame cadena_Frames[frames_table_size];

  //rellenamos la tabla de frames con 0, porque ninguno esta ocupado
  for(int i=0;i<frames_table_size;i++)
    {
      list_add_in_index(tabla_de_frames,i,0);
    }

  for(int k=0;k<frames_table_size;k++)
    {
      cadena_Frames[k].numero_frame = k;
      cadena_Frames[k].espacio_ocupado.size = page_size;
      cadena_Frames[k].espacio_ocupado.isFree = false;
    }

  printf("Mostrando cadena de frames: \n" );

  for(int k=0;k<frames_table_size;k++)
  {
    printf("frame numero %d\n",cadena_Frames[k].numero_frame );
    printf("bit de ocupado : %d\n",cadena_Frames[k].espacio_ocupado.isFree );
  }

  //pp ->

  cambiarValor(tabla_de_frames,0,1);
  cambiarValor(tabla_de_frames,1,1);

   // lista de frames
  printf("\n\nLista de frames: \n" );
  for(int j=0;j<frames_table_size;j++)
  {
    printf("%d-", list_get(tabla_de_frames,j));
  }


  printf("\n\n::::::::INICIAMOS EL SERVIDOR::::::::\n");

  iniciar_servidor(atoi(puerto));
  log_info(logger,"Servidor corriendo\n");

  //uint32_t pet = tratar_muse_alloc(15);
  //free(UPCM);

  free(tabla_de_frames);
  return 0;
}

int recibir_peticion(uint32_t tam)
{
    printf("Comprobar si hay %d libre\n",tam );
    pasar_a_frames(tam);
    //buscar frames libres..
}

int pasar_a_frames(uint32_t tam)
{
  int frames= 0;
  frames = page_size / tam;
  return frames;
}

uint32_t tratar_muse_alloc(uint32_t tam){

	int frames_necesarios,free_frame;
	int flag = 0;

	frames_necesarios = tam / (page_size - 5);
	if(frames_necesarios == 0)
	{
		frames_necesarios = 1;
	}

	//buscamos marcos libres
  for(int i=0;i<frames_table_size;i++)
    {
      if(flag == 0)
      {
        if(list_get(tabla_de_frames,i)==0)
        {
          //-> pp
          printf("Frame libre numero %d\n",i );
          //<- pp

          free_frame = i;
          flag = 1;
        }
      }
    }

  //comprobamos que tenga espacio suficiente ?

  // cambiamos el valor de ese frame de libre a ocupado
  //cambiarValor(tabla_de_frames,free_frame,1);
  //cambiarValorCadena(free_frame,1);

  cambiarValor(tabla_de_frames,free_frame,1);

//mostramos como quedo la tabla de frames

  printf("\n\nLista de frames: \n" );
  for(int j=0;j<frames_table_size;j++)
  {
    printf("%d-", list_get(tabla_de_frames,j));
  }

  printf("\n\n\n" );

  if(flag==1)
  {
    return free_frame; //este sera el frame libre que se le dara al proceso
		  // return (uint32_t) malloc(tam);
  }
  else
  {
    //-> pp
    printf("No hay frames libres, pasar los que están a MV\n");
    // <- pp

    return -1; //esto significa que hay que swapear
  }


}



void cambiarValor(t_list *tabla_de_frames,int index,int valor)
{
  list_replace(tabla_de_frames,index,valor); // 1 o 0
}


int iniciarLogger()
{
	crearLogger();
	leerArchivoDeConfiguracion();

	log_info(logger,"\n [+]La configuracion es la siguiente \n");
	log_info(logger,"ID ->  %d",id );
	log_info(logger,"IP ->  %s",ip );
	log_info(logger,"Puerto ->  %s",puerto );
	log_info(logger,"memory size ->  %d",memory_size);
	log_info(logger,"page size ->  %d",page_size);
	log_info(logger,"swap size ->  %d",swap_size);
	log_info(logger,"--------------\n");

	frames_table_size = memory_size / page_size ;
	return 0;
}


void crearLogger()
{
	char* logPath = "MUSE.log";//"/home/utnso/workspace/tp-2019-2c-SuperandO/Muse/src/MUSE.log";
	char* nombreArch = "MUSE";
	bool consolaActiva = true;
	logger = log_create(logPath, nombreArch, consolaActiva, LOG_LEVEL_INFO);
	log_info(logger, "El logger se creo con exito");
//	free(logPath);
}

void leerArchivoDeConfiguracion()
{
	char* configPath = "MUSE.cfg";// "/home/utnso/workspace/tp-2019-2c-SuperandO/Muse/src/MUSE.cfg";
	archivoConfig = config_create(configPath);
	if (archivoConfig == NULL){
		perror("ERROR: Archivo de configuracion no encontrado");
	}
	setearValores(archivoConfig);
	log_info(logger,"La configuracion fue cargada exitosamente");
	config_destroy(archivoConfig);
}

void setearValores(t_config* archivoConfig)
{
	id = config_get_int_value(archivoConfig,"ID");
	puerto = strdup(config_get_string_value(archivoConfig,"PUERTO"));
	memory_size = config_get_int_value(archivoConfig,"MEMORY_SIZE");
	page_size =	config_get_int_value(archivoConfig,"PAGE_SIZE");
	swap_size = config_get_int_value(archivoConfig,"SWAP_SIZE");
}
