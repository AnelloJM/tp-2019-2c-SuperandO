#ifndef LISTA_H_
#define LISTA_H_
#include <stdlib.h>
#include <commons/collections/list.h>
#include <stdbool.h>
//Dada una lista y un elemento, si ese elemento esta en la lista, retorna su indice
int list_get_index(t_list* self,void* elemento,bool (*comparator)(void *,void *));
// Dice que existen elementos repetidos en una lista
bool list_element_repeats(t_list* self, bool (*comparator)(void *,void *));

#endif /* LISTA_H_ */
