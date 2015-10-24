#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

struct DFEntryType {
  char field[17];
  int spf;
  int fp;
  char type;
};

#define NDF 15
#define SCOUNT 0
#define FCOUNT 1
#define SINE 2
#define SSINE 3
#define COS 4
#define TIME 5
#define EXTRA 6

struct DFEntryType df[NDF] = {
  {"scount", 1, -1, 'f'},
  {"fcount", 20, -1, 'f'},
  {"sine", 20, -1, 'f'},
  {"ssine", 1, -1, 'f'},
  {"cos", 20, -1, 'f'},
  {"time", 20, -1, 'd'},
  {"E0", 20, -1, 'f'},
  {"E1", 20, -1, 'f'},
  {"E2", 20, -1, 'f'},
  {"E3", 20, -1, 'f'},
  {"E4", 20, -1, 'f'},
  {"E5_test", 20, -1, 'f'},
  {"E6_test", 20, -1, 'f'},
  {"E7[m]", 20, -1, 'f'},
  {"E8^2", 20, -1, 'f'}
};
  
int main() {
  char dirfilename[120];
  char tmpstr[150];
  FILE *fpf;
  int i, count = 0;
  int j;
  float x;
  double dx;
  
  struct timeval tv;
  
  sprintf(dirfilename, "%lu.dm", time(NULL));

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
  /* make curfile */
  unlink("dm.lnk");
  if (symlink(dirfilename, "dm.lnk")<0) {
    fprintf(stderr, "symlink of %s to dm.lnk failed\n", dirfilename);
  }

  sleep(1);
  for (i=0; i<NDF; i++) {
    fprintf(fpf,"%s RAW %c %d\n", df[i].field, df[i].type, df[i].spf);
    fflush(fpf);
    usleep(100000);
    sprintf(tmpstr,"%s/%s", dirfilename, df[i].field);
    df[i].fp = open(tmpstr, O_WRONLY|O_CREAT, 00644);
  }

  fprintf(fpf,"CONST1 CONST FLOAT32 16.4\nCONST2 CONST DOUBLE 3.14159\n"
  "STR1 STRING Test\\ String\n");
  fprintf(fpf,"META cos metaC CONST FLOAT64 3.291882\n"
  "META cos metaS STRING Test_String\n");

  fprintf(fpf, "cos/units STRING ^o\ncos/quantity STRING Angle\n");
  fclose(fpf);


  printf("starting loop\n");
  while (1) {
    int nw;
    /* write 'fcount' */
    for (i=0; i<df[FCOUNT].spf; i++) {
      x = count*df[FCOUNT].spf+i;
      nw = write(df[FCOUNT].fp, &x, sizeof(float));
      if (nw<0) {
        fprintf(stderr, "error writing fcount\n");
      }
    }

    /* write 'sine' */
    for (i=0; i<df[SINE].spf; i++) {
      dx = count*df[SINE].spf+i;
      x = sin(2.0*M_PI*dx/100.0);
      nw = write(df[SINE].fp, &x, sizeof(float));
      if (nw<0) {
        fprintf(stderr, "error writing sin\n");
      }
    }
    
    /* write 'ssine' */
    for (i=0; i<df[SSINE].spf; i++) {
      dx = count*df[SSINE].spf+i;
      x = sin(2.0*M_PI*dx/100.0);
      nw = write(df[SSINE].fp, &x, sizeof(float));
      if (nw<0) {
        fprintf(stderr, "error writing ssine\n");
      }

    }
    
    /* write 'cos' */
    for (i=0; i<df[COS].spf; i++) {
      dx = count*df[COS].spf+i;
      x = cos(2.0*M_PI*dx/100.0);
      nw = write(df[COS].fp, &x, sizeof(float));
      if (nw<0) {
        fprintf(stderr, "error writing cos\n");
      }

    }
  
    gettimeofday(&tv, 0);
    for (i=0; i<df[TIME].spf; i++) {
      dx = (double)tv.tv_sec +( double)(tv.tv_usec)/1000000.0 + (double)i/100.0;
      nw = write(df[TIME].fp, &dx, sizeof(double));
      if (nw<0) {
        fprintf(stderr, "error writing time\n");
      }

    }
    
    /* write extras */
    for (j=6; j<NDF; j++) {
      for (i=0; i<df[j].spf; i++) {
        x = (double)rand()/(double)RAND_MAX;
        nw = write(df[j].fp, &x, sizeof(float));
        if (nw<0) {
          fprintf(stderr, "error writing E%d\n", j);
        }

      }
    }

    /* write 'count' */
    x = count;
    nw = write(df[SCOUNT].fp, &x, sizeof(float));
    if (nw<0) {
      fprintf(stderr, "error writing count\n");
    }


    printf("writing frame %d  \r", count);
    fflush(stdout);
    usleep(200000);
    count++;
  }
  return (0);
}
