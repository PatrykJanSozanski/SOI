//Nazwa pliku: monitor.cpp

//Opis pliku: Plik wykonywalny z kodem 'nadzorcy', ktory tworzy bufor
//komunikacyjny dla procesow, a nastepnie wywoluje w petli watki o typie
//(czytelnik lub pisarz) wybranym przez uzytkownika. Plik jest czescia
//implementacji bufora komunikacyjnego sterowanego monitorem, pisanego na
//potrzeby laboratorium numer 4 z przedmiotu Systemy Operacyjne.

#include "monitor.h"                    //klasa Monitor
#include "lista.cpp"                    //klasa Lista
#include <stdlib.h>                     //obsluga funkcji atoi oraz rand
#include <ctime>                        //obsluga funkcji time
#include <unistd.h>                     //wywolania systemowe - fork i exec
#include <stdio.h>                      //operacje wejscia-wyjscia


#define WYSOKI  1                       //priorytety
#define NISKI 0
#define BUFFER_SIZE 5                  //wielkosc bufora

// Baza wiadomosci.
#define SHORT "soi"          //wiadomosc niepoprawna
#define S1 "kalafior"
#define S2 "marchewka"
#define S3 "kalarepa"
#define S4 "brukselka"
#define S5 "roszponka"
#define S6 "ziemniaki"
#define S7 "pietruszka"
#define S8 "slonecznik"
#define S9 "rzodkiewka"
#define LONG "dlugosc wieksza niz szescdziesiat cztery znaki dlugosc wieksza niz szescdziesiat cztery znaki"

const string BAZA[11] =
{
    {SHORT},
    {S1},
    {S2},
    {S3},
    {S4},
    {S5},
    {S6},
    {S7},
    {S8},
    {S9},
    {LONG}
};

void* Producent(void*);                //watek producenta
void* Konsument(void*);                //watek konsumenta

string generujWiadomosc();              //funkcja generujaca tresc wiadomosci
int losujPriorytet();                   //funkcja losujaca priorytet

//struktura do przekazywania argumentow
struct Arg
{
  string message;
  int priority;
};

//definicja klasy Bufor
class Bufor : private Monitor {
  private:
    int count;                          //ilosc elementow w buforze
    Condition full, empty;              //sygnalizacja pustego i pelnego bufora
  public:
    lista buffer;                       //lista przechowujaca bufor
    Bufor() : count(0) {}               //konstruktor domyslny

    //dodawanie wartosci do bufora
    void push(string msg, int priority, time_t stamp) {
      enter();
      while(count == BUFFER_SIZE){
        cout << "\n###BUFOR PELNY - NIE MOZNA DODAC WIADOMOSCI\n";
        wait(full);
      }

      if(buffer.wstaw(msg, priority, stamp)){
        count++;
      }

      if(count == 1) signal(empty);
      leave();
    }

    //pobieranie wartosci z bufora
    struct Res pop() {
      enter();
      Res result;
      if(count == 0){
        cout << "\n###BUFOR PUSTY - NIE MOZNA POBRAC WIADOMOSCI\n";
        wait(empty);
      }

      count--;
      result = buffer.pobierz();

      if(count == BUFFER_SIZE - 1) signal(full);
      leave();
      return result;
    }

};

void tDzialanieOgolne();
void tPrzepelnienieBufora();
void tPustegoBufora();
void tPriorytetow();
void tDlugosci();

Bufor bufor_komunikacyjny;              //tworzymy egzemplarz klasy Bufor

int main(int argc, char *argv[])
{
  srand(time(nullptr));                 //liczby pseudolosowe

  char test;                            //wybor testu

  cout << "\n# WYBIERZ TEST:\n";
  cout << "## 1 TEST OGOLNEGO DZIALANIA\n";
  cout << "## 2 TEST PRZEPELNIENIA BUFORA\n";
  cout << "## 3 TEST PUSTEGO BUFORA\n";
  cout << "## 4 TEST PRIORYTETOW\n";
  cout << "## 4 TEST PRIORYTETOW\n";
  cout << "## 5 TEST DLUGOSCI WIADOMOSCI\n";
  cout << "## 0 WYJDZ\n";

  test = getchar();

  switch(test)
  {
    case '1':
      tDzialanieOgolne();
      break;

    case '2':
      tPrzepelnienieBufora();
      break;

    case '3':
      tPustegoBufora();    
      break;

    case '4':
      tPriorytetow();
      break;

    case '5':
      tDlugosci();
      break;

    case '0':

      break;

    default:
      printf("ZLY WYBOR!\n");
  }

  return 0;

}//main

