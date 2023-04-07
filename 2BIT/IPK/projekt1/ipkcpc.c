/* IPK projekt 1
 * Fakulta informacnych technologii VUT
 * klient pre IPK Calculator Protokol
 * autor: Simona Janosikova (xjanos19)
 */

#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define INPUT_SIZE 1024

int clientSocket;

/* 
 * @brief Funkcia, ktora po detekcii CTRL-C v TCP korektne vypne spojenie
 */
void detect_CTRLC_in_TCP()
{
    char responseMsg[INPUT_SIZE];
    // odoslanie bye hlasky
    int sendCheck = send(clientSocket, "BYE\n", 4, 0);
    if (sendCheck < 0)
    {
        perror("ERROR in sendto");
    }

    memset(responseMsg, '\0', INPUT_SIZE);
    // pockanie na odpoved od servera a vypis jeho odpovede
    int responseCheck = recv(clientSocket, responseMsg, INPUT_SIZE, 0);
    if (responseCheck < 0)
    {
        perror("ERROR in recvfrom");
    }

    printf("%s", responseMsg);

    close(clientSocket);

    exit(1);
}

/// @brief Funkcia, ktora overuje ci je zadana cast IP adresy cislo
/// @param str Pointer na cast zadanej IP adresy
/// @return uspech = 0, chyba = 1
int chcekIfSubstringIsNumber(char *str)
{
    while (*str)
    {
        if (!isdigit(*str))
        {
            return 1;
        }
        str++;
    }
    return 0;
}

