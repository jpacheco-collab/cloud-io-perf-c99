#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv)
{
    // Esta línea es la magia: si editas este archivo, nob se recompila a sí mismo
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};

    // --- Compilar el Generador de Datos ---
    nob_cmd_append(&cmd, "gcc", "-std=c99", "-Wall", "-Wextra", "-O2");
    nob_cmd_append(&cmd, "-o", "generador", "generador_datos.c");
    
    if (!nob_cmd_run(&cmd)) return 1;

    // --- Nota: Aquí añadiremos el benchmark de io_uring más adelante ---
    // cmd.count = 0;
    // nob_cmd_append(&cmd, "gcc", "-std=c99", "-Wall", "-Wextra", "-O2", "-o", "benchmark", "benchmark.c");
    // if (!nob_cmd_run(&cmd)) return 1;

    nob_log(NOB_INFO, "¡Compilación exitosa, José! Todo está listo para la nube.");

    return 0;
}
