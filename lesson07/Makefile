target=app
source=sub.o add.o mult.c div.c main.c
$(target):$(source)
	$(CC) $(^) -o $(@)

%.o:%.c
	$(CC) -c $(^) -o $(@)
.PHONY:clean
clean:
	rm -f *.o
