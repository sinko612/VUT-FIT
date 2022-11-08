/* c201.c **********************************************************************
** Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                              Úpravy: Kamil Jeřábek, září 2020
**                                                    Daniel Dolejška, září 2021
**                                                    Daniel Dolejška, září 2022
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu List.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ List:
**
**      List_Dispose ....... zrušení všech prvků seznamu,
**      List_Init .......... inicializace seznamu před prvním použitím,
**      List_InsertFirst ... vložení prvku na začátek seznamu,
**      List_First ......... nastavení aktivity na první prvek,
**      List_GetFirst ...... vrací hodnotu prvního prvku,
**      List_DeleteFirst ... zruší první prvek seznamu,
**      List_DeleteAfter ... ruší prvek za aktivním prvkem,
**      List_InsertAfter ... vloží nový prvek za aktivní prvek seznamu,
**      List_GetValue ...... vrací hodnotu aktivního prvku,
**      List_SetValue ...... přepíše obsah aktivního prvku novou hodnotou,
**      List_Next .......... posune aktivitu na další prvek seznamu,
**      List_IsActive ...... zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

/* vypracovala: Simona Jánošíková (xjanos19), október 2022 */

#include "c201.h"

#include <stdio.h> // printf
#include <stdlib.h> // malloc, free

int error_flag;
int solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě. Nastaví error_flag na logickou 1.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void List_Error() {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = TRUE;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným
 * seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
 * že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu jednosměrně vázaného seznamu
 */
void List_Init( List *list ) {
	
	/* inicializacia zoznamu priradenim hodnoty NULL prvemu a aktivnemu prvku
	   zoznamu, co znaci ze je zoznam prazdny */
	list->firstElement = NULL;
	list->activeElement = NULL;

}

/**
 * Zruší všechny prvky seznamu list a uvede seznam list do stavu, v jakém se nacházel
 * po inicializaci. Veškerá paměť používaná prvky seznamu list bude korektně
 * uvolněna voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 **/
void List_Dispose( List *list ) {
	
	// pomocna premenna, ukladam do nej prvy prvok zoznamu
	ListElementPtr helper_saver;

	while(list->firstElement != NULL){
		// ulozenie prveho prvku do pomocnej premennej
		helper_saver = list->firstElement;
		// prvemu prvku zoznamu priradim jeho nasledovnika 
		list->firstElement = list->firstElement->nextElement;
		// vymazanie prveho prvku zoznamu
		free(helper_saver);
	}

	//vratenie zoznamu do povodneho stavu po inicializacii
	list->firstElement = NULL;
	list->activeElement = NULL;

}

/**
 * Vloží prvek s hodnotou data na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void List_InsertFirst( List *list, int data ) {
	
	// inicializacia noveho prvku
	struct ListElement *newElement = (struct ListElement*) malloc(sizeof(struct ListElement));

	if(newElement == NULL){
		// overenie ci zlyhal malloc, ak ano tak error funkcia
		List_Error();
		return;
	}
	else{
		newElement->data = data;
		// nasledujucim prvkom noveho prvku sa stane prvy prvok
		newElement->nextElement = list->firstElement;
		// prvym prvok sa stane novy prvok
		list->firstElement = newElement;
	}

}

/**
 * Nastaví aktivitu seznamu list na jeho první prvek.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_First( List *list ) {
	
	// activnym prvkom sa stane prvy prvok
	list->activeElement = list->firstElement;

}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void List_GetFirst( List *list, int *dataPtr ) {
	
	if(list->firstElement == NULL){
		List_Error();
		return;
	}
	else{
		// predanie hodnoty prveho prvku pointru dataPtr
		*dataPtr = list->firstElement->data;
	}

}

/**
 * Zruší první prvek seznamu list a uvolní jím používanou paměť.
 * Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_DeleteFirst( List *list ) {
	
	// overenie ci je zoznam prazdny
	if(list->firstElement != NULL){
		// ulozenie prveho prvku zoznamu do pomocnej premennej
		ListElementPtr helper_saver = list->firstElement;
		// prvym prvok sa stane nasledujuci
		list->firstElement = list->firstElement->nextElement;
		// ak bol mazany prvok aktivny, aktiva zoznamu sa straca
		if (helper_saver == list->activeElement){
			list->activeElement = NULL;
		}
		// vymazanie prveho prvku
		free(helper_saver);
	}	

}

/**
 * Zruší prvek seznamu list za aktivním prvkem a uvolní jím používanou paměť.
 * Pokud není seznam list aktivní nebo pokud je aktivní poslední prvek seznamu list,
 * nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_DeleteAfter( List *list ) {

	ListElementPtr helper_nextElem;
	// overenie aktivity zoznamu
	if(list->activeElement != NULL){
		// overenie ci je aktivny prvok aktivny
		if(list->activeElement->nextElement != NULL){
			// ulozenie prvku co nasleduje po aktivnom
			helper_nextElem = list->activeElement->nextElement;
			// nadstavenie pointru aktivnemu prvku na naslednika nasledujuceho prvku
			list->activeElement->nextElement = list->activeElement->nextElement->nextElement;
			// vymazanie prvku co nasleduje po aktivnom
			free(helper_nextElem);
		}
	}

}

/**
 * Vloží prvek s hodnotou data za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje!
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * zavolá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void List_InsertAfter( List *list, int data ) {

	// overenie aktivity zoznamu
	if(list->activeElement != NULL){
		// alokacia noveho prvku
		struct ListElement *newElement = (struct ListElement*) malloc(sizeof(struct ListElement));
		// overenie uspesnosti mallocu
		if(newElement == NULL){
			List_Error();
			return;
		}
		else{
			// priradenie dat novemu prvku
			newElement->data = data;
			// priradenie nasledovnika novemu prvku
			newElement->nextElement = list->activeElement->nextElement;
			// vlozenie neveho prvku za aktivny prvok zoznamu
			list->activeElement->nextElement = newElement;
		}
	}

}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam není aktivní, zavolá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void List_GetValue( List *list, int *dataPtr ) {

	//overenie aktivity zoznamu
	if(list->activeElement == NULL){
		List_Error();
		return;
	}
	else{
		// predanie dat prostrednictvom pointra
		*dataPtr = list->activeElement->data;
	}

}

/**
 * Přepíše data aktivního prvku seznamu list hodnotou data.
 * Pokud seznam list není aktivní, nedělá nic!
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void List_SetValue( List *list, int data ) {

	// overenie aktivity zoznamu
	if(list->activeElement != NULL){
		// vymena dat aktivneho prvku
		list->activeElement->data = data;
	}

}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
 * Pokud není předaný seznam list aktivní, nedělá funkce nic.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_Next( List *list ) {

	// overenie aktivity zoznamu
	if(list->activeElement != NULL){
		list->activeElement = list->activeElement->nextElement;
	}

}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Tuto funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
int List_IsActive( List *list ) {
	
	return (list->activeElement != NULL);

}

/* Konec c201.c */
