#include "moonlib.h"

#define YEARS_TO_RENDER 10
#define MOONPHASE_ARRAY_SIZE 365*YEARS_TO_RENDER

/*  Main program  */

int main(int argc, char *argv[])
{
  time_t t;
  long jmoonepic, jd;
  struct tm *gm;
  int yy, mm, dd;
  static char *moname[] = {"January", "February", "March",
           "April", "May", "June", "July", "August", "September",
           "October", "November", "December"};
  double p, aom, cphase, lastcphase, cdist, cangdia, csund, csuang, lptime;

  time(&t);
  gm = gmtime(&t);
  jmoonepic = jdate(gm);
  jmoonepic--; /* Make sure that with GMT that we still have today */
  jyear(jmoonepic, &yy, &mm, &dd);
  printf( "#define JULIAN_MOON_EPIC %ld /* %d %s %d */\n", jmoonepic, dd, moname[mm - 1], yy);
  printf( "#define MOONPHASE_ARRAY_SIZE %d\n\n", MOONPHASE_ARRAY_SIZE);
  printf("static uint8_t MoonPhaseDateLookup[%d][2] =\n{\n\t/* {Julian Date-JULIAN_MOON_EPIC = array position, Phase (0-14), Waxing (1 - Yes, 0 - Waning)} */\n", MOONPHASE_ARRAY_SIZE);
  p = phase(jmoonepic-1, &lastcphase, &aom, &cdist, &cangdia, &csund, &csuang);
  for (jd=jmoonepic; jd<(jmoonepic+MOONPHASE_ARRAY_SIZE); jd++)
  {
     jyear(jd, &yy, &mm, &dd);
     p = phase(jd, &cphase, &aom, &cdist, &cangdia, &csund, &csuang);
     printf( "\t{%d, %d}%c /* %ld - %d %s %d - %d%%  */\n", myround(cphase*14),((lastcphase < cphase) ? 1 : 0),jd==jmoonepic+MOONPHASE_ARRAY_SIZE-1 ? ' ' : ',',jd,dd, moname[mm - 1], yy,(int) (cphase * 100));
     lastcphase = cphase;
  }
  printf ("};\n");

}
