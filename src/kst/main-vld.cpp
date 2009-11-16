
#include  <windows.h>

// Visual Leak Detector
// http://sites.google.com/site/dmoulding/vld
#include "vld.h"


typedef int (*Main)(int argc, char *argv[]);

int main(int argc, char *argv[]) 
{
  // build kst as Dll
  HMODULE kst = LoadLibraryA("kst2d.exe");

  Main m = (Main)GetProcAddress(kst, "main");
  
  return (*m)(argc, argv);
}