void* Producent(void* argument) {
  Arg* arg = (Arg*) argument;
  string msg = arg -> message;
  int p = arg -> priority;
  time_t timeStamp = time(nullptr);
  //bufor_komunikacyjny.buffer.wyswietl();
  bufor_komunikacyjny.push(msg, p, timeStamp);
  cout << "\n## Producent dodal do bufora wiadomosc: " << msg << " o priorytecie: " << p << " znacznik czasu: " << timeStamp << "\n"; //dodawanie wiadomosci
  //bufor_komunikacyjny.buffer.wyswietl();
  delete (Arg*)argument;
  return 0;
}

void* Konsument(void* value) {
  //bufor_komunikacyjny.buffer.wyswietl();
  Res r = bufor_komunikacyjny.pop();
  cout << "\n## Czytelnik pobrał z bufora wiadomosc: " << r.message << " znacznik czasu: " << r.timeStamp << "\n";   //czytanie wiadomosci
  //bufor_komunikacyjny.buffer.wyswietl();
  return 0;
}

string generujWiadomosc(){
  return BAZA[rand() % 10];
}

int losujPriorytet(){
  if(rand() % 2){
    return WYSOKI;
  }
  else{
    return NISKI;
  }
}

void tDzialanieOgolne(){

  char znak;                            //znak wpisany przez uzytkownika
  Arg* argument;                        //argument do przekazania do funcji pthread_create
  pthread_t id_watku;                   //identyfikator watku

  cout << "Co chcesz uruchomic?:";
  cout << "\n(p) - producent";
  cout << "\n(k) - konsument";
  cout << "\n(w) - wyswielt liste wiadomosci";
  cout << "\n(q) - quit\n";

  //petla uruchamiajaca procesy
    while(1)
    {
      znak = getchar();

      switch(znak)
      {
        case 'k':
          //wywolujemy czytelnika

          cout << "\n### CZYTELNIK ROZPOCZYNA PRACE\n";

          pthread_create(&id_watku, NULL, Konsument, NULL);
          break;
        case 'p':
          //wywolujemy pisarza
          cout << "\n### PISARZ ROZPOCZYNA PRACE\n";

          argument = new Arg;
          argument -> priority = losujPriorytet();        //losowanie priorytetu
          argument -> message = generujWiadomosc();       //generowanie wiadomosci       
        
          pthread_create(&id_watku, NULL, Producent, argument); 
          break;
        case 'w':
          bufor_komunikacyjny.buffer.wyswietl();
          break;
        case 'q':
          return;
        default:
          break;
      }//switch
    }//while(1)
}

void tPrzepelnienieBufora(){

  getchar();

  while(1)
  {
    pthread_t id_watku;                   //identyfikator watku
    Arg* argument;                        //argument do przekazania do funcji pthread_create

    //wywoanie pisarzy
    for(int i = 0; i < BUFFER_SIZE + 3; ++i){
      cout << "\n### PISARZ ROZPOCZYNA PRACE\n";

      argument = new Arg;
      argument -> priority = losujPriorytet();        //losowanie priorytetu
      argument -> message = generujWiadomosc();       //generowanie wiadomosci       
            
      pthread_create(&id_watku, NULL, Producent, argument);

      getchar();
    }

    //wywolujemy czytelnikow
    for(int i = 0; i < BUFFER_SIZE; ++i){
      cout << "\n### CZYTELNIK ROZPOCZYNA PRACE\n";

      pthread_create(&id_watku, NULL, Konsument, NULL);

      getchar();
    }

    getchar();

    return;
  }
}

