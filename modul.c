#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "modul.h"
#define W_OK 0                   /* wartosc oznaczajaca brak bledow */
#define B_NIEPOPRAWNAOPCJA -1    /* kody bledow rozpoznawania opcji */
#define B_BRAKNAZWY   -2
#define B_BRAKWARTOSCI  -3
#define B_BRAKPLIKU   -4
#define B_BRAKNAZWYWYJ -6
#define MAX 512        /* Maksymalny rozmiar wczytywanego obrazu */
#define DL_LINII 512     /* Dlugosc buforow pomocniczych */
/************************************************************************************
 * Funkcja wczytuje obraz PGM lub PPM z pliku do tablicy       	       	       	       	    *
 *										    *
 * \param[in] plik_we uchwyt do pliku z obrazem w formacie PGM			    *
 * \param[out] obraz_pgm tablica, do ktorej zostanie zapisany obraz		    *
 * \param[out] wymx szerokosc obrazka						    *
 * \param[out] obraz->wymy wysokosc obrazka						    *
 * \param[out] obraz->szarosci liczba odcieni obraz->szarosci					    *
 * \return liczba wczytanych pikseli						    *
 ************************************************************************************/

int czytaj(FILE *plik_we,TObraz *obraz) {
  char buf[DL_LINII];      /* bufor pomocniczy do czytania naglowka i komentarzy */
  int znak;                /* zmienna pomocnicza do czytania komentarzy */
  int koniec=0;            /* czy napotkano koniec danych w pliku */
  int i,j,tmp;
 
  /*Sprawdzenie czy podano prawidłowy uchwyt pliku */
  if (plik_we==NULL) {
    fprintf(stderr,"Blad: Nie podano uchwytu do pliku\n");
    return(0);
  }
  /* Sprawdzenie "numeru magicznego" - powinien być P2 lub P3*/
  if (fgets(buf,DL_LINII,plik_we)==NULL)   /* Wczytanie pierwszej linii pliku do bufora */
    koniec=1;                              /* Nie udalo sie? Koniec danych! */

  if ( (buf[0]!='P') || (buf[1]!='2' && buf[1]!='3') || koniec) {  /* Czy jest magiczne "P2" lub "P3"? */
    fprintf(stderr,"Blad: To nie jest plik PGM ani PPM\n");
    return(0);
  }
  if (buf[1]=='2') obraz->format=2;/*w zaleznosci od tego ustawienie odpowiedniego obiektu aby wiedziec z jakim obrazem mamy do czynienia*/
  else obraz->format=3;
  /* Pominiecie komentarzy */
  do {
    if ((znak=fgetc(plik_we))=='#') {         /* Czy linia rozpoczyna sie od znaku '#'? */
      if (fgets(buf,DL_LINII,plik_we)==NULL)  /* Przeczytaj ja do bufora                */
	koniec=1;                   /* Zapamietaj ewentualny koniec danych */
    }  else {
      ungetc(znak,plik_we);                   /* Gdy przeczytany znak z poczatku linii */
    }                                         /* nie jest '#' zwroc go                 */
  } while (znak=='#' && !koniec);   /* Powtarzaj dopoki sa linie komentarza */
                 /* i nie nastapil koniec danych         */

  /* Pobranie wymiarow obrazu i liczby odcieni szarosci */
  fscanf(plik_we,"%d %d %d",&(obraz->wymx),&(obraz->wymy),&(obraz->szarosci));
if(obraz->wymx<=0 || obraz->wymy<=0 || obraz->szarosci<=0){
      fprintf(stderr,"Blad: Niepoprawne wymiary obrazu lub liczba stopni szarosci\n");
   return(-1);
    }

obraz->p3=3*obraz->wymx; /*kazdy piksel w obrazie PPM potrzebuje 3x wiecej miejsca*/

if(obraz->format==2) tmp=obraz->wymx;
else tmp=obraz->p3;

obraz->piksele=(int**)malloc(obraz->wymy*sizeof(int *)); /*alokacja dwuwymiarowej tablicy dynamicznej*/
for(i=0; i<obraz->wymy; i++)
{
    obraz->piksele[i]=(int*)malloc(tmp*sizeof(int));
}
  /* Pobranie obrazu i zapisanie w tablicy piksele*/
  for (i=0;i<obraz->wymy;i++) {
    for (j=0;j<tmp;j++) {
      fscanf(plik_we,"%d",&(obraz->piksele[i][j]));
	if(obraz->piksele[i][j]>obraz->szarosci)
	  {/*sprawdzenie czy liczba wpisywana do tablicy nie jest wieksza od maksymalnej dopuszczalnej wartosci obraz->szarosci*/
	fprintf(stderr,"Blad: Liczba wieksza niz maksymalna wartosc szarosci");
	}
      if(obraz->piksele[i][j]<0){
	/* tutaj czy nie jest mniejsza od 0*/
	fprintf(stderr,"Blad: Liczba nie moze byc ujemna\n");
	return(0);
      }
    }
  }
  return obraz->wymx*obraz->wymy;   /* Czytanie zakonczone sukcesem    */
}                       /* Zwroc liczbe wczytanych pikseli */


