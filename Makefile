heat: heat.o heatCalculations.o heatArrayAllocate.o heatMisc.o heat.h
	gcc -o heat heat.o heatCalculations.o heatArrayAllocate.o heatMisc.o -lpgplot -lcpgplot -lX11 -lm

heat.o: heat.c heat.h
	gcc -c heat.c

heatCalculations.o: heatCalculations.c heat.h
	gcc -c heatCalculations.c

heatArrayAllocate.o: heatArrayAllocate.c heat.h
	gcc -c heatArrayAllocate.c

heatMisc.o: heatMisc.c heat.h
	gcc -c heatMisc.c