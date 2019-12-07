//  MUSE :gestor de memoria central
#include "muse.h"


int main()
{

  iniciarLogger();


  //reservamos la UPCM y aniadimos tabla de frames
  UPCM = malloc(memory_size);
  printf("[+]La direccion inicial de la UPCM es: %u\n",UPCM );

  tabla_de_frames = list_create();

  //rellenamos la tabla de frames con -1, porque ninguno esta ocupado
  for(int i=0;i<frames_table_size;i++)
    {
      list_add_in_index(tabla_de_frames,i,-1);
    }
  //pp ->

//  cambiarValor(tabla_de_frames,0,1);
  //cambiarValor(tabla_de_frames,1,2);
  //cambiarValor(tabla_de_frames,2,2);
   // lista de frames
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


  printf("\n\n::::::::INICIAMOS EL SERVIDOR::::::::\n");

//-pp->
  uint32_t pet1 =tratar_muse_alloc(59,1);
  uint32_t pet2 = tratar_muse_alloc(70,6);
  uint32_t pet3 = tratar_muse_alloc(2,2);
  uint32_t pet4 =tratar_muse_alloc(200,1);
  uint32_t pet5 = tratar_muse_alloc(7,6);
  uint32_t pet6 = tratar_muse_alloc(2,2);

  printf("Pet 1 en pos %d\n",pet1 );
  printf("Pet2 en pos: %d\n",pet2 );
  //printf("Pet3 en pos %d\n",pet3 );

// <-pp-


  /*iniciar_servidor(atoi(puerto));
  log_info(logger,"Servidor corriendo\n");
*/

  printf("\n\n\n[+]Liberando memoria asignada a upcm..\n" );
  free(UPCM);
  free(tabla_de_frames);
  printf("Adios!\n" );
  return 0;
}

void poner_heap(Heap *heap,uint32_t posicion){
  memcpy(UPCM+posicion,&(heap->isFree),1);
  memcpy(UPCM+posicion+1,&(heap->size),4);
  printf("[+]Se puso el heap(%d,%d) en la posicion(de UPCM) %u\n",heap->isFree,heap->size,posicion);
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
  printf(" La posicion donde esta el heap free es %d\n",respuesta );
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
    uint32_t memoria_disponible_en_index = frame_free_size(calcular_posicion_en_UPCM(frame_index));
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

  printf("\n" );

  return free_frame_pos; //devolvemos la posicion donde se encuentra reservada la memoria

}


uint32_t frame_free_size(uint32_t posicion)
{
    uint32_t free_size;
    memcpy(&free_size,UPCM+posicion+1,4);
    printf("Memoria disponible : %d\n",free_size );
    return free_size;
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


void mostrar_frames_table()
{
  for(int j=0;j<frames_table_size;j++)
  {
    printf("%d,", list_get(tabla_de_frames,j));
  }
}
