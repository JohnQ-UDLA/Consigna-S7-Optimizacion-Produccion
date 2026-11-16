/*
 * SISTEMA DE OPTIMIZACION DE PRODUCCION
 * Fabrica de componentes electronicos - ISWZ1102 Programacion 1
 *
 * Restricciones del caso:
 *  - No se usan estructuras (struct) ni archivos.
 *  - Los datos se guardan en arreglos paralelos: la misma posicion
 *    (indice) en todos los arreglos corresponde al mismo producto.
 *  - La busqueda es propia: recorrido secuencial con un for.
 *    No se usa ninguna funcion predefinida de busqueda u ordenamiento.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define MAX 5      /* cantidad maxima de productos */
#define LARGO 50   /* largo maximo del nombre (49 letras + '\0') */
HANDLE consola;


/* Arreglos paralelos */
char nombres[MAX][LARGO];
int demanda[MAX];
float tiempo[MAX];     /* horas necesarias para fabricar 1 unidad */
float recursos[MAX];   /* recursos necesarios para fabricar 1 unidad */
int cantidad = 0;      /* cuantos productos hay registrados */

/* Limites de produccion de la fabrica.
   Se piden una sola vez y se conservan hasta que el usuario
   elija la opcion de modificarlos. Editar o eliminar productos NO los cambia. */
float tiempoDisponible = 0;     /* horas disponibles */
float recursosDisponibles = 0;  /* recursos disponibles */
int limitesDefinidos = 0;       /* 0 = aun no se ingresan, 1 = ya se ingresaron */

/* Prototipos */
void limpiarBuffer();
void salir();
int leerEntero(char mensaje[], int minimo, int maximo);
float leerReal(char mensaje[], float maximo);
void leerNombre(char mensaje[], char destino[]);
int buscarProducto(char nombre[]);
void ingresarProducto();
void mostrarProductos();
void editarProducto();
void eliminarProducto();
void definirLimites();
void calcularProduccion();

/* ---------- Funciones para leer datos del teclado ---------- */

/* Borra lo que haya quedado escrito en la linea (por ejemplo el Enter). */
void limpiarBuffer()
{
    int c;

    c = getchar();
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}

/* Si el teclado se cierra (Ctrl+D / Ctrl+Z) se termina el programa
   para no quedar repitiendo errores para siempre. */
void salir()
{
    printf("\nSe cerro la entrada de datos. Fin del programa.\n");
    exit(0); // código de salida. En sistemas operativos envíar un 0 significa "el programa finaliza con éxito"
}

/* Pide un entero y repite la pregunta hasta que este entre minimo y maximo. */
int leerEntero(char mensaje[], int minimo, int maximo)
{
    int valor;
    int resultado;
    int valido = 0;

    while (valido == 0)  //0 es igual a falso, así inicia el programa. Si el usuario ingresa un dato correcto cambiará a 1.
    {
        printf("%s", mensaje);
        resultado = scanf("%d", &valor);   /* resultado = 1 si se leyo un numero */
        if (resultado == EOF)
        {
            salir(); //función para cerrar el programa
        }
        limpiarBuffer();

        if (resultado != 1)
        {
            SetConsoleTextAttribute(consola, 4);
            printf("Error: debe ingresar un numero entero.\n");
            SetConsoleTextAttribute(consola, 0);
        }
        else if (valor < minimo || valor > maximo)
        {
            SetConsoleTextAttribute(consola, 4);
            printf("Error: el valor debe estar entre %d y %d.\n", minimo, maximo);
            SetConsoleTextAttribute(consola, 0);
        }
        else
        {
            valido = 1;
        }
    }
    return valor;
}

/* Pide un numero real mayor que 0 y menor o igual que maximo.
   Si el usuario escribe nan o inf la condicion falla y se vuelve a preguntar. */
float leerReal(char mensaje[], float maximo)
{
    float valor;
    int resultado;
    int valido = 0;

    while (valido == 0)
    {
        printf("%s", mensaje);
        resultado = scanf("%f", &valor);
        if (resultado == EOF)
        {
            salir();
        }
        limpiarBuffer();

        if (resultado != 1)
        {
            printf("Error: debe ingresar un numero.\n");
        }
        else if (valor > 0 && valor <= maximo)
        {
            valido = 1;
        }
        else
        {
            printf("Error: el valor debe ser mayor que 0 y como maximo %.0f.\n", maximo);
        }
    }
    return valor;
}

