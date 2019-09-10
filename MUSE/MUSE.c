#include "MUSE.h"

void sigchld_handler(int s) {
	while (wait(NULL) > 0);
}

void levantarServidor() {

	int sockfd, socket_MUSE; // Escuchar sobre sock_fd, nuevas conexiones sobre socket_MUSE
	struct sockaddr_in my_addr;    // información sobre mi dirección
	struct sockaddr_in their_addr; // información sobre la dirección del cliente
	int sin_size;
	struct sigaction sa;
	int yes = 1;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		log_error(logger,"Socket: %s",strerror(errno));

	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		log_error(logger,"Setsockopt: %s",strerror(errno));
	}

	my_addr.sin_family = AF_INET;       // Ordenación de bytes de la máquina
	my_addr.sin_port = htons(listen_port); // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = inet_addr(listen_IP); // Rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))== -1) {
		log_error(logger,"Bind: %s",strerror(errno));
	}
	log_info(logger,"El servidor espera nuevas conexiones");
	if (listen(sockfd, 10) == -1) {  //Solo puede aceptar hasta 10 conexiones simultaneamente
		log_error(logger,"Listen: %s",strerror(errno));
	}

	sa.sa_handler = sigchld_handler; // Eliminar procesos muertos
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		log_error(logger,"Sigaction: %s",strerror(errno));
	}

	while (true) {  // main accept() loop
		sin_size = sizeof(struct sockaddr_in);
		if ((socket_MUSE = accept(sockfd, (struct sockaddr *) &their_addr,&sin_size)) == -1) {
			log_error(logger,"Accept: %s",strerror(errno));
			continue;
		}
		log_info(logger,"Se recibio una conexion de: %s",inet_ntoa(their_addr.sin_addr));

		/*if (!fork()) { // Este es el proceso hijo
		 close(sockfd); // El hijo no necesita este descriptor
		 if (send(new_fd, "Hello, world!\n", 14, 0) == -1)
		 perror("send");
		 close(new_fd);
		 exit(0);
		 */
	}
	close(socket_MUSE);  // El proceso padre no lo necesita
	log_info(logger,"Se cerro el socket: %d",socket_MUSE);
}

void crearLogger(char* file, char *program_name, bool is_active_console, t_log_level level){
	logger = log_create(file,program_name,is_active_console,level);
	free(file);
}

void leerConfig(char* configPath, t_log* logger){
	leerArchivoConfiguracion(configPath,logger);
	log_info(logger, "Se leyo exitosamente el archivo de configuracion");
}

void leerArchivoConfiguracion(char* configPath, t_log* logger){
	t_config* archivoConfiguracion;

	archivoConfiguracion = config_create(configPath);

	if(archivoConfiguracion == NULL){
		perror("No se encontro el archivo de configuracion");
		log_error(logger,"No se encontro el archivo de configuracion");
	}

	setearValores(archivoConfiguracion);
	config_destroy(archivoConfiguracion);
}

void setearValores(t_config* archivoConfiguracion){
	listen_IP = strdup(config_get_string_value(archivoConfiguracion, "LISTEN_IP"));
	listen_port = config_get_int_value(archivoConfiguracion, "LISTEN_PORT");
	memory_size = config_get_int_value(archivoConfiguracion, "MEMORY_SIZE");
	page_size = config_get_int_value(archivoConfiguracion, "PAGE_SIZE");
	swap_size = config_get_int_value(archivoConfiguracion, "SWAP_SIZE");
}
