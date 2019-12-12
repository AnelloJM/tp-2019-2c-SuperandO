#include "Suse2.h"

int main(void) {
	crearLogger();
	leerArchivoDeConfiguracion();
	socket_suse = iniciar_servidor(suse_ip,suse_port,logger);
	inicializarEstructuras();
	inicializarSemaforos();

	while(1){
		log_info(logger,"Esperando por clientes");
		socket_cliente = esperar_cliente_con_accept(socket_suse,logger);
		pthread_t* hiloRecibirPaquetes = malloc(sizeof(pthread_t));
		if(pthread_create(hiloRecibirPaquetes, NULL,(void*)atenderCliente,(void*)(socket_cliente) )== 0){
			pthread_detach(*hiloRecibirPaquetes);
			log_info(logger,"Se creo el hilo RecibirPaquetes correctamente");
		}else{
			log_error(logger,"No se ha podido crear el hilo: RecibirPaquetes");
		}
	}
	//ACORDATE DE LIBERAR Y DESTRUIR A TODOS ANTES DE SALIR
	return 0;
}

void crearLogger(){
	char* logPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/Suse2/src/Suse2.log";
	char* nombreArch = "Suse2";
	logger = log_create(logPath, nombreArch, 1, LOG_LEVEL_INFO);
	log_info(logger, "El logger se creo con exito");
}

void leerArchivoDeConfiguracion(){
	char* configPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/Suse2/src/Suse2.config";
	archivoConfig = config_create(configPath);
	if (archivoConfig == NULL){
		log_error(logger,"Archivo de configuracion no encontrado");
	}
	setearValores(archivoConfig);
	log_info(logger,"La configuracion fue cargada exitosamente");
}

void setearValores(t_config* archivoConfig){
	suse_ip = config_get_string_value(archivoConfig,"SERVER_IP");
	suse_port = config_get_string_value(archivoConfig,"LISTEN_PORT");
	metrics_timer = config_get_int_value(archivoConfig,"METRICS_TIMER");
	max_multiprog = config_get_int_value(archivoConfig,"MAX_MULTIPROG");
	sems_ids_suse = config_get_array_value(archivoConfig, "SEM_IDS");
	sem_init_suse = config_get_array_value(archivoConfig, "SEM_INIT");
	sem_max_suse = config_get_array_value(archivoConfig, "SEM_MAX");
	alpha_sjf = config_get_double_value(archivoConfig, "ALPHA_SJF");
}

void inicializarEstructuras(){
	cola_new = list_create();
	cola_blocked = list_create();
	cola_exit = list_create();
	semaforos = list_create();
	log_info(logger, "Las estructuras se han inicializado con exito");
}

void inicializarSemaforos(){
	//Para todos los ID de semaforos, voy creando un semaforo nuevo y lo guardo en la lista de semaforos
	int i;
	int final = posicionFinalDoblePuntero(sems_ids_suse);
	for(i=0; i<= final; i++){
		t_semaforo* semaforo = malloc(sizeof(t_semaforo));
		char* valorID = malloc(strlen(sems_ids_suse[i])+1);
		semaforo->semID = malloc (sizeof(valorID));
		memcpy(semaforo->semID,valorID,(strlen(sems_ids_suse[i])+1)); //el +1 es por el /0
		int valorInit = atoi(sem_init_suse[i]);
		semaforo->semInit = valorInit;
		semaforo->semActual = valorInit;
		int valorMax = atoi(sem_max_suse[i]);
		semaforo->semMax =	valorMax;
		semaforo->hilosEnEspera = list_create();
		list_add(semaforos, semaforo);
	}
	liberarDoblePuntero(sems_ids_suse);
	liberarDoblePuntero(sem_init_suse);
	liberarDoblePuntero(sem_max_suse);
	log_info(logger,"Se han inicializado todos los semaforos con exito");
}

//No me toma el include de manejos comunes, por ahora las dejo locales
int posicionFinalDoblePuntero(char **puntero){
	int contador = 0;
	while(puntero[contador] != NULL)
		contador = contador+1;
	return contador-1;
}

void liberarDoblePuntero(char **puntero){
	int posicionFinal = posicionFinalDoblePuntero(puntero);
	for(int i=0; i<=posicionFinal; i=i+1)
	{
		free(puntero[i]);
	}
	free(puntero);
}

int suse_create(int pid){
	return 0;
}

int suse_schedule_next(int pid){
	return 0;
}

int suse_wait(int pid, char* semaforoID){
	return 0;
}

int suse_signal(int pid, char* semaforoID){
	return 0;
}

int suse_join(int pid, int tid){
	return 0;
}

int suse_close(int pid, int tid){
	return 0;
}


