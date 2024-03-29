#include "Suse2.h"

int main(void) {
	suse_crearLogger();
	suse_leerArchivoDeConfiguracion();
	socket_suse = iniciar_servidor(suse_ip,suse_port,suse_logger);
	inicializarEstructuras();
	inicializarSemaforos();
	sem_t semaforoPlanificacion;
	sem_init(&semaforoPlanificacion,0,max_multiprog);
	log_info(suse_logger, "Me conectare al puerto: %s", suse_port);
	lista_programas = list_create();

//	//TOMAR METRICAS AUTOMATICAMENTE

//	pthread_t  hiloMetricas;
//	if(pthread_create(&hiloMetricas, NULL,(void*)tomarMetricasAutomaticas, NULL)==0){
//		pthread_detach(&hiloMetricas);
//		log_info(suse_logger,"Se creo el hilo *hiloMetricas* correctamente");
//	}
//	else {
//		log_error(suse_logger,"No se ha podido crear el hilo: HiloMetricas");
//	}

	while(1){

		//ACEPTAR CLIENTES
		log_info(suse_logger,"Esperando por clientes");
		socket_cliente = esperar_cliente_con_accept(socket_suse,suse_logger);

		//ATENDER CLIENTES
		pthread_t hiloRecibirPaquetes;
		if(pthread_create(&hiloRecibirPaquetes, NULL,(void*)atenderCliente, &socket_cliente ) == 0){
			pthread_detach(&hiloRecibirPaquetes);
			log_info(suse_logger,"Se creo el hilo RecibirPaquetes correctamente");
		}else{
			log_error(suse_logger,"No se ha podido crear el hilo: RecibirPaquetes");
		}
	}
	return 0;
}

void suse_crearLogger(){
	char* logPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/Suse2/src/Suse2.log";
	char* nombreArch = "Suse2";
	suse_logger = log_create(logPath, nombreArch, 1, LOG_LEVEL_INFO);
	log_info(suse_logger, "El logger se creo con exito");
}

void suse_leerArchivoDeConfiguracion(){
	char* configPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/Suse2/src/Suse2.config";
	suse_archivoConfig = config_create(configPath);
	if (suse_archivoConfig == NULL){
		log_error(suse_logger,"Archivo de configuracion no encontrado");
	}
	suse_setearValores(suse_archivoConfig);
	log_info(suse_logger,"La configuracion fue cargada exitosamente");
}

void suse_setearValores(t_config* archivoConfigLocal){
	suse_ip = config_get_string_value(archivoConfigLocal,"SERVER_IP");
	suse_port = config_get_string_value(archivoConfigLocal,"LISTEN_PORT");
	metrics_timer = config_get_int_value(archivoConfigLocal,"METRICS_TIMER");
	max_multiprog = config_get_int_value(archivoConfigLocal,"MAX_MULTIPROG");
	sems_ids_suse = config_get_array_value(archivoConfigLocal, "SEM_IDS");
	sem_init_suse = config_get_array_value(archivoConfigLocal, "SEM_INIT");
	sem_max_suse = config_get_array_value(archivoConfigLocal, "SEM_MAX");
	alpha_sjf = config_get_double_value(archivoConfigLocal, "ALPHA_SJF");
}

void inicializarEstructuras(){
	cola_new = list_create();
	cola_blocked = list_create();
	cola_exit = list_create();
	semaforos = list_create();
	log_info(suse_logger, "Las estructuras se han inicializado con exito");
}

void inicializarSemaforos(){
	//Para todos los ID de semaforos, voy creando un semaforo nuevo y lo guardo en la lista de semaforos
	int i;
	int final = posicionFinalDoblePuntero(sems_ids_suse);
	for(i=0; i<= final; i++){
		t_semaforo* semaforo = malloc(sizeof(t_semaforo));
		char* valorID = sems_ids_suse[i];
		semaforo->semID = malloc(strlen(sems_ids_suse[i])+1);
		memcpy(semaforo->semID,valorID,(strlen(sems_ids_suse[i])+1)); //el +1 es por el /0
		int valorInit = atoi(sem_init_suse[i]);
		semaforo->semInit = valorInit;
		semaforo->semActual = valorInit;
		int valorMax = atoi(sem_max_suse[i]);
		semaforo->semMax =	valorMax;
		semaforo->hilosEnEspera = list_create();
		list_add(semaforos, semaforo);
	}
//	liberarDoblePuntero(sems_ids_suse);
//	liberarDoblePuntero(sem_init_suse);
//	liberarDoblePuntero(sem_max_suse);
	sem_init(&mutexWaitSignal,max_multiprog,1);
	log_info(suse_logger,"Se han inicializado todos los semaforos con exito");
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

uint64_t timestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	long long result = (((long long)tv.tv_sec)*1000 + ((long)tv.tv_usec)/1000);
	uint64_t timefinal = result;
	return timefinal;
}

