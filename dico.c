#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int piocheMot(char* motPioche);
int nbRand(int nbMaximum);


int piocheMot(char* motPioche)
{
    FILE* dico = NULL;
    int nbMots = 0, nbMotChois = 0, i = 0;
    int caractereLu = 0;
    dico = fopen("dico.txt", "r");
    if (dico == NULL)
    {
        printf("\nLes mots du dictionnaire n'ont pas été trouvés");
        return 0;
    }
    do
    {
        caractereLu = fgetc(dico);
        if (caractereLu == '\n')
            nbMots++;
    } while (caractereLu != EOF);

    nbMotChois = nbRand(nbMots);

    rewind(dico);
    while (nbMotChois > 0)
    {
        caractereLu = fgetc(dico);
        if (caractereLu == '\n')
            nbMotChois--;
    }
    fgets(motPioche, 100, dico);

    motPioche[strlen(motPioche) - 1] = '\0';
    fclose(dico);

    return 1;
}

int nbRand(int nbMaximum)
{
    srand(time(NULL));
    return (rand() % nbMaximum);
}
