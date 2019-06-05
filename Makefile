.PHONY : clean




SRCDIR := src
BUILDDIR := build
LIBDIR :=lib

CC := g++
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
LIB := -lm -L lib -lcvdetect
INC := -I include
CPPFLAGS=   -pipe -stdlib=libc++ -g -std=gnu++11  -mmacosx-version-min=10.8 -Wall -W -fPIC -DCVDETECT_LIBRARY
LDFLAGS= -single_module -dynamiclib -compatibility_version	1.0 -current_version	1.0.0 -install_name $(LIBDIR)/libcvdetect.dylib


LIBTARGET=$(LIBDIR)/libcvdetect.dylib
TARGET=$(BUILDDIR)/test


all:  $(TARGET)

$(TARGET):  $(LIBTARGET) test.o
	@echo " Linking..."
	@echo " $(CC) $(BUILDDIR)/test.o -o $(TARGET) $(LIB)"; $(CC) $(BUILDDIR)/test.o  -o $(TARGET) $(LIB)



$(LIBTARGET) : $(OBJECTS)
	$(CC) $(LDFLAGS)  -o $(LIBTARGET) $(OBJECTS)


$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CPPFLAGS)  $(INC) -c -o $@ $<"; $(CC) $(CPPFLAGS) $(INC) -c -o $@ $<

test.o:
	@echo " test compilation ..."
	$(CC) $(CPPFLAGS) $(INC) -c -o $(BUILDDIR)/test.o $(SRCDIR)/test.cpp

clean:
	rm -f $(OBJECTS) $(TARGET) $(LIBTARGET) $(BUILDDIR)/test.o
