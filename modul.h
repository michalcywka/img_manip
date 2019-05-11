typedef struct{

  int **piksele;
  int format;
  int wymx;
  int wymy;			/*STRUKTURA TObraz*/
  int szarosci;
  int p3;
  int **temp;

} TObraz;

typedef struct {
  FILE *plik_we, *plik_wy;        /* uchwyty do pliku wej. i wyj. */
  int wyswietlenie,negatyw,progowanie,konturowanie,rozciaganie_histogramu,polprogowanie_bieli,konwersja;      /* opcje */
  int w_progu;              /* wartosc progu dla opcji progowanie */ 
  char nazwa_pliku_wy[30]; /*nazwa pliku wyjscia*/
} w_opcje;

int czytaj(FILE *,TObraz *);
void wyswietl(char *);
int zapisz(FILE *, TObraz *);
int rozciaganie_histogramu(TObraz *);
int progowanie(TObraz *, int);
int polprogowanie_bieli(TObraz *,int);
int konturowanie(TObraz *);
int negatyw(TObraz *);
int konwersja(TObraz *);
void wyzeruj_opcje(w_opcje *);
int przetwarzaj_opcje(int, char **, w_opcje *);
int bezw(int);
