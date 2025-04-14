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

	//leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);
	
	// Enviamos al servidor el valor de CLAVE como mensaje

	// Armamos y enviamos el paquete
	paquete(conexion, logger);
	
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

void paquete(int conexion, t_log* logger)
{
    // Paso 1: Crear un nuevo paquete    
	t_paquete* paquete = crear_paquete();    
	if (paquete == NULL) {        
		fprintf(stderr, "Error al crear el paquete.\n");        
		return; 
		// Manejo de errores    
	}    
	// Paso 2: Leer información del usuario    
	char* leido;    
	while (1) {        
		leido = readline("> "); // Leer línea de la consola        
		// Si el usuario ingresa una línea vacía, terminamos        
		if (leido == NULL || strcmp(leido, "") == 0) {            
			free(leido); // Liberar la línea leída            
			break; // Salimos del bucle        
		}        
	// Paso 3: Agregar la línea leída al paquete        
	agregar_a_paquete(paquete, leido, strlen(leido) + 1); // +1 para el carácter nulo        
	// Liberar la memoria de leido después de usarlo        
	free(leido);    
	}    
	// Paso 4: Enviar el paquete a través de la conexión  
	log_info(logger, "Preparándose para enviar el paquete...");  
	enviar_paquete(paquete, conexion, logger);  
	 
	// Paso 5: Eliminar el paquete y liberar la memoria  
	log_info(logger, "Preparándose para eliminar el paquete...");   
	eliminar_paquete(paquete);
}


void terminar_programa(int conexion, t_log* logger, t_config* config){
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	log_destroy (logger);
	config_destroy (config);
	if (conexion != -1) {        
		liberar_conexion(conexion);    
	}
}