void* atenderCliente(int socket_cliente){
	t_programa* programaNuevo = malloc(sizeof(programaNuevo));
	programaNuevo->pid = pidMAX;
	pidMAX++;
	programaNuevo->hilos = list_create();
	programaNuevo->cola_ready = list_create();
	programaNuevo->cola_exec = list_create();
	list_add(lista_programas, programaNuevo);
	while(1){
		HeaderSuse headerRecibido;
		headerRecibido = Suse_RecieveHeader(socket_cliente);
		log_info(logger, "Codigo de operacion: %i", headerRecibido.operaciones);
		log_info(logger, "Tamanio: %i", headerRecibido.tamanioMensaje);
		if(headerRecibido.operaciones == -1){
			log_error(logger, "Se desconecto el cliente\n");
			break;
		}
		uint32_t tam = headerRecibido.tamanioMensaje;
		switch(headerRecibido.operaciones){

		//Las operaciones no tienen que enviar ninguna respuesta de vuelta creo

		case S_CREATE:;
			void* paqueteCreate = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(logger, "Se recibió un pedido de Suse_Create");
			int pidCreate = Suse_Unpack_Uint32_pid(paqueteCreate);
			free(paqueteCreate);
			int respuestaCreate = suse_create(pidCreate);
			if (respuestaCreate == 0){
				log_info(logger, "La operacion Suse_Create se realizó con exito");
				break;
			}
			log_error(logger, "La operacion Suse_Create ha fallado");
			break;

		case S_SCHEDULE_NEXT:;
			void* paqueteScheduleNext = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(logger, "Se recibió un pedido de Suse_Schedule_Next");
			int pidNext = Suse_Unpack_Uint32_pid(paqueteScheduleNext);
			free(paqueteScheduleNext);
			int respuestaNext = suse_schedule_next(pidNext);
			if (respuestaNext != 0){ //Si devuelvo cualquier TID entra aca
				log_info(logger, "La operacion Suse_Schedule_Next se realizó con exito");
				log_info(logger, "El proximo hilo a ejecutar es el tid=%d",respuestaNext);
				break;
			}
			//Si falla, devuelve 0
			log_error(logger, "La operacion Suse_Schedule_Next ha fallado");
			break;

		case S_WAIT:;
			void* paqueteWait = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(logger, "Se recibió un pedido de Suse_Wait");
			int pidWait = Suse_Unpack_Uint32_pid(paqueteWait);
			char* semIDWait = Suse_Unpack_Char(paqueteWait);
			free(paqueteWait);
			int respuestaWait = suse_wait(pidWait, semIDWait);
			if (respuestaWait == 0){
				log_info(logger, "La operacion Suse_Wait se realizó con exito");
				break;
			}
			log_error(logger, "La operacion Suse_Wait ha fallado");
			break;

		case S_SIGNAL:;
			void* paqueteSignal = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(logger, "Se recibió un pedido de Suse_Signal");
			int pidSignal = Suse_Unpack_Uint32_pid(paqueteSignal);
			char* semIDSignal = Suse_Unpack_Char(paqueteSignal);
			free(paqueteSignal);
			int respuestaSignal = suse_signal(pidSignal, semIDSignal);
			if (respuestaSignal == 0){
				log_info(logger, "La operacion Suse_Signal se realizó con exito");
				break;
			}
			log_error(logger, "La operacion Suse_Signal ha fallado");
			break;

		case S_JOIN:;
			void* paqueteJoin = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(logger, "Se recibió un pedida de Suse_Join");
			int pidJoin = Suse_Unpack_Uint32_pid(paqueteJoin);
			int tidJoin = Suse_Unpack_Uint32_tid(paqueteJoin);
			free(paqueteJoin);
			int respuestaJoin = suse_join(pidJoin, tidJoin);
			if (respuestaJoin == 0){
				log_info(logger, "La operacion Suse_Join se realizó con exito");
			}
			log_error(logger, "La operacion Suse_Join ha fallado");
			break;

		case S_CLOSE:;
			void* paqueteClose = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(logger, "Se recibió un pedida de Suse_Close");
			int pidClose = Suse_Unpack_Uint32_pid(paqueteClose);
			int tidClose = Suse_Unpack_Uint32_tid(paqueteClose);
			free(paqueteClose);
			int respuestaClose = suse_close(pidClose, tidClose);
			if (respuestaClose == 0){
				log_info(logger, "La operacion Suse_Close se realizó con exito");
			}
			log_error(logger, "La operacion Suse_Close ha fallado");
			break;

		default:
			log_error(logger, "No es un codigo conocido: %i", headerRecibido.operaciones);
			break;
		}
	}
}

