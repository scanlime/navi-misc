# Makefile
#
# Evan Sheehan
#
# For testing Hangman.c
#
main: test_main.o Hangman.o HangmanGUI.o GUI_Node.o
	gcc `sdl-config --libs` -lSDL_gfx -lSDL_ttf -lSDL_image -o main test_main.o GUI_Node.o Hangman.o HangmanGUI.o
#
HangmanGUI.o: HangmanGUI.c HangmanGUI.h Hangman.h GUI.h
	gcc -g -c -Wall `sdl-config --cflags` HangmanGUI.c
#
GUI_Node.o: GUI_Node.c GUI.h
	gcc -g -c -Wall `sdl-config --cflags` GUI_Node.c
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
clean:
	rm -f main *.o
