# value       : compiler
# ----------- : -----------------------------
# none        : <- default; make will fail
# gcc         : gcc++-4.7
# clang       : clang++
# mingwgcc    : g++
# macportsgcc : g++-mp-4.7

# modify the following line so COMPILER is set to an
# appropriate value (gcc,clang,mingwgcc,macportsgcc)
COMPILER = none




# set up general compiler flags

CFLAGS  = -c -Wall -std=c++11 -DGL_GLEXT_PROTOTYPES -I. -Isrc/ -Isrc/ext/ -Isrc/common/ -g
LDFLAGS = -std=c++11 
#LIBS    = -lXmu -lXi




# set up compiler-specific flags (set COMPILER above)

ifeq ($(COMPILER),gcc)
	CC       = g++-4.7
	CFLAGS  += -Wno-sign-compare
	LIBS     = -lGL -lGLU -lglut
endif

ifeq ($(COMPILER),clang)
	CC       = clang++
	CFLAGS  +=  -stdlib=libc++
	LDFLAGS += -stdlib=libc++ -lc++abi
	LIBS     = -framework Carbon -framework OpenGL -framework GLUT
endif

ifeq ($(COMPILER),mingwgcc)
	CC       = g++
	CFLAGS  += -DFREEGLUT_STATIC -DGLEW_STATIC -Iext_win32/include -Iext_win32/include/GL -Wno-sign-compare
	LIBS     = -Lext_win32/lib -lfreeglut32_static -lglew32 -lopengl32 -lglu32 -lgdi32 -lwinmm 
endif

ifeq ($(COMPILER),macportsgcc)
	CC       = g++-mp-4.7
	LIBS     = -framework Carbon -framework OpenGL -framework GLUT
endif

ifeq ($(COMPILER),none)
	COMPILERCHECK = $(error Set COMPILER variable!)
else
	COMPILERCHECK = $(info COMPILER = $(COMPILER))
endif




# define source files

COMMONSOURCES = \
	src/common/debug.cpp src/common/json.cpp \
	src/ext/lodepng/lodepng.cpp \
	src/igl/accelerator.cpp src/igl/camera.cpp \
	src/igl/distraytrace.cpp src/igl/draw.cpp \
	src/igl/gizmo.cpp src/igl/gl_utils.cpp \
	src/igl/image.cpp src/igl/intersect.cpp src/igl/keyframed.cpp \
	src/igl/light.cpp src/igl/material.cpp src/igl/node.cpp \
	src/igl/pathtrace.cpp src/igl/primitive.cpp src/igl/raytrace.cpp \
	src/igl/scene.cpp src/igl/serialize.cpp src/igl/shape.cpp \
	src/igl/tesselate.cpp src/igl/texture.cpp \
	src/vmath/geom.cpp src/vmath/interpolate.cpp
COMMONOBJECTS = $(COMMONSOURCES:.cpp=.o)
SOURCES = \
	src/apps/view.cpp src/apps/trace.cpp \
	$(COMMONSOURCES)
OBJECTS = $(SOURCES:.cpp=.o)
INCLUDES = $(wildcard src/vmath/*.h) $(wildcard src/igl/*.h) $(wildcard src/ext/*.h) $(wildcard src/ext/tclap/*.h) $(wildcard src/ext/lodepng/*.h) $(wildcard src/common/*.h)



# define targets and build rules

all: compilercheck $(SOURCES) view trace

view: $(OBJECTS)
	$(CC) src/apps/view.o $(COMMONOBJECTS) $(LDFLAGS) -o $@ $(LIBS)

trace: $(OBJECTS)
	$(CC) src/apps/trace.o $(COMMONOBJECTS) $(LDFLAGS) -o $@ $(LIBS)

convert_ply: src/convert/convert_ply.o $(COMMONOBJECTS) ${INCLUDES}
	$(CC) $(CFLAGS) src/convert/convert_ply.cpp $(COMMONOBJECTS) -o src/convert/convert_ply.o
	$(CC) src/convert/convert_ply.o $(COMMONOBJECTS) $(LDFLAGS) -o $@ $(LIBS)

#$(EXECUTABLE): $(OBJECTS)
#	$(CC) $(OBJECTS) $(LDFLAGS) -o $@ $(LIBS)

%.o: %.cpp ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f src/apps/*.o
	rm -f src/common/*.o
	rm -f src/igl/*.o
	rm -f src/vmath/*.o
	rm -f src/ext/lodepng/*.o
	rm -f view view.exe
	rm -f trace trace.exe
	rm -f convert_ply convert_ply.exe

compilercheck:
	$(COMPILERCHECK)