void tPustegoBufora(){
  
  getchar();

  while(1)
  {
    pthread_t id_watku;                   //identyfikator watku
    Arg* argument;                        //argument do przekazania do funcji pthread_create

    //wywolujemy czytelnikow
    for(int i = 0; i < BUFFER_SIZE; ++i){
      cout << "\n### CZYTELNIK ROZPOCZYNA PRACE\n";

      pthread_create(&id_watku, NULL, Konsument, NULL);

      getchar();
    }

    //wywoanie pisarzy
    for(int i = 0; i < BUFFER_SIZE + 3; ++i){
      cout << "\n### PISARZ ROZPOCZYNA PRACE\n";

      argument = new Arg;
      argument -> priority = losujPriorytet();        //losowanie priorytetu
      argument -> message = generujWiadomosc();       //generowanie wiadomosci       
            
      pthread_create(&id_watku, NULL, Producent, argument);

      getchar();
    }

    getchar();

    return;
  }
}

void tPriorytetow(){

  char znak;                            //znak wpisany przez uzytkownika
  Arg* argument;                        //argument do przekazania do funcji pthread_create
  pthread_t id_watku;                   //identyfikator watku

  cout << "Co chcesz uruchomic?:";
  cout << "\n(0) - dodaj wiadomosc o niskim priorytecie";
  cout << "\n(1) - dodaj wiadomosc o wysokim priorytecie";
  cout << "\n(k) - konsument";
  cout << "\n(w) - wyswielt liste wiadomosci";
  cout << "\n(q) - quit\n"; 

    //petla uruchamiajaca procesy
    while(1)
    {
      znak = getchar();

      switch(znak)
      {
        case 'k':
          //wywolujemy czytelnika

          cout << "\n### CZYTELNIK ROZPOCZYNA PRACE\n";

          pthread_create(&id_watku, NULL, Konsument, NULL);
          break;
        case '0':
          //wywolujemy pisarza
          cout << "\n### PISARZ ROZPOCZYNA PRACE\n";

          argument = new Arg;
          argument -> priority = 0;        
          argument -> message = generujWiadomosc();       //generowanie wiadomosci       
        
          pthread_create(&id_watku, NULL, Producent, argument); 
          break;
        case '1':
          //wywolujemy pisarza
          cout << "\n### PISARZ ROZPOCZYNA PRACE\n";

          argument = new Arg;
          argument -> priority = 1;        
          argument -> message = generujWiadomosc();       //generowanie wiadomosci       
        
          pthread_create(&id_watku, NULL, Producent, argument); 
          break;
        case 'w':
          bufor_komunikacyjny.buffer.wyswietl();
          break;
        case 'q':
          return;
        default:
          break;
      }//switch
    }//while(1)
}

void tDlugosci(){

   char znak;                            //znak wpisany przez uzytkownika
  Arg* argument;                        //argument do przekazania do funcji pthread_create
  pthread_t id_watku;                   //identyfikator watku

  cout << "Co chcesz uruchomic?:";
  cout << "\n(0) - dodaj wiadomosc za krotka";
  cout << "\n(1) - dodaj wiadomosc za dluga";
  cout << "\n(k) - konsument";
  cout << "\n(w) - wyswielt liste wiadomosci";
  cout << "\n(q) - quit\n"; 

    //petla uruchamiajaca procesy
    while(1)
    {
      znak = getchar();

      switch(znak)
      {
        case 'k':
          //wywolujemy czytelnika

          cout << "\n### CZYTELNIK ROZPOCZYNA PRACE\n";

          pthread_create(&id_watku, NULL, Konsument, NULL);
          break;
        case '0':
          //wywolujemy pisarza
          cout << "\n### PISARZ ROZPOCZYNA PRACE\n";

          argument = new Arg;
          argument -> priority = losujPriorytet();        
          argument -> message = SHORT;       //generowanie wiadomosci       
        
          pthread_create(&id_watku, NULL, Producent, argument); 
          break;
        case '1':
          //wywolujemy pisarza
          cout << "\n### PISARZ ROZPOCZYNA PRACE\n";

          argument = new Arg;
          argument -> priority = losujPriorytet();        
          argument -> message = LONG;       //generowanie wiadomosci       
        
          pthread_create(&id_watku, NULL, Producent, argument); 
          break;
        case 'w':
          bufor_komunikacyjny.buffer.wyswietl();
          break;
        case 'q':
          return;
        default:
          break;
      }//switch
    }//while(1)
}