/* Wyswietlenie obrazu o zadanej nazwie za pomoca programu "display"   */
void wyswietl(char *n_pliku) {
  char polecenie[DL_LINII];      /* bufor pomocniczy do zestawienia polecenia */

  strcpy(polecenie,"display ");  /* konstrukcja polecenia postaci */
  strcat(polecenie,n_pliku);     /* display "nazwa_pliku" &       */
  strcat(polecenie," &");
  printf("%s\n",polecenie);      /* wydruk kontrolny polecenia */
  system(polecenie);             /* wykonanie polecenia        */
}
/*Funkcja zapisz

Dane wejsciowe: wskaznik na plik do ktorego chcemy zapisac tablice, wskaznik na obiekt typu TObraz o formacie PGM lub PPM.

Dane wyjsciowe: Zapisany plik na dysku w formacie PGM
*/
int zapisz(FILE *plik_wy,TObraz *obraz)
{
  int i,j,tmp;

  if(obraz->format==2) 
{
tmp=obraz->wymx;
fprintf(plik_wy,"P2\n");
} /*Magiczna liczba P2 na poczatku*/
  else 
{
fprintf(plik_wy,"P3\n");/*Lub P3 w zależności od potrzeby*/
tmp=obraz->p3;
}
  fprintf(plik_wy,"%d %d\n",obraz->wymx,obraz->wymy);/*Nastepnie wymiar x i wymiar y*/
  fprintf(plik_wy,"%d\n",obraz->szarosci);/*Dalej wg specyfikacji maks. szarosc*/
for(i=0;i<obraz->wymy;i++)
    {
      for(j=0;j<tmp;j++)
	{
	  fprintf(plik_wy,"%d ",obraz->piksele[i][j]);/*kolejne wartosci pikseli na obrazie*/
	if(j>1 && j%15==0) fprintf(plik_wy,"\n");
	}

    }
  return 0;  
}

/*Funkcja rozciaganie_histogramu

Dane wejsciowe: wskaznik na obiekt typu TObraz o formacie PGM lub PPM.

Dane wyjsciowe:tablica reprezentujaca obraz poddany operacji rozciagania histogramu.
*/

