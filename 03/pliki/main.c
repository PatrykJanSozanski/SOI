#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include "list.c"
#include <string.h>

#define STANDARD 0
#define VIP 1
#define ADMIN 2

#define MAX_WIAD 4

// Baza wiadomosci.
#define SZAKAZANE "SOI"
#define S1 "PCHNAC"
#define S2 "W"
#define S3 "TE"
#define S4 "LODZ"
#define S5 "JEZA"
#define S6 "LUB"
#define S7 "OSM"
#define S8 "SKRZYN"
#define S9 "FIG"

char baza [10][10] =
{
    {SZAKAZANE},
    {S1},
    {S2},
    {S3},
    {S4},
    {S5},
    {S6},
    {S7},
    {S8},
    {S9}
};

// Lista wiadomości.
node_t* wiadomosci;

// Wiadomość wystawiona w danej chwili przez dystrybutora do przeczytania.
char* aktualna_wiadomosc;

// Liczba wiadomosci.
int lb_wiad;


sem_t sem_mutex; // Semafor pilnujący dostępu do bufora wiadomości.
sem_t sem_pusty; // Semafor informujący o tym, że lista wiadomości jest pusta.
sem_t sem_pelny; // Semafor informujący o tym, że lista wiadomości jest pełna.

sem_t *sem_przeczytane; // Semafor informujacy o tym, czy nowa wiadomosc została już przeczytana.
sem_t *sem_dostepne; // Semafor informujący o dostępnosci nowej wiadomości.



int *typ; // {STANDARD, VIP, ADMIN}

int lb_uzyt; // liczba użytników.

// Nadawanie nazw uzytkownikom.
char* nazwa(int id){
    
    char numer[10];
    sprintf(numer, "%d", id);
    char* nazwa = malloc(sizeof(char)*20);
    
    if(typ[id] == STANDARD)
    {
        strcpy(nazwa, "UZYTKOWNIK ");
        strcat(nazwa, numer);
    }
    else if(typ[id] == VIP)
    {
        strcpy(nazwa, "UZYTKOWNIK VIP ");
        strcat(nazwa, numer);
    }
    else
    {
        strcpy(nazwa, "ADMIN ");
        strcat(nazwa, numer);
    }

    return nazwa;
}

// Czyszczenie kolejki.
void wyczyscKolejke(){

    sem_wait(&sem_mutex);
    sem_init(&sem_pusty, 0, 0);
    sem_init(&sem_pelny, 0, MAX_WIAD);

    while(wiadomosci != NULL)
    {
        pop(&wiadomosci);
    }

    sem_post(&sem_mutex);

    aktualna_wiadomosc = "# ADMIN USUNAL WIADOMOSC";
}

// KONSUMENT
/*
 Każdy użytkownik ma dwie opcje (wątki):
 - słuchania - odczytywanie nowej wiadomości od dystrybutora.
 - wysyłania - wysyłanie nowej wiadomości do dystrybutora.
 */
void* sluchacz(void* arg)
{
    int id = *((int *) arg);
    printf("%s SLUCHACZ: ROZPOCZAL CZYTANIE WIADOMOSCI \n", nazwa(id));

    while(1)
    {

        sem_wait(&sem_dostepne[id]);

        printf("%s SLUCHACZ: OTRZYMAL WIADOMOSC \"%s\"\n", nazwa(id), aktualna_wiadomosc);

        if(strcmp(aktualna_wiadomosc, SZAKAZANE) == 0 && typ[id] == ADMIN)
        {
            printf("KOMUNIKAT ADMINA: WYKRYTO ZAKAZANE SLOWO\n");
            wyczyscKolejke();
        }
        sem_post(&sem_przeczytane[id]);
    }
}

// Wstawianie wiadomosci do kolejki.
void dodajWiadomosc(char* wiad, int typ){


    if(typ == STANDARD) {
        if(wiadomosci == NULL) 
            wiadomosci = init(wiad);
        else push_end(wiadomosci, wiad);
    }else{
        if(wiadomosci == NULL) 
            wiadomosci = init(wiad);
        else 
            push_begin(&wiadomosci, wiad);
    }
}

struct uzytkownik {
    int indeks_u;
    char* wiadomosc_u;
};

