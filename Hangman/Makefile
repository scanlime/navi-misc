# Makefile
#
# Evan Sheehan
#
# For testing Hangman.c
#
main: test_main.o Hangman.o
	gcc -o main test_main.o Hangman.o
#
gui_main: GUI_test.o HangmanGUI.o
	gcc `sdl-config --libs` -lSDL_gfx -lSDL_ttf -lSDL_image -o gui_main GUI_test.o HangmanGUI.o
#
GUI_test.o: GUI_test.c HangmanGUI.h
	gcc -g -c -Wall GUI_test.c
#
HangmanGUI.o: HangmanGUI.c HangmanGUI.h
	gcc -g -c -Wall `sdl-config --cflags`  HangmanGUI.c
#
test_main.o: test_main.c Hangman.h
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
