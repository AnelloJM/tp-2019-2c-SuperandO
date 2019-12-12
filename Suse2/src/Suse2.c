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

int gettimeofday(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long)tv.tv_sec)*1000 + ((unsigned long long)tv.tv_usec)/1000);
	int a = result;
	return a;
}

bool comparadorPrograma(int unPid, t_programa* unPrograma){
	return (unPid == unPrograma->pid);
}

t_hilo calcularEstimacion(t_hilo unHilo){
	unHilo.rafagasEstimadas = (alpha_sjf * unHilo.estimacionAnterior + ((1 - alpha_sjf)*unHilo.rafagasEjecutadas));
	return unHilo;
}

bool comparadorDeRafagas(t_hilo unHilo, t_hilo otroHilo){
	return unHilo.rafagasEstimadas <= otroHilo.rafagasEstimadas;
}

bool comparadorDeHilos(t_hilo* unHilo, t_hilo* otroHilo){
	return (unHilo->tid == otroHilo->tid);
}

int buscadorSemaforo (char* semaforoID){
	int final = posicionFinalDoblePuntero(sems_ids_suse);
	for(int i = 0; i<=final; i++){
		if (strcmp(semaforoID,sems_ids_suse[i]) == 0){
			return 0;
		}
		i++;
	}
	return -1;
}

bool comparadorDeSemaforos(char* unSem, t_semaforo otroSem){
	return (strcmp(unSem,otroSem.semID)==0);
}

bool buscadorDeHilos(int tid, t_hilo* hilo){
	return (tid == hilo->tid);
}

void tomarMetricas(){
	for(int i=0;i<=list_size(lista_programas);i++){
		t_programa* unPrograma = malloc(sizeof(t_programa));
		unPrograma = list_get(lista_programas,i);
		t_list* hilosDelPrograma = unPrograma->hilos;
		hilosDelPrograma = list_map(hilosDelPrograma,(void*)calcularTiempoEjecucion);
		int tiempoTotalEjecucion = 0;
		//Hago una iteracion completa de los hilos para calcular el total de ejecucion
		for(int l=0;l<=list_size(hilosDelPrograma);l++){
			t_hilo* unhilo = malloc(sizeof(t_hilo));
			unhilo = list_get(hilosDelPrograma,l);
			tiempoTotalEjecucion += unhilo->tiempoEjecucion;
		}
		//METRICAS POR HILO DE UN PROCESO
		for(int j=0;j<=list_size(hilosDelPrograma);j++){
			t_hilo* unHilo = malloc(sizeof(t_hilo));
			unHilo = list_get(hilosDelPrograma,j);
			unHilo->porcentajeTiempoEjecucion = (unHilo->tiempoEjecucion / tiempoTotalEjecucion)*100 ;
			printf("Proceso:%d/n Hilo:%d/n Tiempo de Ejecucion:%d/n Tiempo de Espera:%d/n Tiempo de Uso de CPU:%d/n Porcentaje del tiempo de Ejecucion:%f/n",
					unPrograma->pid,unHilo->tid,unHilo->tiempoEjecucion,unHilo->tiempo_espera,unHilo->tiempoUsoCPU,unHilo->porcentajeTiempoEjecucion);
		}
		//METRICAS DEL PROCESO
		int hilosEnNew = list_size(cola_new);
		int hilosEnBlocked = list_size(cola_blocked);
		int hilosEnReady = list_size(unPrograma->cola_ready);
		int hilosEnExec = list_size(unPrograma->cola_exec);
		printf("Proceso:%d/n Hilos en estado NEW:%d/n Hilos en estado BLOCKED:%d/n Hilos en estado READY:%d/n Hilos en estado EXEC:%d/n",unPrograma->pid,
				hilosEnNew,hilosEnBlocked,hilosEnReady,hilosEnExec);
	}
	//METRICAS DEL SISTEMA
	for(int k=0;k<=list_size(semaforos);k++){
		t_semaforo* semaforo = malloc(sizeof(t_semaforo));
		semaforo = list_get(semaforos,k);
		printf("Semaforo:%s/n Valor Actual:%d/n",semaforo->semID, semaforo->semActual);
	}
	printf("Grado actual de multiprogramacion:%d/n",list_size(lista_programas));


	//REVISAR LOS FREES, VER BIEN DONDE VAN

}

