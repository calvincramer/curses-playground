curses : curses.o
	gcc -o $@ curses.o -lncurses -lpanel -lmenu -lform -lm -Wall -Wextra -O2

clean :
	rm -f ctest ctest.o
