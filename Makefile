curses : curses.o
	gcc -o $@ curses.o -lncurses -lpanel -lmenu -lform -lm -Wall -Wextra

clean :
	rm -f ctest ctest.o
