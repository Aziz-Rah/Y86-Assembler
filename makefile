ALL:y86emul y86dis

y86emul:
	gcc -g -Wall y86emul.c -lm -o y.out
y86dis:
	gcc -g -Wall y86dis.c -lm -o yd.out
clean:
	rm -rf *.out