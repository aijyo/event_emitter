# event_emitter
A event manager/emmiter like node.js event emitter(but not equal). 
Muti thread is support now,to use it, just need define a macro _ENABLE_MULTITHREAD in program

fix me up
1) Didn't support callback with return value, because I'm cannot find a suitable way to 
distinguish between "void emit(const EventType& eventid, Ret& value, Args&& ... args)" 

说明：
一个简单好用的事件管理/分发类，曾经为了找一个简单通用的类似node.js event emitter的类，花费了“无数个昼夜”，找的的要么是依赖太大，要么是对回调函数限制太多（参数个数，回调函数类型...），要么是平台限制，反正就是没找到一个简单通用的。

这个目前支持多线程，可变参事件回调，注意回调函数返回值必须为空，大多数情况回调函数返回值其实也没什么用。实际上是我没找到一个可靠度解决方案，我有个注释掉的void emit(const EventType& eventid, Ret& value, Args&& ... args)在里面，可以放出来试试（可以理解成任意类型的函数回调，但目前因为回调函数返回值无法适当处理的问题，暂时不支持void返回值之外的函数回调）。

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
	emitter.emit("test1", iParam, strParam, dParam);	
	emitter.emit("test2", strParam, iParam, dParam);

	return 0;
}


