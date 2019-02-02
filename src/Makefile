#Release build: nmake all
CC = cl.exe
CFLAGS = /nologo /MDd /W3 /DNDEBUG /D_CRT_SECURE_NO_DEPRECATE
PROG = cube.exe

SRCS = Main.c \
	linearAlg.c \
	arcballCamera.c \
	window.c \
	buffer.c \
	instance.c \
	device.c \
	swapChain.c \
	textures.c \
	shader.c \
	sphere.c \
	quadCube.c

MAKE_DIR = %cd%
#SRCS := $(shell echo dir1/*.c)
INCLUDE_PATHS = -I "..\dependencies\lib" -I "..\dependencies\include" -I "..\dependencies\include\vulkan"
INCLUDE_PATHS = $(INCLUDE_PATHS) -I $(MAKE_DIR)
LIBS = C:\Users\TJ\Documents\programming\vulkan\dependencies\lib\glfw3dll.lib C:\Users\TJ\Documents\programming\vulkan\dependencies\lib\SOIL.lib C:\Users\TJ\Documents\programming\vulkan\dependencies\lib\vulkan-1.lib C:\Users\TJ\Documents\programming\vkProjects\dependencies\lib\freetype.lib

all: $(PROG) clean
	@echo "Building"

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(SRCS) /link $(LIBS) /NODEFAULTLIB:libcmt.lib

clean:
	del *.obj
