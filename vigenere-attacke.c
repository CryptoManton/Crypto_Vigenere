/*************************************************************
**         Europäisches Institut für Systemsicherheit        *
**         Praktikum "Kryptoanalyse"                         *
**                                                           *
** Versuch 1: Klassische Chiffrierverfahren                  *
**                                                           *
**************************************************************
**
** vigenere_attacke.c: Brechen der Vigenere-Chiffre
**/

#include <stdio.h>
#include <stdlib.h>
#include <praktikum.h>
#include <math.h>

#define GNUPLOT_CMD_FILENAME "gnuplot.in.cmd" /* Name fuer das erzeugte
						 gnuplot-Kommandofile */
#define GNUPLOT_DATA_FILENAME "gnuplot.in.data" /* Name fuer Datenfile */

#define NUMCHARS    26       /* Anzahl der Zeichen, die betrachtet werden ('A' .. 'Z') */
#define MaxFileLen  32768    /* Maximale Größe des zu entschlüsselnden Textes */
#define MAXPERIOD   200      /* Maximale Periodenl"ange, f"ur die die
				Autokorrelation berechnet wird */

const char *StatisticFileName = "statistik.data";  /* Filename der Wahrscheinlichkeitstabelle */
const char *WorkFile          = "testtext.ciph";   /* Filename des verschlüsselten Textes */

double PropTable[NUMCHARS]; /* Tabellke mit den Zeichenwahrscheinlichkeiten.
			     * ProbTable[0] == 'A', PropTable[1] == 'B' usw. */
char TextArray[MaxFileLen]; /* die eingelesene Datei */
int TextLength;             /* Anzahl der gültigen Zeichen in TextArray */

double AutoCor[MAXPERIOD+1]; /* Normierte Autokorrelationen */
int Period;                  /* berechnete Periodenlaenge */

/*--------------------------------------------------------------------------*/

/*
 * GetStatisticTable(): Liest die Statistik-Tabelle aus dem File
 * STATISTICFILENAME in das globale Array PROPTABLE ein.
 */

static void GetStatisticTable(void)
  {
    FILE *inp;
    int i;
    char line[64];

    if (!(inp=fopen(StatisticFileName,"r"))) {
      fprintf(stderr,"FEHLER: File %s kann nicht geöffnet werden: %s\n",
	      StatisticFileName,strerror(errno));
      exit(20);
    }

    for (i=0; i<TABSIZE(PropTable); i++) {
      fgets(line,sizeof(line),inp);
      if (feof(inp)) {
        fprintf(stderr,"FEHLER: Unerwartetes Dateieine in %s nach %d Einträgen.\n",
		StatisticFileName,i);
	exit(20);
      }
      PropTable[i] = atof(line);
    }
    fclose(inp);
  }

/*-------------------------------------------------------------------------*/

/* GetFile(void) : Ließt den verschlüsselten Text aus dem File
 *   WORKFILE zeichenweise in das globale Array TEXTARRAY ein und zählt
 *   TEXTLENGTH für jedes Zeichen um 1 hoch.
 *   Eingelesen werden nur Buchstaben. Satz- und Sonderzeichen werden weggeworfen,
 *   Kleinbuchstaben werden beim Einlesen in Großbuchstaben gewandelt.
 */

static void GetFile(void)
  {
    FILE *inp;
    char c;

    if (!(inp=fopen(WorkFile,"r"))) {
      fprintf(stderr,"FEHLER: File %s kann nicht geöffnet werden: %s\n",
	      WorkFile,strerror(errno));
      exit(20);
    }

    TextLength=0;
    while (!feof(inp)) {
      c = fgetc(inp);
      if (feof(inp)) break;
      if (c>='a' && c<='z') c -= 32;
      if (c>='A' && c<='Z') {
	if (TextLength >= sizeof(TextArray)) {
	  fprintf(stderr,"FEHLER: Eingabepuffer nach %d Zeichen übergelaufen!\n",TextLength);
	  exit(20);
	}
        TextArray[TextLength++] = c;
      }
    }
    fclose(inp);
  }


/*--------------------------------------------------------------------------*/

/*
 * CountChars( int start, int offset, int h[] )
 *
 * CountChars zählt die Zeichen (nur Buchstaben!) im globalen Feld
 * TEXTARRAY. START gibt an, bei welchen Zeichen (Offset vom Begin der
 * Tabelle) die Zählung beginnen soll und OFFSET ist die Anzahl der
 * Zeichen, die nach dem 'Zählen' eines Zeichens weitergeschaltet
 * werden soll. 'A' wird in h[0], 'B' in h[1] usw. gezählt.
 *  
 *  Beispiel:  OFFSET==3, START==1 --> 1,  4,  7,  10, ....
 *             OFFSET==5, START==3 --> 3,  8, 13,  18, ....
 *
 * Man beachte, daß das erste Zeichen eines C-Strings den Offset 0 besitzt!
 */

static void CountChars( int start, int offset, int h[NUMCHARS])
  {
    int i;
    char c;

    for (i=0; i<NUMCHARS; i++) h[i] = 0;

    /*****************  Aufgabe Teil 3 *****************/
	for (unsigned int j = start; j < TextLength; j += offset) {

		h[TextArray[j] - 65]++;
	}

  }