/* Lee un nombre (puede llevar espacios). %49[^\n] lee hasta el Enter
   y guarda maximo 49 caracteres, asi nunca se sale del arreglo. */
void leerNombre(char mensaje[], char destino[])
{
    int resultado;

    printf("%s", mensaje);
    resultado = scanf(" %49[^\n]", destino);
    if (resultado == EOF)
    {
        salir();
    }
    limpiarBuffer();
}

/* ---------- Busqueda propia (sin funciones predefinidas de busqueda) ---------- */

/* Recorre los productos uno por uno.
   Devuelve la posicion del producto o -1 si no existe. */
int buscarProducto(char nombre[])
{
    int i;

    for (i = 0; i < cantidad; i++)
    {
        if (strcmp(nombres[i], nombre) == 0)
        {
            return i;
        }
    }
    return -1;
}

/* ---------- Programa principal ---------- */

int main()
{
    int opcion;
    consola = GetStdHandle(STD_OUTPUT_HANDLE);

    do
    {
        SetConsoleTextAttribute(consola, 1);
        printf("\n===== SISTEMA DE OPTIMIZACION DE PRODUCCION =====\n");
        SetConsoleTextAttribute(consola, 0);
        printf("1. Ingresar producto\n");
        printf("2. Mostrar productos\n");
        printf("3. Editar producto\n");
        printf("4. Eliminar producto\n");
        printf("5. Definir o modificar limites de produccion\n");
        printf("6. Calcular produccion y verificar capacidad\n");
        printf("7. Salir\n");

        opcion = leerEntero("Seleccione una opcion (1-7): ", 1, 7);

        switch (opcion)
        {
            case 1: ingresarProducto();  break;
            case 2: mostrarProductos();  break;
            case 3: editarProducto();    break;
            case 4: eliminarProducto();  break;
            case 5: definirLimites();    break;
            case 6: calcularProduccion(); break;
            case 7: printf("\nFin del programa.\n"); break;
        }
    } while (opcion != 7);

    return 0;
}

/* ---------- Opciones del menu ---------- */

void ingresarProducto()
{
    consola = GetStdHandle(STD_OUTPUT_HANDLE);
    char nombre[LARGO];

    if (cantidad == MAX)
    {
        SetConsoleTextAttribute(consola, 4);
        printf("\nYa se registraron los %d productos permitidos.\n", MAX);
        return;
    }

    leerNombre("\nNombre del producto: ", nombre);

    if (buscarProducto(nombre) != -1)
    {
        SetConsoleTextAttribute(consola, 12);
        printf("Error: ya existe un producto con ese nombre.\n");
        return;
    }

    /* Cada lectura se repite hasta que el dato sea valido,
       por eso se puede guardar directamente en la posicion libre. */
    strcpy(nombres[cantidad], nombre);
    demanda[cantidad] = leerEntero("Demanda (unidades, 1 a 1000000): ", 1, 1000000);
    tiempo[cantidad] = leerReal("Tiempo por unidad (horas): ", 10000);
    recursos[cantidad] = leerReal("Recursos por unidad: ", 10000);
    cantidad++;

    SetConsoleTextAttribute(consola, 2);
    printf("\n Producto registrado correctamente.\n");
}

void mostrarProductos()
{
    consola = GetStdHandle(STD_OUTPUT_HANDLE);
    int i;

    if (cantidad == 0)
    {
        SetConsoleTextAttribute(consola, 4);
        printf("\nNo hay productos registrados.\n");
        SetConsoleTextAttribute(consola, 0);
        return;
    }

    SetConsoleTextAttribute(consola, 1);
    printf("\n%-20s %-10s %-15s %-15s\n",
           "PRODUCTO", "DEMANDA", "TIEMPO/UNIDAD", "RECURSOS/UNIDAD");
    
    SetConsoleTextAttribute(consola, 7);       
    for (i = 0; i < cantidad; i++)
    {
        printf("%-20s %-10d %-15.2f %-15.2f\n",
               nombres[i], demanda[i], tiempo[i], recursos[i]);
    }
}

