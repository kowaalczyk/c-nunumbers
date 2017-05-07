/* Krzysztof Kowalczyk 385830 */
#include <stdlib.h>
#include <stdio.h>

typedef int bool;
typedef struct liliczba liliczba;
struct liliczba {
    liliczba *child; /* cyfry biezacej liliczby */
    liliczba *next; /* nastepna cyfra */
    liliczba *prev; /* poprzednia cyfra */
    bool normalized; /* ==1 jezeli liliczba jest znormalizowana, uzywane do debugowania */
};

/* funkcja wczytuje liliczbe z std wejscia do drzewa */
liliczba *loadLi(liliczba *prev) {
    liliczba *li;
    char c;
    c = (char)getchar();
    if(c=='Z') {
        return NULL;
    } else {
        /* Zaklada poprawnosc danych na wejsciu, zgodnie z trescia zadania */
        li = (liliczba*)malloc(sizeof(liliczba));
        li->normalized = 0;
        li->prev = prev;
        li->child = loadLi(NULL);
        li->next = loadLi(li);
        return li;
    }
}

/* funkcja wypisujaca liliczbe zapisana w postaci drzewa na std wyjscie */
void printLi(liliczba *wsk) {
    if(wsk == NULL)
        printf("%c", 'Z');
    else {
        printf("%c", 'Y');
        printLi(wsk->child);
        printLi(wsk->next);
    }
}

/* funkcja usuwajaca liliczbe, zwalnia pamiec */
void del(liliczba *li) {
    if(li!=NULL) {
        del(li->child);
        del(li->next);
        free(li);
    }
}

/* funkcja zwracajaca wskaznik do ostatniej cyfry danej liliczby (ostatniego brata w drzewie) */
liliczba *pointerToLastItem(liliczba *li) {
    while(li!=NULL && li->next!=NULL)
        li = li->next;
    return li;
}

/* Funkcja zwracajaca 0 jezeli liliczby sa rowne, 1 jezeli l1 jest wieksza, 2 jezeli l2 jest wieksza
 * l1 i l2 MUSZA BYC ZNORMALIZOWANE, funkcje compare wywolujemy na wskazniku do ostatniej cyfry liliczby */
unsigned int compare(liliczba *l1, liliczba *l2) {
    liliczba *w1, *w2;
    unsigned int tmp;
    w1 = l1;
    w2 = l2;
    /* warunki koncowe konczace rekurencyjne wywolywanie funkcji */
    if(w1==NULL) {
        if(w2==NULL)
            return 0;
        else
            return 2;
    } else if(w2==NULL) {
        return 1;
    } else {
        /* jezeli zadna z liliczb nie jest zerem, wywolujemy porownanie na ich dzieciach w grafie */
        liliczba *t1, *t2;
        t1 = pointerToLastItem(w1->child);
        t2 = pointerToLastItem(w2->child);
        tmp = compare(t1, t2);
        if(tmp)
            return tmp;
        else
            return compare(w1->prev, w2->prev);
    }
}

/* mnozenie x2 zdefiniowane jako osobna funkcja (potrzebne do normalizacji) */
void makeDouble(liliczba *li) {
    /* mnozenie przez 2 to po prostu dopisanie pojedynczego wezla do pierwszego dziecka liliczby */
    if(li->child == NULL) {
        li->child = (liliczba*)malloc(sizeof(liliczba));
        (li->child)->prev = NULL;
        (li->child)->child = NULL;
        (li->child)->next = NULL;
    } else {
        liliczba *wsk = (liliczba*)malloc(sizeof(liliczba));
        wsk->prev = NULL;
        wsk->child = NULL;
        wsk->next = li->child;
        (li->child)->prev = wsk;
        li->child = wsk;
    }
}

/* funkcja usuwajaca podwojnie wystepujace cyfry w liliczbie (bez zmieniania jej wartosci) */
void removeDuplicates(liliczba *li) {
    if(li!=NULL) {
        liliczba *w1 = li;
        while(w1!=NULL && w1->next!=NULL) {
            liliczba *w2 = w1->next;
            while(w2!=NULL) {
                if(!compare(pointerToLastItem(w1->child), pointerToLastItem(w2->child))) {
                    makeDouble(w1);
                    /* po podwojeniu pierwszej z rownych cyfr druga trzeba usunac aby wartosc liliczby byla poprawna */
                    if(w2->prev != NULL)
                        (w2->prev)->next = w2->next;
                    if(w2->next != NULL)
                        (w2->next)->prev = w2->prev;
                    del(w2->child);
                    free(w2);
                    removeDuplicates(w1->child);
                    /* Aby upewnic sie ze nowo otrzymana wartosc takze sie nie powtarza, cofamy wskaznikki na poczatek */
                    w1=li;
                    w2=li->next;
                } else {
                    w2 = w2->next;
                }
            }
            w1=w1->next;
        }
    }
}

