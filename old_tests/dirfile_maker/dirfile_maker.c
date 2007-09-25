#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

struct DFEntryType {
  char field[17];
  int spf;
  int fp;
};

#define NDF 5
#define SCOUNT 0
#define FCOUNT 1
#define SINE 2
#define SSINE 3
#define COS 4

struct DFEntryType df[NDF] = {
  {"scount", 1, -1},
  {"fcount", 20, -1},
  {"sine", 20, -1},
  {"ssine", 1, -1},
  {"cos", 20, -1}
};
  
int main() {
  char dirfilename[120];
  char tmpstr[150];
  FILE *fpf;
  int i, count = 0;
  float x;

  sprintf(dirfilename, "%d.dm", time(NULL));

  printf("Writing dirfile %s\n", dirfilename);
  printf("The fields are:\n");
  for (i=0; i<NDF; i++) {
    printf("%16s %2d samples per frame\n", df[i].field, df[i].spf);
  }
  
  if (mkdir(dirfilename, 00755) < 0) {
    perror("dirfile mkdir()");
    exit(0);
  }
  
  sprintf(tmpstr,"%s/format", dirfilename);

  fpf = fopen(tmpstr,"w");
  
  for (i=0; i<NDF; i++) {
    fprintf(fpf,"%s RAW f %d\n", df[i].field, df[i].spf);
    sprintf(tmpstr,"%s/%s", dirfilename, df[i].field);
    df[i].fp = open(tmpstr, O_WRONLY|O_CREAT, 00644);
  }
  fclose(fpf);

  /* make curfile */
  fpf = fopen("dm.cur", "w");
  fprintf(fpf,"%s", dirfilename);
  fclose(fpf);

  while (1) {
    /* write 'count' */
    x = count;
    write(df[SCOUNT].fp, &x, sizeof(float));

    /* write 'fcount' */
    for (i=0; i<df[FCOUNT].spf; i++) {
      x = count*df[FCOUNT].spf+i;
      write(df[FCOUNT].fp, &x, sizeof(float));
    }

    /* write 'sine' */
    for (i=0; i<df[SINE].spf; i++) {
      x = count*df[SINE].spf+i;
      x = sin(2.0*M_PI*x/100.0);
      write(df[SINE].fp, &x, sizeof(float));
    }
    
    /* write 'ssine' */
    for (i=0; i<df[SSINE].spf; i++) {
      x = count*df[SSINE].spf+i;
      x = sin(2.0*M_PI*x/100.0);
      write(df[SSINE].fp, &x, sizeof(float));
    }
    
    /* write 'cos' */
    for (i=0; i<df[COS].spf; i++) {
      x = count*df[COS].spf+i;
      x = cos(2.0*M_PI*x/100.0);
      write(df[COS].fp, &x, sizeof(float));
    }
    printf("writing frame %d  \r", count);
    fflush(stdout);
    usleep(200000);
    count++;
  }
  return (42);
}
