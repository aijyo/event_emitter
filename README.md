# event_emitter
A event manager/emmiter like node.js event emitter(but not equal). 
Muti thread is support now,to use it, just need define a macro _ENABLE_MULTITHREAD in program

fix me up
1) Didn't support callback with return value, because I'm cannot find a suitable way to 
distinguish between "void emit(const EventType& eventid, Ret& value, Args&& ... args)" 
   
Example:

#include "event_emitter.h"

void fnTest(int p1, std::string p2)
{
	return;
}

void fnTest1(int p1, std::string p2, double p3)
{
	return ;
}

void fnTest2(std::string p1, int p2, double p3 )
{
	return;
}

class test
{
public:
	void fn(int p1, std::string p2)
	{
		return;
	}
};

#include <type_traits>
int main(void)
{
	test t;
	x_util::EventEmitter<std::string, int> emitter;

	auto fn = [](int a, std::string str)
	{ 
	};

	int iParam = 1;
	double dParam = 2.0;
	std::string strParam = "z";

	emitter.on("test", fn);
	emitter.on("test", fnTest);
	emitter.on("test", &t, &test::fn);
	emitter.on("test1", fnTest1);
	emitter.on("test2", fnTest2);
	emitter.emit("test", iParam, strParam);
	emitter.emit("test1", iParam, strParam, dParam);	// strParam is empty now!!!!!!!!!!!!!
	emitter.emit("test2", strParam, iParam, dParam);

	return 0;
}