void calcularTiempoEjecucion(t_hilo* hilo){
	int tiempoFinal = gettimeofday();
	hilo->tiempoEjecucion = (tiempoFinal - hilo->tiempoEjecucionInicial);
}


//FUNCIONES DE SUSE


int suse_create(int pid){
	/*
	t_hilo* hiloNuevo = malloc(sizeof(t_hilo));
	hiloNuevo->pid = pid;
	hiloNuevo->tid = tidMAX;
	hiloNuevo->tiempoEjecucionInicial = gettimeofday();
	tidMAX++;
	t_programa * programaBuscado = malloc(sizeof(t_programa));
	int index = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
	programaBuscado = list_get(lista_programas,index);
	list_add(programaBuscado->hilos,hiloNuevo);
	list_add(cola_new, hiloNuevo);
	log_info(logger,"Se ha agregado un hilo nuevo a la cola de new.\n");
	int cantidadColaNew = list_size(cola_new);
	log_info(logger,"Cantidad de elementos en cola new: %d\n", cantidadColaNew);
	log_info(logger,"ID del programa: %d\n",hiloNuevo->pid);
	log_info(logger,"ID del hilo: %d\n",hiloNuevo->tid);
	free(hiloNuevo);
	free(programaBuscado);
	*/
	return 0;
}

int suse_schedule_next(int pid){
	/*
	t_programa * programaBuscado = malloc(sizeof(t_programa));
	int index = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
	programaBuscado = list_get(lista_programas,index);
	if (!list_is_empty(programaBuscado->cola_ready)&& list_is_empty(programaBuscado->cola_exec)){
		log_info(logger, "Se comenzará a planificar");
		t_list* aux;
		aux = list_map(programaBuscado->cola_ready,(void*)calcularEstimacion);
		list_sort(aux, (void*)comparadorDeRafagas);
		t_hilo* hiloAux = malloc(sizeof(t_hilo));
		hiloAux = (t_hilo*) list_remove(aux,0);
		int indice = list_get_index(programaBuscado->cola_ready,hiloAux,(void*)comparadorDeHilos);
		t_hilo* hiloAEjecutar = list_remove(programaBuscado->cola_ready,indice);
		hiloAEjecutar->tiempoEsperaFinal = gettimeofday();
		hiloAEjecutar->tiempo_espera += (hiloAEjecutar->tiempoEsperaFinal - hiloAEjecutar->tiempoEsperaInicial);
		list_add(programaBuscado->cola_exec,hiloAEjecutar);
		hiloAEjecutar->tiempoUsoCPUInicial = gettimeofday();
		//Tomo el tiempo final de espera
		hiloAEjecutar->tiempoEsperaFinal = gettimeofday();
		//Y aca ya me queda guardado el tiempo de espera
		hiloAEjecutar->tiempo_espera += (hiloAEjecutar->tiempoEsperaInicial - hiloAEjecutar->tiempoEsperaFinal);
		free(programaBuscado);
		list_clean_and_destroy_elements(aux,(void*)free);
		list_destroy(aux);
		free(hiloAux);
		return hiloAEjecutar->tid;
	}
	free(programaBuscado);
	log_error(logger, "La cola de ready del programa está vacia o ya tiene un hilo ejecutando");
	*/
	return 0;
}

int suse_wait(int pid, char* semaforoID){
	/*
	if(buscadorSemaforo(semaforoID) == 0){
		int indice = list_get_index(semaforos,semaforoID,(void*)comparadorDeSemaforos);
		t_semaforo* semAUsar = malloc(sizeof(t_semaforo));
		semAUsar = list_get(semaforos,indice);
		if (semAUsar->semActual <= 0){
			semAUsar->semActual--;
			log_info(logger,"%d","Contador inicial:", semAUsar->semInit);
			log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
			log_info(logger,"%d","El semaforo se ha bloqueado, contador actual:",semAUsar->semActual);
			int index = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
			t_programa* programaBuscado = malloc(sizeof(t_programa));
			programaBuscado = list_get(lista_programas,index);
			t_hilo* hiloBuscado = malloc(sizeof(t_hilo));
			hiloBuscado = list_remove(programaBuscado->cola_exec,0);
			hiloBuscado->tiempoUsoCPUFinal = gettimeofday();
			hiloBuscado->tiempoUsoCPU += (hiloBuscado->tiempoUsoCPUFinal - hiloBuscado->tiempoUsoCPUInicial);
			list_add(cola_blocked,hiloBuscado);
			list_add(semAUsar->hilosEnEspera,hiloBuscado);
			free(programaBuscado);
			free(hiloBuscado);
			free(semAUsar);
			return 0;
		}
		semAUsar->semActual--;
		log_info(logger,"%d","Contador inicial:", semAUsar->semInit);
		log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
		log_info(logger, "%d","Contador actual:", semAUsar->semActual);
		free(semAUsar);
		return 0;
	}
	log_error(logger,"El semaforo no fue encontrado");
	return -1;
	*/
	return 0;
}

