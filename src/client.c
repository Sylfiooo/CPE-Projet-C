/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <string.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include "client.h"

#include "bmp.h"

void dataToJson(char * data, int type) {
    
    //type 1 pour data avec un int nb devant les valeurs
    //type 2 pour data qui donne directement les valeurs
    char jsonData[1024] = "{ \"code\" : \"";
    int boucle = 0;
    const char * separators = ":,";
    char * strToken = strtok ( data, separators);
    while ( strToken != NULL) {
        if (boucle == 0) {
            strcat(jsonData, strToken);
            strcat(jsonData, "\", \"valeurs\" : [ ");
        } else if (boucle > 0 && type == 2) {
            if (isdigit(strToken)) {

            } else {
                strcat(jsonData, "\"");
                strcat(jsonData, strToken);
                strcat(jsonData, "\",");
            }

        } else if (boucle > 1) {
            strcat(jsonData, "\"");
            strcat(jsonData, strToken);
            strcat(jsonData, "\",");
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

/* 
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoie_recois_message(int socketfd) {

    char data[1024];
    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // Demandez à l'utilisateur d'entrer un message
    char message[100];
    printf("Votre message (max 1000 caracteres): ");
    fgets(message, 1024, stdin);
    strcpy(data, "message:");
    strcat(data, message);

    //donne directement les valeurs derriere message donc type 2
    dataToJson(data, 2);

    int write_status = write(socketfd, data, strlen(data));
    if (write_status < 0) {
        perror("erreur ecriture");
        exit(EXIT_FAILURE);
    }

    printf("Message envoyé: %s\n", data);

    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // lire les données de la socket
    int read_status = read(socketfd, data, sizeof(data));
    if (read_status < 0) {
        perror("erreur lecture");
        return -1;
    }

    printf("Message reçu: %s\n", data);

    return 0;
}

/* 
 * Fonction d'envoi et de retour du hostname
 * Il faut un argument : l'identifiant de la socket
 */
int envoie_nom_de_client(int socketfd) {
    char data[1024];
    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // Demandez à l'utilisateur d'entrer un message
    char nom[100];
    gethostname(nom, sizeof(nom));
    strcpy(data, "nom:");
    strcat(data, nom);

    //donne directement les valeurs derriere message donc type 2
    dataToJson(data, 2);

    int write_status = write(socketfd, data, strlen(data));
    if (write_status < 0) {
        perror("erreur ecriture");
        exit(EXIT_FAILURE);
    }

    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // lire les données de la socket
    int read_status = read(socketfd, data, sizeof(data));
    if (read_status < 0) {
        perror("erreur lecture");
        return -1;
    }

    printf("Hostname reçu: %s\n", data);

    return 0;
}

/* 
 * Fonction d'envoi de calcul et de retour de résultat
 * Il faut un argument : l'identifiant de la socket
 */
int envoie_operateur_numeros(int socketfd) {
    char data[1024];
    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // Demandez à l'utilisateur d'entrer un message
    char calcul[100];
    int count = 0;
    printf("Votre calcul (max 1000 caracteres): ");
    fgets(calcul, 1024, stdin);
    strcpy(data, "calcul:");
    char * strtoken = strtok(calcul, " ");
    while(strtoken !=NULL){
    	if(count == 0){
			strcat(data, strtoken); 
		}else{
			strcat(data, ",");
			strcat(data, strtoken);
		}
		count++;
		strtoken = strtok(NULL, " ");
    }

    //donne directement les valeurs derriere message donc type 2
    dataToJson(data, 2);
    
    int write_status = write(socketfd, data, strlen(data));
    if (write_status < 0) {
        perror("erreur ecriture");
        exit(EXIT_FAILURE);
    }

    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // lire les données de la socket
    int read_status = read(socketfd, data, sizeof(data));
    if (read_status < 0) {
        perror("erreur lecture");
        return -1;
    }

    printf("Résultat reçu: %s\n", data);

    return 0;
}

void analyse(char * pathname, char * data) {
    //compte de couleurs
    couleur_compteur * cc = analyse_bmp_image(pathname);

    //choisir un nombre de couleurs
    char nbcouleur[10];
    printf("Nombre de couleurs (max 30): ");
    fgets(nbcouleur, 1024, stdin);
    int intnbcouleur = atoi(nbcouleur);

    if(intnbcouleur > 30) {
        perror("erreur ecriture");
        exit(EXIT_FAILURE);
    }

    int count; 
    strcpy(data, "bmp:");
    char temp_string[10] = "";
    strcat(temp_string, nbcouleur);
    strcat(temp_string, ",");

    if (cc -> size < 10) {
        sprintf(temp_string, "%d,", cc -> size);
    }
    strcat(data, temp_string);

    intnbcouleur++;

    for (count = 1; count < intnbcouleur && cc -> size - count > 0; count++) {
        if (cc -> compte_bit == BITS32) {
            sprintf(temp_string, "#%02x%02x%02x,", cc -> cc.cc24[cc -> size - count].c.rouge, cc -> cc.cc32[cc -> size - count].c.vert, cc -> cc.cc32[cc -> size - count].c.bleu);
        }
        if (cc -> compte_bit == BITS24) {
            sprintf(temp_string, "#%02x%02x%02x,", cc -> cc.cc32[cc -> size - count].c.rouge, cc -> cc.cc32[cc -> size - count].c.vert, cc -> cc.cc32[cc -> size - count].c.bleu);
        }
        strcat(data, temp_string);
    }

    printf("%s", data);

    //enlever le dernier virgule
    data[strlen(data) - 1] = '\0';
}

int envoie_couleurs(int socketfd) {
    char data[1024];
    //analyse(pathname, data);

    // Demandez à l'utilisateur d'entrer un nombre de couleurs et des couleurs en hexa
    char nb[100];
    printf("Votre nombre de couleurs (max 30 caracteres): ");
    fgets(nb, 1024, stdin);
    strcpy(data, "couleur: ");
    strcat(data, nb);
    int nbint = atoi(nb);

    if(nbint > 30) {
        perror("erreur ecriture");
        exit(EXIT_FAILURE);
    }

    char couleur[100];

    while(nbint != 0) {
        nbint = nbint - 1;
        printf("Rentrez une couleur: ");
        fgets(couleur, 1024, stdin);
        strcat(data, ",");
        strcat(data, couleur);
    }

    //donne un nombre de couleurs avant les valeurs derriere message donc type 1
    dataToJson(data, 1);

    int write_status = write(socketfd, data, strlen(data));
    if (write_status < 0) {
        perror("erreur ecriture");
        exit(EXIT_FAILURE);
    }

    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // lire les données de la socket
    int read_status = read(socketfd, data, sizeof(data));
    if (read_status < 0) {
        perror("erreur lecture");
        return -1;
    }

    printf("Résultat reçu: %s\n", data);

    return 0;
}

int envoie_couleurs_image(int socketfd, char *pathname) {
  char data[1024];
  memset(data, 0, sizeof(data));
  analyse(pathname, data);
  
  dataToJson(data, 2);
  
  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int envoie_balises(int socketfd) {
    char data[1024];
    //analyse(pathname, data);

    // Demandez à l'utilisateur d'entrer un nombre de couleurs et des couleurs en hexa
    char nb[100];
    printf("Votre nombre de balises (max 30 caracteres): ");
    fgets(nb, 1024, stdin);
    strcpy(data, "balise:");
    strcat(data, nb);
    int nbint = atoi(nb);

    if(nbint > 30) {
        perror("erreur ecriture");
        exit(EXIT_FAILURE);
    }

    char balise[100];

    while(nbint != 0) {
        nbint = nbint - 1;
        printf("Rentrez une balise: ");
        fgets(balise, 1024, stdin);
        strcat(data, ",");
        strcat(data, balise);
    }

    //donne un nombre de couleurs avant les valeurs derriere message donc type 1
    dataToJson(data, 1);

    int write_status = write(socketfd, data, strlen(data));
    if (write_status < 0) {
        perror("erreur ecriture");
        exit(EXIT_FAILURE);
    }

    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // lire les données de la socket
    int read_status = read(socketfd, data, sizeof(data));
    if (read_status < 0) {
        perror("erreur lecture");
        return -1;
    }

    printf("Résultat reçu: %s\n", data);

    return 0;
}

int main(int argc, char ** argv) {
    int socketfd;
    int bind_status;

    struct sockaddr_in server_addr, client_addr;

    /*
     * Creation d'une socket
     */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //détails du serveur (adresse et port)
    memset( & server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //demande de connection au serveur
    int connect_status = connect(socketfd, (struct sockaddr * ) & server_addr, sizeof(server_addr));
    if (connect_status < 0) {
        perror("connection serveur");
        exit(EXIT_FAILURE);
    }
    if (argv[1]){
    	if(strcmp(argv[1], "nom") == 0){
    		envoie_nom_de_client(socketfd);
    	}else if(strcmp(argv[1], "message") == 0){
    		envoie_recois_message(socketfd);
    	}else if(strcmp(argv[1], "calcul") == 0){
    		envoie_operateur_numeros(socketfd);
    	}else if(strcmp(argv[1], "couleurs") == 0){
    		envoie_couleurs(socketfd);
    	}else if(strcmp(argv[1], "balises") == 0){
    		envoie_balises(socketfd);
    	}else if(strcmp(argv[1], "bmp") == 0 && argv[2]){
    		envoie_couleurs_image(socketfd, argv[2]);
    	}else{
    		printf("La saisie n'est pas correcte. Utilisation : ./client (nom/message/calcul/couleurs/balises/bmp) [path_for_bmp]\n");
    	}
    }else{
    	printf("Il n'y a pas d'arguments, la saisie n'est pas correcte. Utilisation : ./client (nom/message/calcul/couleurs/balises/bmp) [path_for_bmp]\n");
    }
    close(socketfd);
}

// void constjson(char * data){
// 	char json[1024] = "";
// 	int count = 0;
// 	strcat(json, "{ \"code\" : \"");
	
// 	char * strtoken = strtok(data, ": ");
// 	strcat(json, strtoken);
// 	strcat(json, "\", \"valeurs\" : [ ");
	
// 	strtoken = strtok(NULL, ",");
	
// 	while (strtoken != NULL ){
// 		if(count == 0){
// 			strcat(json, "\"");
// 			strcat(json, strtoken);
// 			strcat(json, "\""); 
// 		}else{
// 			strcat(json, ", \"");
// 			strcat(json, strtoken);
// 			strcat(json, "\""); 
// 		}
// 		count++;
// 		strtoken = strtok(NULL, ",");
// 	}
	
// 	strcat(json, " ] }");
// 	strcpy(data, json);
// }
