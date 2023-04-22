/**
 * Fakulta informačných technologií VUT
 * IPK projekt 2
 * variant: ZETA - Network sniffer
 * autor: Simona Jánošíková (xjanos19)
 */

#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <pcap.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// premenná, ktorá nadvazuje spojenie so sieťou a packetmi
pcap_t *connectToInterface;

/**
 * @brief štruktúra, ktorá na základe zadaných argumentov zadá flagy a hodnoty premenným
 */
struct arguments
{
    char *interface;
    bool tcp;
    bool udp;
    int port;
    bool icmp4;
    bool icmp6;
    bool arp;
    bool ndp;
    bool igmp;
    bool mld;
    int n;
};

/**
 *  @brief Funkcia, ktorá po detekcíí CTRL-C korektne ukončí program, a to tak, že uzavrie spojenie
 */
void exitWhenCTRLC()
{
    pcap_close(connectToInterface);
    exit(0);
}

/**
 * @brief Funkcia, ktorá vypíše korektne MAC adresu v hexadecimálnom formáte
 *
 * @param mac MAC adresa spojenia
 * @param len Počet dvojíc MAC adresy
 *
 * @source https://stackoverflow.com/questions/7939238/dereferencing-pointer-to-incomplete-type-with-struct-ip-and-also-with-struct-iph
 */

void printMAC(unsigned char *mac, int len)
{
    while (len--)
    {
        if (len == 0)
            printf("%02X", *mac);
        else
            printf("%02X:", *mac);
        mac++;
    }
}

/**
 * @brief Funkcia, ktorá vypíše čas v RFC 3339 formáte
 *
 * @param header Header packetov, z ktorého získame čas
 */
void printTimestamp(struct pcap_pkthdr header)
{

    struct tm *timeNow = localtime(&header.ts.tv_sec);

    char timeBuf[100];
    size_t len = strftime(timeBuf, sizeof timeBuf - 1, "%FT%T%z", timeNow);

    // uloženie milisekúnd a konverzia na string
    int milisec = (header.ts.tv_usec) / 1000;
    char milisecBuf[10] = {'\0'};
    sprintf(milisecBuf, ".%03d", milisec);

    // uloženie časovej zóny
    char zoneBuf[] = {timeBuf[len - 5], timeBuf[len - 4], timeBuf[len - 3], timeBuf[len - 2], timeBuf[len - 1], '\0'};

    sprintf(timeBuf + len - 5, "%s", milisecBuf); // pridanie milisekúnd do času

    sprintf(timeBuf + len - 1, "%s", zoneBuf); // pridanie časovej zóny do času

    int lenTimeBuf = strlen(timeBuf);

    if (lenTimeBuf > 1)
    {
        // pridanie dvojbodky do časovej zóny
        char minute[] = {timeBuf[lenTimeBuf - 2], timeBuf[lenTimeBuf - 1], '\0'};
        sprintf(timeBuf + lenTimeBuf - 2, ":%s", minute);
    }

    printf("timestamp: %s\n", timeBuf);
}

/**
 * @brief Funkcia vypisujúca dáta packetu v korektnom formáte
 *
 * @param packetsHeader Hlavička packetu
 * @param packet Konkrétny packet
 *
 * inšpirácia: https://www.opensourceforu.com/2011/02/capturing-packets-c-program-libpcap/
 */
void printPcaketData(struct pcap_pkthdr packetsHeader, const u_char *packet)
{
    unsigned char symbol;
    int o = 0;

    for (int i = 0; i < packetsHeader.caplen; i++)
    {
        if (((i % 16) == 0))
        {
            printf("0x%04x  ", o);
        }

        symbol = packet[i];
        printf("%02x ", symbol);

        if (((i % 16) == 15) || (i == packetsHeader.caplen - 1))
        {
            // ak sa jedná o kratší string, tak sa dopisujú medzery pre zarovnanie
            for (int j = 0; j < 15 - (i % 16); j++)
            {
                printf("   ");
            }

            printf(" ");

            for (int k = (i - (i % 16)); k <= i; k++)
            {
                symbol = packet[k];
                // výpis ASCII symbolu
                if ((symbol < 32) || (symbol > 126))
                {
                    printf(".");
                }
                else
                {
                    printf("%c", symbol);
                }
            }
            printf("\n");
            o = o + 16;
        }
    }
}

