/*
 * muse.c
 *
 *  Created on: Dec 12, 2019
 *      Author: cruz636
 */


//  MUSE :gestor de memoria central
#include "muse.h"


int main()
{

  iniciarLogger();

  uint32_t cliente;

  //reservamos la UPCM y aniadimos tabla de frames
  UPCM = malloc(memory_size);
  printf("[+]La direccion inicial de la UPCM es: %u\i",UPCM );

  tabla_de_frames = list_create(); //creamos la tabla de frames

  tabla_de_procesos = list_create(); //creamos la tabla de procesos para saber que proceso ya esta en memoria

  //rellenamos la tabla de frames con -1, porque ninguno esta ocupado
  for(int i=0;i<frames_table_size;i++)
    {
      list_add_in_index(tabla_de_frames,i,-1);
    }

  printf("\n\nLista de frames: \n" );

  mostrar_frames_table();

  printf("\n[+]Cargando heaps en la UPCM..\n");


  for(int i=0;i<frames_table_size;i++)
  {
    uint32_t pos = calcular_posicion_en_UPCM(i);
    Heap *heap = malloc(sizeof(Heap));
    heap->isFree = true;
    heap->size = page_size - 5;
    poner_heap(heap,pos);
    free(heap);
  }

  printf("   Heaps cargados de forma exitosa!\n");

/*
  printf("\n\n::::::::INICIAMOS EL SERVIDOR::::::::\n");


  socketMuse = iniciar_servidor(puerto, logger);

  log_info(logger,"Servidor corriendo\n");
  while(1){
    cliente = esperar_cliente_con_accept(socketMuse, logger);

    pthread_t* cody = malloc(sizeof(pthread_t));
    if(pthread_create(cody,NULL,(void*)recibir_paquete,(void*)cliente) == 0){
      pthread_detach(*cody);
      log_info(logger,"Se creo el hilo sin problema, cliente: %i", cliente);
    }else{
      log_error(logger,"No se pudo crear el hilo, cliente: %i", cliente);
    }
  }

*/

  printf("\n\n\n[+]Liberando memoria asignada a upcm..\n" );
  free(UPCM);
  free(tabla_de_frames);
  printf("Adios!\n" );


  return 0;
}


int buscar_proceso_en_tabla(char *pid)
{
	int posicion_en_tabla = -1;
	int tamanio_tabla_procesos = list_size(tabla_de_procesos);

	for(int i=0;i<tamanio_tabla_procesos;i++)
	{
		if(pid == list_get(tabla_de_procesos,i))
		{
			posicion_en_tabla = i;
		}

	}

	if(posicion_en_tabla == -1)
	{
		list_add(tabla_de_procesos,pid);
		posicion_en_tabla = tamanio_tabla_procesos + 1;
	}


	return posicion_en_tabla;
}














void poner_heap(Heap *heap,uint32_t posicion){
  memcpy(UPCM+posicion,&(heap->isFree),1);
  memcpy(UPCM+posicion+1,&(heap->size),4);
  printf("[+]Se puso el heap(%d,%d) en la posicion(de UPCM) %u\n",heap->isFree,heap->size,posicion);
}

void poner_heap_meta_free(uint32_t posicion,uint32_t free_space)
{
	memcpy(UPCM+posicion,true,1);
	memcpy(UPCM+posicion+1,free_space,4);
	printf("Se indico que hay %d de espacion libre luego de la posicion %d",free_space,posicion);
}

uint32_t calcular_posicion_en_UPCM(uint32_t n_frame)
{
  uint32_t posicion = n_frame * page_size;
  return posicion;
}

uint32_t free_frame_heap(uint32_t posicion)
{
  uint32_t val,respuesta;
  memcpy(&val,UPCM+posicion,1);
  if(val==1)
  {
    respuesta = posicion;
  }
  else
  {
    uint32_t size;
    memcpy(&size,UPCM+posicion+1,4);
    if(size+10 < page_size) //el tamanio usado + los dos heaps tiene que ser menor
    {                       // que el tamanio de la pagina, sino no tiene espacio libre
      respuesta = posicion + size + 5;
    }
    else
    {
      respuesta = -1;
    }

  }
  printf(" La posicion que esta free es %d\n",respuesta );
  return respuesta;
}

