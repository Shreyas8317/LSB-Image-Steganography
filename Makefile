obj = $(patsubst %.c,%.o,$(wildcard *.c))

stego.exe : $(obj)
	gcc -o $@ $^

clean :
	rm * .exe * .o
