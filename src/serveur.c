/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <sys/types.h>

#include <sys/socket.h>

#include <sys/epoll.h>

#include <netinet/in.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include "serveur.h"

#include "validation.c"

void deleteLn(char *text)
{
    int j;
    j = strlen(text);
    while(j--)
    {
        if(text[j] == 10)
            text[j] = '\0';

    }
}

char isNumber(char * text)
{
    int j;
    j = strlen(text);
    while(j--)
    {
        if(text[j] >= '0' && text[j] <= '9')
            continue;

        return 0;
    }
    return 1;
}

void dataToJson(char * data, int type) {
    
    //type 1 pour data avec un int nb devant les valeurs
    //type 2 pour data qui donne directement les valeurs
    char jsonData[1024] = "{ \"code\" : \"";
    int boucle = 0;
    const char * separators = ":,";
    char * strToken = strtok ( data, separators);
    while ( strToken != NULL) {
        deleteLn(strToken);
        if (boucle == 0) {
            strcat(jsonData, strToken);
            strcat(jsonData, "\", \"valeurs\" : [");
        } else if (boucle > 0 && type == 2) {
            if (isNumber(strToken)) {
                strcat(jsonData, strToken);
                strcat(jsonData, ",");
            } else {
                strcat(jsonData, "\"");
                strcat(jsonData, strToken);
                strcat(jsonData, "\",");
            }

        } else if (boucle > 1) {
            if (isNumber(strToken)) {
                strcat(jsonData, strToken);
                strcat(jsonData, ",");   
            } else {
                strcat(jsonData, "\"");
                strcat(jsonData, strToken);
                strcat(jsonData, "\",");
            }

        }
        strToken = strtok ( NULL, separators );
        boucle++;
    }
    //enlever le dernier virgule
    int size = strlen(jsonData);
    jsonData[size-1] = '\0';
    strcat(jsonData, "]}");

    strcpy(data,jsonData);
}

void jsonToData(char * data) {
    char newData[1024] = "";
    int boucle = 0;
    const char * separators = "\",[]}";
    char * strToken = strtok ( data, separators);
    while ( strToken != NULL) {
        //code
        if (boucle == 3) {
            strcat(newData, strToken);
            strcat(newData, ": ");
        //valeur
        } else if (boucle > 6  && strToken != "valeurs") {
            //else if (boucle > 6 && boucle%2 == 1) {
            strcat(newData, strToken);
            strcat(newData, ",");
        }
        strToken = strtok ( NULL, separators );
        boucle++;
    }
    //enlever le dernier virgule
    int size = strlen(newData);
    newData[size-1] = '\0';

    strcpy(data,newData);
}

void plot(char * data) {

    

    //Extraire le compteur et les couleurs RGB 
    FILE * p = popen("gnuplot -persist", "w");
    printf("Plot");
    int count = 0;
    int countcol = 0;
    int n;
    char * saveptr = NULL;
    char * str = data;
    const char * separators = ":";
    char nbcouleur[10];
    int intNbCouleur;
    fprintf(p, "set xrange [-15:15]\n");
    fprintf(p, "set yrange [-15:15]\n");
    fprintf(p, "set style fill transparent solid 0.9 noborder\n");
    while (1) {
        char * token = strtok_r(str, ",", & saveptr);
        if (token == NULL) {
            break;
        }
        str = NULL;
        if (count == 0) {
            n = atoi(token);
            char * strToken = strtok ( token, separators );
            while ( strToken != NULL ) {
                if (countcol == 1) {
                    strcat(nbcouleur, strToken);
                }
                strToken = strtok ( NULL, separators );
                countcol++;
            }
            intNbCouleur = atoi(nbcouleur);
    		fprintf(p, "set title 'Top %d colors'\n", intNbCouleur);
    		fprintf(p, "plot '-' with circles lc rgbcolor variable\n");
        } else {
            // Le numéro 36, parceque 360° (cercle) / 10 couleurs = 36
            fprintf(p, "0 0 10 %f %f 0x%s\n", (float)((count - 1) * 360 / intNbCouleur), (float)(count * 360 / intNbCouleur), token + 1);
        }
        count++;
    }
    fprintf(p, "e\n");
    printf("Plot: FIN\n");
    pclose(p);
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char * data) {
    int data_size = write(client_socket_fd, (void * ) data, strlen(data));

    if (data_size < 0) {
        perror("erreur ecriture");
        return (EXIT_FAILURE);
    }
}

