#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <string.h>


#ifdef _WIN32
#define mkdir(x, y) mkdir(x)
#endif

struct DFEntryType {
  char field[17];
  int spf;
  int fp;
  char filename[150];
};

int main(int argc, char *argv[]) {
  char dirfilename[120];
  char tmpstr[150];
  FILE *fpf;
  int i, count = 0;
  float x;

  int frameTypes = 10;
  int maxFrames = -1;
  int delay = 0;
  if (argc > 1) {
    frameTypes = atoi(argv[1]);
    printf("Requested Frame Types of %d\n", frameTypes);
  } else {
    printf("Frame Type count not specified, defaulting to 10 types\n");
  }

  if (argc > 2) {
    maxFrames = atoi(argv[2]);
    printf("Requested maximum frames of %d\n", maxFrames);
  } else {
    printf("No Maximum frame count specified, running until aborted\n");
  }

  if (argc > 3) {
    delay = atoi(argv[3]);
    printf("Delay between updates is %d ms\n", delay);
  } else if (maxFrames == -1) {
    delay = 50;
    printf("No Delay specified, and no max frames, defaulting to 200 ms\n");
  } else {
    printf("No Delay specified running without delay\n");
  }

  //struct DFEntryType dfList2[frameTypes];

  struct DFEntryType dfList[frameTypes];
  for (i=0; i<frameTypes; i++) {
    //char idName[20];
    sprintf(dfList[i].field, "%dxINDEX", i+1);
    dfList[i].spf = 1;
    dfList[i].fp = -1;
  }

  sprintf(dirfilename, "%d.dm", (int) time(NULL));

  printf("Writing dirfile %s\n", dirfilename);
  printf("The fields are:\n");
  for (i=0; i<frameTypes; i++) {
    printf("%16s %2d samples per frame\n", dfList[i].field, dfList[i].spf);
  }

  if (mkdir(dirfilename, 00755) < 0) {
    perror("dirfile mkdir()");
    exit(0);
  }

  sprintf(tmpstr,"%s/format", dirfilename);

  fpf = fopen(tmpstr,"w");

  for (i=0; i<frameTypes; i++) {
    fprintf(fpf,"%s RAW f %d\n", dfList[i].field, dfList[i].spf);
    sprintf(tmpstr,"%s/%s", dirfilename, dfList[i].field);
#ifndef _WIN32
    dfList[i].fp = open(tmpstr, O_WRONLY|O_CREAT , 00644);
#endif
    strcpy(dfList[i].filename, tmpstr);
  }

  fclose(fpf);

  /* make curfile */
  fpf = fopen("dm.cur", "w");
  fprintf(fpf,"%s", dirfilename);
  fclose(fpf);

  while (1) {

    for (i=0; i<frameTypes; i++) {
      x = count * (i+1);
#ifndef _WIN32
      write(dfList[i].fp, &x, sizeof(float));
#else
      FILE *fd = fopen(dfList[i].filename, "a");
      fwrite(&x, sizeof(float), 1, fd);
      fclose(fd);
#endif
    }

    printf("writing frame %d  \r", count);
    fflush(stdout);
    if (maxFrames != -1) {
      if (count == maxFrames) {
        break;
      }
    }
    usleep(delay*1000);
    count++;
  }
  return (42);
}
