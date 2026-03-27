# cloud-io-perf-c99
Investigación sobre el rendimiento de E/S en Linux: `io_uring` vs Síncrono.

Este proyecto forma parte de una investigación académica para la **Universidad de Carabobo**, centrada en la eficiencia de las interfaces de entrada/salida en entornos de nube con recursos limitados (instancias micro).

## Estructura del Proyecto
- `nob.h` / `nob.c`: Sistema de construcción minimalista basado en C.(Tsoding)
- `generador_datos.c`: Herramienta para crear archivos de prueba de 500MB.

## Cómo Compilar
1. Bootstrap del sistema de construcción:
   ```bash
   gcc -o nob nob.c
   ```bash
2. Ejecutar solo una vez
   ```bash   
    ./nob
   ```
   
