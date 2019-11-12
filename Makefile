
CPPFLAGS = -O3 -fopenmp -fPIC 
CPPFLAGS += -I$(CGAL_DIR)/include
CPPFLAGS += -I$(BOOST_DIR)/include
CPPFLAGS += -I$(MPFR_DIR)/include
CPPFLAGS += -I$(GMP_DIR)/include

CPPFLAGS += -I$(QGLVIEWER_DIR)/include
CPPFLAGS += -I$(QTDIR)/include -I$(QTDIR)/include/QtCore -I$(QTDIR)/include/QtWidgets -I$(QTDIR)/include/QtXml -I$(QTDIR)/include/QtOpenGL -I$(QTDIR)/include/QtGui


LIBS  = -L$(CGAL_DIR)/lib -lCGAL
LIBS += -L$(MPFR_DIR)/lib -lmpfr
LIBS += -L$(GMP_DIR)/lib -lgmp

LIBS += -L$(QTDIR)/lib -lQt5Core -lQt5Xml -lQt5OpenGL -lQt5Widgets -lQt5Gui -lGL -lGLU
LIBS += -L$(QGLVIEWER_DIR)/lib -lQGLViewer

rslc:RadialSlicer.o
	g++ -o rslc RadialSlicer.o $(LIBS)

rimg:RadialImage.o
	g++ -o rimg RadialImage.o $(LIBS)

.o:.cpp
	g++ $(CPPFALGS) $<

clean:
	\rm -rf *.o rot*.off

