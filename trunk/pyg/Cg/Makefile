all:		_Cg.so _CgGL.so

_Cg.so:		Cg.i
		swig -python Cg.i
		gcc -c Cg_wrap.c -I/usr/include/Cg -I/usr/include/python2.2
		ld -shared Cg_wrap.o -o _Cg.so -lCg

_CgGL.so:	CgGL.i
		swig -python CgGL.i
		gcc -c CgGL_wrap.c -I/usr/include/Cg -I/usr/include/python2.2
		ld -shared CgGL_wrap.o -o _CgGL.so -lCgGL -lGL

clean:
		rm -f *.o *.so Cg.py *.pyc *.c
