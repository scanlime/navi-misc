# Makefile
#
# Evan Sheehan
#
# For testing Hangman.c
#
main: test_main.o Hangman.o HangmanGUI.o
	gcc `sdl-config --libs` -o main test_main.o Hangman.o HangmanGUI.o
#
GUI_Node.o: GUI_Node.c GUI.h
	gcc -g -c -Wall `sdl-config --cflags` GUI_Node.c
#
GUI_test.o: GUI_test.c HangmanGUI.h
	gcc -g -c -Wall GUI_test.c
#
HangmanGUI.o: HangmanGUI.c GUI_Node.c HangmanGUI.h Hangman.h GUI.h
	gcc -g -c -Wall `sdl-config --cflags` -lSDL_ttf -lSDL_gfxPrimitives -lSDL_image GUI_Node.c HangmanGUI.c
#
test_main.o: test_main.c Hangman.h HangmanGUI.h
	gcc -g -c -Wall test_main.c
#
Hangman.o: Hangman.c Hangman.h
	gcc -g -c -Wall Hangman.c
#
run: main
	./main
#
gui: gui_main
	./gui_main
#
clean:
	rm -f main gui_main *.o