/**
 * @brief Funkcia zachytávajúca packety a vypisujúca MAC adresy, Ip adresy a porty
 *
 * @param args Argumenty programu
 */
void catchingPackets(struct arguments args)
{
    struct pcap_pkthdr packetsHeader; // štruktúra definujúca header, ktorý je daný pred každý packet
    const u_char *packet;

    // detekcia CTRL-C signálu, v prípade zadania ctrlC sa program korektne ukončí
    signal(SIGINT, exitWhenCTRLC);

    for (int i = 0; i < args.n; i++)
    {
        packet = pcap_next(connectToInterface, &packetsHeader); // získanie ďalšieho packetu

        printTimestamp(packetsHeader);

        struct ether_header *ethernetHeader = (struct ether_header *)packet; // získanie destination a source MAC adresy
        // výpis MAC adries
        printf("src MAC: ");
        printMAC(ethernetHeader->ether_shost, 6);
        printf("\ndst MAC: ");
        printMAC(ethernetHeader->ether_dhost, 6);
        printf("\nframe length: %d bytes\n", packetsHeader.len);

        // detekcia, o akú sieť sa jedná a následný výpis IP adries a prípadne, portov
        if (ntohs(ethernetHeader->ether_type) == ETHERTYPE_IP) // IPv4 spojenie, výpis IP adries v prípade ICMPv4, IGMP, TCP a UDP
        {
            struct ip *ipHeader = (struct ip *)(packet + 14);

            char ipSrc[100] = {'\0'};
            inet_ntop(AF_INET, &ipHeader->ip_src.s_addr, ipSrc, 100);
            char ipDst[100] = {'\0'};
            inet_ntop(AF_INET, &ipHeader->ip_dst.s_addr, ipDst, 100);
            // výpis source a destination IP adresy
            printf("src IP: %s\n", ipSrc);
            printf("dst IP: %s\n", ipDst);

            if (ipHeader->ip_p == 6)
            {
                // vypis portov pri TCP
                struct tcphdr *tcpHeader = (struct tcphdr *)(packet + 14 + ipHeader->ip_hl * 4);
                printf("src port: %d\n", ntohs(tcpHeader->th_sport));
                printf("dst port: %d\n", ntohs(tcpHeader->th_dport));
            }
            else if (ipHeader->ip_p == 17)
            {
                // vypis portov pri UDP
                struct udphdr *udpHeader = (struct udphdr *)(packet + 14 + ipHeader->ip_hl * 4);
                printf("src port: %d\n", ntohs(udpHeader->uh_sport));
                printf("dst port: %d\n", ntohs(udpHeader->uh_dport));
            }
        }
        else if (ntohs(ethernetHeader->ether_type) == ETHERTYPE_IPV6) // IPv6 spojenie, výpis IP adries v prípade ICMPv6, MLD, NDP, TCP a UDP
        {
            struct ip6_hdr *ipv6Header = (struct ip6_hdr *)(packet + 14);
            char ipSrc[100] = {'\0'};
            inet_ntop(AF_INET6, &ipv6Header->ip6_src, ipSrc, 100);
            char ipDst[100] = {'\0'};
            inet_ntop(AF_INET6, &ipv6Header->ip6_dst, ipDst, 100);

            printf("src IP: %s\n", ipSrc);
            printf("dst IP: %s\n", ipDst);

            if (ipv6Header->ip6_ctlun.ip6_un1.ip6_un1_nxt == 6)
            {
                // vypis portov pri TCP
                struct tcphdr *tcpHeader = (struct tcphdr *)(packet + 14 + 40);
                printf("src port: %d\n", ntohs(tcpHeader->th_sport));
                printf("dst port: %d\n", ntohs(tcpHeader->th_dport));
            }
            else if (ipv6Header->ip6_ctlun.ip6_un1.ip6_un1_nxt == 17)
            {
                // vypis portov pri UDP
                struct udphdr *udpHeader = (struct udphdr *)(packet + 14 + 40);
                printf("src port: %d\n", ntohs(udpHeader->uh_sport));
                printf("dst port: %d\n", ntohs(udpHeader->uh_dport));
            }
        }
        else if (ntohs(ethernetHeader->ether_type) == ETHERTYPE_ARP) // ARP spojenie
        {
            struct ether_arp *arpHeader = (struct ether_arp *)(packet + 14);

            char ipSrc[100] = {'\0'};
            inet_ntop(AF_INET, &arpHeader->arp_spa, ipSrc, 100);
            char ipDst[100] = {'\0'};
            inet_ntop(AF_INET, &arpHeader->arp_tpa, ipDst, 100);

            printf("src IP: %s\n", ipSrc);
            printf("dst IP: %s\n", ipDst);
        }

        printf("\n");

        printPcaketData(packetsHeader, packet);
    }
}