uint32_t buscar_frame_libre()
{
  uint32_t frame_libre;
  int flag=0;
  while(flag==0)
  {
    for(int i=0;i<frames_table_size;i++)
    {
      if(list_get(tabla_de_frames,i)==-1)
      {
        frame_libre = i;
        flag = 1;
        i = frames_table_size-1;
      }

    }
    //->
    //si no hay frames libres hay que swapear
  }

  printf("El frame libre es el numero %d\n",frame_libre );
  return frame_libre;
}

void reservar_espacio(uint32_t posicion,uint32_t tamanio)
{
  Heap * heap = malloc(sizeof(Heap));
  heap->isFree=false;
  heap->size=tamanio;
  poner_heap(heap,posicion);

  free(heap);
}

void liberar_espacio(uint32_t posicion)
{
  uint32_t tamanio;
  memcpy(&tamanio,UPCM+posicion+1,4);
  Heap * heap = malloc(sizeof(Heap));
  uint32_t fragmentacion_interna;
  memcpy(&fragmentacion_interna,UPCM+posicion+5+tamanio,1);
  if(fragmentacion_interna == 0)
  {
	 heap->isFree=true;
	 heap->size=tamanio;
  }
  else
  {
	  heap->isFree=true;
	  heap->size=page_size;
	  uint32_t index = posicion % page_size;
	  cambiarValor(tabla_de_frames,index,-1);
  }

  poner_heap(heap,posicion);
  free(heap);
  printf("Se libero la posicion %d",posicion);
}

void alloc_tam(uint32_t tam,uint32_t posicion)
{
  //@tam : siempre es menor que page_size - 5 (menos el heap)
  //@posicion : donde esta el heap
  reservar_espacio(posicion,tam);

  //si tam + 10 < page size, tenemos que indicar cuanto espacio libre queda
  // y para eso aniadimos otro heap
  if(tam+10 <= page_size)
  {
    Heap *heap_free = malloc(sizeof(Heap));
    heap_free->isFree=true;
    heap_free->size=(page_size - tam - 10);
    memcpy(UPCM+posicion+5+tam,&(heap_free->isFree),1);
    memcpy(UPCM+posicion+6+tam,&(heap_free->size),4);
    free(heap_free);
  }


}

uint32_t buscar_proceso(uint32_t id_proceso)
{
  uint32_t posicion_en_tabla = -1;
  for(int i=0;i<frames_table_size;i++)
  {
    if(list_get(tabla_de_frames,i)==id_proceso)
    {
      posicion_en_tabla = i;
    }

  }

  printf("El proceso %d esta cargado en el marco %d\n",id_proceso,posicion_en_tabla );

  return posicion_en_tabla;
}

