#include "Suse.h"

int main(){

	/* INICIALIZAR SERVIDOR */

	crearLogger();
	leerArchivoDeConfiguracion();

	cola_new = list_create();
	cola_blocked = list_create();
	cola_exit = list_create();

	tidMAX=0;

	log_info(logger,"\n [+]La configuracion es la siguiente \n");
	log_info(logger,"LISTEN_PORT ->  %s",listen_port );
	log_info(logger,"Metrics_timer ->  %d",metrics_timer );
	log_info(logger,"MAX_MULTIPROG ->  %d",max_multiprog);
	log_info(logger,"--------------\n");
	printf("\n\n::::::::INICIAMOS EL SERVIDOR SUSE::::::::\n");

	socket_Suse = iniciar_servidor(server_ip,listen_port,logger);


	/* PLANIFICADOR NEW -> READY */

	lista_programas = list_create();
	int cantidadProgramas = list_size(lista_programas);

	while (cantidadProgramas < max_multiprog){
		pthread_t * hiloPlani = malloc(sizeof(pthread_t));
		int estadoHilo = pthread_create(hiloPlani, NULL,planificador_NEW_READY(), NULL);
		if (estadoHilo){
			printf("No se pudo crear el hilo para *PLANIFICADOR NEW READY*\n");
			free(hiloPlani);
			return 0;
		}
	}

while(1){

	socket_cliente = esperar_cliente_con_accept(socket_Suse,logger);
	enviar_mensaje(socket_cliente,logger);


	/* CREAR PROGRAMA NUEVO CUANDO LLEGA UNA CONEXION*/

	programa_t* programaNuevo;
	programaNuevo->pid = socket_cliente;
	programaNuevo->cola_ready = list_create();
	programaNuevo->cola_exec = list_create();
	list_add(lista_programas, programaNuevo); //Este warning no afecta


	/* TOMAR METRICAS */

	pthread_t * hiloMetricas = malloc(sizeof(pthread_t));
	int estadoHilo = pthread_create(hiloMetricas, NULL,tomarMetricasAutomaticas(), NULL);
	if (estadoHilo){
		printf("No se pudo crear el hilo para *TOMAR METRICAS*\n");
		free(hiloMetricas);
		return 0;
	}

	/* RECIBIR PAQUETES */

	Paquete* paquete;

	int status = 1;		// Estructura que manjea el status de los recieve.


	pthread_t * hiloPaquetes = malloc(sizeof(pthread_t));

	while (status){
			status = pthread_create(hiloPaquetes, NULL,recibir_paquete_deserializar(socket_cliente,paquete), NULL);
		}
		if(!status)
			free(hiloPaquetes);
			puts("Se Desconecto el cliente ...");

	}
	//ACA CREO QUE HAY QUE JOINEAR TODOS LOS HILOS



	return 0;
}

void* tomarMetricasAutomaticas(){
	while(1){
		tomarMetricas();
		sleep(metrics_timer);
	}
}

int gettimeofday(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long)tv.tv_sec)*1000 + ((unsigned long long)tv.tv_usec)/1000);
	int a = result;
	return result;
}

void crearLogger(){
	char* logPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/Suse/src/SUSE.log";
	char* nombreArch = "SUSE";
	bool consolaActiva = true;
	logger = log_create(logPath, nombreArch, consolaActiva, LOG_LEVEL_INFO);
	log_info(logger, "El logger se creo con exito");
	//free(logPath);
}

void leerArchivoDeConfiguracion(){
	char* configPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/Suse/src/SUSE.cfg";
	archivoConfig = config_create(configPath);
	if (archivoConfig == NULL){
		log_error(logger,"ERROR: Archivo de configuracion no encontrado");
	}
	setearValores(archivoConfig);
	log_info(logger,"La configuracion fue cargada exitosamente");
	config_destroy(archivoConfig);
}

void setearValores(t_config* archivoConfig){
	server_ip = strdup(config_get_string_value(archivoConfig,"SERVER_IP"));
	listen_port = strdup(config_get_string_value(archivoConfig,"LISTEN_PORT"));
	metrics_timer = config_get_int_value(archivoConfig,"METRICS_TIMER");
	max_multiprog = config_get_int_value(archivoConfig,"MAX_MULTIPROG");
	sems_ids = config_get_array_value(archivoConfig, "SEM_IDS");
	sem_init = config_get_array_value(archivoConfig, "SEM_INIT");
	sem_max = config_get_array_value(archivoConfig, "SEM_MAX");
	alpha_sjf = config_get_int_value(archivoConfig, "ALPHA_SJF");
}

