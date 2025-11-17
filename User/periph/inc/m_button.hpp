#pragma once



#include <functional>

#include "m_sys_time.hpp"



namespace periph
{
	namespace buttons
	{
		enum class Action : int
		{
			// PRESSED,
			RELEASED,
			// LONG_PRESSED,
			LONG_RELEASED
		};



		struct Event
		{
			Action action;
			sys_time::time_val duration;
		};
		


		// class Button
		// {
		// 	public:
		// 		using StabIRQ = std::function<void()>;



		// 		Button(StabIRQ&& stab);

		void init(sys_time::time_val longPressMs, std::function<void()>&& stab);
		bool hasEvent();
		Event getLastEvent();

		// 	private:
		// 		const StabIRQ _stab;
		// 		Event _event;
		// 		bool _hasEventFlag;
		// };

	}
}