/* Sortowanie babelkowe liliczby */
void sort(liliczba *li) {
    liliczba *w1, *w2;
    w1 = li;
    while(w1!=NULL) {
        w2 = w1->next;
        /* w2 jest zawsze po prawej stronie w1 */
        while(w2!=NULL) {
            if(compare(pointerToLastItem(w1->child), pointerToLastItem(w2->child)) == 1) {
                /* jezeli liczba po lewej stronie jest wieksza, trzeba je zamienic */
                liliczba *pom = w1->child;
                w1->child = w2->child;
                w2->child = pom;
            }
            w2 = w2->next;
        }
        w1 = w1->next;
    }
}

/* funkcja sprowadzajaca liliczbe do znormalizowanej postaci */
void normalize(liliczba *li) {
    liliczba *wsk = li;
    /* normalizujemy kolejne cyfry (i ich cyfry, rekurencyjnie) */
    while(wsk!=NULL) {
        if(wsk->child!=NULL) {
            normalize(wsk->child);
        }
        wsk->normalized = 1;
        wsk = wsk->next;
    }
    /* poniewaz wszystkie cyfry liliczby sa juz znormalizowane, mozemy pozbyc sie duplikatow i je posortowac */
    removeDuplicates(li);
    sort(li);
}

/* kopiuje cyfre liliczby i zwraca wskaznik na stworzona kopie */
liliczba *copy(liliczba *li) {
    if(li==NULL)
        return NULL;
    else {
        liliczba *wynik = (liliczba*)malloc(sizeof(liliczba));
        wynik->normalized = li->normalized;
        wynik->prev = li->prev;
        wynik->child = copy(li->child);
        wynik->next = copy(li->next);
        return wynik;
    }
}

/* zwraca wskaznik do (niekoniecznie zmnormalizowanej) sumy 2 liliczb, nie modyfikuje skladnikow l1, l2 */
liliczba *sum(liliczba *l1, liliczba *l2) {
    liliczba *wynik, *w1, *w2, *w3, *pom;
    unsigned int c;
    w1 = l1;
    w2 = l2;
    w3 = NULL;
    wynik = w3;
    while(w1!=NULL || w2!=NULL) {
        pom = (liliczba*)malloc(sizeof(liliczba));
        /* jezeli jedna z cyfr jest NULL to po prostu kopiujemy kolejne cyfry drugiej liczby */
        if(w1 == NULL) {
            pom->child = copy(w2->child);
            w2 = w2->next;
        } else if(w2 == NULL) {
            pom->child = copy(w1->child);
            w1 = w1->next;
        } else {
            /* porownujemy kolejne cyfry i przylaczamy kopie mniejszej z nich do wyniku */
            c = compare(pointerToLastItem(w1->child), pointerToLastItem(w2->child));
            switch(c) {
                case 0:
                    pom->child = copy(w1->child);
                    makeDouble(pom);
                    w1 = w1->next;
                    w2 = w2->next;
                    /* jezeli mamy 2 takie same cyfry to mozemy je od razu zamienic na cyfre o 2x wiekszej wartosci */
                    break;
                case 1:
                    pom->child = copy(w2->child);
                    w2 = w2->next;
                    break;
                case 2:
                    pom->child = copy(w1->child);
                    w1 = w1->next;
                    break;
                default:
                    break;
            }
        }
        /* wstawiamy nowa cyfre na koniec liliczby */
        pom->prev = w3;
        pom->next = NULL;
        pom->normalized = 0;
        if(w3!=NULL)
            w3->next = pom;
        else
            wynik = pom;
        w3 = pom;
    }
    return wynik;
}

/* funkcja mnozy 2 liliczby i zwraca wskaznik do liczby bedacej wynikiem tego dzialania */
liliczba *multiply(liliczba *l1, liliczba *l2) {
    liliczba *wynik, *w1, *w2, *w3;
    w1 = l1;
    w2 = l2;
    w3 = NULL;
    wynik = w3;
    /* mnozenie zgodne ze wzorem (specyfikacja) z tresci zadania */
    while(w1!=NULL) {
        w2 = l2;
        while(w2!=NULL) {
            liliczba *pom;
            pom = (liliczba*)malloc(sizeof(liliczba));
            pom->child = sum(w1->child, w2->child);
            pom->prev = w3;
            pom->next = NULL;
            pom->normalized = 0;
            if(w3!=NULL)
                w3->next = pom;
            else
                wynik = pom;
            w3 = pom;
            w2 = w2->next;
        }
        w1 = w1->next;
    }
    return wynik;
}

int main() {
    liliczba *l1, *l2, *l3;
    /* wczytywanie liliczb */
    l1 = loadLi(NULL);
    getchar();
    l2 = loadLi(NULL);
    getchar();
    /* wykonywanie mnozenia i normalizacji */
    normalize(l1);
    normalize(l2);
    l3 = multiply(l1, l2);
    normalize(l3);
    printLi(l3);
    printf("\n");
    /* czyszczenie pamieci */
    del(l1);
    del(l2);
    del(l3);
    return 0;
}
