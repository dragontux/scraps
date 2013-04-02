all: assemble link

assemble:
	for thing in `ls *.s`; do\
		nasm -f elf64 $$thing;\
	done

link:
	ld -o libthing *.o

clean:
	rm *.o

.PHONY: all
