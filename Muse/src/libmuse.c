#include "libmuse.h"



int muse_init(int id, char* ip, int puerto)
{


//Nos conectamos a Muse
	socket_pipe = conectarse_a_servidor(ip,puerto);
	muse_alloc(30);

  return 0;
}


uint32_t muse_alloc(uint32_t tam)
{
  Paquete_muse_alloc *paquete = malloc(sizeof(Paquete_muse_alloc));
  paquete->op = 0;
  paquete->size_alloc = tam;

  enviar_muse_alloc(socket_pipe,paquete);

  return 0;
}
/*
int conectarse_a_servidor(char *ip,int puerto)
{
	int numbytes,socket_cliente;
	char buf[100];

	struct hostent *he;
	struct sockaddr_in server;

	he = gethostbyname(ip);

	if((socket_cliente=socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("[-] Error socket()");
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(puerto);
	server.sin_addr = *((struct in_addr *)he -> h_addr);

	bzero(&(server.sin_zero),8);

	if(connect(socket_cliente, (struct sockaddr *)&server,sizeof(struct sockaddr))==-1){
		printf("[-] Error connect() \n");
		exit(-1);
	}



	return socket_cliente;
}

*/

/*void crearLogger()
{
	char* logPath = "proceso.log";//"/home/utnso/workspace/tp-2019-2c-SuperandO/Muse/src/MUSE.log";
	char* nombreArch = "proceso";
	bool consolaActiva = true;
	libmuse_logger = log_create(logPath, nombreArch, consolaActiva, LOG_LEVEL_INFO);
	log_info(libmuse_logger, "El logger del proceso se creo con exito");
//	free(logPath);
}
*/
void muse_close()
{

	printf("Cerrando conexion\n" );
	close(socket_pipe);
}




void muse_free(uint32_t dir) {
    free((void*) dir);
}

int muse_get(void* dst, uint32_t src, size_t n){
    memcpy(dst, (void*) src, n);
    return 0;
}

int muse_cpy(uint32_t dst, void* src, int n){
    memcpy((void*) dst, src, n);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
uint32_t muse_map(char *path, size_t length, int flags){
    return 0;
}

int muse_sync(uint32_t addr, size_t len){
    return 0;
}

int muse_unmap(uint32_t dir){
    return 0;
}




int main()
{

  muse_init(12,"127.0.0.1",5555);
  muse_close();
  return 0;
}
