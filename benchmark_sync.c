#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096 // 4KB

double obtener_tiempo() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo_datos>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Paso científico: Invalidar el caché para asegurar lectura de disco
    posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED);

    char *buffer = malloc(BUFFER_SIZE);
    ssize_t bytes_leidos;
    size_t total_bytes = 0;

    printf("Iniciando benchmark síncrono...\n");
    double inicio = obtener_tiempo();

    // Ciclo de lectura tradicional
    while ((bytes_leidos = read(fd, buffer, BUFFER_SIZE)) > 0) {
        total_bytes += (size_t)bytes_leidos;
    }

    double fin = obtener_tiempo();
    double tiempo_total = fin - inicio;

    printf("--- Resultados ---\n");
    printf("Total procesado: %.2f MB\n", (double)total_bytes / (1024 * 1024));
    printf("Tiempo total:    %.4f segundos\n", tiempo_total);
    printf("Velocidad:       %.2f MB/s\n", ((double)total_bytes / (1024 * 1024)) / tiempo_total);

    free(buffer);
    close(fd);
    return 0;
}