void editarProducto()
{
    char buscado[LARGO];
    char nuevoNombre[LARGO];
    int pos;
    int otro;

    if (cantidad == 0)
    {
        SetConsoleTextAttribute(consola, 4);
        printf("\nNo hay productos para editar.\n");
        SetConsoleTextAttribute(consola, 0);
        return;
    }

    leerNombre("\nNombre del producto que desea editar: ", buscado);
    pos = buscarProducto(buscado);

    if (pos == -1)
    {
        SetConsoleTextAttribute(consola, 4);
        printf("Producto no encontrado.\n");
        SetConsoleTextAttribute(consola, 0);
        return;
    }

    SetConsoleTextAttribute(consola, 2);
    printf("Producto encontrado. Ingrese los nuevos datos.\n");
    SetConsoleTextAttribute(consola, 0);
    leerNombre("Nuevo nombre: ", nuevoNombre);

    /* El nuevo nombre no puede ser el de OTRO producto. */
    otro = buscarProducto(nuevoNombre);
    if (otro != -1 && otro != pos)
    {
        SetConsoleTextAttribute(consola, 4);
        printf("Error: ese nombre ya pertenece a otro producto. No se modifico nada.\n");
        SetConsoleTextAttribute(consola, 0);
        return;
    }

    strcpy(nombres[pos], nuevoNombre);
    demanda[pos] = leerEntero("Nueva demanda (unidades, 1 a 1000000): ", 1, 1000000);
    tiempo[pos] = leerReal("Nuevo tiempo por unidad (horas): ", 10000);
    recursos[pos] = leerReal("Nuevos recursos por unidad: ", 10000);

    SetConsoleTextAttribute(consola, 2);
    printf("Producto actualizado correctamente.\n");
    SetConsoleTextAttribute(consola, 0);
}

void eliminarProducto()
{
    char buscado[LARGO];
    int pos;
    int i;

    if (cantidad == 0)
    {
        SetConsoleTextAttribute(consola, 4);
        printf("\nNo hay productos para eliminar.\n");
        SetConsoleTextAttribute(consola, 0);
        return;
    }

    leerNombre("\nNombre del producto que desea eliminar: ", buscado);
    pos = buscarProducto(buscado);

    if (pos == -1)
    {
        SetConsoleTextAttribute(consola, 4);
        printf("Producto no encontrado.\n");
        SetConsoleTextAttribute(consola, 0);
        return;
    }

    /* Se corren todos los productos una posicion hacia la izquierda
       para tapar el espacio que deja el producto eliminado. */
    for (i = pos; i < cantidad - 1; i++)
    {
        strcpy(nombres[i], nombres[i + 1]);
        demanda[i] = demanda[i + 1];
        tiempo[i] = tiempo[i + 1];
        recursos[i] = recursos[i + 1];
    }

    cantidad--;
    SetConsoleTextAttribute(consola, 2);
    printf("Producto eliminado correctamente.\n");
    SetConsoleTextAttribute(consola, 0);
}

/* Pide los limites de la fabrica. Solo se llama la primera vez que se
   necesitan o cuando el usuario elige la opcion 5 del menu. */
void definirLimites()
{
    if (limitesDefinidos == 1)
    {
        SetConsoleTextAttribute(consola, 1);
        printf("\nLimites actuales: %.2f horas y %.2f recursos.\n",
               tiempoDisponible, recursosDisponibles);
               SetConsoleTextAttribute(consola, 8);
        printf("Ingrese los nuevos limites.\n");
    }
    else
    {
        SetConsoleTextAttribute(consola, 1);
        printf("\n===== LIMITES DE PRODUCCION =====\n");
        SetConsoleTextAttribute(consola, 0);
    }

    tiempoDisponible = leerReal("Tiempo de produccion disponible (horas): ", 1000000000);
    recursosDisponibles = leerReal("Recursos disponibles: ", 1000000000);
    limitesDefinidos = 1;

    SetConsoleTextAttribute(consola, 2);
    printf("Limites guardados correctamente.\n");
    SetConsoleTextAttribute(consola, 0);
}

