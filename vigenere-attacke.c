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
    //char c;

    for (i=0; i<NUMCHARS; i++) {
      h[i] = 0;
    }
    for (i=start; i < TextLength; i += offset) {
      // convert to ASCII-Code
      int letterToInt = TextArray[i]-65;
      //printf("%i : Nr. %i\n", TextArray[i], letterToInt);
      h[letterToInt]++;
    }

    //}
    /*****************  Aufgabe Teil 3 *****************/
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
  double count = 0;
  for (int i = 0; i < TextLength-d; i++) {
    if (TextArray[i] == TextArray[i+d]) count++;
  }
  return count;
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
  for (int i = 1; i < MAXPERIOD+1; i++) {
    if (AutoCor[i] > 0.05f) {
      Period = i;
      return;
    }
  }
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

  system ("gnuplot " GNUPLOT_CMD_FILENAME);

  CalcPeriod ();

  /*****************  Aufgabe 4 *****************/
  int counts1[NUMCHARS];
  int counts2[NUMCHARS];
  int counts3[NUMCHARS];
  int counts4[NUMCHARS];
  int counts5[NUMCHARS];
  int counts6[NUMCHARS];
  int counts7[NUMCHARS];
  int counts8[NUMCHARS];
  int counts9[NUMCHARS];
  int counts10[NUMCHARS];
  int counts11[NUMCHARS];
  int counts12[NUMCHARS];
  int counts13[NUMCHARS];
  int counts14[NUMCHARS];
  
  CountChars(0, Period, counts1);
  CountChars(1, Period, counts2);
  CountChars(2, Period, counts3);
  CountChars(3, Period, counts4);
  CountChars(4, Period, counts5);
  CountChars(5, Period, counts6);
  CountChars(6, Period, counts7);
  CountChars(7, Period, counts8);
  CountChars(8, Period, counts9);
  CountChars(9, Period, counts10);
  CountChars(10, Period, counts11);
  CountChars(11, Period, counts12);
  CountChars(12, Period, counts13);
  CountChars(13, Period, counts14);

  int maxNum = 0;
  int maxI = 0;
  printf("1.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts1[i] > maxNum) {
      maxNum = counts1[i];
      maxI = i;
      printf("neu mit %i: %i\n", maxI, maxNum);
    }
    printf("%i: %i, ", i, counts1[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("2.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts2[i] > maxNum) {
      maxI = i;
      maxNum = counts2[i];
    }
    printf("%i: %i, ", i, counts2[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("3.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    maxI = 0;
    maxNum = 0;
    if (counts3[i] > maxNum) {
      maxI = i;
      maxNum = counts3[i];
    }
    printf("%i: %i, ", i, counts3[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("4.)\n");
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts4[i] > maxNum) {
      maxI = i;
      maxNum = counts4[i];
    }
    printf("%i: %i, ", i, counts4[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("5.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts5[i] > maxNum) {
      maxI = i;
      maxNum = counts5[i];
    }
    printf("%i: %i, ", i, counts5[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("6.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts6[i] > maxNum) {
      maxI = i;
      maxNum = counts6[i];
    }
    printf("%i: %i, ", i, counts6[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("7.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts7[i] > maxNum) {
      maxI = i;
      maxNum = counts7[i];
    }
    printf("%i: %i, ", i, counts7[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("8.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts8[i] > maxNum) {
      maxI = i;
      maxNum = counts8[i];
    }
    printf("%i: %i, ", i, counts8[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("9.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts9[i] > maxNum) {
      maxI = i;
      maxNum = counts9[i];
    }
    printf("%i: %i, ", i, counts9[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("10.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts10[i] > maxNum) {
      maxI = i;
      maxNum = counts10[i];
    }
    printf("%i: %i, ", i, counts10[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("11.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts11[i] > maxNum) {
      maxI = i;
      maxNum = counts11[i];
    }
    printf("%i: %i, ", i, counts11[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("12.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts12[i] > maxNum) {
      maxI = i;
      maxNum = counts12[i];
    }
    printf("%i: %i, ", i, counts12[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("13.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts13[i] > maxNum) {
      maxI = i;
      maxNum = counts13[i];
    }
    printf("%i: %i, ", i, counts13[i]);
  }
  printf("Max at (%i) with %i\n", maxI, maxNum);
  printf("14.)\n");
  maxI = 0;
  maxNum = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    if (counts14[i] > maxNum) {
      maxI = i;
      maxNum = counts14[i];
    }
    printf("%i: %i, ", i, counts14[i]);
  }
  
  return 0;
}