/**
 * @brief funkcia, ktorá podľa zadaných argumentov programu vytvorí string podľa ktorého sa budú filtrovať packety
 *
 * @param arg Vstupné argumenty programu
 * @param filter String, String na základe ktorého sa budú filtrovať packety
 */
void createStringForFilter(struct arguments arg, char filterString[])
{
    char portNum[20] = {'\0'};
    // vytvorenie stringu pre port
    if (arg.port != -1)
    {
        sprintf(portNum, "port %d", arg.port);
    }
    else
    {
        sprintf(portNum, "portrange 0-65535");
    }

    if (arg.tcp)
    {
        // vloženie do filterStringu TCP parameter
        sprintf(filterString, "(tcp and %s)", portNum);
    }

    if (arg.udp)
    {
        // vloženie do filterStringu UDP parameter
        if (filterString[0] == '\0')
        {
            sprintf(filterString, "(udp and %s)", portNum);
        }
        else
        {
            char pomBuf[200] = {'\0'};
            sprintf(pomBuf, " or (udp and %s)", portNum);
            strcat(filterString, pomBuf);
        }
    }

    if (arg.icmp4)
    {
        // vloženie do filterStringu ICMPv4 parameter
        if (filterString[0] == '\0')
        {
            sprintf(filterString, "(icmp)");
        }
        else
        {
            char pomBuf[200] = {'\0'};
            sprintf(pomBuf, " or (icmp)");
            strcat(filterString, pomBuf);
        }
    }

    if (arg.icmp6)
    {
        // vloženie do filterStringu ICMPv6 parameter
        if (filterString[0] == '\0')
        {
            sprintf(filterString, "(icmp6)");
        }
        else
        {
            char pomBuf[200] = {'\0'};
            sprintf(pomBuf, " or (icmp6)");
            strcat(filterString, pomBuf);
        }
    }

    if (arg.arp)
    {
        // vloženie do filterStringu ARP parameter
        if (filterString[0] == '\0')
        {
            sprintf(filterString, "(arp)");
        }
        else
        {
            char pomBuf[200] = {'\0'};
            sprintf(pomBuf, " or (arp)");
            strcat(filterString, pomBuf);
        }
    }

    if (arg.ndp)
    {
        // vloženie do filterStringu NDP parameter
        if (filterString[0] == '\0')
        {
            sprintf(filterString, "((icmp6[icmp6type] >= 133 and icmp6[icmp6type] <= 137) or (icmp6[icmp6type] == 139))");
        }
        else
        {
            char pomBuf[200] = {'\0'};
            sprintf(pomBuf, " or ((icmp6[icmp6type] >= 133 and icmp6[icmp6type] <= 137) or (icmp6[icmp6type] == 139))");
            strcat(filterString, pomBuf);
        }
    }

    if (arg.igmp)
    {
        // vloženie do filterStringu IGMP parameter
        if (filterString[0] == '\0')
        {
            sprintf(filterString, "(igmp)");
        }
        else
        {
            char pomBuf[200] = {'\0'};
            sprintf(pomBuf, "or (igmp)");
            strcat(filterString, pomBuf);
        }
    }

    if (arg.mld)
    {
        // vloženie do filterStringu MLD parameter
        if (filterString[0] == '\0')
        {
            sprintf(filterString, "(icmp6[icmp6type] == 130 or icmp6[icmp6type] == 131 or icmp6[icmp6type] == 132) or (icmp6 and ip6[40] == 143)");
        }
        else
        {
            char pomBuf[200] = {'\0'};
            sprintf(pomBuf, " or (icmp6[icmp6type] == 130 or icmp6[icmp6type] == 131 or icmp6[icmp6type] == 132) or (icmp6 and ip6[40] == 143)");
            strcat(filterString, pomBuf);
        }
    }

    if (!arg.tcp && !arg.udp && !arg.icmp4 && !arg.icmp4 && !arg.arp && !arg.ndp && !arg.igmp && !arg.mld)
    {
        // vloženie do filterStringu všetky parametre, pretože neboli zadané žiadne argumenty
        sprintf(filterString, "(tcp and %s) or (udp and %s) or (icmp) or (icmp6) or (arp) or ((icmp6[icmp6type] >= 133 and icmp6[icmp6type] <= 137) or (icmp6[icmp6type] == 139)) or (igmp) or (icmp6[icmp6type] == 130 or icmp6[icmp6type] == 131 or icmp6[icmp6type] == 132) or (icmp6 and ip6[40] == 143)", portNum, portNum);
    }
}