void cargarSemaforos(){
	int i;
	//Para todos los ID de semaforos, voy creando un semaforo nuevo y lo guardo en la lista de semaforos
	for(i=0; i<= list_size(sems_ids); i++){
		semaforo_t* semaforo;
		strcpy(semaforo->semID,list_get(sems_ids,i)); //este warning no afecta
		semaforo->semInit = list_get(sem_init,i);
		semaforo->semActual = semaforo->semInit;
		semaforo->semMax =	list_get(sem_max,i);
		list_add(semaforos, semaforo);
	}
	log_info(logger,"Se han inicializado todos los semaforos con exito");
}

void calcularTiempoEjecucion(hilo_t* hilo){
	int tiempoFinal = gettimeofday();
	hilo->tiempoEjecucion = (tiempoFinal - hilo->tiempoEjecucionInicial);
}

void tomarMetricas(){
	for(int i=0;i<=list_size(lista_programas);i++){
		programa_t* unPrograma = list_get(lista_programas,i);
		t_list* hilosDelPrograma = unPrograma->hilos;
		hilosDelPrograma = list_map(hilosDelPrograma,(void*)calcularTiempoEjecucion);
		int tiempoTotalEjecucion = 0;
		//Hago una iteracion completa de los hilos para calcular el total de ejecucion
		for(int l=0;l<=list_size(hilosDelPrograma);l++){
			hilo_t* unhilo = list_get(hilosDelPrograma,l);
			tiempoTotalEjecucion += unhilo->tiempoEjecucion;
		}
		//METRICAS POR HILO DE UN PROCESO
		for(int j=0;j<=list_size(hilosDelPrograma);j++){
			hilo_t* unHilo = list_get(hilosDelPrograma,j);
			unHilo->porcentajeTiempoEjecucion = (unHilo->tiempoEjecucion / tiempoTotalEjecucion)*100 ;
			printf("Proceso:%s/n Hilo:%s/n Tiempo de Ejecucion:%d/n Tiempo de Espera:%d/n Tiempo de Uso de CPU:%d/n Porcentaje del tiempo de Ejecucion:%f/n",
					unPrograma->pid,unHilo->tid,unHilo->tiempoEjecucion,unHilo->tiempo_espera,unHilo->tiempoUsoCPU,unHilo->porcentajeTiempoEjecucion);
		}
		//METRICAS DEL PROCESO
		int hilosEnNew = list_size(cola_new);
		int hilosEnBlocked = list_size(cola_blocked);
		int hilosEnReady = list_size(unPrograma->cola_ready);
		int hilosEnExec = list_size(unPrograma->cola_exec);
		printf("Proceso:%s/n Hilos en estado NEW:%d/n Hilos en estado BLOCKED:%d/n Hilos en estado READY:%d/n Hilos en estado EXEC:%d/n",unPrograma->pid,
				hilosEnNew,hilosEnBlocked,hilosEnReady,hilosEnExec);
	}
	//METRICAS DEL SISTEMA
	for(int k=0;k<=list_size(semaforos);k++){
		semaforo_t* semaforo = list_get(semaforos,k);
		printf("Semaforo:%s/n Valor Actual:%d/n",semaforo->semID, semaforo->semActual);
	}
	printf("Grado actual de multiprogramacion:%d/n",list_size(lista_programas));
}

/*
//funcion prueba
int sumar2(int a){
	return a+2;
}
*/

void* suse_create(int pid_prog){

	hilo_t* hiloNuevo = malloc(sizeof(hilo_t));

	hiloNuevo->pid = pid_prog;
	hiloNuevo->tid = tidMAX;
	hiloNuevo->tiempoEjecucionInicial = gettimeofday();
	tidMAX++;

	programa_t * programaBuscado;
	int index = list_get_index(lista_programas,pid_prog,(void*)comparadorPrograma);
	programaBuscado = list_get(lista_programas,index);
	list_add(programaBuscado->hilos,hiloNuevo);

	list_add(cola_new, hiloNuevo);
	log_info(logger,"Se ha agregado un hilo nuevo a la cola de new.\n");

	int cantidadCola = list_size(cola_new);
	printf("Cantidad de elementos en cola new: %d\n", cantidadCola);
	printf("ID del programa: %s\n",hiloNuevo->pid);
	//printf("ID del hilo: %d\n",hiloNuevo->tid);

	free(hiloNuevo);

}