/// @brief Funkcia, ktora overuje, ci ja zadana IP adresa vo validnom IPv4 formate
/// @param ipAdress zadana IP adresa
/// @return uspech = 0, chyba = 1
int checkValidIPv4Formath(char *ipAdress)
{
    if (ipAdress == NULL)
    {
        return 1;
    }

    int dotsCount = 0; //pocitadlo bodiek
    char *dividedIP; //cast ip adresy

    //oddelenie prvej casti IP adresy po bodku
    dividedIP = strtok(ipAdress, ".");

    if (dividedIP != NULL)
    {
        while (dividedIP != NULL)
        {   
            // kontrola ci oddelena cast je ciselna 
            if (chcekIfSubstringIsNumber(dividedIP))
            {
                return 1;
            }

            int number = atoi(dividedIP);

            // kontrola velkosti cisla v IP adrese
            if (number >= 0 && number <= 255)
            {
                dividedIP = strtok(NULL, ".");
                if (dividedIP != NULL)
                {
                    dotsCount++;
                }
            }
            else
            {
                return 1;
            }
        }
        
        // kontrola poctu bodiek 
        if (dotsCount != 3)
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

/// @brief Funkcia overujuca korektnost vstupnych argumentov
/// @param argc Pocet argumentov
/// @param argv Pole argumentov
void chcekCorrectProgramArgs(int argc, char *argv[])
{
    // kontrola ci je zadany pocet argumentov taky, aky ma byt
    if (argc != 7)
    {
        fprintf(stderr, "Wrong argument count! Usage: ./ipkcpc -h <host> -p <port> -m <mode>\n");
        exit(1);
    }

    bool h = false, p = false, m = false;

    // loop prechadzajuci pole argumentov, podla prepinaca kontroluje korektnost zadanych argumentov
    for (int i = 1; i < 7; i = i + 2)
    {
        if ((strcmp("-h", argv[i]) == 0) && (!h))
        {
            h = true;
            char ipAdress_help[INPUT_SIZE];
            strcpy(ipAdress_help, argv[i + 1]);
            // kontrola IP adresy
            int check_ip = checkValidIPv4Formath(ipAdress_help);
            if (check_ip != 0)
            {
                fprintf(stderr, "Wrong IPv4 formath! Usage: ./ipkcpc -h <host> -p <port> -m <mode>\n");
                exit(1);
            }
        }
        else if ((strcmp("-p", argv[i]) == 0) && (!p))
        {
            p = true;
            // kontrola portu
            int port_check = atoi(argv[i + 1]);
            if ((port_check <= 0) || (port_check > 65535))
            {
                fprintf(stderr, "Invalid port number! Usage: ./ipkcpc -h <host> -p <port> -m <mode>\n");
                exit(1);
            }
        }
        else if ((strcmp("-m", argv[i]) == 0) && (!m))
        {
            m = true;
            // kontrola modu
            if ((strcmp("tcp", argv[i + 1]) != 0) && (strcmp("udp", argv[i + 1]) != 0))
            {
                fprintf(stderr, "Invalid mode name! Usage: ./ipkcpc -h <host> -p <port> -m <mode>\n");
                exit(1);
            }
        }
        else
        {
            fprintf(stderr, "Wrong argument switch or argument specified multiple times! Usage: ./ipkcpc -h <host> -p <port> -m <mode>\n");
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    // kontrola vstupnych argumentov
    chcekCorrectProgramArgs(argc, argv);

    // inicializacia potrebnych premennych pre komunikaciu so sietou
    int portNumber = 0; // cislo portu
    int sendCheck; // kontroluje stav po odoslani spravy
    int responseCheck; // kontroluje stav po prijati spravy
    socklen_t serverLength; // dlzka servera
    char *serverHostname; // IP adresa
    struct hostent *server; 
    struct sockaddr_in serverAdress;
    char stdinInpud[INPUT_SIZE]; // buffer s inputom z stdin

    int modePosition = 0; // pozicia modu

    // priradenie premennym ich hodnoty
    for (int i = 0; i < 7; i++)
    {
        if (strcmp("-h", argv[i]) == 0)
        {
            serverHostname = argv[i + 1];
        }
        if (strcmp("-p", argv[i]) == 0)
        {
            portNumber = atoi(argv[i + 1]);
        }
        if (strcmp("-m", argv[i]) == 0)
        {
            modePosition = i + 1;
        }
    }

    // ziskanie adresy servery
    if ((server = gethostbyname(serverHostname)) == NULL)
    {
        fprintf(stderr, "ERROR: no such host as %s\n", serverHostname);
        exit(1);
    }
    
    // najdenie IP adresy
    memset((char *)&serverAdress, '\0', sizeof(serverAdress));
    serverAdress.sin_family = AF_INET;
    // inicializacia
    memcpy((char *)&serverAdress.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
    serverAdress.sin_port = htons(portNumber);

    if (strcmp("tcp", argv[modePosition]) == 0)
    {
        // vytvorenie socketu
        if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        {
            perror("ERROR: socket");
            exit(1);
        }

        // pokus o pripojenie
        if (connect(clientSocket, (const struct sockaddr *)&serverAdress, sizeof(serverAdress)) != 0)
        {
            perror("ERROR: connect");
            exit(1);
        }

        // detekcia CTRL-C
        signal(SIGINT, detect_CTRLC_in_TCP);

        while (1)
        {
            // nacitanie vstupu
            memset(stdinInpud, '\0', INPUT_SIZE);
            if (fgets(stdinInpud, INPUT_SIZE, stdin) == NULL)
            {
                fprintf(stderr, "Error while reading from stdin!\n");
                exit(1);
            }

            // poslanie spravy
            sendCheck = send(clientSocket, stdinInpud, strlen(stdinInpud), 0);
            if (sendCheck < 0)
            {
                perror("ERROR in sendto");
            }

            memset(stdinInpud, '\0', INPUT_SIZE);

            // prijatie odpovede
            responseCheck = recv(clientSocket, stdinInpud, INPUT_SIZE, 0);
            if (responseCheck < 0)
            {
                perror("ERROR in recvfrom");
            }

            // vypis odpovede
            printf("%s", stdinInpud);

            // v pripade bye koncim spojenie
            if (strcmp("BYE\n", stdinInpud) == 0)
            {
                break;
            }
        }

        // ukoncenie spojenia
        close(clientSocket);
    }
    else
    {
        // vytvorenie socketu
        if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
        {
            perror("ERROR: socket");
            exit(1);
        }

        char sendBuf[INPUT_SIZE];

        serverLength = sizeof(serverAdress);

        while (1)
        {
            // nacitanie vstupu
            memset(stdinInpud, '\0', INPUT_SIZE);
            if (fgets(stdinInpud, INPUT_SIZE, stdin) == NULL)
            {
                fprintf(stderr, "Error while reading from stdin!\n");
                exit(1);
            }

            // kontrola validnej dlzky vstupu
            if (strlen(stdinInpud) >= 256)
            {
                fprintf(stderr, "UDP supports input with 255 characters only!\n");
                exit(1);
            }

            // upravenie odosielanej spravy na korektny bitovy format 
            memset(sendBuf, '\0', INPUT_SIZE);
            sendBuf[0] = 'S';
            sendBuf[1] = '\0' + strlen(stdinInpud);
            strcat(sendBuf, stdinInpud);
            sendBuf[0] = '\0';

            // odoslanie spravy
            sendCheck = sendto(clientSocket, sendBuf, strlen(stdinInpud) + 2, 0, (struct sockaddr *)&serverAdress, serverLength);
            if (sendCheck < 0)
            {
                perror("ERROR: sendto");
            }

            // nadstavenie casu 10 sekund pre cakanie na odpoved
            struct timeval tv;
            tv.tv_sec = 10;
            tv.tv_usec = 0;

            // nastavenie timeout socketu
            if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
            {
                perror("ERROR: setsockopt");
            }

            // nacitanie odpovede
            memset(stdinInpud, '\0', INPUT_SIZE);
            responseCheck = recvfrom(clientSocket, stdinInpud, INPUT_SIZE, 0, (struct sockaddr *)&serverAdress, &serverLength);

            if (responseCheck < 0)
            {
                perror("ERROR: recvfrom");
                exit(1);
            }

            if (stdinInpud[0] != 1)
            {   
                // opcode musi byt vzdy 1 pri UDP
                fprintf(stderr, "Wrong Opcode in receive message!\n");
                exit(1);
            }

            // rozlisenie odpovede s chybou alebo s vysledkom
            if (stdinInpud[1] == 1)
            {
                printf("ERR: %s\n", stdinInpud + 3);
            }
            else if (stdinInpud[1] == 0)
            {
                printf("OK: %s\n", stdinInpud + 3);
            }
            else
            {
                fprintf(stderr, "Wrong Status Code in receive message!\n");
            }
        }
    }

    exit(0);
}