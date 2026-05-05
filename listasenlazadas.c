/*
 * main.c
 * TP4 - Modulo To-Do
 * Todo el programa en un unico archivo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
   ESTRUCTURAS
*/
typedef struct Tarea {
    int TareaID;
    char *Descripcion;
    int Duracion;
} Tarea;

typedef struct Nodo {
    Tarea T;
    struct Nodo *Siguiente;
} Nodo;

/* ============================================================
   CONSTANTES
*/
#define ID_INICIAL 1000
#define DUR_MIN    10
#define DUR_MAX    100
#define MAX_DESC   256

/* ============================================================
   FUNCIONES DE LISTA
*/

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

Nodo* crearNodo(Tarea t) {
    Nodo *nuevo = (Nodo*) malloc(sizeof(Nodo));
    if (nuevo == NULL) {
        printf("ERROR: No se pudo reservar memoria.\n");
        return NULL;
    }
    nuevo->T = t;
    nuevo->Siguiente = NULL;
    return nuevo;
}

void agregarAlFinal(Nodo **lista, Tarea t) {
    Nodo *nuevo = crearNodo(t);
    if (nuevo == NULL) return;

    if (*lista == NULL) {
        *lista = nuevo;
        return;
    }

    Nodo *actual = *lista;
    while (actual->Siguiente != NULL) {
        actual = actual->Siguiente;
    }
    actual->Siguiente = nuevo;
}

int marcarComoRealizada(Nodo **pendientes, Nodo **realizadas, int id) {
    if (*pendientes == NULL) {
        printf("La lista de pendientes esta vacia.\n");
        return 0;
    }

    Nodo *actual = *pendientes;
    Nodo *anterior = NULL;

    while (actual != NULL && actual->T.TareaID != id) {
        anterior = actual;
        actual = actual->Siguiente;
    }

    if (actual == NULL) {
        printf("No se encontro la tarea con ID %d.\n", id);
        return 0;
    }

    /* Desconectamos el nodo de pendientes */
    if (anterior == NULL) {
        *pendientes = actual->Siguiente;
    } else {
        anterior->Siguiente = actual->Siguiente;
    }
    actual->Siguiente = NULL;

    /* Lo agregamos al final de realizadas */
    if (*realizadas == NULL) {
        *realizadas = actual;
    } else {
        Nodo *fin = *realizadas;
        while (fin->Siguiente != NULL) {
            fin = fin->Siguiente;
        }
        fin->Siguiente = actual;
    }

    return 1;
}

void listarTareas(Nodo *lista, const char *titulo) {
    printf("\n========================================\n");
    printf("  %s\n", titulo);
    printf("========================================\n");

    if (lista == NULL) {
        printf("  (Sin tareas)\n");
        printf("========================================\n");
        return;
    }

    Nodo *actual = lista;
    int i = 1;
    while (actual != NULL) {
        printf("  [%d] ID: %d\n", i, actual->T.TareaID);
        printf("       Descripcion: %s\n", actual->T.Descripcion);
        printf("       Duracion: %d minutos\n", actual->T.Duracion);
        printf("  ----------------------------------------\n");
        actual = actual->Siguiente;
        i++;
    }
    printf("========================================\n");
}

void buscarTarea(Nodo *pendientes, Nodo *realizadas, const char *criterio) {
    printf("\n========================================\n");
    printf("  Resultados: \"%s\"\n", criterio);
    printf("========================================\n");

    int encontrados = 0;
    int idBuscado = atoi(criterio); /* Si es numero, busca por ID */

    Nodo *actual = pendientes;
    while (actual != NULL) {
        int coincide = idBuscado ? (actual->T.TareaID == idBuscado)
                                 : (strstr(actual->T.Descripcion, criterio) != NULL);
        if (coincide) {
            printf("  [PENDIENTE] ID:%d | %s | %d min\n",
                   actual->T.TareaID, actual->T.Descripcion, actual->T.Duracion);
            encontrados++;
        }
        actual = actual->Siguiente;
    }

    actual = realizadas;
    while (actual != NULL) {
        int coincide = idBuscado ? (actual->T.TareaID == idBuscado)
                                 : (strstr(actual->T.Descripcion, criterio) != NULL);
        if (coincide) {
            printf("  [REALIZADA] ID:%d | %s | %d min\n",
                   actual->T.TareaID, actual->T.Descripcion, actual->T.Duracion);
            encontrados++;
        }
        actual = actual->Siguiente;
    }

    if (encontrados == 0)
        printf("  No se encontraron resultados.\n");
    else
        printf("  Total: %d\n", encontrados);

    printf("========================================\n");
}