bool comparadorPrograma(char* unPid, programa_t* unPrograma){
	return strcmp(unPid, unPrograma->pid);
}

void * suse_schedule_next(int pid_prog){
	programa_t * programaBuscado;
	int index = list_get_index(lista_programas,pid_prog,(void*)comparadorPrograma);
	programaBuscado = list_get(lista_programas,index);
	if (!list_is_empty(programaBuscado->cola_ready)&& list_is_empty(programaBuscado->cola_exec)){
		log_info(logger, "Se comenzará a planificar");
		t_list* aux;
		aux = list_map(programaBuscado->cola_ready,(void*)calcularEstimacion);
		list_sort(aux, (void*)comparadorDeRafagas);
		hilo_t* hiloAux = (hilo_t*) list_remove(aux,0);
		int indice = list_get_index(programaBuscado->cola_ready,hiloAux,(void*)comparadorDeHilos);
		hilo_t* hiloAEjecutar = list_remove(programaBuscado->cola_ready,indice);
		hiloAEjecutar->tiempoEsperaFinal = gettimeofday();
		hiloAEjecutar->tiempo_espera += (hiloAEjecutar->tiempoEsperaFinal - hiloAEjecutar->tiempoEsperaInicial);
		list_add(programaBuscado->cola_exec,hiloAEjecutar);
		hiloAEjecutar->tiempoUsoCPUInicial = gettimeofday();
		//Tomo el tiempo final de espera
		hiloAEjecutar->tiempoEsperaFinal = gettimeofday();
		//Y aca ya me queda guardado el tiempo de espera
		hiloAEjecutar->tiempo_espera += (hiloAEjecutar->tiempoEsperaInicial - hiloAEjecutar->tiempoEsperaFinal);
		free(programaBuscado);
		free(aux);
		free(hiloAux);
		return hiloAEjecutar;
	}
	free(programaBuscado);
	log_error(logger, "La cola de ready del programa está vacia o ya tiene un hilo ejecutando");
	return 0;
}
bool comparadorDeHilos(hilo_t* unHilo, hilo_t* otroHilo){
	return (strcmp(unHilo->tid,otroHilo->tid)== 0);
}

hilo_t calcularEstimacion(hilo_t unHilo){
	unHilo.rafagasEstimadas = (alpha_sjf * unHilo.estimacionAnterior + ((1 - alpha_sjf)*unHilo.rafagasEjecutadas));
	return unHilo;
}
bool comparadorDeRafagas(hilo_t unHilo, hilo_t otroHilo){
	return unHilo.rafagasEstimadas <= otroHilo.rafagasEstimadas;
}
//Verifica que el semaforo que se pasa por parametro tenga un ID que exista en la lista de IDs de semaforos
int buscadorSemaforo (char* semaforo){
	for(int i = 0; i<=list_size(sems_ids); i++){
		if (strcmp(semaforo,list_get(sems_ids,i)) == 0){
			return 0;
		}
		i++;
	}
	return -1;
}

