# Reference:
# http://hpcf.nersc.gov/training/tutorials/make/make.html
#

# ---------------------
# Your current directory
# ---------------------
DIST_DIR = .

# ----------------------------------------------
# Modify the following entries for different lab
# ----------------------------------------------
TARGET	= lab2

# ------
# Comment it out if you need to include this file
# ------
#H	= include.h


# ----------------------
# System dependent paths
# ----------------------
CPLUS 	= g++ 
GLUT = /usr/local/glut-3.7b
GLUT_INC = $(GLUT)/include
GLUT_LIB_PATH = $(GLUT)/lib/glut
INCS = -g -I./ -I/usr/include -I/usr/local/gcc-2.95.2/include/g++-3 -I$(GLUT_INC) 

OPENGL_LIB = -lGLw -lGLU -lGL
GLUT_LIB = -lglut 

XLIB = -lXm -lXt -lXext -lX11 -lXmu
OTHER_LIB = -lm

LIBS = -L/lib -L/usr/lib -L$(GLUT_LIB_PATH) $(OPENGL_LIB) $(XLIB) $(OTHER_LIB) $(GLUT_LIB) 
CFLAGS	= $(INCS)

#------------------------------------------------------------------
all	:	$(TARGET)

depend	:
	makedepend -f- -D__cplusplus $(INCLUDES) $(SRC) | \
	sed -e 's@\(..*/\)\(..*\.o\)@obj/\2@' -e 's@ [^ ]*\.p@@g' > Makefile.depend

etags	:
	etags $(SRC) $(H)

# ------------------------------------------------------------------
#
# ------------------------------------------------------------------
lab2	:	lab2.o matrix.o polygon.o line.o
	$(CPLUS) -o lab2 lab2.o matrix.o polygon.o line.o $(LIBS)

matrix.o	:	matrix.cc
	$(CPLUS) -c $(INCS) matrix.cc

polygon.o	:	polygon.cc
	$(CPLUS) -c $(INCS) polygon.cc

line.o	:	line.cc 
	$(CPLUS) -c $(INCS) line.cc

lab2.o	:	lab2.cc	matrix.h polygon.h point.h line.h
	$(CPLUS) -c $(INCS) lab2.cc matrix.cc polygon.cc line.cc
clean	:
	-rm core $(OBJ) *~ *.o

