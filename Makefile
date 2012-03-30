DIRS=sistemi_operativi_1 reti

all:
	for d in $(DIRS); do (cd $$d; make ); done

clean:
	for d in $(DIRS); do (cd $$d; make clean ); done
