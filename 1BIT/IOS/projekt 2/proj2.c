/**
 * IOS projekt 2
 * @file proj2.c
 * @author Simona Jánošíková, xjanos19
 * @brief Tvorenie molekul H2O
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/wait.h>

FILE *file;
int *proces_id; //pocitadlo procesov pre vypis do suboru
unsigned *O_rad; //pocet kyslikov, co cakaju v bariere aby sa mohli molekulovat
unsigned *H_rad; //pocet vodikov, co cakaju v bariere aby sa mohli molekulovat
int *max_poc_molekul; //maximalny pocet molekul, co sa mozu vytvorit 
bool *all_mol; //signal, ci boli vytvorene vsetktky molekuly, co sa vytvorit mohli
int *molekula_number; //cislo vytvaranej molekuly
sem_t *mutex;
sem_t *sem_kyslik_O; //semafor pre kysliky
sem_t *sem_vodik_H; //semafor pre vodiky
sem_t *sem_bariera_1; //signal ukoncenia vytvorenie molekuly vodikom
sem_t *sem_bariera_2; //vodik posiela signal kysliku ze vodik dopisal a kyslik moze pisat
sem_t *sem_bariera_3; //signal pre kyslik, ze tvarenie molekul konci
sem_t *sem_vytvaranie_molekul; //povolenie vytvarania molekul


/**
 * @brief Funkcia sluziaca na uvolnenie zdielanej pamate a semaforov
 */
void upratanie_neporiadku_a_arivederci(void)
{
    bool chyba = false; //chyba nenastala
    if((sem_destroy(mutex) == -1) ||
       (sem_destroy(sem_kyslik_O) == -1) ||
       (sem_destroy(sem_vodik_H) == -1) ||
       (sem_destroy(sem_bariera_1) == -1) ||
       (sem_destroy(sem_bariera_2) == -1) ||
       (sem_destroy(sem_bariera_3) == -1) ||
       (sem_destroy(sem_vytvaranie_molekul) == -1))
    {
        fprintf(stderr,"Chyba pri niceni semaforov!\n");
        chyba = true;
    }   

    if ((munmap(mutex,sizeof(sem_t)) == -1) ||
        (munmap(sem_kyslik_O,sizeof(sem_t)) == -1) ||
        (munmap(sem_vodik_H,sizeof(sem_t)) == -1) ||
        (munmap(sem_bariera_1,sizeof(sem_t)) == -1) ||
        (munmap(sem_bariera_2,sizeof(sem_t)) == -1) ||
        (munmap(sem_bariera_3,sizeof(sem_t)) == -1) ||
        (munmap(sem_vytvaranie_molekul,sizeof(sem_t)) == -1))
    {
        fprintf(stderr,"Chyba pri uvolnovani semaforov zo zdielanej pamate!\n");
        chyba = true;
    } 

    if((munmap(proces_id,sizeof(int)) == -1) ||
       (munmap(O_rad,sizeof(unsigned)) == -1) ||
       (munmap(H_rad,sizeof(unsigned)) == -1) ||
       (munmap(max_poc_molekul,sizeof(int)) == -1) ||
       (munmap(all_mol,sizeof(bool)) == -1) ||
       (munmap(molekula_number,sizeof(int)) == -1))
    {
        fprintf(stderr,"Chyba pri uvolnovani premennych zo zdielanej pamate!\n");
        chyba = true;
    }

    if(chyba)
    {
        exit(1);
    }   
}

/**
 *@brief Funkcia sluzi na inicializaciu zdielanej pamate a semaforov
 */