int rozciaganie_histogramu(TObraz *obraz)
{
  int i,j;/* zmienne pomocnicze do przechodzenia po tablicy */
  int wemax=0; /*domyslna wartosc szarosci maksymalnej*/
  int wemin=255;/*a tu minimalnej*/
  int rmin=255;/*wartosci pomocnicze dla obrazow PPM*/
  int gmin=255;
  int bmin=255;
  int rmax=0;
  int gmax=0;
  int bmax=0;
float h;
	
if(obraz->format==2){
for(i=0;i<obraz->wymy;i++)
    {
      for(j=0;j<obraz->wymx;j++)
	{
	  if(obraz->piksele[i][j]>wemax)  wemax=obraz->piksele[i][j];/*poszukiwanie wartosci maksymalnej szarosci*/
	  if(obraz->piksele[i][j]<wemin) wemin=obraz->piksele[i][j];/*tutaj minimalnej*/
	}
    }
for(i=0;i<obraz->wymy;i++)
    {
      for(j=0;j<obraz->wymx;j++)
	{
h=(obraz->piksele[i][j]-wemin)*(obraz->szarosci/(wemax-wemin));
	  obraz->piksele[i][j]=h;/*obliczanie wartosci pola na podstawie wzoru*/
            }
    }
return 0;
}
else{
for(i=0;i<obraz->wymy;i++)
    {
      for(j=0;j<obraz->p3;j++)
	{
	  if(obraz->piksele[i][j]>rmax)  rmax=obraz->piksele[i][j];/*poszukiwanie wartosci maksymalnej szarosci*/
	  if(obraz->piksele[i][j]<rmin) rmin=obraz->piksele[i][j];/*tutaj minimalnej*/
	j++;
	  if(obraz->piksele[i][j]>gmax)  gmax=obraz->piksele[i][j];
	  if(obraz->piksele[i][j]<gmin) gmin=obraz->piksele[i][j];
	j++;
	  if(obraz->piksele[i][j]>bmax)  bmax=obraz->piksele[i][j];
	  if(obraz->piksele[i][j]<bmin) bmin=obraz->piksele[i][j];

	}
    }
for(i=0;i<obraz->wymy;i++)
    {
      for(j=0;j<obraz->p3;j++)
	{
h=(obraz->piksele[i][j]-rmin)*obraz->szarosci/(rmax-rmin);
obraz->piksele[i][j]=h;
j++;
h=(obraz->piksele[i][j]-gmin)*(obraz->szarosci/(gmax-gmin));
obraz->piksele[i][j]=h;
j++;
h=(obraz->piksele[i][j]-bmin)*(obraz->szarosci/(bmax-bmin));
obraz->piksele[i][j]=h;
            }
    
}

  return 0;
}
}

/*Funkcja progowanie

Dane wejsciowe: wskaznik na obiekt typu TObraz o formacie PGM lub PPM, prog operacji.

Dane wyjsciowe:tablica reprezentujaca obraz poddany operacji progowania zadanym progiem.
*/
int progowanie(TObraz *obraz, int prog)
{

  int i,j,tmp;
if(obraz->format==2) tmp=obraz->wymx;
else tmp=obraz->p3;
for(i=0;i<obraz->wymy;i++)
    {
      for(j=0;j<tmp;j++)
	{
	  if(obraz->piksele[i][j]<=prog)/*sprawdzanie jaka jest wartosc piksela w stosunku do progu*/
	    {
	      obraz->piksele[i][j]=0;/*w zaleznosci od tego przypisawnie wartosci minimalnej szarosci*/
	    }
	  else{
	    obraz->piksele[i][j]=obraz->szarosci;/*lub maksymalnej*/
	  }
	}
    }
  
  return 0;
}
/*Funkcja polprogowanie_bieli

Dane wejsciowe: wskaznik na obiekt typu TObraz o formacie PGM lub PPM, prog operacji.

Dane wyjsciowe:tablica reprezentujaca obraz poddany operacji polprogowania bieli zadanym progiem.
*/

int polprogowanie_bieli(TObraz *obraz, int prog)
{

  int i,j,tmp;
if(obraz->format==2) tmp=obraz->wymx;
else tmp=obraz->p3;
for(i=0;i<obraz->wymy;i++)
    {
      for(j=0;j<tmp;j++)
	{
	  if(obraz->piksele[i][j]>prog) obraz->piksele[i][j]=obraz->szarosci; /* sprwadzanie czy wartosc jest wieksza od progu, jezeli tak przypisz maks szarosc*/
	 
	}
	}
      
  return 0;
}

int bezwzgl(int l)/*funkcja pomocnicza do konturowania, zwraca wartosc bezwzgledna*/
{
	if(l<0) return l*=-1;

	else return l;
}

