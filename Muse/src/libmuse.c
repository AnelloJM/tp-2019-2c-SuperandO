#include "libmuse.h"



int muse_init(int id, char* ip, int puerto)
{
	uint32_t pet;

//Nos conectamos a Muse
	socket_pipe = conectarse_a_servidor(ip,puerto);
	pet = muse_alloc(30);
	printf("La respuesta de muse es %d\n\n",pet );
	socket_pipe = conectarse_a_servidor(ip,puerto);
	muse_free(25293);

	socket_pipe = conectarse_a_servidor(ip,puerto);
	pet = muse_get(23, 33, 5);
	printf("La respuesta de muse es %d\n\n",pet );



  return 0;
}


uint32_t muse_alloc(uint32_t tam)
{
	uint32_t respuesta;
  Paquete_muse_alloc *paquete = malloc(sizeof(Paquete_muse_alloc));
  paquete->op = 0;
  paquete->size_alloc = tam;

  enviar_muse_alloc(socket_pipe,paquete);
	respuesta = esperar_respuesta_uint(socket_pipe);

	free(paquete);
  return respuesta;


}

void muse_free(uint32_t dir) {

	Paquete_muse_free *paquete = malloc(sizeof(Paquete_muse_free));
	paquete->op=1;
	paquete->direccion=dir;
	enviar_muse_free(socket_pipe,paquete);
	free(paquete);
		//free((void*) dir);

}


int muse_get(void* dst, uint32_t src, size_t n){
	uint32_t respuesta;
	Paquete_muse_get *paquete = malloc(sizeof(Paquete_muse_get));
	paquete->op=2;
	paquete->p_muse_read = src;
	paquete->read_size = n;

	enviar_muse_get(socket_pipe,paquete);
	respuesta = esperar_respuesta_uint(socket_pipe);
	free(paquete);
  //  memcpy(dst, (void*) src, n);
  return 0;
}

void muse_close()
{

	printf("Cerrando conexion\n" );
	close(socket_pipe);
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
