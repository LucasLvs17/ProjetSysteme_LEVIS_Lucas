#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h> //à décommenter sur linux
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <sys/select.h> //à décommenter sur linux
#include <ctype.h>
#include <windows.h> //à commenter sur linux
#include <winsock.h> //à commenter sur linux

#include "dico.c"


#define nom_tube_appel "appel"

#define TAILLETAUX 10
#define TAILLE_DU_MOT 10

#define TAILLE_DU_STRING 100
#define MAXIMUM_CLIENT 17


#define DEBUG 1

int piocherMot(char* motPioche);
int nombreAleatoire(int nombreMax);

int gagne(int lettreTrouvee[], long tailleMot);
int rechercheLettre(char lettre, char motSecret[], int lettreTrouvee[]);
char lireCaractere(char lettre);

char lireCaractere(char lettre)
{
    char caractere = lettre;

    caractere = toupper(caractere);

    return caractere;
}


int gagne(int lettreTrouvee[], long tailleMot)
{
    long i = 0;
    int joueurGagne = 1;

    for (i = 0; i < tailleMot; i++)
    {
        if (lettreTrouvee[i] == 0)
            joueurGagne = 0;
    }

    return joueurGagne;
}

int rechercheLettre(char lettre, char motSecret[], int lettreTrouvee[])
{

    long i = 0;
    int bonneLettre = 0;


    for (i = 0; motSecret[i] != '\0'; i++)
    {
        if (lettre == motSecret[i])
        {
            bonneLettre = 1;
            lettreTrouvee[i] = 1;
        }
    }


    return bonneLettre;
}


int main(int argc, char* argv[])
{
    int tube_appel;
    char nomTube[MAXIMUM_CLIENT][TAILLE_DU_STRING];
    char message[MAXIMUM_CLIENT][TAILLE_DU_STRING];
    char mot[TAILLE_DU_STRING];
    char pseudos[MAXIMUM_CLIENT][TAILLE_DU_STRING];
    char score[MAXIMUM_CLIENT][TAILLE_DU_STRING];

    int pid;

    pid_t fils;

    int tube[MAXIMUM_CLIENT];
    int numcli;
    char result[TAILLE_DU_STRING];

    int i, points, continuer, choix;

    char lettre = 0;
    char motSecret[100] = { 0 };
    int* lettreTrouvee = NULL;
    long coupsRestants = 10;
    long tailleMot = 0;


    numcli = 0;
    points = 0;
    continuer = 1;

    mkfifo("/tmp/serveur", 0666);


    while (1) {

        tube_appel = open("/tmp/serveur", O_RDONLY);
        if (tube_appel < 0)
        {
            perror("open tube d'appel ");
            exit(EXIT_FAILURE);
        }

        if (read(tube_appel, message[numcli], TAILLE_DU_STRING) < 0)
        {
            perror("read tube d'appel ");
            exit(EXIT_FAILURE);
        }
        close(tube_appel);

        sscanf(message[numcli], "%s %s", nomTube[numcli], pseudos[numcli]);

        printf("Connexion de %s sur le tube %s \n", pseudos[numcli], nomTube[numcli]);

        numcli++;

        if ((fils = fork()) == 0) {
            while (continuer == 1) {
                printf("moi %d m'occupe de %d \n", getpid(), numcli - 1);
                if (!piocherMot(motSecret))
                    exit(0);

                tailleMot = strlen(motSecret);

                lettreTrouvee = malloc(tailleMot * sizeof(int));
                exit(0);

                for (i = 0; i < tailleMot; i++)
                    lettreTrouvee[i] = 0;

                while (coupsRestants > 0 && !gagne(lettreTrouvee, tailleMot))
                {
                    printf("\n\nIl vous reste %ld coups a jouer\n", coupsRestants);

                    for (i = 0; i < tailleMot; i++)
                    {
                        if (lettreTrouvee[i]) {
                            mot[i] = motSecret[i];
                        }

                        else
                            mot[i] = '*';

                    }

                    
                    if (DEBUG == 1)
                    {
                        printf("Le mot a trouver du serveur est: %s. \n", mot);
                    }


                    tube[numcli - 1] = open(nomTube[numcli - 1], O_WRONLY);
                    if (tube[numcli - 1] < 0)
                    {
                        perror("open tube client ");
                        exit(EXIT_FAILURE);
                    }


                    if (write(tube[numcli - 1], mot, strlen(mot) + 1) < 0)
                    {
                        perror("write tube client ");
                        exit(EXIT_FAILURE);
                    }
                    close(tube[numcli - 1]);



                    tube[numcli - 1] = open(nomTube[numcli - 1], O_RDONLY);
                    if (tube[numcli - 1] < 0)
                    {
                        perror("open tube");
                        exit(EXIT_FAILURE);
                    }


                    if (read(tube[numcli - 1], message[numcli - 1], TAILLE_DU_STRING) < 0)
                    {
                        perror("read tube");
                        exit(EXIT_FAILURE);
                    }
                    close(tube[numcli - 1]);


                    printf("La lettre reçu est %s\n", message[numcli - 1]);


                    for (i = 0; message[numcli - 1][i] != '\0'; i++)
                    {
                        lettre = message[numcli - 1][i];
                    }


                    if (!rechercheLettre(lettre, motSecret, lettreTrouvee))
                    {
                        coupsRestants--;
                    }
                }


                if (gagne(lettreTrouvee, tailleMot))
                {
                    points++;
                    sprintf(score[numcli - 1], "%d", points);


                    printf("\n\nGagné ! Le mot à trouver était : %s %d\n", motSecret, points);
                    sprintf(result, "Gagné ! Le mot à trouver était : %s \nScore:%s", motSecret, score[numcli - 1]);

                }
                else
                {
                    printf("\nPerdu ! Le mot à trouver était : %s\n", motSecret);
                    sprintf(result, "Perdu ! Le mot à trouver était : %s", motSecret);
                }

                // Ouverture du tube client
                tube[numcli - 1] = open(nomTube[numcli - 1], O_WRONLY);
                if (tube[numcli - 1] < 0)
                {
                    perror("open tube client ");
                    exit(EXIT_FAILURE);
                }

                if (write(tube[numcli - 1], result, strlen(result) + 1) < 0)
                {
                    perror("write tube client ");
                    exit(EXIT_FAILURE);
                }
                close(tube[numcli - 1]);

                free(lettreTrouvee);
                continuer = 0;
                if (continuer == 0)
                {

                    tube[numcli - 1] = open(nomTube[numcli - 1], O_RDONLY);
                    if (tube[numcli - 1] < 0)
                    {
                        perror("open tube d'appel ");
                        exit(EXIT_FAILURE);
                    }

                    if (read(tube[numcli - 1], message[numcli - 1], TAILLE_DU_STRING) < 0)
                    {
                        perror("read tube d'appel ");
                        exit(EXIT_FAILURE);
                    }
                    close(tube[numcli - 1]);
                    sscanf(message[numcli - 1], "%d", &choix);
                    continuer = choix;

                }
                else
                    exit(EXIT_SUCCESS);
            }
        }
    }
    return 0;
}
