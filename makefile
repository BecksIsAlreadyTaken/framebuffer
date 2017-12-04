framebuffer	: framebuffer.o
framebuffer.o	: framebuffer.c
		  cc -c framebuffer.c
run		: 
		  ./framebuffer
clean		:
		  rm  *.o framebuffer -fr