/*Funkcja konturowanie

Dane wejsciowe: wskaznik na obiekt typu TObraz o formacie PGM lub PPM.
Dane wyjsciowe:tablica reprezentujaca obraz poddany operacji konturowania.
*/
int konturowanie(TObraz *obraz)
{
int i,j,tmp;

if(obraz->format==2){
tmp=obraz->wymx;
for(i=0;i<(obraz->wymy-1);i++)
    {
      for(j=0;j<(tmp-1);j++)
	{ 
	    obraz->piksele[i][j]=bezwzgl(obraz->piksele[i+1][j]-obraz->piksele[i][j])+bezwzgl(obraz->piksele[i][j+1]-obraz->piksele[i][j]);/*sprawdzanie "okolicy" piksela i decydowanie czy jest on konturem*/

	   if(obraz->piksele[i][j]>255) obraz->piksele[i][j]=255;
	}
    }

}
else{
konwersja(obraz);
konturowanie(obraz);
}

  return 0;
}

/*Funkcja negatyw

Dane wejsciowe: wskaznik na obiekt typu TObraz o formacie PGM lub PPM.

Dane wyjsciowe:tablica reprezentujaca obraz poddany operacji negatywu.
*/
int negatyw(TObraz *obraz)
{

  int i,j,tmp;
if(obraz->format==2) tmp=obraz->wymx;
else tmp=obraz->p3;
for(i=0;i<obraz->wymy;i++)
    {
      for(j=0;j<tmp;j++)
	{
	  obraz->piksele[i][j]=obraz->szarosci-obraz->piksele[i][j]; /*"odwracanie" wartosci piksela*/

	}
    }
  return 0;
}

/*Funkcja konwersji

Dane wejsciowe: wskaznik na obiekt typu TObraz o formacie PGM.

Dane wyjsciowe: przekonwertowany obiekt TObraz na PPM.
*/

int konwersja (TObraz *obraz)
{ 
int i,j,k;
if(obraz->format==2) 
{
 fprintf(stderr,"To nie jest obraz PPM.");
return -5;
}

else {
obraz->temp=(int**)malloc(obraz->wymy*sizeof(int *));
for(i=0; i<obraz->wymy; i++)
{
    obraz->temp[i]=(int*)malloc(obraz->wymx*sizeof(int));
}

	for(i=0;i<obraz->wymy;i++)
    {
      for(j=0,k=0;j<obraz->p3;j++,k++)
	{
 
	  obraz->temp[i][k]=(obraz->piksele[i][j]+obraz->piksele[i][j+1]+obraz->piksele[i][j+2])/3;
		
	j=j+2;
	}
    }
free(obraz->piksele);
obraz->piksele=(int**)malloc(obraz->wymy*sizeof(int *));
for(i=0; i<obraz->wymy; i++)
{
    obraz->piksele[i]=(int*)malloc(obraz->wymx*sizeof(int));
}
obraz->format=2;
for(i=0;i<obraz->wymy;i++)
    {
      for(j=0;j<obraz->wymx;j++)
	{
	  obraz->piksele[i][j]=obraz->temp[i][j];
	}
    }
free(obraz->temp);
return 0;
}
}
/********************************************************************/
/*                                                                  */
/* ALTERNATYWNA DO PRZEDSTAWIONEJ NA WYKLADZIE WERSJA OPRACOWANIA   */
/* PARAMETROW WYWOLANIA PROGRAMU UWZGLEDNIAJACA OPCJE Z PARAMETRAMI */
/* Z ODPOWIEDNIO ZAPROPONOWANYMI STRUKTURAMI DANYCH PRZEKAZUJACYMI  */
/* WCZYTANE USTAWIENIA                                              */
/*                                         COPYRIGHT (c) 2007 ZPCiR */
/*                                                                  */
/* Autorzy udzielaja kazdemu prawa do kopiowania tego programu      */
/* w calosci lub czesci i wykorzystania go w dowolnym celu, pod     */
/* warunkiem zacytowania zrodla                                     */
/*                                                                  */
/********************************************************************/