/**
 * @brief Funkcia nadväzujúca spojenie s interfacom a nadstavujúca filter packetov
 *
 * @param args Argumenty programu
 */
void startConnectionWithPackets(struct arguments args)
{
    char errorBuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 maskP, netP; // maska a sieť

    // určenie čísla siete IPv4 a masky zadaného interfacu
    if (pcap_lookupnet(args.interface, &netP, &maskP, errorBuf) == -1)
    {
        fprintf(stderr, "Error in pcap_lookupnet function: %s\n", errorBuf);
        exit(1);
    }

    // otvorenie spojania s interface-om
    if ((connectToInterface = pcap_open_live(args.interface, BUFSIZ, 1, 1, errorBuf)) == NULL)
    {
        fprintf(stderr, "Error in pcap_open_live due to: %s\n", errorBuf);
        exit(1);
    }

    if (pcap_datalink(connectToInterface) != DLT_EN10MB)
    {
        fprintf(stderr, "Not supporting Ethernet headers in interface %s!n", args.interface);
        exit(1);
    }

    // vytvorenie filter-stringu
    char filterString[200] = {'\0'};
    createStringForFilter(args, filterString);

    struct bpf_program progFiltered;

    // kompilácia filter-stringu na filtrovací program
    if (pcap_compile(connectToInterface, &progFiltered, filterString, 1, netP) == -1)
    {
        fprintf(stderr, "Error in pcap_compile function!\n");
        exit(1);
    }

    // špecifikácia filtrovacieho programu
    if (pcap_setfilter(connectToInterface, &progFiltered) == -1)
    {
        fprintf(stderr, "Error in pcap_setfilter function!\n");
        exit(1);
    }
}

/**
 * @brief Funkcia, ktorá v prípade nezadaného interface, vypíše všetky dostupné interfaces zariadenia
 */
void printInterfaces()
{

    char errorBuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *allI;
    pcap_if_t *interface;

    signal(SIGINT, exitWhenCTRLC);

    if (pcap_findalldevs(&allI, errorBuf) == -1)
    {
        fprintf(stderr, "Error in PCAP function findalldevs!\n");
        exit(1);
    }

    if (allI == NULL)
    {
        fprintf(stderr, "No available interfaces for this device!\n");
        exit(1);
    }

    interface = allI;
    int order = 1;

    printf("\nList of all available interfaces: \n");
    while (interface != NULL)
    {
        printf("    %d. %s\n", order, interface->name);
        interface = interface->next;
        order++;
    }

    pcap_freealldevs(allI);

    exit(0);
}
/**
 * @brief Funkcia, ktorá rozparsuje argumenty programu a do štruktúry arguments zapíše podľa zadaných argumentov flagy(true/false)
 *
 * @param progArgs Argumenty programu
 * @param argc Počet argumentov programu
 * @param argv Pole argumentov programu
 */
