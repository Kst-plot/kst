#
#  Hand-written Makefile until we have an auto*-generated one :-)
# (based on Ingo Berg's original Makefile for the muParser)
#

CC	=	g++ 

#Release Version
#CFLAGS	=	-mcpu=pentium -O3 -ffast-math -fomit-frame-pointer -fexpensive-optimizations
#CFLAGS	=	-mcpu=supersparc -O3 -ffast-math -fomit-frame-pointer -fexpensive-optimizations
# the -fomit-frame-pointer flag seems to create wrong code, better remove it... 
# the -mcpu=... flag works, but it will make the Makefile generation more complicated. 
# Not sure it is worh the trouble.
CFLAGS	=	-O3 -fPIC -ffast-math -fexpensive-optimizations

#Debug-Version
#CFLAGS	=	-g -Wall 

OBJ_LIB      = 	muParser.o \
                muParserBase.o \
                muParserError.o \
                muParserCallback.o \
                muParserTokenReader.o \
                muParserBytecode.o

TARGET_LIB   =  muparser
KST_PLUGIN   =  kstfit_general_levenberg_marquardt.so

INCDIR	     =  -I. -I/usr/local/kde/include
#LINK_LIBS          =  -L. -lmuparser -L/usr/local/kde/lib -lgsl
LINK_LIBS          =  libmuparser.a -L/usr/local/kde/lib -lgsl -lgslcblas

###  Parser - list of targets ###################################################

all:	$(KST_PLUGIN)


###  Parser - library  ##########################################################
#
# generate the parser library
#
		
$(TARGET_LIB): $(OBJ_LIB)
	@ar ru lib$(TARGET_LIB).a $(OBJ_LIB)
	@ranlib lib$(TARGET_LIB).a
	@$(CC) -shared -o lib$(TARGET_LIB).so $(OBJ_LIB)
	
$(KST_PLUGIN): $(TARGET_LIB) kstfit_general_levenberg_marquardt.o
	@echo Linking...
	@echo $(CC) -shared -o $(KST_PLUGIN) kstfit_general_levenberg_marquardt.o $(LINK_LIBS)
	@$(CC) -shared -o $(KST_PLUGIN) kstfit_general_levenberg_marquardt.o $(LINK_LIBS)

###  Parser - secondary compile targets #########################################

%.o:	%.cpp
	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

clean:
	@echo -n "cleaning up..."	
	@rm -f $(TARGET_LIB) *.[ao] *~ &> /dev/null
	@rm -f DEADJOE core &> /dev/null
	@rm -f $(OBJ_LIB)
	@echo    "done"

new:	
	$(MAKE) clean
	$(MAKE) all