/* renvoyer le hostname (*data) au client (client_socket_fd)
 */
int renvoie_nom(int client_socket_fd, char * data) {
    int data_size = write(client_socket_fd, (void * ) data, strlen(data));

    if (data_size < 0) {
        perror("erreur ecriture");
        return (EXIT_FAILURE);
    }
}

/* renvoyer le résultat du calcul (*data) au client (client_socket_fd)
*/

int recois_couleurs(int client_socket_fd, char * data) {
    
    // La définitions de séparateurs connus.
    const char * separators = ":,";

    int boucle = 0;

    //open File
    FILE *fp;
    fp = fopen("./couleur.txt", "w");

    // On cherche à récupérer, un à un, tous les mots (token) de la phrase
    // et on commence par le premier.
    char * strToken = strtok ( data, separators);
    while ( strToken != NULL) {

        if (boucle > 0) {
            fputs( strToken, fp );
            fputs("\n", fp);
        }
        // On demande le token suivant.
        strToken = strtok ( NULL, separators );
        boucle++;
    }

    fclose(fp);
	
	strcpy(data, "{\"code\" : \"couleurs\" , \"valeurs\" : [ \"enregistré\" ] }");
    printf("Resultat envoyé format JSON: %s\n", data); 
    int data_size = write(client_socket_fd, (void * ) data, strlen(data));
	
    if (data_size < 0) {
        perror("erreur ecriture");
        return (EXIT_FAILURE);
    }

}

int recois_balises(int client_socket_fd, char * data) {
    
    // La définitions de séparateurs connus.
    const char * separators = ":,";

    int boucle = 0;

    FILE *fp;
    fp = fopen("./balise.txt", "w");

    // On cherche à récupérer, un à un, tous les mots (token) de la phrase
    // et on commence par le premier.
    char * strToken = strtok ( data, separators);
    while ( strToken != NULL) {
        if (boucle > 0) {
            fputs( strToken, fp );
            fputs("\n", fp);
        }
        // On demande le token suivant.
        strToken = strtok ( NULL, separators );
        boucle++;
    }

    fclose(fp);
	
    strcpy(data,"{\"code\" : \"balises\" , \"valeurs\" : [ \"enregistré\" ] }");
	printf("Resultat envoyé format JSON: %s\n", data); 
    int data_size = write(client_socket_fd, (void * ) data, strlen(data));
	
    if (data_size < 0) {
        perror("erreur ecriture");
        return (EXIT_FAILURE);
    }

}

/* renvoyer le résultat du calcul (*data) au client (client_socket_fd)
 */
