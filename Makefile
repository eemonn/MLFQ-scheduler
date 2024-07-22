scheduler: scheduler.c
	clang -Wall -Wpedantic -Wextra -Werror scheduler.c -o scheduler
clean:
	rm -rf scheduler.o scheduler

