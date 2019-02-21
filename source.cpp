#include "event_emitter.h"

void fnTest(int a, std::string str)
{
	return ;
}

class test
{
public:
	void fn(int a, std::string str)
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
	std::string strParam = "z";

	emitter.on("test", fn);
	emitter.on("test1", &t, &test::fn);
	emitter.on("test2", fnTest);
	emitter.emit("test", iParam, strParam);
	emitter.emit("test1", iParam, strParam);
	emitter.emit("test2", iParam, strParam);

	return 0;
}