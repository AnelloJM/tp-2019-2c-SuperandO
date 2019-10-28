#include "muse.h"
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
int main()
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

  printf("\n\n::::::::INICIAMOS EL SERVIDOR::::::::\n");

  socketMuse = iniciar_servidor(ip,puerto,logger);
  socket_cliente = esperar_cliente_con_accept(socketMuse,logger);
  enviar_mensaje(socket_cliente,logger);

  return 0;
}

void crearLogger()
{
	char* logPath = /*"MUSE.log";*/"/home/utnso/workspace/tp-2019-2c-SuperandO/Muse/src/MUSE.log";
	char* nombreArch = "MUSE";
	bool consolaActiva = true;
	logger = log_create(logPath, nombreArch, consolaActiva, LOG_LEVEL_INFO);
	log_info(logger, "El logger se creo con exito");
//	free(logPath);
}

void leerArchivoDeConfiguracion()
{
	char* configPath = /*"MUSE.cfg";*/ "/home/utnso/workspace/tp-2019-2c-SuperandO/Muse/src/MUSE.cfg";
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
	ip = strdup(config_get_string_value(archivoConfig,"IP"));
	id = config_get_int_value(archivoConfig,"ID");
	puerto = strdup(config_get_string_value(archivoConfig,"PUERTO"));
	memory_size = config_get_int_value(archivoConfig,"MEMORY_SIZE");
	page_size =	config_get_int_value(archivoConfig,"PAGE_SIZE");
	swap_size = config_get_int_value(archivoConfig,"SWAP_SIZE");
}

int inicializarTablas(){
	bloqueMemoria = malloc(sizeof(t_memoria));
	bloqueMemoria->tamanioPagina = page_size;
	bloqueMemoria->bytesMemoria = malloc(memory_size);

	//bitarray para saber los marcos ocupados
	bloqueMemoria->cantidadFrames = memory_size / page_size;
	bloqueMemoria->bitData = malloc(bloqueMemoria->cantidadFrames/8 + 1);
	bloqueMemoria->framesBita = bitarray_create_with_mode(bloqueMemoria->bitData, bloqueMemoria->cantidadFrames/8+1, LSB_FIRST);

	//seteo marcos como libres
	for(int i=0; i < bitarray_get_max_bit(bloqueMemoria->framesBita);i++){
		bitarray_clean_bit(bloqueMemoria->framesBita,i);
	}

	log_info(logger,"Se ha inicializado el bloque de memoria. Tamaño: %d. Cantidad de Paginas: %d-%d. Tamaño de Pagina: %d",memory_size,
			bloqueMemoria->cantidadFrames,bitarray_get_max_bit(bloqueMemoria->framesBita),page_size);

	return 0;
}
/*SEGMENTOS*/

Segmento *agregarSegmento(){
	Segmento *segmento = malloc(sizeof(Segmento));
	segmento->numero = contadorSegmentos;
	contadorSegmentos++;
	list_add(tablaSegmentos, (Segmento*)segmento);
	return segmento;
}

Segmento *leerSegmento(int numero){
	Segmento *seg = NULL;

	bool finder(void* seg){
		return ((Segmento*) seg)-> numero == numero;
	}
	Segmento *seg = list_find(tablaSegmentos,&finder);

	return seg;
}

/*PAGINAS*/

int obtenerMarcoLibre(){
	int indice= -1;

	for(int i=0; i< bitarray_get_max_bit(bloqueMemoria->framesBita); i++){
		if(i==bloqueMemoria->cantidadFrames){
			break;
		}
		if(!bitarray_test_bit(bloqueMemoria->framesBita,i)){
			indice = i;
			break;
		}
	}
	return indice;
}

int escribirPagina(Pagina *pagina, int indice, bool modificado, bool uso){
	int resultado = reemplazarPagina(pagina,indice,modificado,uso);

	if(resultado !=-1){
		list_add(tablaPaginas,pagina);
	}
	return resultado;
}

//Pensar bien como leer paginas


//Algoritmo de reemplazo
int reemplazarPagina(Pagina *pagina, int indice, bool modificado, bool uso){
	return 1;
}

int obtenerIndiceDeReemplazo(){
	int indice;

	return indice;
}

//Falta agregar las funciones para liberar cosas


//Tengo que saber bien el formato de los registros para poder pasarlos a bytes y poder operar











