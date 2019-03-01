# event_emitter
A event manager/emmiter like node.js event emitter(but not equal). 
Muti thread is support now,to use it, just need define a macro _ENABLE_MULTITHREAD in program

fix me up
1) Didn't support callback with return value, because I'm cannot find a suitable way to 
distinguish between "void emit(const EventType& eventid, Ret& value, Args&& ... args)" 

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Pay attention!!!!!!!!!!!!!!!!!!!!!!!!!!
x_util::EventEmitter<std::string, long> emitter;
The second type of the EventEmitter template is the event cookie type, the cookie is init by void*,
Therefore event type must be initialized by void* and compared with void*...

说明：
一个简单好用的事件管理/分发类，曾经为了找一个简单通用的类似node.js event emitter的类，花费了“无数个昼夜”，找的的要么是依赖太大，要么是对回调函数限制太多（参数个数，回调函数类型...），要么是平台限制，反正就是没找到一个简单通用的。

这个目前支持多线程，可变参事件回调，注意回调函数返回值必须为空，大多数情况回调函数返回值其实也没什么用。实际上是我没找到一个可靠度解决方案，我有个注释掉的void emit(const EventType& eventid, Ret& value, Args&& ... args)在里面，可以放出来试试（可以理解成任意类型的函数回调，但目前因为回调函数返回值无法适当处理的问题，暂时不支持void返回值之外的函数回调）。就是因为防止非空返回值回调函数的注册，引入了 function traist库，如果这个限制去掉了，这个依赖也可以去掉，当然也可以自己写替代function traits，我只用了一个功能，我只是懒得写而已、、、、毕竟我也觉得他们那个开源的比较好用，说以顺手戴上了。

注意事项：
emitter的第二个参数是cookie类型，为保证cookie唯一，目前直接用callback对象的指针转换而来，这就要求cookie的类型必须能装下void*并且跟void*比较。
比如int在32位程序是32bit，在64位程序下，也还是32bit这就导致，如果你使用int作为cookie的类型，在64位程序下，cancel就无法正常运行！！！！！
就目前来看 默认的std::uint64_t能解决一切问题。


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
	x_util::EventEmitter<std::string, long> emitter;
	
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


