#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "modul.h"

int main(int argc, char ** argv) {
  TObraz obraz;
  w_opcje opcje;
 switch(przetwarzaj_opcje(argc,argv,&opcje))
{
	case -1: printf("Nie ma takiej opcji\n");
	return -1;
	break;
	
	case -2: printf("Plik o podanej nazwie nie istnieje\n");
	return -2;
	break;
	
	case -3: printf("Brak wartosci progu lub wartosc niepoprawna\n");
	return -3;
	break;
	
	case -4: printf("Brak pliku\n");
	return -4;
	break;
	
	case -6: printf("Nie podano nazwy pliku wyjsciowego\n");
	return -6;
	case 0:
	break;
}
	czytaj(opcje.plik_we,&obraz);
	fclose(opcje.plik_we);
if(opcje.negatyw==1)
{

	negatyw(&obraz);
      	printf("Negatyw zapisano\n");
}


if(opcje.konturowanie==1)
{
	konturowanie(&obraz);
      	printf("Obraz po konturowaniu zapisano\n");
}
if(opcje.rozciaganie_histogramu==1)
{
	rozciaganie_histogramu(&obraz);
      	printf("Obraz po rozciagnieciu histogramu zapisano\n");
}

if(opcje.progowanie==1)
{
	progowanie(&obraz,opcje.w_progu);
      	printf("Obraz po progowaniu zapisano\n");
}

if(opcje.polprogowanie_bieli==1)
{
	polprogowanie_bieli(&obraz,opcje.w_progu);
      	printf("Obraz po polprogowaniu bieli zapisano\n");
}
if(opcje.konwersja==1)
{
	konwersja(&obraz);
      	printf("Obraz po konwersji zapisano\n");
}

if(opcje.wyswietlenie==1)
{
	zapisz(opcje.plik_wy,&obraz);
      	fclose(opcje.plik_wy);
	wyswietl(opcje.nazwa_pliku_wy);
}
else{
      	zapisz(opcje.plik_wy,&obraz);
      	fclose(opcje.plik_wy);
}
free(obraz.piksele);
return 0;
}





