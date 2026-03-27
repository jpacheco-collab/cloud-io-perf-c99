#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Definimos un bloque de 1 MB para escribir de forma eficiente
#define TAMANO_BLOQUE (1024 * 1024) 

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <nombre_archivo_salida>\n", argv[0]);
        return 1;
    }

    const char *nombre_archivo = argv[1];
    FILE *archivo = fopen(nombre_archivo, "wb");
    if (!archivo) {
        perror("Error al crear el archivo");
        return 1;
    }

    // Reservamos un buffer de 1 MB en el heap
    uint8_t *buffer = malloc(TAMANO_BLOQUE);
    if (!buffer) {
        perror("Error de memoria");
        fclose(archivo);
        return 1;
    }

    // Llenamos el buffer con datos dummy (puedes usar rand() si prefieres)
    for (size_t j = 0; j < TAMANO_BLOQUE; j++) {
        buffer[j] = (uint8_t)(j % 256);
    }

    size_t megabytes_a_escribir = 500;
    printf("Generando archivo de %zu MB: %s\n", megabytes_a_escribir, nombre_archivo);

    // Escribimos bloque por bloque usando size_t para el iterador
    for (size_t i = 0; i < megabytes_a_escribir; i++) {
        size_t escritos = fwrite(buffer, 1, TAMANO_BLOQUE, archivo);
        if (escritos < TAMANO_BLOQUE) {
            perror("Error durante la escritura");
            break;
        }
        
        // Un pequeño indicador de progreso
        if ((i + 1) % 100 == 0) {
            printf("Progreso: %zu/500 MB...\n", i + 1);
        }
    }

    printf("¡Listo! Archivo generado exitosamente.\n");

    free(buffer);
    fclose(archivo);
    return 0;
}