int recois_numeros_calcul(int client_socket_fd, char * data) {

	char nb1[10];
	char nb2[10];
    double inb1;
    double inb2;
    double result;
	char operator[10];
    int boucle = 0;
    const char * separators = ":,";
    
    char * strToken = strtok ( data, separators);
    while ( strToken != NULL) {
        if (boucle == 1) {
            strcpy(operator, strToken);
        } else if (boucle == 2) {
            strcpy(nb1, strToken);
            inb1 = atof(nb1);
        } else if (boucle == 3) {
            strcpy(nb2, strToken);
            inb2 = atof(nb2);
        }
        // On demande le token suivant.
        strToken = strtok ( NULL, separators );
        boucle++;
    }

	if (strcmp(operator, " +") == 0){
		result = inb1 + inb2;
	} else if (strcmp(operator, " -") == 0){
		result = inb1 - inb2;
	} else if (strcmp(operator, " *") == 0){
		result = inb1 * inb2;
	} else if (strcmp(operator, " /") == 0){
		result = inb1 / inb2;
	} else {
		perror("erreur écriture");
		return (EXIT_FAILURE);
	}
	
	memset(data, 0, sizeof(data));
	char s[10];
	sprintf(s, "%f", inb1);
    strcat(data, s);
    strcat(data, operator);
    strcat(data, " ");
    sprintf(s, "%f", inb2);
    strcat(data, s);
    strcat(data, " = ");
    sprintf(s, "%f", result);
    strcat(data, s);
	
	printf("Calcul envoyé : %s\n", data);

    strcpy(data, "{\"code\" : \"calcule\" , \"valeurs\" : [ ");
    strcat(data, s);
    strcat(data, " ]}"); 

    printf("Resultat envoyé format JSON: %s\n", data); 
    int data_size = write(client_socket_fd, (void * ) data, strlen(data));
	
    if (data_size < 0) {
        perror("erreur ecriture");
        return (EXIT_FAILURE);
    }
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd) {
    struct sockaddr_in client_addr;
    char data[1024];

    int client_addr_len = sizeof(client_addr);

    // nouvelle connection de client
    int client_socket_fd = accept(socketfd, (struct sockaddr * ) & client_addr, & client_addr_len);
    if (client_socket_fd < 0) {
        perror("accept");
        return (EXIT_FAILURE);
    }

    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    //lecture de données envoyées par un client
    int data_size = read(client_socket_fd, (void * ) data, sizeof(data));

    if (data_size < 0) {
        perror("erreur lecture");
        return (EXIT_FAILURE);
    }
    
    printf("Message reçu format json:%s\n", data);
    jsonToData(data);
    printf("Message reçu format data:%s\n", data);

    /*
     * extraire le code des données envoyées par le client. 
     * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
     */
    char code[10];
    sscanf(data, "%s", code);

    //Si le message commence par le mot: 'message:' 
    if (strcmp(code, "message:") == 0) {
        // la réinitialisation de l'ensemble des données
        memset(data, 0, sizeof(data));

        // Demandez à l'utilisateur d'entrer un message
        char message[100];
        printf("Votre message (max 1000 caracteres): ");
        fgets(message, 1024, stdin);
        strcpy(data, "message:");
        strcat(data, message);



        printf("Message envoyé format data: %s\n", data);
        dataToJson(data, 2);
        printf("Message envoyé format json: %s\n", data);
        renvoie_message(client_socket_fd, data);
    } else if (strcmp(code, "nom:") == 0) {
        printf("Hostname envoyé format data: %s\n", data);
        dataToJson(data, 2);
        printf("Hostname envoyé format json: %s\n", data);
        renvoie_nom(client_socket_fd, data);
    } else if (strcmp(code, "calcul:") == 0) {
        recois_numeros_calcul(client_socket_fd, data);
    } else if (strcmp(code, "couleur:") == 0) {
        recois_couleurs(client_socket_fd, data);
    } else if (strcmp(code, "balise:") == 0) {
        recois_balises(client_socket_fd, data);
    } else if (strcmp(code, "bmp:") == 0) {
        plot(data);
    }

    //fermer le socket 
    close(socketfd);
}

int main() {

    int socketfd;
    int bind_status;
    int client_addr_len;

    struct sockaddr_in server_addr, client_addr;

    /*
     * Creation d'une socket
     */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("Unable to open a socket");
        return -1;
    }

    int option = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, & option, sizeof(option));

    //détails du serveur (adresse et port)
    memset( & server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Relier l'adresse à la socket
    bind_status = bind(socketfd, (struct sockaddr * ) & server_addr, sizeof(server_addr));
    if (bind_status < 0) {
        perror("bind");
        return (EXIT_FAILURE);
    }

    // Écouter les messages envoyés par le client
    listen(socketfd, 10);

    //Lire et répondre au client
    recois_envoie_message(socketfd);

    return 0;
}