void wyswietlKolejke(){
    printf((" ### KOLEJKA WIADOMOSCI:\n"));
    print_list(wiadomosci);
}

// PRODUCENT
/*
Każdy użytkownik ma dwie opcje (wątki):
- słuchania - odczytywanie nowej wiadomości od dystrybutora.
- wysyłania - wysyłanie nowej wiadomości do dystrybutora.
*/
void* nadawca(void* uzyt)
{
    struct uzytkownik *uzytkownicy = (struct uzytkownik *) uzyt;
    printf("%s NADAWCA: ROZPOCZAL WYSYLANIE WIADOMOSCI\n", nazwa(uzytkownicy->indeks_u));

    char* wiadomosc = uzytkownicy->wiadomosc_u;

    sem_wait(&sem_pelny);
    sem_wait(&sem_mutex);

    
    dodajWiadomosc(wiadomosc, typ[uzytkownicy->indeks_u]);
    printf("%s NADAWCA: WIADOMOSC \"%s\" WYSLANA\n", nazwa(uzytkownicy->indeks_u), uzytkownicy->wiadomosc_u);
    wyswietlKolejke();

    

    sem_post(&sem_mutex);
    sem_post(&sem_pusty);

}

/*
 Gdy wiadomość została wysłana, semafor sem_dostepne[index_użytkownika] zmienia wartość i informuje o tym użytkownika.

 Gdy użytkownik przeczyta wiadomość, powiadomi o tym listonosza semaforem sem_przeczytane[index_użytkownika].

 Listonosz wysyła wiadomość do kolejnego użytkownika.
 */

// LISTONOSZ JEST KONSUMENTEM
void* listonosz(void* arg){
    printf("LISTONOSZ: ROZPOCZAL PRACE\n");

    while(1)
    {
        sem_wait(&sem_pusty);
        sem_wait(&sem_mutex);

        char* msg = pop(&wiadomosci);

        sem_post(&sem_mutex);
        sem_post(&sem_pelny);

        aktualna_wiadomosc = msg;

        int i;
        for(i=0; i<lb_uzyt; i++)
        {
            sem_post(&sem_dostepne[i]);
            printf("LISTONOSZ: WIADOMOSCI DLA %s\n", nazwa(i));
            sem_wait(&sem_przeczytane[i]);
        }
        wyswietlKolejke();
    }
}

void dodajUzytkownika(int typ_u) {

    int i;

    // Zwiększ tablicę słuchających nowej wiadomości o 1
    sem_t *pom_sem_dostepne;
    pom_sem_dostepne = malloc(sizeof(sem_t)*(lb_uzyt + 1));
    for(i=0; i<lb_uzyt; i++) pom_sem_dostepne[i] = sem_dostepne[i];
    sem_init(&pom_sem_dostepne[lb_uzyt], 0, 0);
    sem_dostepne = pom_sem_dostepne;

    // Zwiększ tablicę odpowiadających, że przeczytali dostępną wiadomość o 1
    sem_t *pom_sem_przeczytane;
    pom_sem_przeczytane = malloc(sizeof(sem_t)*(lb_uzyt + 1));
    for(i=0; i<lb_uzyt; i++) pom_sem_przeczytane[i] = sem_przeczytane[i];
    sem_init(&pom_sem_przeczytane[lb_uzyt], 0, 0);
    sem_przeczytane = pom_sem_przeczytane;

    // Zwiększ tablicę przechowującą typ użytkownika (NORMAL, VIP, ADMIN) o 1
    int *pom_typ;
    pom_typ = malloc(sizeof(int)*(lb_uzyt + 1));
    for(i=0; i<lb_uzyt; i++) pom_typ[i] = typ[i];
    pom_typ[lb_uzyt] = typ_u;
    typ = pom_typ;

    // Zwiększ liczbę użytkowników
    lb_uzyt++;

    // Rozpocznij dla nowego użytkownika wątek słuchacza
    int *uzytkownik = malloc(sizeof(*uzytkownik));
    *uzytkownik = lb_uzyt - 1;
    pthread_t t;
    pthread_create(&t,NULL, sluchacz, uzytkownik);

}

char* losujTresc(){

    int i, zarodek;

    return baza[rand()%10];
}

