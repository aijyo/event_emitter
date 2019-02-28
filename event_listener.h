#ifndef __X_EVENT_LISTENER_H__
#define __X_EVENT_LISTENER_H__

#include <memory>
#include <list>
#include "function_traits/callable.hpp"

namespace x_util
{
	//////////////////////////////////////////////////////////////////////////
	//
	class event_listener_base
	{
	public:
		event_listener_base()
			: m_bCallOnce(false)
		{

		}

		bool is_equal(void* pItem)
		{
			return pItem == (void*)this;
		}

		void call_once(bool bOnce)
		{
			m_bCallOnce = bOnce;
		}

		bool call_once() const
		{
			return m_bCallOnce;
		}
	private:
		bool m_bCallOnce;
	};
	using event_listener_ptr = std::shared_ptr<event_listener_base>;
	using event_listener_list = std::list<event_listener_ptr>;
	//////////////////////////////////////////////////////////////////////////
	//
	template<class Ret, typename... Args>
	class event_listener_t : public event_listener_base
	{
	public:
		virtual Ret operator()(Args... args) = 0;
		virtual Ret Invoke(Args... args) = 0;

	};
	template<class Ld>
	class event_listener : public event_listener<decltype(&Ld::operator())>
	{
	public:
		event_listener(Ld callback)
			: event_listener<decltype(&Ld::operator())>(callback)
		{

		}
	//	event_listener(Ld callback)
	//		: m_fnCallback(callback)
	//	{

	//	}
	//	virtual Ret operator()(typename function_traits<decltype(&Ld::operator())>::argumets_type &&... args)
	//	{
	//		//return Invoke(std::forward<Args>(args) ...);
	//	}
	//	virtual Ret Invoke(typename function_traits<decltype(&Ld::operator())>::argumets_type&&... args)
	//	{
	//		return m_fnCallback(std::forward<Args>(args) ...);
	//	}

	//private:
	//	Ld m_fnCallback;
	};


	template< class Cls, class Ret, typename... Args>
	class event_listener<Ret(Cls::*)(Args...)>
		: public event_listener_t<Ret, Args...>
	{
	public:
		event_listener(Cls ld)
			: m_fnCallback(ld)
		{

		}

		Ret operator()(Args... args)
		{
			return Invoke(std::forward<Args>(args) ...);
		}

		Ret Invoke(Args... args)
		{
			return m_fnCallback(std::forward<Args>(args) ...);
		}
	private:
		Cls m_fnCallback;
	};

	template<class Cls, class Ret, typename... Args >
	class event_listener<Ret(Cls::*)(Args...) const>
		: public event_listener_t<Ret, Args...>
	{
	public:
		event_listener(Cls ld)
			: m_fnCallback(ld)
		{

		}

		Ret operator()(Args... args)
		{
			return Invoke(std::forward<Args>(args) ...);
		}

		Ret Invoke(Args... args)
		{
			return m_fnCallback(std::forward<Args>(args) ...);
		}
	private:
		Cls m_fnCallback;
	};
	//////////////////////////////////////////////////////////////////////////
	//
	template<class Ret, class Cls, typename... Args>
	class event_handler : public event_listener_t < Ret, Args... >
	{
		using ClassType = Cls;
		using ReturnType = Ret;
		using FuncType = ReturnType(ClassType::*)(Args...);
	public:
		event_handler(FuncType f, ClassType* t)
			: m_pInstance(t)
			, m_pFunc(f)
		{

		}
		~event_handler()
		{

		}

		ReturnType operator()(Args... args)
		{
			return Invoke(std::forward<Args>(args) ...);
		}

		ReturnType Invoke(Args... args)
		{
			return (m_pInstance->*m_pFunc)(std::forward<Args>(args) ...);
		}
	private:
		ClassType* m_pInstance;
		FuncType m_pFunc;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	template<class Ret, typename... Args>
	class event_handler<Ret, void, Args...> : public event_listener_t < Ret, Args... >
	{
		using ReturnType = Ret;
		using FuncType = ReturnType(*)(Args...);
	public:
		event_handler(FuncType f)
			: m_pFunc(f)
		{

		}
		~event_handler()
		{

		}

		ReturnType operator()(Args... args)
		{
			return Invoke(std::forward<Args>(args) ...);
		}

		ReturnType Invoke(Args... args)
		{
			return (*m_pFunc)(std::forward<Args>(args) ...);
		}
	private:
		FuncType m_pFunc;
	};

}

#endif