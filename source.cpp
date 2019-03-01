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

	auto cooike = emitter.on("test", fn);
	emitter.on("test", fnTest);
	emitter.on("test", &t, &test::fn);
	emitter.on("test1", fnTest1);
	emitter.on("test2", fnTest2);
	emitter.once("test2", fnTest2);
	emitter.emit("test", iParam, strParam);
	emitter.emit("test1", iParam, strParam, dParam);
	emitter.emit("test2", strParam, iParam, dParam);
	emitter.emit("test2", strParam, iParam, dParam);

	emitter.cancle(cooike);
	emitter.emit("test", iParam, strParam);

	return 0;
}