/*Funkcja zerujaca opcje*/
/*dane wejsciowe: wskaznik na obiekt typu w_opcje

dane wyjsciowe: wyzerowane pozycje w obiekcie w_opcje*/
void wyzeruj_opcje(w_opcje * wybor) {
  wybor->plik_we=NULL;
  wybor->plik_wy=NULL;
  strcpy(wybor->nazwa_pliku_wy, "0");
  wybor->negatyw=0;
  wybor->konturowanie=0;
  wybor->progowanie=0;
  wybor->wyswietlenie=0;
  wybor->rozciaganie_histogramu=0;
  wybor->polprogowanie_bieli=0;
  wybor->konwersja=0;
}
/*Funkcja przetwarzajaca opcje podane przy uruchamianiu programu
Dane wejsciowe: ilosc argumentow, wskaznik na tablice wskaznikow z wartosciami argumentow, wskaznik na obiekt typu w_opcje

Dane wyjsciowe: odpowiednio postawione flagi dla wybranych opcji*/
int przetwarzaj_opcje(int argc, char **argv, w_opcje *wybor) {
  int i, prog;
  char *nazwa_pliku_we;

  wyzeruj_opcje(wybor);
  wybor->plik_wy=stdout;        /* na wypadek gdy nie podano opcji "-o" */

  for (i=1; i<argc; i++) {
    if (argv[i][0] != '-')  /* blad: to nie jest opcja - brak znaku "-" */
      return B_NIEPOPRAWNAOPCJA; 
    switch (argv[i][1]) {
    case 'i': {                 /* opcja z nazwa pliku wejsciowego */
      if (++i<argc) {   /* wczytujemy kolejny argument jako nazwe pliku */
	nazwa_pliku_we=argv[i];
	if (strcmp(nazwa_pliku_we,"-")==0) /* gdy nazwa jest "-"        */
	  wybor->plik_we=stdin;            /* ustwiamy wejscie na stdin */
	else                               /* otwieramy wskazany plik   */
	  wybor->plik_we=fopen(nazwa_pliku_we,"r");
      } else 
	return B_BRAKNAZWY;                   /* blad: brak nazwy pliku */
      break;
    }
    case 'o': {                 /* opcja z nazwa pliku wyjsciowego */
      if (++i<argc) {   /* wczytujemy kolejny argument jako nazwe pliku */
	strcpy(wybor->nazwa_pliku_wy, argv[i]);
	if (strcmp(wybor->nazwa_pliku_wy,"-")==0)/* gdy nazwa jest "-"         */
	  wybor->plik_wy=stdout;          /* ustwiamy wyjscie na stdout */
	else                              /* otwieramy wskazany plik    */
	  wybor->plik_wy=fopen(wybor->nazwa_pliku_wy,"w");
      } else 
	return B_BRAKNAZWYWYJ;                   /* blad: brak nazwy pliku */
      break;
    }
    case 'p': {
	if(argv[i][2]=='b') /*sprawdzanie czy wpisano pb czy samo p*/
	{ 
		wybor->polprogowanie_bieli=1;
	}
      if (++i<argc) { /* wczytujemy kolejny argument jako wartosc progu */
	if (sscanf(argv[i],"%d",&prog)==1) {
	if(!wybor->polprogowanie_bieli) wybor->progowanie=1;
	  wybor->w_progu=prog;
	} else
	  return B_BRAKWARTOSCI;     /* blad: niepoprawna wartosc progu */
      } else 
	return B_BRAKWARTOSCI;             /* blad: brak wartosci progu */
      break;
    }
    case 'n': {                 /* mamy wykonac negatyw */
      wybor->negatyw=1;
      break;
    }
    case 'k': {                 /* mamy wykonac konturowanie */
      wybor->konturowanie=1;
      break;
    }
    case 'h': {
      wybor->rozciaganie_histogramu=1; /*wykonanie operacji rozciagniecia histogramu*/
	break;
    }
	case 'c':{			/*wykonanie konwersji z PPM na PGM*/
	wybor->konwersja=1;
	break;
	}
    case 'd': {                 /* mamy wyswietlic obraz */
      wybor->wyswietlenie=1;
      break;
    }
    default:                    /* nierozpoznana opcja */
      return B_NIEPOPRAWNAOPCJA;
    } /*koniec switch */
  } /* koniec for */

  if (wybor->plik_we!=NULL)     /* ok: wej. strumien danych zainicjowany */
    return W_OK;
  else 
    return B_BRAKPLIKU;         /* blad:  nie otwarto pliku wejsciowego  */
}


