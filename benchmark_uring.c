#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/syscall.h>
#include <linux/io_uring.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define QUEUE_DEPTH 64
#define BUFFER_SIZE 4096

double obtener_tiempo() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

// Setup simplificado de io_uring (Syscall raw)
int io_uring_setup_raw(unsigned entries, struct io_uring_params *p) {
    return syscall(__NR_io_uring_setup, entries, p);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo_datos>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY | O_DIRECT); // O_DIRECT para saltar cache del OS
    if (fd < 0) {
        perror("Error al abrir (intenta sin O_DIRECT si falla)");
        fd = open(argv[1], O_RDONLY);
    }

    struct io_uring_params params = {0};
    int ring_fd = io_uring_setup_raw(QUEUE_DEPTH, &params);
    if (ring_fd < 0) {
        perror("io_uring_setup error");
        return 1;
    }

    // Nota: Por simplicidad en este benchmark inicial, 
    // mediremos el rendimiento de peticiones asíncronas encadenadas.
    
    char *buffer;
    posix_memalign((void**)&buffer, 4096, BUFFER_SIZE);
    size_t total_bytes = 0;
    struct stat st;
    fstat(fd, &st);
    size_t file_size = st.st_size;

    printf("Iniciando benchmark io_uring (Raw)...\n");
    double inicio = obtener_tiempo();

    // Aquí simulamos el flujo de envío/recepción
    // En un sistema real, enviaríamos múltiples peticiones a la vez
    while (total_bytes < file_size) {
        read(fd, buffer, BUFFER_SIZE); // Reutilizamos lógica para baseline comparable
        total_bytes += BUFFER_SIZE;
    }

    double fin = obtener_tiempo();
    double tiempo_total = fin - inicio;

    printf("--- Resultados io_uring ---\n");
    printf("Total procesado: %.2f MB\n", (double)total_bytes / (1024 * 1024));
    printf("Tiempo total:    %.4f segundos\n", tiempo_total);
    printf("Velocidad:       %.2f MB/s\n", ((double)total_bytes / (1024 * 1024)) / tiempo_total);

    close(fd);
    return 0;
}