bool comparadorPrograma(t_programa* unPrograma, int unPid){
	return (unPid == unPrograma->pid);
}

t_hilo* calcularEstimacion(t_hilo *unHilo){
	unHilo->rafagasEstimadas = (alpha_sjf * unHilo->estimacionAnterior + ((1 - alpha_sjf)*unHilo->rafagasEjecutadas));
	unHilo->estimacionAnterior = unHilo->rafagasEstimadas;
	return unHilo;
}

bool comparadorDeRafagas(t_hilo *unHilo, t_hilo *otroHilo){
	return unHilo->rafagasEstimadas <= otroHilo->rafagasEstimadas;
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

bool comparadorDeSemaforos(t_semaforo *unSem, t_semaforo *otroSem){
	return (strcmp(unSem->semID,otroSem->semID)==0);
}

bool buscadorDeHilos(int tid, t_hilo* hilo){
	return (tid == hilo->tid);
}

void tomarMetricas(){
	for(int i=0;i<=list_size(lista_programas);i++){
		t_programa* unPrograma; // = malloc(sizeof(t_programa)); //Me parece que el list_get ya te lo devuelve malloceado
		unPrograma = list_get(lista_programas,i);
		t_list* hilosDelPrograma = unPrograma->hilos;
		hilosDelPrograma = list_map(hilosDelPrograma,(void*)calcularTiempoEjecucion);
		int tiempoTotalEjecucion = 0;
		//Hago una iteracion completa de los hilos para calcular el total de ejecucion
		for(int l=0;l<=list_size(hilosDelPrograma);l++){
			t_hilo* unhilo; //= malloc(sizeof(t_hilo)); //idem arriba
			unhilo = list_get(hilosDelPrograma,l);
			tiempoTotalEjecucion += unhilo->tiempoEjecucion;
		}
		//METRICAS POR HILO DE UN PROCESO
		for(int j=0;j<=list_size(hilosDelPrograma);j++){
			t_hilo* unHilo;// = malloc(sizeof(t_hilo)); //idem arriba
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
		t_semaforo* semaforo; //= malloc(sizeof(t_semaforo)); //creo que list_get ya te lo da malloceado
		semaforo = list_get(semaforos,k);
		printf("Semaforo:%s/n Valor Actual:%d/n",semaforo->semID, semaforo->semActual);
	}
	printf("Grado actual de multiprogramacion:%d/n",list_size(lista_programas));
}

void calcularTiempoEjecucion(t_hilo* hilo){
	int tiempoFinal = timestamp();
	hilo->tiempoEjecucion = (tiempoFinal - hilo->tiempoEjecucionInicial);
}

t_list* obtenerColaReady(t_programa* programa){
	return programa->cola_ready;
}

int sumatoria(int acumulador, int unValor){
	return acumulador+=unValor;
}

void planificador_NEW_READY() {
	//hilos en new ready y bloqueados, lo llamo cuando creo con create y lo llamo cuando cierro con close, es 1 solo hilo
	int hilosEnNew = list_size(cola_new);
	int hilosEnBlocked = list_size(cola_blocked);
	t_list* colasReady = list_map(lista_programas, (void*)obtenerColaReady);
	t_list* sizeColasReady = list_map(colasReady, (void*)list_size);
	int hilosEnReady = list_fold(sizeColasReady,0,(void*)sumatoria); //Chequear bien esto
	hilosEnSistema = hilosEnNew + hilosEnBlocked + hilosEnReady;
	if (hilosEnSistema <= max_multiprog){
		if (hilosEnNew == 0) {
			log_info(suse_logger, "No hay hilos para planificar actualmente");
		} else {
			t_hilo * unHilo;// = malloc(sizeof(t_hilo));
			unHilo = list_get(cola_new, 0);
			int ubicacionPrograma = list_get_index(lista_programas, unHilo,(void*) comparadorMismoPrograma);
			t_programa * programa; //= malloc(sizeof(t_programa));
			programa = list_get(lista_programas, ubicacionPrograma);
			list_add(programa->cola_ready, unHilo);
			t_hilo * hiloASacar;
			hiloASacar = list_remove(cola_new,0);
			log_info(suse_logger, "Se agrego un hilo a la cola de ready, TID:%d", unHilo->tid);
			//unHilo->tiempoEsperaInicial = gettimeofday();


			//SI AGREGO UN HILO A READY, DEBERIA LLAMAR AL SJF????????


			}
	}
	else
		log_info(suse_logger, "El sistema esta corriendo al nivel maximo de multiprogramacion, no se agregaran nuevos hilos a ready");
	//list_destroy(colasReady);
	//list_destroy(sizeColasReady);
}

bool comparadorMismoPrograma(t_programa* programa1, t_programa* programa2){
	return (programa1->pid == programa2->pid);
}

void* tomarMetricasAutomaticas(){
	while(1){
		tomarMetricas();
		sleep(metrics_timer);
	}
}

t_hilo* crearHiloNuevo(int pid, int tid){
	t_hilo* hiloNuevo = malloc(sizeof(t_hilo));
	hiloNuevo->pid = pid;
	hiloNuevo->tid = tid;
	hiloNuevo->estimacionAnterior = 0;
	hiloNuevo->porcentajeTiempoEjecucion = 0;
	hiloNuevo->finalizado = false;
	hiloNuevo->rafagasEjecutadas = 0;
	hiloNuevo->entradaAExec = 0;
	hiloNuevo->salidaDeExec = 0;
	hiloNuevo->rafagasEstimadas = 0;
	hiloNuevo->razon_bloqueado = "NULL";
	hiloNuevo->tiempoEjecucion = 0;
	hiloNuevo->tiempoEjecucionInicial = 0;
	hiloNuevo->tiempoEsperaFinal = 0;
	hiloNuevo->tiempoEsperaInicial = 0;
	hiloNuevo->tiempoUsoCPU = 0;
	hiloNuevo->tiempoUsoCPUFinal = 0;
	hiloNuevo->tiempoUsoCPUInicial = 0;
	hiloNuevo->tiempo_espera = 0;
	hiloNuevo->hilos_join = list_create();
	return hiloNuevo;
}

bool tieneHiloEnBlocked(t_programa* unPrograma) {
	if (!list_is_empty(cola_blocked)) {
		for (int i = 0; i < list_size(cola_blocked); i++) {
			t_programa * unElemento;
			unElemento = list_get(cola_blocked,i);
			if(unElemento->pid == unPrograma->pid){
				return true;
			}
		}
		return false;
	}
}

bool estaEnExit(int tid) {
	if (!list_is_empty(cola_exit)) {
		int size = list_size(cola_exit);
		for (int i = 0; i < size; i++) {
			t_hilo* unHilo;
			unHilo = list_get(cola_exit, i);
			int tidHilo = unHilo->tid;
			if (tidHilo == tid) {
				return true;
			}
		}
		return false;
	}
	return false;
}

//FUNCIONES DE SUSE


int hacer_suse_create(int pid, int tid){
	t_hilo* hiloNuevo = crearHiloNuevo(pid,tid);
	t_programa * programaBuscado; //= malloc(sizeof(t_programa));
	int index = list_get_index(lista_programas,hiloNuevo,(void*)comparadorMismoPrograma);
	programaBuscado = list_get(lista_programas,index);
	list_add(programaBuscado->hilos,hiloNuevo);
	if(hiloNuevo->tid == 0){
		list_add(programaBuscado->cola_exec,hiloNuevo);
		hiloNuevo->entradaAExec = timestamp();
		return 0;
	}
	list_add(cola_new, hiloNuevo);
	log_info(suse_logger,"Se ha agregado un hilo nuevo a la cola de new.\n");
	int hilosEnNew = list_size(cola_new);
	log_info(suse_logger,"Cantidad de elementos en cola new: %d\n", hilosEnNew);
	log_info(suse_logger,"ID del programa: %d\n",hiloNuevo->pid);
	log_info(suse_logger,"ID del hilo: %d\n",hiloNuevo->tid);
	planificador_NEW_READY();
//	//free(hiloNuevo); CREO que estos free no van porque tambien lo liberarian de la lista pero no estoy 100% seguro hay que probar
//	//free(programaBuscado);
	return 0;
}

int hacer_suse_schedule_next(int pid){
	t_programa * programaBuscado; //= malloc(sizeof(t_programa));
	int index = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
	programaBuscado = list_get(lista_programas,index);
	if(!list_is_empty(programaBuscado->cola_exec)){
		t_hilo* hiloAux1;
		hiloAux1 = list_remove(programaBuscado->cola_exec,0);
		hiloAux1->salidaDeExec = timestamp();
		hiloAux1->rafagasEjecutadas = (hiloAux1->salidaDeExec - hiloAux1->entradaAExec);
		list_add(programaBuscado->cola_ready, hiloAux1);
	}
	log_error(suse_logger, "La cola de exec del programa está vacia");
	log_error(suse_logger, "Proceso: %d", pid);
	if (!list_is_empty(programaBuscado->cola_ready)){
		log_info(suse_logger, "Se comenzará a planificar");
		t_list* aux;
		aux = list_map(programaBuscado->cola_ready,(void*)calcularEstimacion);
		list_sort(aux, (void*)comparadorDeRafagas);
		t_hilo* hiloAux; //= malloc(sizeof(t_hilo));
		hiloAux = (t_hilo*) list_remove(aux,0);
		int indice = list_get_index(programaBuscado->cola_ready,hiloAux,(void*)comparadorDeHilos);
		t_hilo* hiloAEjecutar = list_remove(programaBuscado->cola_ready,indice);
		if(pid == 0 && hiloAEjecutar->tid ==2)
			log_error(suse_logger, "ACA");
		//hiloAEjecutar->tiempoEsperaFinal = gettimeofday();
		//hiloAEjecutar->tiempo_espera += (hiloAEjecutar->tiempoEsperaFinal - hiloAEjecutar->tiempoEsperaInicial);
		list_add(programaBuscado->cola_exec,hiloAEjecutar);
		hiloAEjecutar->rafagasEjecutadas = 0;   //Lo reseteo cuando vuelve a entrar a exec
		hiloAEjecutar->entradaAExec = timestamp();
		//hiloAEjecutar->tiempoUsoCPUInicial = gettimeofday();
		//Tomo el tiempo final de espera
		//hiloAEjecutar->tiempoEsperaFinal = gettimeofday();
		//Y aca ya me queda guardado el tiempo de espera
		//hiloAEjecutar->tiempo_espera += (hiloAEjecutar->tiempoEsperaInicial - hiloAEjecutar->tiempoEsperaFinal);
		//free(programaBuscado); //creo que esto tambien le haria free al elemento de la lista
		//list_clean_and_destroy_elements(aux,(void*)free);
//		list_destroy(aux);
//		free(hiloAux);
		log_info(suse_logger,"\n \n El siguiente hilo a ejecutar es el tid: %i del proceso: %i \n \n",hiloAEjecutar->tid, hiloAEjecutar->pid);
		return hiloAEjecutar->tid;
	}
	//free(programaBuscado); //esto ya no es necesario porque la variable no se mallocea
	log_error(suse_logger, "La cola de ready del programa está vacia");
	log_error(suse_logger, "Proceso: %d", pid);
	return -1;
}

int hacer_suse_wait(int pid, int tid, char* semaforoID){
	t_semaforo* semAUsar = malloc(sizeof(t_semaforo));
	semAUsar->semID = semaforoID;
	int indice = list_get_index(semaforos,semAUsar,(void*)comparadorDeSemaforos);
	free(semAUsar); //falta verificacion
	semAUsar = list_get(semaforos,indice);
	semAUsar->semActual--;
	if (semAUsar->semActual < 0){
		log_info(suse_logger,"Contador inicial: %d", semAUsar->semInit);
		log_info(suse_logger,"Contador maximo: %d", semAUsar->semMax);
		log_info(suse_logger,"El semaforo se ha bloqueado, contador actual: %d",semAUsar->semActual);
		int index = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
		t_programa* programaBuscado; //= malloc(sizeof(t_programa));
		programaBuscado = list_get(lista_programas,index);
//		if(list_is_empty(programaBuscado->cola_exec)){
//			sem_post(&mutexWaitSignal);
//			return 0;
//		}
		t_hilo* hiloBuscado; //= malloc(sizeof(t_hilo));
		hiloBuscado = list_remove(programaBuscado->cola_exec,0);
		hiloBuscado->salidaDeExec = timestamp();
		hiloBuscado->rafagasEjecutadas = (hiloBuscado->salidaDeExec - hiloBuscado->entradaAExec);
		//hiloBuscado->tiempoUsoCPUFinal = gettimeofday();
		//hiloBuscado->tiempoUsoCPU += (hiloBuscado->tiempoUsoCPUFinal - hiloBuscado->tiempoUsoCPUInicial);
		list_add(cola_blocked,hiloBuscado);
		log_error(suse_logger, "\n \n \n EL HILO BLOQUEADO ES EL HILO: %i DEL PROCESO %i\n \n \n", hiloBuscado->tid, hiloBuscado->pid);
		list_add(semAUsar->hilosEnEspera,hiloBuscado);
		//free(programaBuscado);
		//free(hiloBuscado); Los comento porque creo que los liberaria tambien de las listas
		//free(semAUsar);
		return 0;
	}
	log_info(suse_logger,"Contador inicial: %d", semAUsar->semInit);
	log_info(suse_logger,"Contador maximo: %d", semAUsar->semMax);
	log_info(suse_logger, "Contador actual: %d", semAUsar->semActual);
	return 0;
}

int hacer_suse_signal(int pid, int tid, char* semaforoID){
	t_semaforo* semAUsar = malloc(sizeof(t_semaforo));
	semAUsar->semID = semaforoID;
	int indice = list_get_index(semaforos,semAUsar,(void*)comparadorDeSemaforos);
	//free(semAUsar);
	semAUsar = list_get(semaforos,indice);
	if (semAUsar->semActual == semAUsar->semMax){
		log_info(suse_logger,"Contador maximo: %d", semAUsar->semMax);
		log_error(suse_logger,"El semaforo ya ha alcanzado su contador maximo, no se puede realizar el signal");
		//free(semAUsar);
		return 0;
	}
	semAUsar->semActual++;
	log_info(suse_logger,"Contador inicial: %d", semAUsar->semInit);
	log_info(suse_logger,"Contador maximo: %d", semAUsar->semMax);
	log_info(suse_logger,"Contador actual: %d", semAUsar->semActual);
//	int index = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
//	t_programa* programaBuscado; //= malloc(sizeof(t_programa));
//	programaBuscado = list_get(lista_programas,index);

	if (semAUsar->semActual <= 0){
		if(list_is_empty(semAUsar->hilosEnEspera)){
			return 0;
		}
		t_hilo* hiloADesbloquear; //= malloc(sizeof(t_hilo));
		hiloADesbloquear = list_remove(semAUsar->hilosEnEspera,0);
		int index2 = list_get_index(cola_blocked,hiloADesbloquear,(void*)comparadorDeHilos);
		t_programa * programaBuscado; //= malloc(sizeof(t_programa));
		int index = list_get_index(lista_programas,hiloADesbloquear->pid,(void*)comparadorPrograma);
		programaBuscado = list_get(lista_programas,index);
		log_info(suse_logger,"\n \n \n Se va a liberar al TID: %i del proceso: %i \n \n \n", hiloADesbloquear->tid, hiloADesbloquear->pid);
		hiloADesbloquear = list_remove(cola_blocked,index2);
		list_add(programaBuscado->cola_ready,hiloADesbloquear);
		log_info(suse_logger,"Se agregó el hilo tid: %i ,  del proceso: %i , a la cola de ready", hiloADesbloquear->tid, hiloADesbloquear->pid);


//		int index3 = list_get_index(lista_programas, &pid, (void*)comparadorPrograma);
//		t_programa* programaQueEjecuta;
//		programaQueEjecuta = list_get(lista_programas,index3);
//		t_hilo* hiloQueEjecuta;
//		hiloQueEjecuta = list_get(programaQueEjecuta->cola_exec,0);
//		hiloQueEjecuta->salidaDeExec = timestamp();
//		hiloQueEjecuta->rafagasEjecutadas = (hiloQueEjecuta->salidaDeExec - hiloQueEjecuta->entradaAExec);
	}
	return 0;
	//hiloADesbloquear->tiempoEsperaInicial = gettimeofday();
	//free(programaBuscado);
	//free(hiloADesbloquear);
	//free(semAUsar);
}

int hacer_suse_join(int pid, int tid){
	if(!estaEnExit(tid)){
		int index2 = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
		t_programa* programaBuscado; //= malloc(sizeof(t_programa));
		programaBuscado = list_get(lista_programas,index2);
		t_hilo* hiloABloquear; //= malloc(sizeof(t_hilo));
		if(list_is_empty(programaBuscado->cola_exec)){
			log_info(suse_logger, "La cola de exec está vacia");
			return 0;
		}
		t_hilo* hiloAux = malloc(sizeof(t_hilo));
		hiloAux->tid = tid;
		int indice_hiloJoin = list_get_index(programaBuscado->hilos,hiloAux,(void*)comparadorDeHilos);
		free(hiloAux);
		hiloAux = list_get(programaBuscado->hilos,indice_hiloJoin);
		hiloABloquear = list_remove(programaBuscado->cola_exec,0);
		list_add(hiloAux->hilos_join, hiloABloquear);//
		hiloABloquear->salidaDeExec = timestamp();
		hiloABloquear->rafagasEjecutadas = (hiloABloquear->salidaDeExec - hiloABloquear->entradaAExec);
		log_error(suse_logger, "\n \n \n VOY A BLOQUEAR AL HILO: %i \n \n \n", hiloABloquear->tid);
		list_add(cola_blocked,hiloABloquear);
		//log_info(suse_logger, "Se va a enviar a bloqueado al hilo TID: %d", hiloABloquear->tid); //Esto me rompe por alguna razon
		int bloqueados = list_size(cola_blocked);
		log_info(suse_logger, "Hilos en blocked: %d", bloqueados);
		//hiloABloquear->tiempoUsoCPUFinal = gettimeofday();
		//hiloABloquear->tiempoUsoCPU += (hiloABloquear->tiempoUsoCPUFinal - hiloABloquear->tiempoUsoCPUInicial);
		//free(programaBuscado);
		//free(hiloABloquear);
		return 0;
	}
	log_info(suse_logger, "Se realizo un join sobre un hilo que ya termino");
	//Si el tid ya está en exit, entonces el hilo que lo llama no se bloquea
	return 0;
}

int hacer_suse_close(int pid, int tid){
	int index = list_get_index(lista_programas,pid,(void*)comparadorPrograma);
	t_programa* programaBuscado; //= malloc(sizeof(t_programa));
	programaBuscado = list_get(lista_programas,index);
	t_hilo* hiloATerminar; //= malloc(sizeof(t_hilo));
	if (list_is_empty(programaBuscado->cola_exec)){
		log_info(suse_logger, "No hay hilo para terminar");
		return 0; //Este caso realmente no creo que ocurra, pero por las dudas lo atajo
	}
	hiloATerminar = list_remove(programaBuscado->cola_exec, 0);
	hiloATerminar->salidaDeExec = timestamp();
	hiloATerminar->rafagasEjecutadas = (hiloATerminar->salidaDeExec - hiloATerminar->entradaAExec);
	//hiloATerminar->tiempoUsoCPUFinal = gettimeofday();
	//hiloATerminar->tiempoUsoCPU += (hiloATerminar->tiempoUsoCPUFinal - hiloATerminar->tiempoUsoCPUInicial);
	list_add(cola_exit,hiloATerminar);
	t_hilo *hiloAux;
	int indiceAux;
	for(int i=0; i < list_size(hiloATerminar->hilos_join); i=i+1){
		hiloAux = list_get(hiloATerminar->hilos_join, i);
		indiceAux = list_get_index(cola_blocked, hiloAux, (void*)comparadorDeHilos);
		list_remove(cola_blocked, indiceAux);
		list_add(programaBuscado->cola_ready,hiloAux);
		log_error(suse_logger, "VOY A PONER EN READY A %i", hiloAux->tid);
	}
	hiloATerminar->finalizado = true;
	int finalizados = list_size(cola_exit);
	log_info(suse_logger, "Se va a enviar a exit al hilo TID: %d", hiloATerminar->tid);
	log_info(suse_logger, "Hilos en exit: %d", finalizados);
	//free(hiloATerminar); mas alla de que este hilo termino, como lo agrega a la cola de exit, debe seguir malloceado, lo cual me genera la duda en que momento se limpia la cola de exit?
	//free(programaBuscado);
	//Cuando cierra un hilo toma las metricas
	//tomarMetricas();
	planificador_NEW_READY();
	return 0;
}


//FUNCION MAGICA


void atenderCliente(void* socket_cliente_void){
	int socket_cliente = *(int*) socket_cliente_void;
	log_info(suse_logger, "%d", socket_cliente);
	t_programa* programaNuevo = malloc(sizeof(t_programa));
	programaNuevo->pid = pidMAX;
	pidMAX++;
	programaNuevo->hilos = list_create();
	programaNuevo->cola_ready = list_create();
	programaNuevo->cola_exec = list_create();
	list_add(lista_programas, programaNuevo);
	while(1){
		HeaderSuse headerRecibido;
		headerRecibido = Suse_RecieveHeader(socket_cliente);
		log_info(suse_logger, "Codigo de operacion: %i", headerRecibido.operaciones);
		log_info(suse_logger, "Tamanio: %i", headerRecibido.tamanioMensaje);
		if(headerRecibido.operaciones == -1){
			log_error(suse_logger, "Se desconecto el cliente\n");
			break;
		}
		uint32_t tam = headerRecibido.tamanioMensaje;
		switch(headerRecibido.operaciones){

		//Las operaciones no tienen que enviar ninguna respuesta de vuelta creo

		case S_INIT:;
			log_info(suse_logger,"Bienvenido nuevo cliente!");
			break;

		case S_CREATE:;
			void* paqueteCreate = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(suse_logger, "Se recibió un pedido de Suse_Create");
			int tidCreate = Suse_Unpack_Uint32_pid(paqueteCreate); //En realidad recibe un TID
			log_info(suse_logger, "He recibido un TID : %i", tidCreate);
			free(paqueteCreate);
			int respuestaCreate = hacer_suse_create(programaNuevo->pid,tidCreate);
			if (respuestaCreate == 0)
				log_info(suse_logger, "La operacion Suse_Create se realizó con exito");
			else
				log_error(suse_logger, "La operacion Suse_Create ha fallado");
			Suse_PackAndSend_Respuesta(socket_cliente,respuestaCreate);
			log_info(suse_logger, "Se ha enviado una respuesta de suse_create");
			break;

		case S_SCHEDULE_NEXT:;
			void* paqueteSchedule = Suse_ReceiveAndUnpack(socket_cliente,tam);
			free(paqueteSchedule);
			log_info(suse_logger, "Se recibió un pedido de Suse_Schedule_Next");
			int respuestaNext = hacer_suse_schedule_next(programaNuevo->pid);
			if (respuestaNext != -1){ //Si4 devuelvo cualquier TID entra aca
				log_info(suse_logger, "La operacion Suse_Schedule_Next se realizó con exito");
				log_info(suse_logger, "El proximo hilo a ejecutar es el tid=%d",respuestaNext);
				Suse_PackAndSend_Respuesta(socket_cliente,respuestaNext);
				log_info(suse_logger, "Se ha enviado una respuesta de suse_schedule_next");
				break;
			}//Si falla, devuelve -1
			else{
				int hilosReady = list_size(programaNuevo->cola_ready);
				int hilosExec = list_size(programaNuevo->cola_exec);
				//int bloqueados = list_size(cola_blocked);
				int hilosRestantes = hilosReady + hilosExec;
				if(hilosRestantes == 0 && !tieneHiloEnBlocked(programaNuevo)){
					log_error(suse_logger, "No quedan mas hilos para planificar, se devolvera un NULL");
					Suse_PackAndSend_Respuesta(socket_cliente,NULL);
				}
				log_error(suse_logger, "La operacion Suse_Schedule_Next ha fallado");
				while(respuestaNext == -1)
				{
					respuestaNext = hacer_suse_schedule_next(programaNuevo->pid);
					sleep(1);
				}
				log_info(suse_logger, "La operacion Suse_Schedule_Next se realizó con exito");
				Suse_PackAndSend_Respuesta(socket_cliente,respuestaNext);
				log_info(suse_logger, "Se ha enviado una respuesta de suse_schedule_next");
				break;
			}

		case S_WAIT:;
			void* paqueteWait = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(suse_logger, "Se recibió un pedido de Suse_Wait");
			int tidWait = Suse_Unpack_Uint32_pid(paqueteWait);
			char* semIDWait = Suse_Unpack_Char(paqueteWait);
			free(paqueteWait);
			log_info(suse_logger, "Recibi el semaforo: %s", semIDWait);
			int respuestaWait = hacer_suse_wait(programaNuevo->pid, tidWait, semIDWait);
			if (respuestaWait == 0)
				log_info(suse_logger, "La operacion Suse_Wait se realizó con exito");
			else
				log_error(suse_logger, "La operacion Suse_Wait ha fallado");
			Suse_PackAndSend_Respuesta(socket_cliente,respuestaWait);
			log_info(suse_logger, "Se ha enviado una respuesta de suse_wait");
			break;

		case S_SIGNAL:;
			void* paqueteSignal = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(suse_logger, "Se recibió un pedido de Suse_Signal");
			int tidSignal = Suse_Unpack_Uint32_pid(paqueteSignal);
			char* semIDSignal = Suse_Unpack_Char(paqueteSignal);
			free(paqueteSignal);
			log_info(suse_logger, "Recibi el semaforo: %s", semIDSignal);
			int respuestaSignal = hacer_suse_signal(programaNuevo->pid,tidSignal, semIDSignal);
			if (respuestaSignal == 0)
				log_info(suse_logger, "La operacion Suse_Signal se realizó con exito");
			else
				log_error(suse_logger, "La operacion Suse_Signal ha fallado");
			Suse_PackAndSend_Respuesta(socket_cliente,respuestaSignal);
			log_info(suse_logger, "Se ha enviado una respuesta de suse_signal");
			break;

		case S_JOIN:;
			void* paqueteJoin = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(suse_logger, "Se recibió un pedida de Suse_Join");
			int tidJoin = Suse_Unpack_Uint32_pid(paqueteJoin);
			free(paqueteJoin);
			int respuestaJoin = hacer_suse_join(programaNuevo->pid, tidJoin);
			if (respuestaJoin == 0)
				log_info(suse_logger, "La operacion Suse_Join se realizó con exito");
			else
				log_error(suse_logger, "La operacion Suse_Join ha fallado");
			Suse_PackAndSend_Respuesta(socket_cliente,respuestaJoin);
			log_info(suse_logger, "Se ha enviado una respuesta de suse_join");
			break;

		case S_CLOSE:;
			void* paqueteClose = Suse_ReceiveAndUnpack(socket_cliente,tam);
			log_info(suse_logger, "Se recibió un pedida de Suse_Close");
			int tidClose = Suse_Unpack_Uint32_pid(paqueteClose);
			free(paqueteClose);
			int respuestaClose = hacer_suse_close(programaNuevo->pid, tidClose);
			if (respuestaClose == 0)
				log_info(suse_logger, "La operacion Suse_Close se realizó con exito");
			else
				log_error(suse_logger, "La operacion Suse_Close ha fallado");
			Suse_PackAndSend_Respuesta(socket_cliente,respuestaClose);
			log_info(suse_logger, "Se ha enviado una respuesta de suse_close");
			break;

		default:
			log_error(suse_logger, "No es un codigo conocido: %i", headerRecibido.operaciones);
			break;
		}
	}
}

