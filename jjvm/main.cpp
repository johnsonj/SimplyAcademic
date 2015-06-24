// JJVM - Inspired by http://felixangell.com/virtual-machine-in-c
#include <iostream>
#define STACK_SIZE 256

#define assert(conditional) \
	if (!(conditional)) \
		throw std::runtime_error("Failed Assertion: " + std::string(#conditional));

enum Instruction
{
	// 0 Args
	POP, // Pop top of stack
	ADD, // Add top 2 values on stack
	HLT, // Halt execution
	// 1 Arg
	PUSH, // (val) Push value onto stack
	ADDD, // (val) Add value on stack with intermediate
	// 2 Args
	BNED, // (val, segments) Branch N segments (+ or -) if top of stack is not equal to val
};


// Example Programs
int simple_add[] = {
	PUSH, 2,
	PUSH, 1,
	ADD,
	POP,
	HLT
};

int add_with_condition[] = {
	PUSH, 2,
	PUSH, 1,
	ADD,
	BNED, 3, 2,
	ADDD, 1,
	POP,
	HLT
};

int for_loop[] = {
	PUSH, 0,
	PUSH, 1,
	ADD,
	BNED, 5, -6,
	POP,
	HLT
};

struct Machine
{
	int sp = -1;
	int pc = 0;
	int stack[STACK_SIZE];
	int *program = nullptr;
	bool running = false;
};

struct ExecutionState
{
	bool verbose_logging = true;
	bool explain_exceptions = true;
};

std::string dump_machine(Machine &vm)
{
	using std::to_string;
	return std::string("sp = " + to_string(vm.sp) + " pc = " + to_string(vm.pc) + " running = " + to_string(vm.running) + " ");
}

class MachineCycle
{
public:
	MachineCycle(Machine &_vm, ExecutionState &_es) : vm(_vm), es(_es)
	{
		if (es.verbose_logging)
			std::cout << dump_machine(vm);
	}
	~MachineCycle()
	{
		if (es.verbose_logging)
			std::cout << "\n";
	}

private:
	Machine &vm;
	ExecutionState &es;
};

void load_program(Machine &vm, int *program)
{
	vm.program = program;
}

void push_direct(Machine &vm, int value)
{
	assert(vm.sp < STACK_SIZE);
	vm.stack[++vm.sp] = value;
}

void push(Machine &vm)
{
	push_direct(vm, vm.program[++vm.pc]);
}

int peak(Machine &vm)
{
	return vm.stack[vm.sp];
}

int pop(Machine &vm)
{
	int v = peak(vm);
	--vm.sp;
	return v;
}

void instruction_cycle(Machine &vm, ExecutionState &es)
{
	MachineCycle mc(vm, es);

	switch (vm.program[vm.pc])
	{
		case PUSH:
		{
			push(vm);
			break;
		}
		case POP:
		{
			std::cout << "Pop: " << pop(vm);
			break;
		}
		case HLT:
		{
			vm.running = false;
			break;
		}
		case ADD:
		{
			push_direct(vm, pop(vm) + pop(vm));
			break;
		}
		case ADDD:
		{
			push_direct(vm, pop(vm) + vm.program[++vm.pc]);
			break;
		}
		case BNED:
		{
			// Branch not equal direct
			int euqal_to = vm.program[++vm.pc];
			int cycles = vm.program[++vm.pc];

			if (peak(vm) != euqal_to)
				vm.pc += cycles;

			break;
		}
		default:
		{
			assert(false);
			break;
		}
	}
	++vm.pc;
}

bool run(Machine &vm, ExecutionState &es)
{
	try
	{
		vm.running = true;

		while (vm.running)
			instruction_cycle(vm, es);

		return true;
	}
	catch (const std::runtime_error& e)
	{
		if (es.explain_exceptions)
		{
			std::cerr << e.what() << "\n";
			std::cerr << dump_machine(vm) << "\n";
		}

		vm.running = false;

		return false;
	}
}

int main(int argc, char** argv)
{
	Machine vm;
	ExecutionState es;
	load_program(vm, &for_loop[0]);

	return run(vm, es);
}