int suse_signal(int pid, char* semaforoID){
	/*
	if(buscadorSemaforo(semaforoID) == 0){
		int indice = list_get_index(semaforos,semaforoID,(void*)comparadorDeSemaforos);
		t_semaforo* semAUsar = malloc(sizeof(t_semaforo));
		semAUsar = list_get(semaforos,indice);
		if (semAUsar->semActual == semAUsar->semMax){
			log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
			log_error(logger,"El semaforo ya ha alcanzado su contador maximo, no se puede realizar el signal");
			free(semAUsar);
			return 0;
		}
		semAUsar->semActual++;
		log_info(logger,"%d","Contador inicial:", semAUsar->semInit);
		log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
		log_info(logger,"%d","Contador actual:", semAUsar->semActual);
		log_info(logger,"Se pasará a desbloquear el primer hilo en la cola de espera del semaforo, este hilo pasará al estado ready");
		int index = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
		t_programa* programaBuscado = malloc(sizeof(t_programa));
		programaBuscado = list_get(lista_programas,index);
		t_hilo* hiloADesbloquear = malloc(sizeof(t_hilo));
		hiloADesbloquear = list_remove(semAUsar->hilosEnEspera,0);
		int index2 = list_get_index(cola_blocked,hiloADesbloquear,(void*)comparadorDeHilos);
		hiloADesbloquear = list_remove(cola_blocked,index2);
		list_add(programaBuscado->cola_ready,hiloADesbloquear);
		hiloADesbloquear->tiempoEsperaInicial = gettimeofday();
		free(programaBuscado);
		free(hiloADesbloquear);
		free(semAUsar);
		return 0;

	}
	log_error(logger, "El semaforo no fue encontrado");
	return -1;
	*/
	return 0;
}

int suse_join(int pid, int tid){
	/*
	int index = list_get_index(cola_exit,tid,(void*)buscadorDeHilos);
	//Si el tid pasado no está en exit entonces procedo normalmente
	if(index == list_size(cola_exit)){
		int index2 = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
		t_programa* programaBuscado = malloc(sizeof(t_programa));
		programaBuscado = list_get(lista_programas,index2);
		t_hilo* hiloABloquear = malloc(sizeof(t_hilo));
		hiloABloquear = list_remove(programaBuscado->cola_exec,0);
		hiloABloquear->tiempoUsoCPUFinal = gettimeofday();
		hiloABloquear->tiempoUsoCPU += (hiloABloquear->tiempoUsoCPUFinal - hiloABloquear->tiempoUsoCPUInicial);
		free(programaBuscado);
		free(hiloABloquear);
		return 0;
	}
	//Si el tid ya está en exit, entonces el hilo que lo llama no se bloquea
	return 0;
	*/
	return 0;
}

int suse_close(int pid, int tid){
	/*
	int index = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
	t_programa* programaBuscado = malloc(sizeof(t_programa));
	programaBuscado = list_get(lista_programas,index);
	t_hilo* hiloATerminar = malloc(sizeof(t_hilo));
	hiloATerminar = list_remove(programaBuscado->cola_exec, 0);
	hiloATerminar->tiempoUsoCPUFinal = gettimeofday();
	hiloATerminar->tiempoUsoCPU += (hiloATerminar->tiempoUsoCPUFinal - hiloATerminar->tiempoUsoCPUInicial);
	list_add(cola_exit,hiloATerminar);
	hiloATerminar->finalizado = true;
	free(hiloATerminar);
	free(programaBuscado);
	//Cuando cierra un hilo toma las metricas
	tomarMetricas(); //Esto por ahi tambien deberia ser un hilo
	*/
	return 0;
}


//FUNCION MAGICA


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