void calcularProduccion()
{
    int i;
    float tiempoProducto;
    float recursosProducto;
    float tiempoTotal = 0;
    float recursosTotal = 0;

    if (cantidad == 0)
    {
        SetConsoleTextAttribute(consola, 4);
        printf("\nRegistre al menos un producto antes de calcular.\n");
        SetConsoleTextAttribute(consola, 0);
        return;
    }

    if (cantidad < MAX)
    {
        SetConsoleTextAttribute(consola, 1);
        printf("\nAtencion: hay %d de %d productos registrados.\n", cantidad, MAX);
        SetConsoleTextAttribute(consola, 0);
    }

    /* 1. Tiempo y recursos necesarios para cada producto */
    SetConsoleTextAttribute(consola, 1);
    printf("\n===== DETALLE DE PRODUCCION =====\n");
    SetConsoleTextAttribute(consola, 0);
    printf("%-18s %-10s %-15s %-15s\n",
           "PRODUCTO", "DEMANDA", "TIEMPO TOTAL", "RECURSOS TOTAL");

    for (i = 0; i < cantidad; i++)
    {
        tiempoProducto = demanda[i] * tiempo[i];
        recursosProducto = demanda[i] * recursos[i];

        tiempoTotal = tiempoTotal + tiempoProducto;
        recursosTotal = recursosTotal + recursosProducto;

        printf("%-18s %-10d %-15.2f %-15.2f\n",
               nombres[i], demanda[i], tiempoProducto, recursosProducto);
    }

    /* 2. Capacidad disponible: ya esta guardada en las variables globales.
       Solo se pide si todavia no se ha ingresado nunca. */
    if (limitesDefinidos == 0)
    {
        definirLimites();
    }

    /* 3. Resumen */
    SetConsoleTextAttribute(consola, 1);
    printf("\n===== RESUMEN =====\n");
    SetConsoleTextAttribute(consola, 2);
    printf("Tiempo total requerido:   %.2f horas\n", tiempoTotal);
    printf("Tiempo disponible:        %.2f horas\n", tiempoDisponible);
    printf("Recursos necesarios:      %.2f\n", recursosTotal);
    printf("Recursos disponibles:     %.2f\n", recursosDisponibles);
    SetConsoleTextAttribute(consola,0);

    /* 4. Viabilidad */
    SetConsoleTextAttribute(consola, 1);
    printf("\n===== VIABILIDAD =====\n");
    SetConsoleTextAttribute(consola, 8);

    if (tiempoTotal <= tiempoDisponible)
    {
        SetConsoleTextAttribute(consola, 2);
        printf("[OK] El tiempo alcanza. Sobran %.2f horas.\n",
               tiempoDisponible - tiempoTotal);
        SetConsoleTextAttribute(consola, 8);
    }
    else
    {
        SetConsoleTextAttribute(consola, 4);
        printf("[NO] El tiempo NO alcanza. Faltan %.2f horas.\n",
               tiempoTotal - tiempoDisponible);
        SetConsoleTextAttribute(consola, 8);
    }

    if (recursosTotal <= recursosDisponibles)
    {
        SetConsoleTextAttribute(consola, 2);
        printf("[OK] Los recursos alcanzan. Sobran %.2f.\n",
               recursosDisponibles - recursosTotal);
        SetConsoleTextAttribute(consola, 8);
    }
    else
    {
        SetConsoleTextAttribute(consola, 4);
        printf("[NO] Los recursos NO alcanzan. Faltan %.2f.\n",
               recursosTotal - recursosDisponibles);
        SetConsoleTextAttribute(consola, 8);
    }

    if (tiempoTotal <= tiempoDisponible && recursosTotal <= recursosDisponibles)
    {
        SetConsoleTextAttribute(consola, 2);
        printf("\nCONCLUSION: LA FABRICA PUEDE CUMPLIR CON TODA LA DEMANDA.\n");
        SetConsoleTextAttribute(consola, 8);
    }
    else
    {
        SetConsoleTextAttribute(consola, 4);
        printf("\nCONCLUSION: LA FABRICA NO PUEDE CUMPLIR CON TODA LA DEMANDA.\n");
        printf("Debe aumentar el tiempo y/o los recursos disponibles.\n");
        SetConsoleTextAttribute(consola, 8);
    }
}
