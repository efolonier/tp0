#include "client.h"


int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;
	

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	log_info (logger, "Hola! Soy un log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");
	// Loggeamos el valor de config
	log_info (logger, ip);
	log_info (logger, puerto);
	log_info(logger, "El valor de CLAVE es: %s", valor);
	//log_info(logger, valor);
	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);
	
	// Enviamos al servidor el valor de CLAVE como mensaje

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}


t_log* iniciar_logger(void){
	t_log* nuevo_logger = log_create("tp0.log", "TP0", true, LOG_LEVEL_INFO);
	if (nuevo_logger == NULL) {
		    printf("Error al crear el logger");    
			exit(EXIT_FAILURE);}
	return nuevo_logger;
}


t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create ("cliente.config");
	if (nuevo_config== NULL) {
		    printf("Error al crear el config");    
			exit(EXIT_FAILURE);
	}
	return nuevo_config;
}

/*void leer_consola(t_log* logger)
{
	char* leido;
	log_info(logger, "Esperando entrada de consola...");

	// La primera te la dejo de yapa
	leido = readline("> ");
	log_info(logger, "Se leyó: %s", leido);

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while (strcmp(leido, "") != 0) {
		free(leido);
        leido = readline("> ");
        log_info(logger, "Se leyó: %s", leido);
    }
	// ¡No te olvides de liberar las lineas antes de regresar!      
		free(leido); // Liberamos la memoria si no es NULL    
	
}*/

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("> ");
	log_info(logger, ">> %s", leido);

	while(strcmp(leido, "") != 0){
		free(leido);
		leido = readline("> ");
		log_info(logger, ">> %s", leido);
	}
	// El resto, las vamos leyendo y logueando hasta recibir un string vacío

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
	
}

/*void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete;

	// Leemos y esta vez agregamos las lineas al paquete


	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
}*/

void paquete(int conexion)
{
    // Paso 1: Crear un nuevo paquete
    t_paquete* paquete = malloc(sizeof(t_paquete));
    if (paquete == NULL) {
        perror("Error al crear el paquete");
        return; // Manejo de errores
    }

    // Definimos el código de operación para el paquete
    paquete->codigo_operacion = PAQUETE; // Asegúrate de tener definido PAQUETE

    // Inicializamos el buffer
    paquete->buffer = malloc(sizeof(t_buffer));
    if (paquete->buffer == NULL) {
        perror("Error al crear el buffer");
        free(paquete);
        return; // Manejo de errores
    }
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;

    // Paso 2: Leer líneas de la consola
    while (1) {
        char* leido = readline("> "); // Leer línea de la consola

        // Si se presiona Enter sin escribir nada, terminamos la lectura
        if (leido == NULL || strcmp(leido, "") == 0) {
            free(leido); // Liberar la línea leída
            break; // Salimos del bucle
        }

        // Paso 3: Agregar la línea leída al buffer
        int longitud_leido = strlen(leido) + 1; // +1 para el carácter nulo
        paquete->buffer->size += longitud_leido; // Actualizamos el tamaño del buffer

        // Reservamos espacio para el nuevo tamaño del buffer
        paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size);
        if (paquete->buffer->stream == NULL) {
            perror("Error al realloc del buffer");
            free(leido); // Liberar leido en caso de error
            free(paquete->buffer);
            free(paquete);
            return; // Manejo de errores
        }

        // Agregamos la línea leída al final del buffer
        memcpy((char*)paquete->buffer->stream + (paquete->buffer->size - longitud_leido), leido, longitud_leido);

        // Liberamos la memoria de leido después de usarlo
        free(leido);
    }

    // Aquí enviarías el paquete al servidor
    enviar_paquete(conexion, paquete); // Asegúrate de tener la función enviar_paquete definida
	// Enviar el paquete
	int bytes_enviados = send(conexion, paquete, sizeof(t_paquete), 0);
	if (bytes_enviados == -1) {    
		perror("Error al enviar el paquete");
		} else {    
		// Esperar confirmación del servidor    
		int confirmacion;    recv(conexion, &confirmacion, sizeof(int), 0); 
		// Lee la confirmación del servidor    
		if (confirmacion == 1) {        
			printf("Paquete enviado y recibido correctamente por el servidor\n");    
	} else {        
		printf("El servidor no recibió el paquete correctamente\n");    }
	}

    // Paso 4: Liberar los recursos
    free(paquete->buffer->stream); // Liberar el contenido del buffer
    free(paquete->buffer); // Liberar el buffer
    free(paquete); // Liberar el paquete
}

void terminar_programa(int conexion, t_log* logger, t_config* config){
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	log_destroy (logger);
	config_destroy (config);
}