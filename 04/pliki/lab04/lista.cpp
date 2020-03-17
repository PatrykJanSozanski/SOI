#ifndef __lista_cpp
#define __lista_cpp

#include <iostream>
#include <algorithm>
#include <iomanip>


using namespace std;

//struktura do przekazywania wynikow
struct Res
{
  string message;
  time_t timeStamp;
};

//Deklaracja klasy listy dwukierunkowej
class lista{
    public:
    class ogniwo{
        public:
        ogniwo * next;
        ogniwo * prev;
        string message;
        int priority;
        time_t stamp;
    };
    class interator{
        public:
        ogniwo * wsk;
        interator next(interator it);
    };
    lista();
    lista(string msg, int p, time_t t);
    void wyswietl();
    interator poczatek;
    interator koniec;
    int wstaw(string msg, int p, time_t t);
    struct Res pobierz();
    interator begin();
    interator end();
    interator get(string msg);
    int size;
};

 
 
//***************
// METODY
//***************
 
//Konstruktor listy
//tworzy nowa liste dwukierunkowa z jednym elementem o zadanej wartosci v i priorytecie p
lista::lista(string msg, int p, time_t t){
    ogniwo * a = new ogniwo;
    a -> next = a;
    a -> prev = a;
    a -> message = msg;
    a -> priority = p;
    a -> stamp = t;
    poczatek.wsk = a;
    koniec.wsk = a;
    size = 1;
}

//tworzy pusta liste
lista::lista(){
    poczatek.wsk = nullptr;
    koniec.wsk = nullptr;
    size = 0;
}
 

//Wstawia nowy element do listy dwukierunkowej
//Element ma wartosc v i priorytet p
int lista::wstaw(string msg, int p, time_t t){

    if(size == 0){      //gdy lista jest pusta
        if(msg.length() >= 8 && msg.length() <= 64){    //warunrk dlugosci 
            if(p == 0){
                transform(msg.begin(), msg.end(), msg.begin(), [](unsigned char c){return tolower(c); });
                ogniwo * a = new ogniwo;
                a -> next = a;
                a -> prev = a;
                a -> message = msg;
                a -> priority = p;
                a -> stamp = t;
                poczatek.wsk = a;
                koniec.wsk = a;
                size = 1;
                return 1;
            }
            else{
                transform(msg.begin(), msg.end(), msg.begin(), [](unsigned char c){return toupper(c); });
                ogniwo * a = new ogniwo;
                a -> next = a;
                a -> prev = a;
                a -> message = msg;
                a -> priority = p;
                a -> stamp = t;
                poczatek.wsk = a;
                koniec.wsk = a;
                size = 1;
                return 1;
            }
        }//warunek dlugosci
        else{
            cout << "\n### WIADOMOSC NIE SPELNIA WARUNKOW DLUGOSCI\n";
            return 0;
        }
    }//rozmiar
    else{
        if(msg.length() >= 8 && msg.length() <= 64){    //warunrk dlugosci 
            
            interator it;

            if(p == 0){     //priorytet niski - dodaje na koncu
                transform(msg.begin(), msg.end(), msg.begin(), [](unsigned char c){return tolower(c); });
                it = koniec;
                ogniwo * a = it.wsk;
                ogniwo * b = new ogniwo;
                b -> prev = a;
                b -> message = msg;
                b -> priority = p;
                b -> stamp = t;
                a -> next = b;
                b -> next = b;
                koniec.wsk = b; //popraw koniec listy
                size++;
            }
            else{       //priorytet wysoki - dodaje na poczatku
                it = poczatek;
                transform(msg.begin(), msg.end(), msg.begin(), [](unsigned char c){return toupper(c); });

                if(it.wsk -> priority == 1){
                    while(it.wsk -> next -> priority == 1 && it.wsk -> next != koniec.wsk){  //szukanie ostatniej w liscie wiadomosci o wysokim priorytecie
                        it = it.next(it);
                    }
                    ogniwo * a = it.wsk;
                    ogniwo * b = new ogniwo;
                    ogniwo * c = a->next;
                    b -> next = c;
                    b -> prev = a;
                    b -> message = msg;
                    b -> priority = p;
                    b -> stamp = t;
                    c -> prev = b;
                    a -> next = b;
                    if( b -> next == b){
                        koniec.wsk = b; //popraw koniec listy, jezeli dodajemy za ostatnim elementem
                    }
                    size++;
                }
                else{
                    ogniwo * a = it.wsk;
                    ogniwo * b = new ogniwo;
                    b -> next = a;
                    b -> prev = b;  //poczatek listy
                    b -> message = msg;
                    b -> priority = p;
                    b -> stamp = t;
                    a -> prev = b;
                    poczatek.wsk = b;   //poczatek listy
                    size++;
                }
                
            }
            return 1;
        }
        else{
            cout << "\n### WIADOMOSC NIE SPELNIA WARUNKOW DLUGOSCI\n";
            return 0;
        }
    }
}
 
//Pobiera z listy dwukierunkowej pierwszy element i usuwa go z listy
struct Res lista::pobierz(){
    interator it = poczatek;
    string msg = it.wsk -> message;
    time_t t = it.wsk -> stamp;
    ogniwo * b = it.wsk;
    ogniwo * a = b -> next;
    poczatek.wsk = a;   //poczatek listy
    a -> prev = a;      //poczatek listy
    delete b;
    size--;
    Res result;
    result.message = msg;
    result.timeStamp = t;
    return result;
}
 
//Zwraca poczatek listy dwukierunkowej
lista::interator lista::begin(){
    return poczatek;
}
 
//Zwraca koniec listy dwukierunkowej
lista::interator lista::end(){
    return koniec;
}
 
//Wyswietla zwartosc listy dwukierunkowej
void lista::wyswietl(){
    cout << "\n### LISTA WIADOMOSCI\n";
    cout << "## " << setw(63) << left << "WIADOMOSC:" << setw(20) << left << "ZNACZNIK CZASU:" << "\n";
    interator it = poczatek;
    for(int i = 0 ; i < size ; i++){
        cout <<"# " << setw(64) << left << it.wsk -> message << setw(20) << left << it.wsk -> stamp << "\n";
        it = it.next(it);
    }
}     
 
//Zwraca wskaznik na kolejny element w liscie dwukierunkowej
lista::interator lista::interator::next(interator it){
    it.wsk = it.wsk -> next;
    return it;
} 
 
//Zwraca wskaznik na element o zadanej wartosci v
lista::interator lista::get(string msg){
    interator it = poczatek;
    for(int i = 0 ; i < size; i++){
        string x = it.wsk -> message;
        if(x == msg){
        break;  
        }    
        else{
            it.wsk = it.wsk->next;
        }
    }
    return it;
}

#endif