int inicializacia(void)
{
    bool chyba = false; //chyba nenastala

    proces_id = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    O_rad = mmap(NULL,sizeof(unsigned),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	H_rad = mmap(NULL,sizeof(unsigned),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    max_poc_molekul = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	all_mol = mmap(NULL,sizeof(bool),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	molekula_number = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	
    if ((proces_id == MAP_FAILED) ||
        (O_rad == MAP_FAILED) || 
        (H_rad == MAP_FAILED) ||
        (max_poc_molekul == MAP_FAILED) || 
        (all_mol == MAP_FAILED) ||
        (molekula_number == MAP_FAILED))
    {
        fprintf(stderr,"Chyba pri vytvarani zdielanej pamate pre premenne!\n");
        chyba = true;
    }    

    mutex = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	sem_kyslik_O = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	sem_vodik_H = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	sem_bariera_1 = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	sem_bariera_2 = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	sem_bariera_3 = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	sem_vytvaranie_molekul = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    
    if((mutex == MAP_FAILED) || 
       (sem_kyslik_O == MAP_FAILED) || 
       (sem_vodik_H == MAP_FAILED) || 
       (sem_bariera_1 == MAP_FAILED) || 
       (sem_bariera_2 == MAP_FAILED) || 
       (sem_vytvaranie_molekul == MAP_FAILED)) 
    {
        fprintf(stderr,"Chyba pri vytvarani zdielanej pamate pre semafory!\n");
        chyba = true;
    }    

    if( (sem_init(mutex,1,1) == -1) ||
        (sem_init(sem_kyslik_O,1,0) == -1) ||
        (sem_init(sem_vodik_H,1,0) == -1) ||
        (sem_init(sem_bariera_1,1,0) == -1) ||
        (sem_init(sem_bariera_2,1,0) == -1) ||
        (sem_init(sem_bariera_3,1,0) == -1) ||
        (sem_init(sem_vytvaranie_molekul,1,1) == -1) )
        {
            fprintf(stderr,"Chyba pri inicializacii semaforov!\n");
            chyba = true;
        }

    if(chyba)
    {
        return 1;
    }

    return 0;    
}

/**
 *@brief Proces kyslik
 *
 *@param ti Cas cakania vodika alebo kyslika po vytvoreni aby sa mohol zaradit do fronty
 *@param oid ID kyslika ktory sa prave "spracovava"
 *
 */
void O_kyslik(int ti, int oid )
{    
    srand(getpid() ^ time(NULL));
    useconds_t cas_cakania_kyslik;

    sem_wait(mutex);
    fprintf(file,"%d: O %d: started\n",*proces_id,oid);
    fflush(file);
    (*proces_id)++;
    sem_post(mutex);
    
    cas_cakania_kyslik = (rand() % (ti + 1)*1000);
    usleep(cas_cakania_kyslik);

    sem_wait(mutex);
    fprintf(file,"%d: O %d: going to queue\n",*proces_id,oid);
    fflush(file);
    (*proces_id)++;
    sem_post(mutex);

    exit(0);
}

/**
 *@brief Proces vodik 
 *
 *@param ti Cas cakania vodika alebo kyslika po vytvoreni aby sa mohol zaradit do fronty
 *@param hid ID vodika ktory sa prave "spracovava"
 *
 */
void H_vodik(int ti, int hid)
{
    srand(getpid() ^ time(NULL));
    useconds_t cas_cakania_vodik;

    sem_wait(mutex);
    fprintf(file,"%d: H %d: started\n",*proces_id,hid);
    fflush(file);
    (*proces_id)++;
    sem_post(mutex);
    
    cas_cakania_vodik = (rand() % (ti + 1))*1000;
    usleep(cas_cakania_vodik);

    sem_wait(mutex);
    fprintf(file,"%d: H %d: going to queue\n",*proces_id,hid);
    fflush(file);
    (*proces_id)++;
    sem_post(mutex);
    
    exit(0);
}

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

    if( (no <= 0) || (nh <= 0) || (ti < 0 || ti > 1000) || (tb < 0 || tb > 1000) || kontrola )
    {
        return true;
    }

    return false;
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
    int NO = atoi(argv[1]); //pocet kyslikov
    int NH = atoi(argv[2]); //pocet vodikov
    int TI = atoi(argv[3]); //maximalny cas cakania vodika alebo kyslika na zaradenie do fronty
    int TB = atoi(argv[4]); //maximalny cas potrebny na vytvorenie molekulu H2O

    bool argumenty = kontrola_argumentov(NO,NH,TI,TB,argv);
    
    if(argumenty)
    {
        fprintf(stderr,"Nekorektne zadane argumenty!\n");
        exit(1);
    }    

    //premenna file, ktora bude potrebna pre vypis vystupu
    file = fopen("proj2.out","w");
    if(file == NULL)
    {
        fprintf(stderr,"Nepodarilo sa otvorit vystupny subor proj2.out!\n");
        fclose(file);
        exit(1);
    }

    int check = inicializacia();
    if (check == 1)
    {
        fprintf(stderr,"Chyba pri inicializacii!\n)");
        fclose(file);
        upratanie_neporiadku_a_arivederci();
        exit(1);
    }

    //printf("check_init = %d\n",check);

    (*proces_id) = 1;
    (*O_rad) = 0;
    (*H_rad) = 0;
    if((NH / 2) < NO)
    {
        (*max_poc_molekul) = (NH/2);
    }
    else
    {
        (*max_poc_molekul) = NO;
    }
    //printf("%d\n",*max_poc_molekul);
    (*all_mol) = false; //nie su vytvorene vsetky molekuly
    (*molekula_number) = 0;

    //predavanie kyslikov do procesu kyslik
    for(int i = 0; i < NO; i++)
    {
        pid_t pid_O = fork();
        if(pid_O < 0)
        {
            fprintf(stderr,"Chyba pri fork-ovani kyslika!\n");
            upratanie_neporiadku_a_arivederci();
            fclose(file);
            exit(1);
        }
        else if(pid_O == 0)
        {
            O_kyslik(TI,i+1);
            exit(0);
        }
    }

    //predavanie vodikov do procesu vodik
    for(int j = 0; j < NH; j++)
    {
        pid_t pid_H = fork();
        if(pid_H < 0)
        {
            fprintf(stderr,"Chyba pri fork-ovani vodika!\n");
            upratanie_neporiadku_a_arivederci();
            fclose(file);
            exit(1); 
        }
        if(pid_H == 0)
        {
            H_vodik(TI,j+1);
            exit(0);
        }
    }

    while(wait(NULL) > 0); 

    //printf("O %u\n",*O_rad);
    //printf("H %u\n",*H_rad);

    upratanie_neporiadku_a_arivederci();
    fclose(file);

    //printf("check_koenc\n");

    exit(0);
}
/* proj2.c */ 