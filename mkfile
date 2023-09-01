</$objtype/mkfile

TARG=prog

OFILES= main.$O 

prog: $OFILES
	$LD -o prog $OFILES


%.$O: %.c
	$CC $CFLAGS $stem.c

clean:V:
	rm -f *.[$OS] [$OS].out prog

