TARGETS = cshell

all: $(TARGETS)

%o: %.c
	gcc -g -c -o $@ $<
cshell: cd.o clock.o echo.o env.o job.o ls.o main.o pinfo.o pwd.o utility.o
	gcc -o cshell cd.o clock.o echo.o env.o job.o ls.o main.o pinfo.o pwd.o utility.o -lreadline

.PHONY: clean
clean:
	rm -f $(TARGETS) *.o
