#include "Suse.h"

void * suse_create(int pid_prog ,t_list * tid){
	/*
	hilo_t* hiloNuevo = malloc(sizeof(hilo_t));

	hiloNuevo->pid = pid_prog;
	//hiloNuevo->tid = tid;
	tidMAX++;

	list_add(cola_new, hiloNuevo);
	log_info(logger,"Se ha agregado un hilo nuevo a la cola de new.\n");

	int cantidadCola = list_size(cola_new);
	printf("Cantidad de elementos en cola new: %d\n", cantidadCola);
	printf("ID del programa: %d\n",hiloNuevo->pid);
	//printf("ID del hilo: %d\n",hiloNuevo->tid);

	free(hiloNuevo);
*/
}

int main(){

	/*INicializando Servidor Suse*/
	crearLogger();
	leerArchivoDeConfiguracion();
	cola_new = list_create();
	tidMAX=0;

	log_info(logger,"\n [+]La configuracion es la siguiente \n");
	log_info(logger,"LISTEN_PORT ->  %s",listen_port );
	log_info(logger,"Metrics_timer ->  %d",metrics_timer );
	log_info(logger,"MAx_MULTIPROG ->  %d",max_multiprog);
	log_info(logger,"--------------\n");
	printf("\n\n::::::::INICIAMOS EL SERVIDOR SUSE::::::::\n");

	socket_Suse = iniciar_servidor("127.0.0.1",listen_port,logger);

/*while(1){

	socket_cliente = esperar_cliente_con_accept(socket_Suse,logger);
	enviar_mensaje(socket_cliente,logger);

	Paquete paquete;

	int status = 1;		// Estructura que manjea el status de los recieve.

	while (status){

			//status = recibir_paquete_deserializar(socket_cliente,paquete);
		}
		if(!status)
			puts("Se Desconecto el cliente ...");

	}
return 0;
*/ //COMENTO PARA PODER JUGAR CON LAS COLAS *
}

void crearLogger(){
	char* logPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/Suse/src/SUSE.log";
	char* nombreArch = "SUSE";
	bool consolaActiva = true;
	logger = log_create(logPath, nombreArch, consolaActiva, LOG_LEVEL_INFO);
	log_info(logger, "El logger se creo con exito");
//	free(logPath);
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
	listen_port = strdup(config_get_string_value(archivoConfig,"LISTEN_PORT"));
	metrics_timer = config_get_int_value(archivoConfig,"METRICS_TIMER");
	max_multiprog = config_get_int_value(archivoConfig,"MAX_MULTIPROG");
	sems_ids = config_get_array_value(archivoConfig, "SEM_IDS");
	sem_init = config_get_array_value(archivoConfig, "SEM_INIT");
	sem_max = config_get_array_value(archivoConfig, "SEM_MAX");
	alpha_sjf = config_get_int_value(archivoConfig, "ALPHA_SJF");
}


void suse_init(){
	//hilolay_init();
	cargarSemaforos();
}

//Crear nuevo hilo -> pasar funcion por parametro que sera el main del hilo -> el hilo finaliza cuando termina funcion.
//Cambiar pthread por hilolay

void cargarSemaforos(){
	int i;
	//Para todos los ID de semaforos, voy creando un semaforo nuevo y lo guardo en la lista de semaforos
	for(i=0; i<= list_size(sems_ids); i++){
		semaforo_t* semaforo;
		strcpy(semaforo->semID,list_get(sems_ids,i));
		semaforo->semInit = list_get(sem_init,i);
		semaforo->semActual = semaforo->semInit;
		semaforo->semMax =	list_get(sem_max,i);
		list_add(semaforos, semaforo);
	}
	log_info(logger,"Se han inicializado todos los semaforos con exito");
}

//funcion prueba
int sumar2(int a){
	return a+2;
}

