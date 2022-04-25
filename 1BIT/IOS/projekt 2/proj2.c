/**
 * @file IOS projekt 2
 * @name proj2.c
 * @author Simona Jánošíková, xjanos19
 * @brief Tvorenie molekul H2O
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

/**
 * @brief kontrola_argumentov Funkcia kontrolujuca spravnost argumentov 
 * @param no Pocet kyslikov
 * @param nh Pocet vodikov
 * @param ti Maximalny cas cakania vodika alebo kyslika na zaradenie do fronty
 * @param tb Maximalny cas potrebny na vytvorenie molekulu H2O
 * @param argv Vstupne argumenty programu
 */
bool kontrola_argumentov(int no, int nh, int ti, int tb, char *argv[])
{
    bool kontrola = false;

    /*Pre argumenty je potrebne skontrolovat, ci sa skladaju iba z cislic.
      Dovod tohto osetrenia je, ze mozu byt rovne nule, co znaci ze by navratova hodnota atoi() neodhalila vyskyt ineho znaku. */
    
    for(int j = 1; j < 5; j++)
    {
        for(unsigned int i = 0; i < strlen(argv[j]); i++)
        {
            if(argv[j][i] < '0' || argv[j][i] > '9')
            {
			    kontrola = true;
                break;
            }    
        }
    }

    if( (no < 0) || (nh < 0) || (ti < 0 || ti > 1000) || (tb < 0 || tb > 1000) || kontrola )
    {
        return true;
    }

    return false;
}

/**
 * @brief Funkcia sluziaca na uvolnenie zdielanej pamate a semaforov
 */
void upratanie_neporiadku(void)
{
}

int main(int argc, char *argv[])
{
    //overenie ci je argumentov 5 => potrebny format ./proj2 NO NH TI TB
    if(argc != 5)
    {
        fprintf(stdout,"Nespravny pocet argumentov!\n");
        exit(1);
    }
    
    // NO NH TI TB
    int NO = atoi(argv[1]); //pocet vodikov
    int NH = atoi(argv[2]); //pocet kyslikov
    int TI = atoi(argv[3]); //maximalny cas cakania vodika alebo kyslika na zaradenie do fronty
    int TB = atoi(argv[4]); //maximalny cas potrebny na vytvorenie molekulu H2O

    bool argumenty = kontrola_argumentov(NO,NH,TI,TB,argv);
    
    if(argumenty == true)
    {
        fprintf(stderr,"Nekorektne zadane argumenty!\n");
        exit(1);
    }    

    //premenna file, ktora bude potrebna pre vypis vystupu
    FILE *file;
    file = fopen("proj2.out","w");
    if(file == NULL)
    {
        fprintf(stderr,"Nepodarilo sa otvorit vystupny subor proj2.out!\n");
        fclose(file);
        upratanie_neporiadku();
        exit(1);
    }

    upratanie_neporiadku();
    fclose(file);
    exit(0);
}
/* proj2.c */ 