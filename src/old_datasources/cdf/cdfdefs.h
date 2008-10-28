/******************************************************************************
*
*  Header extracted from the cdfdist.h file found in the CDF distribution
*
*  Unfortunately, this header is not installed by default and some of its
*  definitions being useful, I have extracted them locally to this file
*  to allow compilation of the cdf datasource for kst in all cases.
*
*  Nicolas Brisset <nicodev@users.sourceforge.net> 11/05/2004
*
******************************************************************************/

#if !defined(CDFDISTh_INCLUDEd__)
#  define CDFDISTh_INCLUDEd__

/******************************************************************************
* Definitions for misguided compilers, etc.
******************************************************************************/

#if defined(WIN32)
#  define IBMPC
#  define win32
#endif

#if defined(MSDOS)              /* Microsoft C. */
#  define dos
#  define MICROSOFTC
#  if _MSC_VER == 600
#    define MICROSOFTC_600
#  endif
#  if _MSC_VER == 700
#    define MICROSOFTC_700
#  endif
#endif

#if defined(__MSDOS__)          /* Borland C. */
#  define dos
#  define BORLANDC
#endif

#if defined(__SALFORD__)        /* Salford C. */
#  define dos 1
#  define SALFORDC 1
#endif

#if defined(dos)
#  define IBMPC
#endif

#if defined(NeXT)
#  define Mach
#endif

#if defined(__ppc__) || defined(__APPLE__)
#  define POWERPC
#  define unix
#  if defined(__MACH__)  	/* Mac OS X cc (gcc) */
#    define macosX
#  endif
#endif

#if defined(AIX)
#  define unix
#endif

#if defined(linux) || defined(__CYGWIN__)
#  if defined(PPC)
#    define POWERPC
#  else
#    if defined(i386)
#      define IBMPC
#    else
#      if defined(__alpha)
#        define alphaosf
#      endif
#    endif
#  endif
#endif

#if defined(__QNX__)
#  define IBMPC
#  define unix
#endif

#if defined(sun) && defined(i386)
#  define IBMPC
#  undef sun
#endif

#if defined(__alpha)
#  if defined(__osf__)
#    define alphaosf
#  else
#    if defined(vms)
#      define alphavms
#      if __D_FLOAT
#        define alphavmsD
#      endif
#      if __G_FLOAT
#        define alphavmsG
#      endif
#      if __IEEE_FLOAT
#        define alphavmsI
#      endif
#    else           /* "vms" not defined if "-W ansi89" used. */
#      if !defined(linux)
#        define posixSHELL
#        define posixSHELLalpha
#        if __D_FLOAT
#          define posixSHELLalphaD
#        endif
#        if __G_FLOAT
#          define posixSHELLalphaG
#        endif
#        if __IEEE_FLOAT
#          define posixSHELLalphaI
#        endif
#      endif
#    endif
#  endif
#endif

#if defined(vax)
#  if defined(vms)
#    define vaxvms
#  else           /* "vms" not defined if "-W ansi89" used. */
#    define posixSHELL
#    define posixSHELLvax
#  endif
#endif

#if defined(macintosh) || defined(__MWERKS__) /* Macintosh MPW or MetroWerks CodeWarrior */
#  define MPW_C
#  define mac

#  if defined(__MWERKS__)			/* MetroWerks CodeWarrior */
#    define STDARG

#    ifdef OLDROUTINENAMES 
#      undef OLDROUTINENAMES
#    endif
#    define OLDROUTINENAMES 1

#    ifdef TARGET_OS_MAC 
#      undef TARGET_OS_MAC
#    endif
#    define TARGET_OS_MAC 1
#  endif
#endif

#if defined(THINK_C)            /* Macintosh Think C */
#  define mac
#endif

#if defined(HPUXposix)
#  define unix
#  define hpux
#  define HP
#  define _HPUX_SOURCE
#endif


/******************************************************************************
* Typedefs.
******************************************************************************/

typedef int Logical;

typedef unsigned short uShort;
typedef unsigned int uInt;
typedef unsigned long uLong;

#if defined(alphaosf) || defined(IRIX64bit)
   /***************************************************************************
   * `long' is 64 bits on a DEC Alpha/OSF1 and an SGi/IRIX 6.x using the `-64'
   * option (64-bit objects).  `int' is 32 bits in these cases.
   ***************************************************************************/
   typedef int Int32;
   typedef uInt uInt32;
#else
   typedef long Int32;
   typedef uLong uInt32;
#endif

typedef short Int16;
typedef uShort uInt16;

#if defined(AIX) || defined(dos) || defined(sgi) || defined(__QNX__)
   /***************************************************************************
   * `char' is unsigned by default under AIX, MS-DOS, IRIX, and QNX...
   ***************************************************************************/
   typedef signed char sChar;
#else
   /***************************************************************************
   * ...and signed by default everywhere else.
   ***************************************************************************/
   typedef char sChar;
#endif

typedef unsigned char uChar;

#if !defined(mac)
  /****************************************************************************
  * `Byte' is already defined in `types.h' on a Macintosh (MPW C & Think C).
  ****************************************************************************/
  typedef uChar Byte;
#endif

typedef uChar uByte;
typedef sChar sByte;


/******************************************************************************
* BOO.
*   Choose one of two things based on a boolean value.
******************************************************************************/

#define BOO(b,t,f) (b ? t : f)



#endif