void wyslijWiadomosc(int indeks_uzyt, char* wiad){

    struct uzytkownik *uzytkownik = malloc(sizeof(struct uzytkownik));
    uzytkownik->indeks_u = indeks_uzyt;
    uzytkownik->wiadomosc_u = wiad;

    pthread_t t;
    pthread_create(&t,NULL, nadawca, uzytkownik);
}

/*
 Listonosz jest konsumentem.
 Pobiera po kolei wiadomości z kolejki i rozsyła je do wszystkich użytkowników.
 */
void zacznijWysylac(){
    pthread_t t_wys;
    pthread_create(&t_wys,NULL, listonosz, NULL);
}


void testWiadomosci(){

    dodajUzytkownika(STANDARD);
    dodajUzytkownika(STANDARD);
    dodajUzytkownika(STANDARD);

    zacznijWysylac();

    wyslijWiadomosc(0, losujTresc());
    wyslijWiadomosc(0, losujTresc());
    wyslijWiadomosc(1, losujTresc());
    wyslijWiadomosc(1, losujTresc());
    wyslijWiadomosc(2, losujTresc());

    sleep(1);
    printf("\n ### WCISNIJ KLAWISZ ENTER\n")
}

void testPrzepelenieKolejki(){

    dodajUzytkownika(STANDARD);
    dodajUzytkownika(STANDARD);
    dodajUzytkownika(STANDARD);

    printf("ROZMIAR KOLEJKI: %d\n", MAX_WIAD);

    wyslijWiadomosc(1, "# 1");
    wyslijWiadomosc(1, "# 2");
    wyslijWiadomosc(1, "# 3");
    wyslijWiadomosc(1, "# 4");
    wyslijWiadomosc(1, "# 5");
    wyslijWiadomosc(1, "# 6");

    zacznijWysylac();

    sleep(1);
    printf("\n ### WCISNIJ KLAWISZ ENTER\n");
    getchar();
}

void testVIP(){

    dodajUzytkownika(STANDARD);
    dodajUzytkownika(STANDARD);
    dodajUzytkownika(VIP);

    wyslijWiadomosc(2, "# UZYTKOWNIK VIP");
    wyslijWiadomosc(2, "# UZYTKOWNIK VIP");
    wyslijWiadomosc(0, "# UZYTKOWNIK 1");
    wyslijWiadomosc(1, "# UZYTKOWNIK 2");

    zacznijWysylac();

    sleep(1);
    printf("\n ### WCISNIJ KLAWISZ ENTER\n");
    getchar();
}

void testADMIN(){

    printf("\"%s\" TO ZAKAZANE SŁOWO.\n\n", SZAKAZANE);

    dodajUzytkownika(ADMIN);
    dodajUzytkownika(STANDARD);
    dodajUzytkownika(STANDARD);

    wyslijWiadomosc(2, "# UZYTKOWNIK 1");
    wyslijWiadomosc(2, SZAKAZANE);
    wyslijWiadomosc(1, "# UZYTKOWNIK 2 WIADOMOSC A");
    wyslijWiadomosc(1, "# UZYTKOWNIK 2 WIADOMOSC B");
    wyslijWiadomosc(1, "# UZYTKOWNIK 2 WIADOMOSC C");

    zacznijWysylac();

    sleep(1);
    printf("\n ### WCISNIJ KLAWISZ ENTER\n");
    getchar();
}

int main()
{
    lb_uzyt = 0;
    lb_wiad = 0;
    wiadomosci = NULL;

    sem_init(&sem_mutex, 0, 1);
    sem_init(&sem_pusty, 0, 0);
    sem_init(&sem_pelny, 0, MAX_WIAD);

    srand(time(NULL));
    int wybor = 1;

    printf("\n# WYBIERZ TEST:\n");
    printf("## 1 TEST WIADOMOSCI\n");
    printf("## 2 TEST PRZEPELNIENIA BUFORA\n");
    printf("## 3 TEST VIPA\n");
    printf("## 4 TEST ADMINA\n");
    printf("## 0 WYJDZ\n");

    scanf("%d", &wybor);
    switch(wybor)
    {
        case 1:
            testWiadomosci();
            break;

        case 2:
            testPrzepelenieKolejki();
            break;

        case 3:
            testVIP();
            break;

        case 4:
            testADMIN();
            break;

        case 0:
            break;

        default:
            printf("ZLY WYBOR!\n");
     }


    return 0;
}
