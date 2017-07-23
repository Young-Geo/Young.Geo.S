
MAKEFILE      = Makefile


CC            = gcc
CXX           = g++
CFLAGS        = -pipe -g -Wall -W -fPIC $(DEFINES)
CXXFLAGS      = -pipe -g -std=gnu++11 -Wall -W -fPIC $(DEFINES)
DEL_FILE      = rm -f
LINK          = g++

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = starter.c \
		sou/m_afx.c \
		sou/xchain.c \
		sou/xkeyfile.c \
		sou/xkeyfilerw.c \
		sou/xlist.c \
		sou/get_cpu.c

OBJECTS       = main.o \
		m_afx.o \
		xchain.o \
		xkeyfile.o \
		xkeyfilerw.o \
		xlist.o \
		get_cpu.o
QMAKE_TARGET  = Starter
DESTDIR       = 
TARGET        = Starter


first: all
####### Build rules

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)



all: Makefile $(TARGET)




clean: compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


distclean: clean 
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) .qmake.stash
	-$(DEL_FILE) Makefile

compiler_clean:


main.o: starter.c inc/xkeyfilerw.h \
		inc/xkeyfile.h \
		inc/xlist.h \
		inc/m_afx.h \
		inc/xchain.h \
		inc/get_cpu.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o main.o starter.c


m_afx.o: sou/m_afx.c inc/m_afx.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o m_afx.o sou/m_afx.c

xchain.o: sou/xchain.c inc/xchain.h \
		inc/m_afx.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o xchain.o sou/xchain.c

xkeyfile.o: sou/xkeyfile.c inc/xkeyfile.h \
		inc/xlist.h \
		inc/m_afx.h \
		inc/xchain.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o xkeyfile.o sou/xkeyfile.c

xkeyfilerw.o: sou/xkeyfilerw.c inc/xkeyfilerw.h \
		inc/xkeyfile.h \
		inc/xlist.h \
		inc/m_afx.h \
		inc/xchain.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o xkeyfilerw.o sou/xkeyfilerw.c

xlist.o: sou/xlist.c inc/xlist.h \
		inc/m_afx.h \
		inc/xchain.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o xlist.o sou/xlist.c

get_cpu.o: sou/get_cpu.c inc/get_cpu.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o get_cpu.o sou/get_cpu.c

####### Install

install:  FORCE

uninstall:  FORCE

FORCE:

