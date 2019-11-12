#include "Suse.h"

int main(){

	/*INicializando Servidor Suse*/
	crearLogger();
	leerArchivoDeConfiguracion();

	log_info(logger,"\n [+]La configuracion es la siguiente \n");
	log_info(logger,"LISTEN_PORT ->  %s",listen_port );
	log_info(logger,"Metrics_timer ->  %d",metrics_timer );
	log_info(logger,"MAx_MULTIPROG ->  %d",max_multiprog);
	log_info(logger,"--------------\n");

	printf("\n\n::::::::INICIAMOS EL SERVIDOR SUSE::::::::\n");

	socket_Suse = iniciar_servidor("127.0.0.1",listen_port,logger);
	socket_cliente = esperar_cliente_con_accept(socket_Suse,logger);
	enviar_mensaje(socket_cliente,logger);

	/*Inicializo biblioteca Hilolay*/
	   /* hilolay_init();
	    hilolay_t th1;
	    hilolay_t th2;

		hilolay_create(&th1, NULL, &test1, NULL);
		hilolay_create(&th2, NULL, &test2, NULL);

		hilolay_join(&th2);
		hilolay_join(&th1);
	    return hilolay_return(0);

	*/

	return 0;
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

void suse_create(hilo_t* hilo){
	hilo_t* hiloNuevo = hilo;
	list_add(cola_new, hiloNuevo);
	log_info(logger,"Se ha agregado un hilo nuevo a la cola de new");
}

void* suse_schedule_next(){
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
			return (unHilo->tid == otroHilo->tid);
		}
		//Busco el indice en la cola de new comparando los TID, si lo encuentro, lo elimino de la cola de new y lo devuelvo
		int indice = list_get_index(cola_new,hiloAux,(void*)comparador);
		hilo_t* hiloAEjecutar = list_remove(cola_new,indice);

		return hiloAEjecutar;
	}
	log_info(logger, "La cola de new está vacia");
	return 0;
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

void suse_wait(semaforo_t* sem){
	semaforo_t* semaforo = sem;

	//Faltaria verificar que exista el semaforo que se está buscando en la lista de semaforos
	//el list_get_index() no verifica que exista

	int indice = list_get_index(semaforos,semaforo,(void*)comparadorDeSemaforos);
	semaforo_t* semAUsar = list_get(semaforos,indice);
	if (semAUsar->semActual == semAUsar->semInit){
		semAUsar->semActual--;
		log_info(logger,"%d","Contador inicial:", semAUsar->semInit);
		log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
		log_info(logger,"%d","El semaforo se ha bloqueado, contador actual:",semAUsar->semActual);
	}
	semaforo->semActual--;
	log_info(logger, "%d","Contador actual:", semaforo->semActual);
}

bool comparadorDeSemaforos(semaforo_t unSem, semaforo_t otroSem){
	return unSem.semID == otroSem.semID;
}

void suse_signal(semaforo_t* sem){
	semaforo_t* semaforo = sem;
	int indice = list_get_index(semaforos,semaforo,(void*)comparadorDeSemaforos);
	semaforo_t* semAUsar = list_get(semaforos,indice);
	if (semAUsar->semActual == semAUsar->semMax){
		log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
		log_error(logger,"El semaforo ya ha alcanzado su contador maximo, no se puede realizar el signal");
	}
	semAUsar->semActual++;
	log_info(logger,"%d","Contador inicial:", semAUsar->semInit);
	log_info(logger,"%d","Contador maximo:", semAUsar->semMax);
	log_info(logger,"%d","Contador actual:", semAUsar->semActual);
}

//hace lo mismo que pthread_join. TIene como parametro un hilo (y un PID?)
void suse_join(){
}


//Funcion que crea las colas ready segun el grado de multiprogramacion
void suse_close(){

}
//Close recibe un int TID, y mandas el thread ese a Exit
