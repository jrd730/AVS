CXX = g++
CPPFLAGS = -g -w

OBJS = main.cpp DDV.cpp IGV.cpp Line.cpp Simulator.cpp Environment.cpp QuadTree.cpp NavMesh.cpp

INCLUDE_PATHS = -I/usr/local/include/GL -I/usr/local/include/GLFW -I/opt/X11/include

LIBDIRS  = -L/usr/local/lib -L/usr/X11R6/lib -L/usr/lib
LIBS = -lX11 -lglut -lglui -lGLU -lGL -lm 

app: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OSX) -o app $(OBJS) $(INCLUDE_PATHS) $(LIBDIRS) $(LIBS)

clean:
	rm -f $(C++OBJ) app