/*
 * AutoCorrelation (int d)
 *
 * AutoCorrelation berechnet die Autokorrelation im Text mit der Verschiebung
 * (Periode) d.
 *
 * Als Metrik soll die Funktion eingesetzt werden, die bei gleichen Zeichen
 * 0, sonst 1 ergibt. Die Autokorrelation muss hier *nicht* normiert werden.
 * dies geschieht unten in main() im Rahmenprogramm.
 *
 * Der Text steht im Feld TextArray und enthaelt TextLength Zeichen.
 *
 * Das Ergebnis soll als Returnwert zur"uckgegeben werden.
 */

static double AutoCorrelation (int d)
{
  /*************** Aufgabe Teil 1 ******************/

	double cor = 0;
	unsigned int index = 0;
	while ((index + d) < TextLength) {
		if (TextArray[index] == TextArray[index + d]) {
			cor++;
		}
		index++;
	}

	return cor;
}

/*
 * CalcPeriod ()
 *
 * Berechnet (oder liest vom Benutzer ein) die Periode der Chiffre.
 * Das Ergebnis soll in der globalen Variable Period gespeichert werden.
 * Zum Beispiel kann dazu das Array AutoCor, das die vorher berechneten
 * Autokorrelationen (normiert!) enth"alt.
 */

static void CalcPeriod (void)
{
  /*************** Aufgabe Teil 2 ******************/

	unsigned int currTop = 1;
	for (unsigned int i = 1; i < MAXPERIOD + 1; i++) {

		if (AutoCor[i] > AutoCor[currTop]) {
			currTop = i;
		}
	}

	Period = currTop;
	
}

/*------------------------------------------------------------------------------*/

int main(int argc, char **argv)
{

  GetStatisticTable();     /* Wahrscheinlichkeiten einlesen */
  GetFile();               /* zu bearbeitendes File einlesen */

  {
    int i;
    for (i = 0; i <= MAXPERIOD; i++) {
      AutoCor [i] = (double) AutoCorrelation (i) / (TextLength - i);
    }
  }

  /* Now prepare gnuplot */
  {
    FILE *f;
    int i;

    f = fopen (GNUPLOT_CMD_FILENAME, "w");
    if (! f) {
      perror ("Error creating file " GNUPLOT_CMD_FILENAME);
      exit (2);
    }
    fprintf (f, "plot [1:%d] \"%s\" using 0:1 with lines\n", MAXPERIOD,
	     GNUPLOT_DATA_FILENAME);
    fprintf (f, "pause -1 \"Bitte Return druecken...\"\n");
    fclose (f);
    f = fopen (GNUPLOT_DATA_FILENAME, "w");
    if (! f) {
      perror ("Error creating file " GNUPLOT_DATA_FILENAME);
      exit (2);
    }
    for (i = 0; i <= MAXPERIOD; i++) {
      fprintf (f, "%f\n", AutoCor[i]);
    }
    fclose (f);
  }

  /* Now call it */

  //system ("gnuplot " GNUPLOT_CMD_FILENAME);

  CalcPeriod ();
  /*****************  Aufgabe 4 *****************/
  //Schlüssel mit fester Schlüssellänge Period
  char keys[Period];
 
  //Starte Schlüsselberechnung
  for (unsigned int k = 0; k < Period; k++) { 
	  int absoluteVerteilung[NUMCHARS] = { 0 };
	  double relativeVerteilung[NUMCHARS] = { 0 };
	  double kreuzTabelle[NUMCHARS] = { 0 };


	//Berechne absolute Häufigkeiten der Zeichen für k-te Caesar-Chiffre mit fester Schlüssellänge Period
	  CountChars(k, Period, absoluteVerteilung);
  
  

  //Berechne relative Häufigkeiten 
  for (unsigned j = 0; j < NUMCHARS; j++) {
	  relativeVerteilung[j] = (double) absoluteVerteilung[j] / (TextLength / Period);

  }


  //berechne Kreuzkorrelation für k-te Caesar-Chiffre (verschoben um Schlüssel sc)
  for (unsigned sc = 0; sc < NUMCHARS; sc++) {
	  for (unsigned int i = 0; i < NUMCHARS; i++) {

		
		  kreuzTabelle[sc] += (PropTable[i] - relativeVerteilung[(i + sc) % 26]) * (PropTable[i] - relativeVerteilung[(i + sc) % 26]);

	  }
  }

 

  //Index des kleinsten Werts aus kreuzTabelle ist gesuchter Schlüssel der k-ten Caesar-Chiffre
  unsigned int currLow = 0;
  for (unsigned int i = 0; i < NUMCHARS; i++) {
	  if (kreuzTabelle[i] < kreuzTabelle[currLow]) {
		  currLow = i;
		  
	  }
  }
   
  
  keys[k] = (char) (currLow +65);

 
  } // end Schlüsselberechnung
  
  for (unsigned int i = 0; i < Period; i++) {
    printf("%c", keys[i]);
  }
  
  printf("\n");


  return 0;
}
