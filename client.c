#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h> //à décommenter sur linux
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <windows.h> //à commenter sur linux

#define nom_tube_appel "appel"

#define TAILLE_DU_STRING 100
#define TAILLE_DU_MOT 20

#define DEBUG 1

int main(int argc, char* argv[])
{
    int tube_appel;
    char nomTube[TAILLE_DU_STRING];
    char lettre[TAILLE_DU_MOT];
    char message[TAILLE_DU_STRING];
    char pseudonyme[TAILLE_DU_STRING];

    char motatrouver[TAILLE_DU_STRING];

    float montant;

    int tube;
    int pid;
    int suite;



    printf("Entrez un pseudonyme:\n");
    scanf("%s", pseudonyme);

    if (sprintf(message, "/tmp/rep%d %s", getpid(), pseudonyme) < 0)
    {
        perror("Ecriture de la requete ");
        exit(EXIT_FAILURE);
    }

    printf("Bienvenue dans le jeu du Pendu !\n\n");


    tube_appel = open("/tmp/serveur", O_WRONLY);
    if (tube_appel < 0)
    {
        perror("open tube d'appel ");
        exit(EXIT_FAILURE);
    }


    if (write(tube_appel, message, strlen(message) + 1) < 0)
    {
        perror("write tube d'appel ");
        exit(EXIT_FAILURE);
    }
    close(tube_appel);


    if (sprintf(nomTube, "/tmp/rep%d", getpid()) < 0)
    {
        perror("Ecriture du nom du tube client ");
        exit(EXIT_FAILURE);
    }

    mkfifo(nomTube, 0666);



    while (1)
    {

        tube = open(nomTube, O_RDONLY);
        if (tube < 0)
        {
            perror("open tube");
            exit(EXIT_FAILURE);
        }


        if (read(tube, motatrouver, TAILLE_DU_STRING) < 0)
        {
            perror("read tube");
            exit(EXIT_FAILURE);
        }
        close(tube);

        if ((strstr(motatrouver, "Vous avez trouvé le mot") != NULL) || (strstr(motatrouver, "Vous n'avez pas trouvé le mot") != NULL))
        {
            printf("%s\n", motatrouver);
            printf("\n\nVoulez vous rejouer ? Oui(1) ou Non(0)\n");
            scanf("%d", &suite);
            sprintf(lettre, "%d", suite);
            if (suite == 1)
            {

                tube = open(nomTube, O_WRONLY);
                if (tube < 0)
                {
                    perror("open tube");
                    exit(EXIT_FAILURE);
                }


                if (write(tube, lettre, strlen(lettre) + 1) < 0)
                {
                    perror("write tube");
                    exit(EXIT_FAILURE);
                }
                close(tube);
                tube = open(nomTube, O_RDONLY);
                if (tube < 0)
                {
                    perror("open tube");
                    exit(EXIT_FAILURE);
                }

                if (read(tube, motatrouver, TAILLE_DU_STRING) < 0)
                {
                    perror("read tube d'appel ");
                    exit(EXIT_FAILURE);
                }
                close(tube);

            }
            else {
                printf("Fin de la partie\n");
                break;
            }
        }

        printf("Le mot a trouver est %s. \n", motatrouver);

        printf("\n Veuillez saisir une lettre : ");
        scanf("%s", lettre);
        tube = open(nomTube, O_WRONLY);
        if (tube < 0)
        {
            perror("open tube");
            exit(EXIT_FAILURE);
        }

        if (write(tube, lettre, strlen(lettre) + 1) < 0)
        {
            perror("write tube");
            exit(EXIT_FAILURE);
        }
        close(tube);

        if (DEBUG == 1)
        {
            printf("Le message a envoyer est '%s'. \n", lettre);
        }

    }



    return 0;
}