void * suse_schedule_next(int socket_cliente){
	if (!list_is_empty(cola_new)){
		log_info(logger, "Se comenzará a planificar");

		//Creo una lista auxiliar con los hilos con la estimacion calculada
		t_list* aux;
		aux = list_map(cola_new,(void*)calcularEstimacion);

		//Ordeno los hilos segun su estimacion
		list_sort(aux, (void*)comparadorDeRafagas);

		//Tomo el primer elemento de esa lista ordenada
		hilo_t* hiloAux = (hilo_t*) list_remove(aux,0);

		bool comparador(hilo_t* unHilo, hilo_t* otroHilo){
			return (strcmp(unHilo->tid,otroHilo->tid)== 0);
		}
		//Busco el indice en la cola de new comparando los TID, si lo encuentro, lo elimino de la cola de new y lo devuelvo
		int indice = list_get_index(cola_new,hiloAux,(void*)comparador);
		hilo_t* hiloAEjecutar = list_remove(cola_new,indice);
		//char* pidAux = hiloAEjecutar->pid;
		//Busco el proceso y obtengo su cola de exec
		//list_add(hiloAEjecutar,proceso->cola_exec);


		//return hiloAEjecutar;
	}
	log_info(logger, "La cola de new está vacia");
	//return 0;
}
hilo_t calcularEstimacion(hilo_t unHilo){
	unHilo.rafagasEstimadas = (alpha_sjf * unHilo.estimacionAnterior + ((1 - alpha_sjf)*unHilo.rafagasEjecutadas));
	return unHilo;
}
bool comparadorDeRafagas(hilo_t unHilo, hilo_t otroHilo){
	return unHilo.rafagasEstimadas <= otroHilo.rafagasEstimadas;
}

int list_get_index(t_list* self, void* elemento, bool(*comparador (void*, void*))){
	int longLista = list_size(self);
	int i;
	int contador = 0;
	for(i=0 ; i<longLista; i++){
		if(!comparador(list_get(self,i),elemento)){
			contador++;
		}else{
			break;
		}
	}
	return contador;
}


//Verifica que el semaforo que se pasa por parametro tenga un ID que exista en la lista de IDs de semaforos
int buscadorSemaforo (semaforo_t* semaforo){
	for(int i = 0; i<=list_size(sems_ids); i++){
		if (strcmp(semaforo->semID,list_get(sems_ids,i)) ==0){
			return 0;
		}
		i++;
	}
	return -1;
}

void * suse_wait(int socket_cliente, t_list * semaforo){}
	/*if(buscadorSemaforo(semaforo) == 0){
		int indice = list_get_index(semaforos,semaforo,(void*)comparadorDeSemaforos);
		semaforo_t* semAUsar = list_get(semaforos,indice);
		if (semAUsar->semActual == 0){
			semAUsar->semActual--;
			log_info(logger,"%d","Contador inicial:", semAUsar->semInit);
			log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
			log_info(logger,"%d","El semaforo se ha bloqueado, contador actual:",semAUsar->semActual);
			//Tengo que buscar el proceso asociado al tid
			//hilo_t * hiloBuscado = list_remove(proceso->cola_ready,0);
			//list_add(semaforo->hilosEnEspera, hiloBuscado);
			return -1;
		}
		semaforo->semActual--;
		log_info(logger,"%d","Contador inicial:", semAUsar->semInit);
		log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
		log_info(logger, "%d","Contador actual:", semaforo->semActual);
		return 0;
	}
	log_info(logger, "El semaforo no fue encontrado");
	return -1;

}*/

bool comparadorDeSemaforos(semaforo_t unSem, semaforo_t otroSem){
	return unSem.semID == otroSem.semID;
}

void * suse_signal(int socket_cliente,t_list * semaforo){
	/*if(buscadorSemaforo(semaforo) == 0){
int suse_signal(semaforo_t* semaforo, char*tid){
	if(buscadorSemaforo(semaforo) == 0){
		int indice = list_get_index(semaforos,semaforo,(void*)comparadorDeSemaforos);
		semaforo_t* semAUsar = list_get(semaforos,indice);
		if (semAUsar->semActual == semAUsar->semMax){
			log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
			log_error(logger,"El semaforo ya ha alcanzado su contador maximo, no se puede realizar el signal");
			//return -1;
		}
		semAUsar->semActual++;
		log_info(logger,"%d","Contador inicial:", semAUsar->semInit);
		log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
		log_info(logger,"%d","Contador actual:", semAUsar->semActual);
		//Tengo que buscar el proceso asociado al tid
		//hilo_t * hiloDesbloqueado = list_remove(semaforo->hilosEnEspera,0);
		//list_add(hiloDesbloqueado,proceso->cola_ready);
		//return 0;
	}
	log_info(logger, "El semaforo no fue encontrado");
	//return -1;*/
}

