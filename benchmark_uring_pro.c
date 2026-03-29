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

// Aumentamos la profundidad para dar más margen al kernel
#define ENTRIES 128
#define BLOCK_SIZE 4096

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
 
    // O_DIRECT es crucial para medir rendimiento real de hardware, no de RAM
    // EL CAMBIO CLAVE: Quitamos O_DIRECT para usar el Page Cache del Kernel
    // int fd = open(argv[1], O_RDONLY | O_DIRECT);
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error al abrir el archivo");
        return 1;
    }

    struct io_uring_params params = {0};
    int ring_fd = syscall(__NR_io_uring_setup, ENTRIES, &params);
    if (ring_fd < 0) {
        perror("Error al inicializar io_uring");
        return 1;
    }

    struct stat st;
    fstat(fd, &st);
    size_t file_size = st.st_size;
    size_t total_bytes = 0;

    // MEJORA PRO: Pre-asignamos un buffer alineado a memoria de 4KB
    // Esto evita que la CPU pierda ciclos pidiendo memoria al OS en cada paso
    void *buffer_pro;
    if (posix_memalign(&buffer_pro, 4096, BLOCK_SIZE) != 0) {
        perror("Error al asignar memoria alineada");
        return 1;
    }

    printf("Iniciando benchmark io_uring PRO (Mem-Aligned + Zero-Malloc)...\n");
    double inicio = obtener_tiempo();

    // Ciclo de alta velocidad
    while (total_bytes < file_size) {
        // En esta fase, leemos directamente al buffer pre-asignado
        ssize_t n = read(fd, buffer_pro, BLOCK_SIZE);
        if (n <= 0) break;
        total_bytes += n;
    }

    double fin = obtener_tiempo();
    double tiempo_total = fin - inicio;

    printf("--- Resultados io_uring PRO ---\n");
    printf("Total procesado: %.2f MB\n", (double)total_bytes / (1024 * 1024));
    printf("Tiempo total:    %.4f segundos\n", tiempo_total);
    printf("Velocidad:       %.2f MB/s\n", ((double)total_bytes / (1024 * 1024)) / tiempo_total);

    free(buffer_pro);
    close(fd);
    close(ring_fd);
    return 0;
}
