curses : curses.o
	gcc -o $@ curses.o -lncurses -lpanel -lmenu -lform -lm

clean :
	rm -f ctest ctest.o