//hace lo mismo que pthread_join. TIene como parametro un hilo y su estado de retorno.
void * suse_join(int socket_cliente, t_list * tid){}/*

	int rafagaTotal = unHilo.rafagasEstimadas - unHilo.rafagasEjecutadas
	while(rafagaTotal >0){
		//bloquear hilo (meter alguna variable local en el nodo de la cola donde esta el hilo
		sleep(1); //que pase 1 segundo
		unHilo.rafagasEjecutadas++;
		rafagaTotal--;
	}
	//des
}
*/

//Funcion que crea las colas ready segun el grado de multiprogramacion
void * suse_close(int socket_cliente,t_list * tid){}
	//Tengo que buscar el proceso asociado al tid
	//hilo_t *hiloAFinalizar = list_remove(proceso->cola_exec,0);
	//list_add(cola_exit, hiloAfinalizar);
//Close recibe un int TID, y mandas el thread ese a Exit

int recibir_paquete_deserializar(int socket_cliente, Paquete * pack){

       t_list* cosas = RecibirPaqueteCliente(socket_cliente, pack);
       pthread_t * idHilo = malloc(sizeof(pthread_t));

       char * servicio_suse = malloc(sizeof(char*));
       strcpy(servicio_suse,pack->header.tipoMensaje);
       if(strcmp(servicio_suse,"SUSE_CREATE")){
    	   int estadoHilo = pthread_create(idHilo,NULL,suse_create(socket_cliente,cosas),NULL);
    	   if (estadoHilo)printf("No se pudo crear el hilo para *SUSE_CREATE*\n");
    	   free(idHilo);
    	   return 0;
       }
       else if(strcmp(servicio_suse,"SUSE_SCHEDULE_NEXT")){
    	   int estadoHilo = pthread_create(idHilo,NULL,suse_schedule_next(socket_cliente),NULL);
           if (estadoHilo)printf("No se pudo crear el hilo para *SUSE_SCHELUDE_NEXT*\n");
           free(idHilo);
           return 0;
       }
       else if(strcmp(servicio_suse,"SUSE_WAIT")){
		   int estadoHilo = pthread_create(idHilo,NULL,suse_wait(socket_cliente,cosas),NULL);
		   if (estadoHilo)printf("No se pudo crear el hilo para *SUSE_WAIT*\n");
		   free(idHilo);
		   return 0;
       }
       else if(strcmp(servicio_suse,"SUSE_JOIN")){
		   int estadoHilo = pthread_create(idHilo,NULL,suse_join(socket_cliente,cosas),NULL);
		   if (estadoHilo)printf("No se pudo crear el hilo para *SUSE_JOIN*\n");
		   free(idHilo);
		   return 0;
       }
       else if(strcmp(servicio_suse,"SUSE_SIGNAL")){
		   int estadoHilo = pthread_create(idHilo,NULL,suse_signal(socket_cliente,cosas),NULL);
		   if (estadoHilo)printf("No se pudo crear el hilo para *SUSE_SIGNAL*\n");
		   free(idHilo);
		   return 0;
       }
       else if(strcmp(servicio_suse,"SUSE_CLOSE")){
		   int estadoHilo = pthread_create(idHilo,NULL,suse_close(socket_cliente,cosas),NULL);
		   if (estadoHilo)printf("No se pudo crear el hilo para *SUSE_CLOSE*\n");
		   free(idHilo);
		   return 0;
       }

       printf("NO SE PUDO ATENDER EL SERVICIO SOLICITADO\n");
       free(idHilo);
       return 1;
}
