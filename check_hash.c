// Estándar
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Algoritmos
#include "sha1.h"
#include "sha256.h"

// Para llamadas del sistema
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

void print_hex(const BYTE *data, size_t size)
{
	int i;
	for(i = 0; i < size; ++i)
		printf("%02X", data[i]);
	printf("\n");
}

void imprimirAyuda(){
	printf("check_hash calcula el hash (SHA1 o SHA256) de un archivo y opcionalmente lo compara con un hash previo.\n");
	printf("\nUso:\n");
	printf("  check_hash [-a sha1|sha256] [-c <hash>] <nombre_archivo>\n");
	printf("  check_hash -h\n");
	printf("\nOpciones:\n");
	printf("  -h\t\t\tAyuda, muestra este mensaje.\n");
	printf("  -a\t\t\tEspecifica el algoritmo de hash. Acepta sha1 y sha256 [default: sha256].\n");
	printf("  -c <hash>\t\tHash a comparar.\n\n");
}

int main(int argc, char **argv)
{
	struct stat mi_stat;
	char *input_file = NULL;

	int option, algorithm_flag, comparison_flag = 0;
	char *input_hash;
	
	while((option = getopt(argc, argv, ":hc:a:")) != -1){
		
		switch (option)
		{
			case '?': 
				fprintf(stderr, "Opción no admitida: '%c'\nUse '-h' para ver la ayuda.\n", optopt);
				return -1;
			case ':': 
				fprintf(stderr, "Faltan argumentos.\nUse '-h' para ver la ayuda.\n");
				return -2;
			case 'h':
				imprimirAyuda();
				return 0;
			case 'c': 
				input_hash = optarg;
				comparison_flag = 1;
				break;
			case 'a': 
				if(strcmp(optarg, "sha1") == 0) {
					algorithm_flag = 1;
					break;
				}
				if (strcmp(optarg, "sha256") == 0) break;
				fprintf(stderr, "Algoritmo no admitido: %s\nUse '-h' para ver la ayuda.\n", optarg);
				return -3;
		}		
	}

	if(optind == argc){
		fprintf(stderr, "¡Ruta de archivo no especificada!\nUse '-h' para ver la ayuda.\n");
		return -4;
	}
	input_file = argv[optind];
	if(stat(input_file, &mi_stat) < 0){
		fprintf(stderr, "¡El archivo '%s' no existe!\n", input_file);
		return -5;
	}

	size_t MAX_RAM = 1024*1024*220;

	char *archivo;
	if(mi_stat.st_size <= MAX_RAM){
		archivo = malloc(mi_stat.st_size);
	}else {
		archivo = malloc(MAX_RAM);
	}
	int fd = open(input_file, O_RDONLY, 0);

	int SHAX_BLOCK_SIZE = SHA256_BLOCK_SIZE;
	if(algorithm_flag != 0){
		SHAX_BLOCK_SIZE = SHA1_BLOCK_SIZE;
	}
	char *codigo_salida = malloc(SHAX_BLOCK_SIZE);
	int BYTES_LEIDOS = 0;

	if(algorithm_flag == 0){
		printf("SHA256: ");
		SHA256_CTX ctx;
		sha256_init(&ctx);
		for(int i = 0; i < mi_stat.st_size; i += MAX_RAM){
			BYTES_LEIDOS = read(fd, archivo, MAX_RAM);
			sha256_update(&ctx, archivo, BYTES_LEIDOS);
		}
		sha256_final(&ctx, codigo_salida);

	}else {
		printf("SHA1: ");
		SHA1_CTX ctx;
		sha1_init(&ctx);
		for(int i = 0; i < mi_stat.st_size; i += MAX_RAM){
			BYTES_LEIDOS = read(fd, archivo, MAX_RAM);
			sha1_update(&ctx, archivo, BYTES_LEIDOS);
		}
		sha1_final(&ctx, codigo_salida);
	}

	print_hex(codigo_salida, SHAX_BLOCK_SIZE);

	if(comparison_flag != 0){
		BYTE hash_binario[SHAX_BLOCK_SIZE];
		BYTE byte, i;
		for(i = 0;i < SHAX_BLOCK_SIZE; i++){
			sscanf(input_hash + 2*i,"%2hhx", &byte);
			hash_binario[i] = byte;
		}
		if(memcmp(hash_binario, codigo_salida, SHAX_BLOCK_SIZE) == 0){
			printf("%s: OK!\n", input_file);
		}else {
			printf("%s: NOT OK!\n", input_file);
		}
	}
	
	free(archivo);
	free(codigo_salida);
	close(fd);
}
