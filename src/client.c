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
    strcpy(data, "message: ");
    strcat(data, message);

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
    strcpy(data, "nom: ");
    strcat(data, nom);

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
    printf("Votre calcul (max 1000 caracteres): ");
    fgets(calcul, 1024, stdin);
    strcpy(data, "calcul: ");
    strcat(data, calcul);

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

    int count;
    strcpy(data, "couleurs: ");
    char temp_string[10] = "10,";
    if (cc -> size < 10) {
        sprintf(temp_string, "%d,", cc -> size);
    }
    strcat(data, temp_string);

    //choisir 10 couleurs
    for (count = 1; count < 11 && cc -> size - count > 0; count++) {
        if (cc -> compte_bit == BITS32) {
            sprintf(temp_string, "#%02x%02x%02x,", cc -> cc.cc24[cc -> size - count].c.rouge, cc -> cc.cc32[cc -> size - count].c.vert, cc -> cc.cc32[cc -> size - count].c.bleu);
        }
        if (cc -> compte_bit == BITS24) {
            sprintf(temp_string, "#%02x%02x%02x,", cc -> cc.cc32[cc -> size - count].c.rouge, cc -> cc.cc32[cc -> size - count].c.vert, cc -> cc.cc32[cc -> size - count].c.bleu);
        }
        strcat(data, temp_string);
    }

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

    //envoie_operateur_numeros(socketfd);
    //envoie_recois_message(socketfd);
    //envoie_nom_de_client(socketfd);
    envoie_couleurs(socketfd);

    close(socketfd);
}
