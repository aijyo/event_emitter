#ifndef __X_EVENT_EMMITTER_H__
#define __X_EVENT_EMMITTER_H__


#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <algorithm> //std::forEach()

#include "event_listener.h"

//#define _ENABLE_MULTITHREAD
namespace x_util
{

	template <class EventType, class EventCookie = std::uint64_t>
	class EventEmitter
	{
	public:
		EventEmitter() = default;
		~EventEmitter() = default;

		template<class Func
			, typename = typename std::enable_if<std::is_same<void, callable_traits<Func>::return_type>::value>::type>
		EventCookie on(const EventType& eventid, const Func& callback)
		{
			EventCookie cookie;

			event_listener_ptr pListener = std::make_shared<event_listener<Func>>(callback);
			cookie = insert(eventid, pListener);

			return cookie;
		}

		template<class Ret, class Cls, class... Args
			, typename = typename std::enable_if<std::is_same<void, Ret>::value>::type>
		EventCookie on(const EventType& eventid, Cls* pThis, Ret(Cls::*callback)(Args...))
		{
			EventCookie cookie;

			event_listener_ptr pListener = std::make_shared<event_handler<Ret, Cls, Args...>>(callback, pThis);
			cookie = insert(eventid, pListener);

			return cookie;
		}

		template<class Ret, class... Args
			, typename = typename std::enable_if<std::is_same<void, Ret>::value>::type>
		EventCookie on(const EventType& eventid, Ret(*callback)(Args...))
		{
			EventCookie cookie;

			event_listener_ptr pListener = std::make_shared<event_handler<Ret, void, Args...>>(callback);
			cookie = insert(eventid, pListener);

			return cookie;
		}

		template<class Func
			, typename = typename std::enable_if<std::is_same<void, callable_traits<Func>::return_type>::value>::type>
		EventCookie once(const EventType& eventid, const Func& callback)
		{
			EventCookie cookie;

			event_listener_ptr pListener = std::make_shared<event_listener<Func>>(callback);
			pListener->call_once(true);
			cookie = insert(eventid, pListener);

			return cookie;
		}


		template<class Ret, class Cls, class... Args
			, typename = typename std::enable_if<std::is_same<void, Ret>::value>::type>
		EventCookie once(const EventType& eventid, Cls* pThis, Ret(Cls::*callback)(Args...))
		{
			EventCookie cookie;

			event_listener_ptr pListener = std::make_shared<event_handler<Ret, Cls, Args...>>(callback, pThis);
			pListener->call_once(true);
			cookie = insert(eventid, pListener);

			return cookie;
		}

		template<class Ret, class... Args
			, typename = typename std::enable_if<std::is_same<void, Ret>::value>::type>
		EventCookie once(const EventType& eventid, Ret(*callback)(Args...))
		{
			EventCookie cookie;

			event_listener_ptr pListener = std::make_shared<event_handler<Ret, void, Args...>>(callback);
			pListener->call_once(true);
			cookie = insert(eventid, pListener);

			return cookie;
		}

		// fix me
		//template <class Ret, typename ... Args>
		//void emit(const EventType& eventid, Ret& value, Args&& ... args)
		//{
		//	event_listener_base* pListener = nullptr;

		//	auto itls = m_mapListener.find(eventid);

		//	if (itls != m_mapListener.end())
		//	{
		//		auto& ls = itls->second;
		//		for each(auto& item in ls)
		//		{
		//			event_listener_t<Ret, Args...>* callback = (event_listener_t<Ret, Args...>*)(item.get());
		//			(*callback)(std::forward<Args>(args)...);
		//		}
		//	}
		//}

		template <typename ... Args>
		void emit(const EventType& eventid, Args&& ... args)
		{
			event_listener_ptr pListener = nullptr;
			std::unordered_map < EventType, event_listener_list> listeners;

			{
#if defined(_ENABLE_MULTITHREAD)
				std::unique_lock<std::mutex> lock(m_lock);
#endif
				listeners = m_mapListener;								// copy to deal with: remove listener in callback
			}

			// do callback with no lock
			auto itls = listeners.find(eventid);

			if (itls != listeners.end())
			{
				auto& ls = itls->second;
				for each(auto& item in ls)
				{
					event_listener_t<void, Args...>* callback = (event_listener_t<void, Args...>*)(item.get());
					(*callback)(std::forward<Args>(args)...);
				}
			}

			{
				// remove callonce
#if defined(_ENABLE_MULTITHREAD)
				std::unique_lock<std::mutex> lock(m_lock);
#endif
				auto itls = m_mapListener.find(eventid);

				if (itls != m_mapListener.end())
				{
					auto& ls = itls->second;
					auto fn = [](event_listener_ptr& item)
					{
						return item->call_once();
					};
					auto it = ls.erase(std::remove_if(ls.begin(), ls.end(), std::move(fn)) , ls.end());
				}
			}
		}

		bool cancle(const EventCookie& cookie)
		{
			bool bSuc = false;
			bSuc = remove(cookie);
			return bSuc;
		}

		std::size_t listener_count(const EventType& event)
		{
			std::size_t count = 0;
			// not impl

			return count;
		}

		private:
			EventCookie insert(const EventType& eventid, event_listener_ptr pListener)
			{
#if defined(_ENABLE_MULTITHREAD)
				std::unique_lock<std::mutex> lock(m_lock);
#endif
				return _insert(eventid, pListener);
			}

			bool remove(const EventCookie& cookie)
			{
#if defined(_ENABLE_MULTITHREAD)
				std::unique_lock<std::mutex> lock(m_lock);
#endif
				return _remove(cookie);
			}

			bool remove(const EventType& eventid, const EventCookie& cookie)
			{
#if defined(_ENABLE_MULTITHREAD)
				std::unique_lock<std::mutex> lock(m_lock);
#endif
				return _remove(eventid, cookie);
			}

			// lockless
			EventCookie _insert(const EventType& eventid, event_listener_ptr pListener)
			{
				EventCookie cookie;
				auto itls = m_mapListener.find(eventid);
				bool bExist = itls != m_mapListener.end();

				if (bExist)
				{
					event_listener_list& ls = itls->second;
					ls.push_back(pListener);
				}
				else
				{
					event_listener_list ls;
					ls.push_back(pListener);
					m_mapListener.insert(std::make_pair(eventid, ls));
				}

				cookie = (EventCookie)pListener.get();
				return cookie;
			}

			bool _remove(const EventCookie& cookie)
			{
				bool bSuc = false;
				auto itls = m_mapListener.begin();

				while (itls != m_mapListener.end())
				{
					auto eventid = itls.first;

					bSuc = _remove(eventid, cookie);
					if (bSuc) break;

					++itls;
				}

				return bSuc;
			}

			bool _remove(const EventType& eventid, const EventCookie& cookie)
			{
				bool bSuc = false;
				auto itls = m_mapListener.find(eventid);
				bool bExist = itls != m_mapListener.end();
				if (bExist)
				{
					//
					auto it = std::remove_if(itls.begin(), itls.end(), [&cookie](event_listener_ptr& pItem)
					{
						return pItem->is_equal(cookie);
					};);
					itls.erase(it, itls.end());
				}
				return bSuc;
			}
	private:

		std::unordered_map < EventType, event_listener_list> m_mapListener;
#if defined(_ENABLE_MULTITHREAD)
		std::mutex    m_lock;
#endif
	};
}
#endif