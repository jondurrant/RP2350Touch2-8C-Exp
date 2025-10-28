/*
 * lv_port_mem.c
 *
 *  Created on: 9 Oct 2025
 *      Author: jondurrant
 */
#include "lv_port_mem.h"
#include  <stdlib.h>
#include <stdio.h>

void *port_malloc( size_t size ){
	void *r = malloc(size);
	if (r == NULL){
		 gpio_put (42, true);
		printf("FAILED to MALLOC %u\n", size);
		fflush(stdout);
		sleep_ms(2000);
	}
	return r;
}

void *port_realloc( void *ptr, size_t new_size ){
	void *r = realloc(ptr, new_size);
	if (r == NULL){
		 gpio_put (42, true);
		printf("FAILED to REALLOC %u\n", new_size);
		fflush(stdout);
		sleep_ms(2000);
	}
	return r;
}

void port_free( void *ptr ){
	free(ptr);
}
