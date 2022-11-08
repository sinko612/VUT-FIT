/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
 * uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
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

#include "c206.h"

int error_flag;
int solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error() {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = TRUE;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {
	
	//inicializacia zoznamu -> inicializacia pointrov na NULL prveho, aktivneho a posledneho prvku zoznamu
	list->firstElement = NULL;
	list->activeElement = NULL;
	list->lastElement = NULL;

}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {

	// pomocna premenna, na ukladanie prveho prvku zoznamu
	DLLElementPtr helper_saver;

	while(list->firstElement != NULL){
		// ulozenie prveho prvku zoznamu do pomocnej premennej
		helper_saver = list->firstElement;
		// prvym prvkom sa stava nasledujuci prvok
		list->firstElement = list->firstElement->nextElement;
		// odstranenie prveho prvku zoznamu
		free(helper_saver);
	}

	// uvedenie zoznamu do povodneho stavu
	list->firstElement = NULL;
	list->activeElement = NULL;
	list->lastElement = NULL;

}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, int data ) {
	
	// alokacia noveho prvku zoznamu
	struct DLLElement *newElement = (struct DLLElement *)malloc(sizeof(struct DLLElement));

	// overenie, ci zlyhal malloc
	if(newElement == NULL){
		DLL_Error();
		return;
	}
	else{
		// priradenie hodnot novemu prvku
		newElement->data = data;
		newElement->previousElement = NULL;
		newElement->nextElement = list->firstElement;


		if(list->firstElement == NULL){
			// zoznam bol prazdny, takze novy prvok sa stava zaroven prvym aj poslednym
			list->lastElement = newElement;
		}
		else{
			// zoznam uz mal prvy prvok, takze prvy prvok musi ukazovat zlava na novy prvok
			list->firstElement->previousElement = newElement;
		}

		// novy prvok sa stava prvym prvkom
		list->firstElement = newElement;
	}

}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, int data ) {

	// alokacia noveho prvku zoznamu
	struct DLLElement *newElement = (struct DLLElement *)malloc(sizeof(struct DLLElement));

	// overenie, ci zlyhal malloc
	if(newElement == NULL){
		DLL_Error();
		return;
	}
	else{
		// inicializacia hodnot noveho prvka
		newElement->data = data;
		newElement->previousElement = list->lastElement;
		newElement->nextElement = NULL;

		if(list->lastElement == NULL){
			// zoznam bol prazdny, takze novy prvok sa stava zaroven prvym aj poslednym
			list->firstElement = newElement;
		}
		else{
			// zoznam uz mal posledny prvok, takze posledny prvok musi ukazovat sprava na novy prvok
			list->lastElement->nextElement = newElement;
		}

		// novy prvok sa stava poslednym 
		list->lastElement = newElement;
	}

}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {

	list->activeElement = list->firstElement;

}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {

	list->activeElement = list->lastElement;

}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, int *dataPtr ) {

	// kontrola, ci je zoznam prazdny
	if(list->firstElement == NULL){
		DLL_Error();
		return;
	}
	else{
		// predanie hodnoty prveho prvku pomocou pointra
		*dataPtr = list->firstElement->data;
	}

}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, int *dataPtr ) {
	
	// kontrola, ci je zoznam prazdny
	if(list->firstElement == NULL){
		DLL_Error();
		return;
	}
	else{
		// predanie hodnoty posledneho prvku pomocou pointra
		*dataPtr = list->lastElement->data;
	}

}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {

	// overenie ci je zoznam prazdny
	if(list->firstElement != NULL){
		
		// overenie aktivity zoznamu
		if(list->activeElement == list->firstElement){
			// ak bol aktivny prvy prvok zoznamu, tak sa aktivita zoznamu straca
			list->activeElement = NULL;
		}

		// pomocona premenna sluziaca na ukladanie prveho prvku
		DLLElementPtr helper_saver = list->firstElement;

		if(list->firstElement == list->lastElement){
			// ak zoznam obsahoval iba jeden prvok, tak mazem ten konkretny prvok, cize prvy aj posledny prvok su NULL
			list->firstElement = NULL;
			list->lastElement = NULL;
		}
		else{
			// prvym prvkom zoznamu sa stava nasledujuci prvok
			list->firstElement = list->firstElement->nextElement;
			// lavy pointer prveho prvku musi byt inicializovany na NULL
			list->firstElement->previousElement = NULL;
		}

		// vymazanie prveho prvku zoznamu, co sme chceli vymazat
		free(helper_saver);
	}

}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {

	// overenie, ci je zoznam prazdny
	if(list->firstElement != NULL){

		if(list->activeElement == list->lastElement){
			// overenie, ci je posledny prvok aktivny, ak ano, tak sa aktivita zoznamu straca
			list->activeElement = NULL;
		}

		// pomocona premenna sluziaca na ukladanie posledneho prvku
		DLLElementPtr helper_saver = list->lastElement;

		if(list->firstElement == list->lastElement){
			// ak zoznam obsahoval iba jeden prvok, tak mazem ten konkretny prvok, cize prvy aj posledny prvok su NULL
			list->firstElement = NULL;
			list->lastElement = NULL;
		}
		else{
			// poslednym prvkom zoznamu sa stava predchadzajuci prvok
			list->lastElement = list->lastElement->previousElement;
			// pravy pointer posledneho prvku musi ukazovat na NULL, aby to indikovalo koniec zoznamu
			list->lastElement->nextElement = NULL;
		}

		// vymazanie posledneho prvku
		free(helper_saver);
	}

}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {
	
	// overnie aktivity zoznamu a overenie, ci posledny prvok nie je aktivny
	if((list->activeElement != NULL) && (list->activeElement->nextElement != NULL)){
		// pomocona premenna sluziaca na ukladanie vymazavaneho prvku
		DLLElementPtr helper_saver = list->activeElement->nextElement;

		// prvku za aktivnym prvkom inicializujem pravy pointer na prvok nasledujuci za vymazavanym prvok
		list->activeElement->nextElement = helper_saver->nextElement;

		if(helper_saver == list->lastElement){
			// ak sa rusi posledny prvok, tak sa posledny prvok stava aktivnym
			list->lastElement = list->activeElement;
		}
		else{
			// korekcia pointra prvku za zrusenym prvkom => musi ukazovat na aktivny prvok
			helper_saver->nextElement->previousElement = list->activeElement;
		}

		// vymazanie vymazavaneho prvku
		free(helper_saver);
	}

}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {

	// kontrola aktivity zoznamu a kontrola, ci prvy prvok nie je aktivny
	if((list->activeElement != NULL) && (list->activeElement->previousElement != NULL)){
		// pomocona premenna sluziaca na ukladanie vymazavaneho prvku
		DLLElementPtr helper_saver = list->activeElement->previousElement;

		// inicializacia pointra prvku pred aktivnym prvkom tak, aby ukazoval na prvok co nasleduje za mazanym prvokm
		list->activeElement->previousElement = helper_saver->previousElement;

		if(helper_saver == list->firstElement){
			// ak je mazany prvy prvok, tak sa prvy stava aktivnym
			list->firstElement = list->activeElement;
		}
		else{
			// prvok pred vymazavanym ukazuje na aktivny prvok
			helper_saver->previousElement->nextElement = list->activeElement;
		}

		// vymazanie vymazavaneho prvku
		free(helper_saver);
	}

}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, int data ) {

	// overenie aktivity zoznamu
	if(list->activeElement != NULL){
		// alokacia noveho prvku
		struct DLLElement *newElement = (struct DLLElement *) malloc(sizeof(struct DLLElement));

		// overenie, ci nezlyhal malloc
		if(newElement == NULL){
			DLL_Error();
			return;
		}

		// inicializacia hodnot noveho prvku
		newElement->data = data;
		newElement->previousElement = list->activeElement;
		newElement->nextElement = list->activeElement->nextElement;

		// pravy pointer aktivneho prvku musi ukazovat na novy prvok
		list->activeElement->nextElement = newElement;

		if(list->activeElement == list->lastElement){
			// ak bol aktivny prvok aktivny, tak sa novy prvok stava poslednym
			list->lastElement = newElement;
		}
		else{
			// pravy naslednik noveho prvku musi zlava ukazovat na novy prvok
			newElement->nextElement->previousElement = newElement;
		}
	}

}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, int data ) {

	// overenie aktivity zoznamu
	if(list->activeElement != NULL){
		// alokacia noveho prvku
		struct DLLElement *newElement = (struct DLLElement *) malloc(sizeof(struct DLLElement));
		
		// overenie, ci nezlyhal malloc
		if(newElement == NULL){
			DLL_Error();
			return;
		}

		// inicializacia hodnot noveho prvku
		newElement->data = data;
		newElement->previousElement = list->activeElement->previousElement;
		newElement->nextElement = list->activeElement;

		// lavy pointer aktivneho prvku musi ukazovat na novy prvok
		list->activeElement->previousElement = newElement;

		if (list->activeElement == list->firstElement){
			// ak bol aktivny prvok prvym prvok, tak sa novy prvok stava prvym prvkom
			list->firstElement = newElement;
		}
		else{
			// pravy pointer predchodcu noveho prvku musi ukazovat na novy prvok
			newElement->previousElement->nextElement = newElement;
		}
	}

}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, int *dataPtr ) {

	// overenie aktivity zoznamu
	if(list->activeElement == NULL){
		DLL_Error();
		return;
	}
	else{
		// predanie dat pomocou pointra
		*dataPtr = list->activeElement->data;
	}

}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, int data ) {

	// overnie aktivity zoznamu
	if(list->activeElement != NULL){
		// priradenie novej hodnoty aktivnemu prvku
		list->activeElement->data = data;
	}

}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {

	// overnie aktivity zoznamu
	if(list->activeElement != NULL){
		// posun aktivity na dalsi prvok
		list->activeElement = list->activeElement->nextElement;
	}

}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {

	// overnie aktivity zoznamu
	if(list->activeElement != NULL){
		// posun aktivity na predchadzajuci prvok 
		list->activeElement = list->activeElement->previousElement;
	}

}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
int DLL_IsActive( DLList *list ) {

	return (list->activeElement != NULL);

}

/* Konec c206.c */