uint32_t tratar_muse_alloc(uint32_t tam,uint32_t id_proceso)
{

  uint32_t cantidad_frames_necesarios = tam / (page_size - 5); //frames enteros a reservar
  uint32_t cantidad_memoria_necesaria = tam % (page_size - 5); //memoria que ocupa menos de una pagina
  uint32_t frame_index = buscar_proceso(id_proceso);
  uint32_t free_frame_pos;
  uint32_t free_frame;

  if(frame_index==-1)
  {
    //si proceso no tiene ninguen frame asignado :
    printf("El proceso %d no tenia ningun marco asignado\n",id_proceso );
    for(int i=0;i<cantidad_frames_necesarios;i++)
    {
      free_frame = buscar_frame_libre();
      free_frame_pos = calcular_posicion_en_UPCM(free_frame);
      alloc_tam(page_size-5,free_frame_pos);
      cambiarValor(tabla_de_frames,free_frame,id_proceso);
    }

    //una vez allocadas las paginas completas, pasamos a
    //allocar la memoria restante

    free_frame = buscar_frame_libre();
    free_frame_pos = calcular_posicion_en_UPCM(free_frame);
    alloc_tam(cantidad_memoria_necesaria,free_frame_pos);
    cambiarValor(tabla_de_frames,free_frame,id_proceso);
  }
  else
  {
    //el proceso ya tenia marcos asignados, entonces :
    printf("El primer marco asignado al proceso %d es %d\n",id_proceso,frame_index);
//    uint32_t memoria_disponible_en_index = frame_free_size(calcular_posicion_en_UPCM(frame_index));
    uint32_t memoria_disponible_en_index = free_size_in_frame(frame_index,tam);
    if(memoria_disponible_en_index<tam)
    {
        alloc_tam(memoria_disponible_en_index,frame_index);
    }
    else
    {
      alloc_tam(tam,frame_index);
    }

    cantidad_frames_necesarios = (tam-memoria_disponible_en_index) / (page_size - 5); //frames enteros a reservar
    cantidad_memoria_necesaria = (tam - memoria_disponible_en_index) % (page_size - 5); //memoria que ocupa menos de una pagina

    for(int i=0;i<cantidad_frames_necesarios;i++)
    {
      free_frame = buscar_frame_libre();
      free_frame_pos = calcular_posicion_en_UPCM(free_frame);
      alloc_tam(page_size-5,free_frame_pos);
      cambiarValor(tabla_de_frames,free_frame,id_proceso);
    }

    free_frame = buscar_frame_libre();
    free_frame_pos = calcular_posicion_en_UPCM(free_frame);
    alloc_tam(cantidad_memoria_necesaria,free_frame_pos);
    cambiarValor(tabla_de_frames,free_frame,id_proceso);

   }

  printf("\n\n[+]Tabla de frames actualizada: \n" );
  mostrar_frames_table();

  printf("\n");
  return free_frame_pos; //devolvemos la posicion donde se encuentra reservada la memoria

}

uint32_t tratar_muse_cpy(uint32_t tam,uint32_t posicion,void * data,uint32_t id_proceso)
{
  //tenemos que verificar que el proceso tenga esa poscion reservada
  memcpy(UPCM+posicion,&data,tam);
  printf("Datos copiados en la posicion %d de forma exitosa!\n",posicion );
  return 0;
}

uint32_t tratar_muse_free(uint32_t dir,uint32_t id_proceso)
{
	liberar_espacio(dir);
	printf("\n\n[+]Tabla de frames actualizada: \n" );
	mostrar_frames_table();

	return 0;
}

uint32_t frame_free_size(uint32_t posicion) //free size in frame
{
	 uint32_t free_size;
	 memcpy(&free_size,UPCM+posicion+1,4);
	 printf("Memoria disponible : %d\n",free_size );
	 return free_size;
}


uint32_t free_size_in_frame(uint32_t frame,uint32_t tam)
{
	uint32_t ini_pos = calcular_posicion_en_UPCM(frame);
	uint32_t value, tamanio_del_segmento;
	uint32_t espacios_recorridos = 0;

	while(espacios_recorridos < page_size)
	{
		memcpy(&value,UPCM+ini_pos,1);
		memcpy(&tamanio_del_segmento,UPCM+ini_pos+1,4);
		if(value==1)
		{
			if(tamanio_del_segmento >= tam + 5)
			{
				return tamanio_del_segmento;
			}
		}
		espacios_recorridos = espacios_recorridos + 5 + tamanio_del_segmento;
		ini_pos = ini_pos + 5 + tamanio_del_segmento;
	}
	return 0;
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
	char* logPath = "MUSE.log";
	char* nombreArch = "MUSE";
	bool consolaActiva = true;
	logger = log_create(logPath, nombreArch, consolaActiva, LOG_LEVEL_INFO);
	log_info(logger, "El logger se creo con exito");
//	free(logPath);
}

void leerArchivoDeConfiguracion()
{
	char* configPath = "MUSE.cfg";
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

void mostrar_frames_table()
{
  for(int j=0;j<frames_table_size;j++)
  {
    printf("%d,", list_get(tabla_de_frames,j));
  }
}
