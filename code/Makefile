build:
	gcc Kernel/process_generator.c Clock/clk_interface.c -o process_generator.out
	gcc Clock/clk.c -o clk.out
	gcc Kernel/process.c Clock/clk_interface.c -o process.out
	gcc Kernel/scheduler.c Clock/clk_interface.c DS/Data_structures.c -o scheduler.out
	gcc Test/test_generator.c -o test_generator.out

clean:
	rm -f *.out  Test/processes.txt

all: clean build

run:
	./process_generator.out