void liberarLista(Nodo *lista) {
    Nodo *actual = lista;
    while (actual != NULL) {
        Nodo *siguiente = actual->Siguiente;
        free(actual->T.Descripcion);
        free(actual);
        actual = siguiente;
    }
}

/* ============================================================
   MENU E INTERACCION
*/

void cargarTareas(Nodo **pendientes, int *proximoId) {
    char descripcion[MAX_DESC];
    int duracion;
    char continuar;

    printf("\n=== CARGA DE TAREAS PENDIENTES ===\n");

    do {
        Tarea nueva;
        nueva.TareaID = *proximoId;
        (*proximoId)++;

        printf("\nNueva tarea (ID: %d)\n", nueva.TareaID);

        printf("Descripcion: ");
        limpiarBuffer();
        fgets(descripcion, MAX_DESC, stdin);
        int largo = strlen(descripcion);
        if (largo > 0 && descripcion[largo - 1] == '\n')
            descripcion[largo - 1] = '\0';

        nueva.Descripcion = (char*) malloc((strlen(descripcion) + 1) * sizeof(char));
        if (nueva.Descripcion == NULL) {
            printf("Error de memoria.\n");
            (*proximoId)--;
            continue;
        }
        strcpy(nueva.Descripcion, descripcion);

        do {
            printf("Duracion (%d-%d min): ", DUR_MIN, DUR_MAX);
            scanf("%d", &duracion);
            if (duracion < DUR_MIN || duracion > DUR_MAX)
                printf("  Valor invalido. Intente de nuevo.\n");
        } while (duracion < DUR_MIN || duracion > DUR_MAX);

        nueva.Duracion = duracion;
        agregarAlFinal(pendientes, nueva);
        printf("  Tarea agregada correctamente.\n");

        limpiarBuffer();
        printf("Desea ingresar otra tarea? (s/n): ");
        scanf("%c", &continuar);

    } while (continuar == 's' || continuar == 'S');
}

void menuMarcarRealizada(Nodo **pendientes, Nodo **realizadas) {
    if (*pendientes == NULL) {
        printf("\nNo hay tareas pendientes.\n");
        return;
    }

    listarTareas(*pendientes, "TAREAS PENDIENTES");

    int id;
    char seguir;

    do {
        printf("\nID de la tarea realizada (0 para cancelar): ");
        scanf("%d", &id);
        if (id == 0) break;

        if (marcarComoRealizada(pendientes, realizadas, id))
            printf("  Tarea %d movida a realizadas.\n", id);

        if (*pendientes == NULL) {
            printf("No quedan tareas pendientes.\n");
            break;
        }

        listarTareas(*pendientes, "PENDIENTES RESTANTES");
        limpiarBuffer();
        printf("Marcar otra? (s/n): ");
        scanf("%c", &seguir);

    } while (seguir == 's' || seguir == 'S');
}

void menuBuscar(Nodo *pendientes, Nodo *realizadas) {
    char criterio[MAX_DESC];
    printf("\n=== BUSQUEDA ===\n");
    printf("Ingrese ID o palabra clave: ");
    limpiarBuffer();
    fgets(criterio, MAX_DESC, stdin);
    int largo = strlen(criterio);
    if (largo > 0 && criterio[largo - 1] == '\n')
        criterio[largo - 1] = '\0';

    if (strlen(criterio) == 0) {
        printf("Criterio vacio.\n");
        return;
    }
    buscarTarea(pendientes, realizadas, criterio);
}

/* ============================================================
   MAIN
*/
int main() {
    Nodo *tareasPendientes = NULL;
    Nodo *tareasRealizadas = NULL;
    int proximoId = ID_INICIAL;
    int opcion;

    printf("Bienvenido al modulo To-Do.\n");

    do {
        printf("\n╔════════════════════════════════╗\n");
        printf("║     MODULO TO-DO - TAREAS      ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║  1. Cargar tareas pendientes   ║\n");
        printf("║  2. Marcar tarea como realizada║\n");
        printf("║  3. Listar todas las tareas    ║\n");
        printf("║  4. Buscar tarea               ║\n");
        printf("║  0. Salir                      ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("Opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: cargarTareas(&tareasPendientes, &proximoId);              break;
            case 2: menuMarcarRealizada(&tareasPendientes, &tareasRealizadas); break;
            case 3: listarTareas(tareasPendientes, "TAREAS PENDIENTES");
                    listarTareas(tareasRealizadas, "TAREAS REALIZADAS");       break;
            case 4: menuBuscar(tareasPendientes, tareasRealizadas);            break;
            case 0: printf("\nHasta pronto.\n");                               break;
            default: printf("\nOpcion invalida.\n");                           break;
        }

    } while (opcion != 0);

    liberarLista(tareasPendientes);
    liberarLista(tareasRealizadas);
    return 0;
}