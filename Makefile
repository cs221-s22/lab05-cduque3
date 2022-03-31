PROG = lab05
OBJS = scan.o
DEBUG = -g

$(PROG) : $(OBJS)
	gcc $(DEBUG) -o $(PROG) $^

%.o : %.c scan.h
	gcc $(DEBUG) -c -o $@ $<

clean:
	rm -rf $(PROG) $(OBJS)