void parseArguments(struct arguments *progArgs, int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if ((strcmp("-i", argv[i]) == 0) || (strcmp("--interface", argv[i]) == 0))
        {
            // zadaný interface
            i++;
            if ((i < argc) && (argv[i][0] != '-'))
            {
                progArgs->interface = argv[i];
            }
            else
            {
                i--;
            }
        }
        else if (((strcmp("-t", argv[i]) == 0) || (strcmp("--tcp", argv[i]) == 0)) && !progArgs->tcp)
        {
            // zadaný TCP flag
            progArgs->tcp = true;
        }
        else if (((strcmp("-u", argv[i]) == 0) || (strcmp("--udp", argv[i]) == 0)) && !progArgs->udp)
        {
            // zadaný UDP flag
            progArgs->udp = true;
        }
        else if (strcmp("-p", argv[i]) == 0)
        {
            // zadaný port
            i++;
            if (i < argc)
            {
                int portNum = atoi(argv[i]);
                if ((portNum <= 0) || (portNum > 65535))
                {
                    fprintf(stderr, "Invalid port number!");
                    exit(1);
                }
                else
                {
                    progArgs->port = portNum;
                }
            }
            else
            {
                fprintf(stderr, "Port number is not specified!");
                exit(1);
            }
        }
        else if ((strcmp("--icmp4", argv[i]) == 0) && !progArgs->icmp4)
        {
            // zadaný ICMPv4 flag
            progArgs->icmp4 = true;
        }
        else if ((strcmp("--icmp6", argv[i]) == 0) && !progArgs->icmp6)
        {
            // zadaný ICMPv6 flag
            progArgs->icmp6 = true;
        }
        else if ((strcmp("--arp", argv[i]) == 0) && !progArgs->arp)
        {
            // zadaný ARP flag
            progArgs->arp = true;
        }
        else if ((strcmp("--ndp", argv[i]) == 0) && !progArgs->ndp)
        {
            // zadaný NDP flag
            progArgs->ndp = true;
        }
        else if ((strcmp("--igmp", argv[i]) == 0) && !progArgs->igmp)
        {
            // zadaný IGMP flag
            progArgs->igmp = true;
        }
        else if ((strcmp("--mld", argv[i]) == 0) && !progArgs->mld)
        {
            // zadaný MLD flag
            progArgs->icmp4 = true;
        }
        else if (strcmp("-n", argv[i]) == 0)
        {
            // zadané n, počet packetov na výpis
            i++;
            if (i < argc)
            {

                int num = atoi(argv[i]);
                if (num < 1)
                {
                    fprintf(stderr, "Invalid n number!");
                    exit(1);
                }
                else
                {
                    progArgs->n = num;
                }
            }
            else
            {
                fprintf(stderr, "Port number is not specified!");
                exit(1);
            }
        }
        else
        {
            fprintf(stderr, "Wrong argument, usage: sudo ./ipk-sniffer [-i interface | --interface interface] {-p port [--tcp|-t] [--udp|-u]} [--arp] [--icmp4] [--icmp6] [--igmp] [--mld] {-n num}");
            exit(1);
        }
    }
}

/**
 * @brief Hlavná funkcia programu
 *
 * @param argc Počet zadaných argumentov programu
 * @param argv Pole zadaných argumentov programu
 * @return 0 v prípade úspšného ukončenia, 1 v prípade chyby
 */
int main(int argc, char *argv[])
{
    struct arguments args = {NULL, false, false, -1, false, false, false, false, false, false, 1};
    parseArguments(&args, argc, argv);

    if (args.interface == NULL)
    {
        printInterfaces();
    }

    // nadviazanie spojenia s rozhraním
    startConnectionWithPackets(args);

    // zachytávanie packetov
    catchingPackets(args);

    // ukončenie spojenia
    pcap_close(connectToInterface);

    exit(0);
}