void * suse_wait(int pid_prog, char * semaforo){
	if(buscadorSemaforo(semaforo) == 0){
		int indice = list_get_index(semaforos,semaforo,(void*)comparadorDeSemaforos);
		semaforo_t* semAUsar = list_get(semaforos,indice);
		if (semAUsar->semActual <= 0){
			semAUsar->semActual--;
			log_info(logger,"%d","Contador inicial:", semAUsar->semInit);
			log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
			log_info(logger,"%d","El semaforo se ha bloqueado, contador actual:",semAUsar->semActual);
			int index = list_get_index(lista_programas,pid_prog,(void*)comparadorPrograma);
			programa_t* programaBuscado = list_get(lista_programas,index);
			hilo_t* hiloBuscado = list_remove(programaBuscado->cola_exec,0);
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
}

bool comparadorDeSemaforos(char* unSem, semaforo_t otroSem){
	return (strcmp(unSem,otroSem.semID)==0);
}

void * suse_signal(int pid_prog, char * semaforo){
	if(buscadorSemaforo(semaforo) == 0){
		int indice = list_get_index(semaforos,semaforo,(void*)comparadorDeSemaforos);
		semaforo_t* semAUsar = list_get(semaforos,indice);
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
		int index = list_get_index(lista_programas,pid_prog,(void*)comparadorPrograma);
		programa_t* programaBuscado = list_get(lista_programas,index);
		hilo_t* hiloADesbloquear = list_remove(semAUsar->hilosEnEspera,0);
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
}

bool buscadorDeHilos(char* tid, hilo_t* hilo){
	return (strcmp(tid,hilo->tid)==0);
}

void * suse_join(int pid_prog, char * tid){
	int index = list_get_index(cola_exit,tid,(void*)buscadorDeHilos);
	//Si el tid pasado no está en exit entonces procedo normalmente
	if(index == list_size(cola_exit)){
		int index2 = list_get_index(lista_programas,pid_prog,(void*)comparadorPrograma);
		programa_t* programaBuscado = list_get(lista_programas,index2);
		hilo_t* hiloABloquear = list_remove(programaBuscado->cola_exec,0);
		hiloABloquear->tiempoUsoCPUFinal = gettimeofday();
		hiloABloquear->tiempoUsoCPU += (hiloABloquear->tiempoUsoCPUFinal - hiloABloquear->tiempoUsoCPUInicial);
		free(programaBuscado);
		free(hiloABloquear);
		return 0;
	}
	//Si el tid ya está en exit, entonces el hilo que lo llama no se bloquea
	return 0;
}

void * suse_close(int pid_prog, char * tid){
	int index = list_get_index(lista_programas,pid_prog,(void*)comparadorPrograma);
	programa_t* programaBuscado = list_get(lista_programas,index);
	hilo_t* hiloATerminar = list_remove(programaBuscado->cola_exec, 0);
	hiloATerminar->tiempoUsoCPUFinal = gettimeofday();
	hiloATerminar->tiempoUsoCPU += (hiloATerminar->tiempoUsoCPUFinal - hiloATerminar->tiempoUsoCPUInicial);
	list_add(cola_exit,hiloATerminar);
	hiloATerminar->finalizado = true;
	free(hiloATerminar);
	free(programaBuscado);
	//Cuando cierra un hilo toma las metricas
	tomarMetricas(); //Esto por ahi tambien deberia ser un hilo
	return 0;
}

int recibir_paquete_deserializar(int socket_cliente, Paquete* pack){

	int resultadoEnvio = RecibirPaqueteCliente(socket_cliente, pack);
	//Verifico que el envio se haya realizado con exito
	if (resultadoEnvio == 0) {
		pthread_t * idHilo = malloc(sizeof(pthread_t));
		char * servicio_suse = malloc(sizeof(char*));
		strcpy(servicio_suse, pack->header.tipoMensaje);
		if (strcmp(servicio_suse, "SUSE_CREATE")) {
			int estadoHilo = pthread_create(idHilo, NULL,suse_create(socket_cliente), NULL);
			if (estadoHilo)
				printf("No se pudo crear el hilo para *SUSE_CREATE*\n");
			free(idHilo);
			return 0;
		} else if (strcmp(servicio_suse, "SUSE_SCHEDULE_NEXT")) {
			int estadoHilo = pthread_create(idHilo, NULL,suse_schedule_next(socket_cliente), NULL);
			if (estadoHilo)
				printf("No se pudo crear el hilo para *SUSE_SCHELUDE_NEXT*\n");
			free(idHilo);
			return 0;
		} else if (strcmp(servicio_suse, "SUSE_WAIT")) {
			int estadoHilo = pthread_create(idHilo, NULL,suse_wait(socket_cliente, pack->mensaje), NULL);
			if (estadoHilo)
				printf("No se pudo crear el hilo para *SUSE_WAIT*\n");
			free(idHilo);
			return 0;
		} else if (strcmp(servicio_suse, "SUSE_JOIN")) {
			int estadoHilo = pthread_create(idHilo, NULL,suse_join(socket_cliente, pack->mensaje), NULL);
			if (estadoHilo)
				printf("No se pudo crear el hilo para *SUSE_JOIN*\n");
			free(idHilo);
			return 0;
		} else if (strcmp(servicio_suse, "SUSE_SIGNAL")) {
			int estadoHilo = pthread_create(idHilo, NULL,suse_signal(socket_cliente, pack->mensaje), NULL);
			if (estadoHilo)
				printf("No se pudo crear el hilo para *SUSE_SIGNAL*\n");
			free(idHilo);
			return 0;
		} else if (strcmp(servicio_suse, "SUSE_CLOSE")) {
			int estadoHilo = pthread_create(idHilo, NULL,suse_close(socket_cliente, pack->mensaje), NULL);
			if (estadoHilo)
				printf("No se pudo crear el hilo para *SUSE_CLOSE*\n");
			free(idHilo);
			return 0;
		}
	}
	log_info(logger, "No se pudo recibir el paquete\n");
	return 1;
}

void * planificador_NEW_READY(){ //aun no se que pasarle como parametro y donde iniciarlo

		if(list_is_empty(cola_new)){
			printf("No hay hilos para planificar.\n");
			return 0;
			//break;
		}

		int cantidadHilosNew = list_size(cola_new);
		int cantidadProgramas = list_size(lista_programas);

		while(cantidadProgramas == 0){
			cantidadHilosNew = list_size(cola_new);
			hilo_t * unHilo = malloc(sizeof(hilo_t));
			unHilo = list_get(cola_new,0); // tomo el primer elemento
			t_list * hilosDeIgualPadre = list_filter(cola_new,(void*)comparadorMismoPrograma); //filtro los elementos que cumplen
			programa_t * nuevoPrograma = malloc(sizeof(programa_t));
			nuevoPrograma->pid = unHilo->pid;
			list_add_all(nuevoPrograma->cola_ready, hilosDeIgualPadre);
			//ESTE FOR ES PARA LA TOMA DE METRICAS
			for (int i=0; i<=list_size(hilosDeIgualPadre); i++){
				hilo_t* hiloReady = list_get(hilosDeIgualPadre,i);
				hiloReady->tiempoEsperaInicial = gettimeofday();
			}
			nuevoPrograma->cola_exec=NULL;
			list_add(lista_programas,nuevoPrograma);
			free(nuevoPrograma);
			free(unHilo);
			cantidadProgramas++;
			break;
			}

		while(cantidadProgramas>0 && cantidadProgramas<=max_multiprog){
			cantidadHilosNew = list_size(cola_new);
			hilo_t * unHilo = malloc(sizeof(hilo_t));
			unHilo = list_get(cola_new,0); // tomo el primer elemento
			t_list * hilosDeIgualPadre = list_filter(cola_new,(void*)comparadorMismoPrograma);
			int ubicacionPrograma = list_get_index(lista_programas,unHilo->pid,(void*)comparadorMismoPrograma);
				if(ubicacionPrograma < list_size(lista_programas)){ //SI ESTÁ EN LA LISTA DE PROGRAMAS
					programa_t * programa = malloc(sizeof(programa_t));
					programa= list_get(lista_programas,ubicacionPrograma);
					list_add_all(programa->cola_ready,hilosDeIgualPadre);
					for (int i=0; i<=list_size(hilosDeIgualPadre); i++){
						hilo_t* hiloReady = list_get(hilosDeIgualPadre,i);
						hiloReady->tiempoEsperaInicial = gettimeofday();
					}
					free(unHilo);
					free(programa);
				}
				if(ubicacionPrograma == list_size(lista_programas)){ //SI NO ESTÁ EN LA LISTA DE PROGRAMAS
					cantidadHilosNew = list_size(cola_new);
					hilo_t * unHilo = malloc(sizeof(hilo_t));
					unHilo = list_get(cola_new,0); // tomo el primer elemento
					t_list * hilosDeIgualPadre = list_filter(cola_new,(void*)comparadorMismoPrograma);
					programa_t * nuevoPrograma = malloc(sizeof(programa_t));
					nuevoPrograma->pid = unHilo->pid;
					list_add_all(nuevoPrograma->cola_ready, hilosDeIgualPadre);
					for (int i=0; i<=list_size(hilosDeIgualPadre); i++){
						hilo_t* hiloReady = list_get(hilosDeIgualPadre,i);
						hiloReady->tiempoEsperaInicial = gettimeofday();
					}
					nuevoPrograma->cola_exec=NULL;
					list_add(lista_programas,nuevoPrograma);
					free(nuevoPrograma);
					free(unHilo);
					cantidadProgramas++;
				}
		}
}

bool comparadorMismoPrograma(hilo_t * hilo1, char * pid_programa){
	return (strcmp(hilo1->pid,pid_programa)==0);
}


