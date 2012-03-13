#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include "MES.h"

typedef struct sockaddr *sad;
 
void error(char *s)
{
	exit((perror(s), -1));
}

/*El main es la función inicial aqui se inicia el fork que separara la parte del servidor y la del cliente*/
int main ()
{
	pid_t hijo;
	hijo = fork();
	if (hijo >= 0)
	{
		if (hijo == 0)
		{
			CL("172.18.26.158", 5000);
		}
		else 
		{
			SL(5000);
		}
	}
	else 
	{
		perror("fork");
		exit(0); 
	}
	return 0;
}

/*función cliente, recibe el ip y el puerto*/
int CL(char *ip, int puerto)
{
	#define PORT1 puerto
	int sock;
        struct sockaddr_in sin;
        char linea[1024];
        int largo =128;
        
        /*se inicia la coneccion del cliente para que reconozca al servidor*/
        if ((sock = socket(AF_INET, SOCK_STREAM, 0))<0)
                error("socket");
        sin.sin_family = AF_INET;
        sin.sin_port = htons(PORT1);
        inet_aton(ip, &sin.sin_addr);
 
        /* Inicio de la llamada*/
        if (connect(sock, (sad) &sin, sizeof(sin))<0)
               error("connect");
 
        /*Inicio de la comunicación */
        int sesion = 0;
        while (sesion != 1)
        {
			pid_t hijopid;
			/* inicio de la Bifurcacion */
			hijopid = fork();
			/*parte hijo del fork*/
			if (hijopid >= 0) 
			{	
				if (hijopid == 0)
				{
					char mensaje[1024];
					printf("\e[34;01m-");
					gets (mensaje);
					if (write (sock,&mensaje,largo) <0 )
						error("write");
					if(strcmp(mensaje,"Adios")==0)
					{
						return 0;
						shutdown(sock,2);
					}		
				}
				/*Parte padre del fork*/
				else 
				{
					if ((largo = read(sock, linea, sizeof(linea))) < 0)
						error ("read");
						
					if(strcmp(linea, "Adios")==0)
						{
							printf("%s",linea);
						return 0;
						shutdown(sock,2);
                        }
					
					linea[largo]=0;
					printf("\e[35;01m servidor: %s \n", linea);	
				}
				
			}
			/*Retorna error si el fork falla*/
			else 
			{
				perror("fork");
				exit(0);
			}
		}
      
       return 0;
}

/*función SL recibe el puerto*/
int SL(int puerto)
{
	#define PORT2 puerto
	int sock, sock1;
        struct sockaddr_in sin, sin1;
        char linea[1024];
        socklen_t conecta;
        int largo = 128; 
        if ((sock = socket (AF_INET, SOCK_STREAM, 0)) <0)
			error("socket");
 
        /*Se abre el servidor y espera que se conecte un cliente */
        memset(&sin, 0, sizeof sin);
        sin.sin_family = AF_INET;
        sin.sin_port = htons(PORT2);
        sin.sin_addr.s_addr = INADDR_ANY;
 
        /*re rotarna un error si la busqueda no es acertada*/
        if(bind(sock, (sad) &sin, sizeof sin)<0)
			error ("bind");
 
        /* se define el maximo de clientes*/
        if (listen(sock, 5) < 0)
                error ("listen");
 
        /*en la espera de un cliente*/
        conecta = sizeof(sin1);
        if((sock1 = accept(sock, (sad)&sin1, &conecta)) < 0)
			error("accept");
			
		while (linea != 0)
		{
			pid_t hijopid;
			/* inicio de la Bifurcacion */
			hijopid = fork();
			if (hijopid >= 0)
			{
				/*parte hijo del fork*/
				if (hijopid == 0) 
				{
					printf("\e[34;01m-");
					gets (linea);
					
					if (write(sock1, linea, largo) < 0)
						error("write");
					
					if(strcmp(linea,"Adios")==0)
						{
						shutdown(sock1,2);
						shutdown(sock,2);
						return 0;}
			
				}
				/*parte padre del fork*/
				else 
				{
					if ((largo = read(sock1, linea, sizeof(linea)))<0)
                        error("read");
                        
                    if(strcmp(linea, "Adios")==0)
						{
						printf("%s",linea);
						shutdown(sock,2);
                        shutdown(sock1,2);
                        return 0;}
					
                    linea[largo] = 0;
                    printf("\e[35;01m cliente: %s \n", linea);
					linea[0]++;
					
				}
			}
			/*retorna un error si falla el fork*/
			else
			{
				perror("fork");
				exit(0); 
			}
		}
      
	return 0;
}
