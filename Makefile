.PHONY : clean




SRCDIR := src
BUILDDIR := build
LIBDIR :=lib


ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CCFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CCFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CCFLAGS += -D IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CPPFLAGS :=  -pipe  -g -std=gnu++11  -Wall -W -fPIC -DCVDETECT_LIBRARY
				LDFLAGS :=  -shared
				LIBTARGET := $(LIBDIR)/libcvdetect.so

    endif
    ifeq ($(UNAME_S),Darwin)
        CPPFLAGS=   -pipe -stdlib=libc++ -g -std=gnu++11  -mmacosx-version-min=10.8 -Wall -W -fPIC -DCVDETECT_LIBRARY
				LDFLAGS= -single_module -dynamiclib -compatibility_version	1.0 -current_version	1.0.0 -install_name $(LIBDIR)/libcvdetect.dylib
				LIBTARGET := $(LIBDIR)/libcvdetect.dylib
    endif

endif

CC := g++
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
SOURCES :=$(filter-out src/cvtest.cpp, $(SOURCES))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
LIB := -lm -L lib -lcvdetect
INC := -I include





TARGET=$(BUILDDIR)/test


all:  $(TARGET)

$(TARGET):  $(LIBTARGET) test.o
	@echo " Creating test"
	@echo " $(CC) $(BUILDDIR)/test.o -o $(TARGET) $(LIB)"; $(CC) $(BUILDDIR)/test.o  -o $(TARGET) $(LIB)

cvtest: $(LIBTARGET)
	g++ -o build/cvtest src/cvtest.cpp -lcvdetect -L lib `pkg-config --cflags  opencv` -I include -L/usr/local/lib -L/usr/local/share/OpenCV/3rdparty/lib -lopencv_contrib -lopencv_stitching  -lopencv_superres -lopencv_ocl -lopencv_ts -lopencv_videostab -lopencv_gpu -lopencv_photo -lopencv_objdetect -lopencv_legacy -lopencv_video -lopencv_ml -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lIlmImf   -lpng -ljpeg -lopencv_imgproc -lopencv_flann -lopencv_core

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
