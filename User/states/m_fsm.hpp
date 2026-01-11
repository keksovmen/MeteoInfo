#pragma once



#include "m_button.hpp"
#include "m_sleep.hpp"
#include "m_globals.hpp"

#include "etl/fsm.h"

#include "debug.h"



namespace states
{
	const etl::message_router_id_t MACHINE_MSG_ID = 0;



	struct EventId
	{
		enum
		{
			TIMEOUT,
			BUTTON_CLICK,
			TICK,
			DATA_EVENT,
		};
	};



	class TimeoutEvent : public etl::message<EventId::TIMEOUT>
	{

	};



	class ButtonClickEvent : public etl::message<EventId::BUTTON_CLICK>
	{
		public:
			ButtonClickEvent(const periph::buttons::Event& event)
				: event(event)
			{

			}

			const periph::buttons::Event& event;
	};



	class TickEvent : public etl::message<EventId::TICK>
	{

	};



	class DataEvent : public etl::message<EventId::DATA_EVENT>
	{

	};



	struct StateId
	{
		enum
		{
			DEEP_SLEEP,
			CURRENT_VALUE,
			GRAPH,
			NUMBER_OF_STATES,
		};
	};


	
	class GuiMachine : public etl::fsm
	{
		public:
			GuiMachine()
				: fsm(MACHINE_MSG_ID)
			{

			}
	};



	class DeepSleepState : public etl::fsm_state<GuiMachine, DeepSleepState, StateId::DEEP_SLEEP, ButtonClickEvent, TickEvent>
	{
		public:
			etl::fsm_state_id_t on_enter_state()
			{
				printf("Entered Deep sleep state\r\n");
				globals::getDisplayHal().turnOff();

				//go to sleep
				//change sleep mode
				return No_State_Change;
			}

			void on_exit_state()
			{
				//change sleep mode to usual
			}

			etl::fsm_state_id_t on_event(const ButtonClickEvent& event)
			{
				printf("Clicked in deep sleep\r\n");

				return StateId::CURRENT_VALUE;
			}

			etl::fsm_state_id_t on_event(const TickEvent& event)
			{
				printf("Tick in deep sleep\r\n");
				//increment sys time by constant
				periph::sys_time::increaseTime(5000);
				//read aht if needed and push it data -> global call to sensor service tick
				//go to sleep
				periph::sleep::sleepForMs<5000>();

				return No_State_Change;
			}

			etl::fsm_state_id_t on_event_unknown(const etl::imessage& event)
			{
				return No_State_Change;
			}
	};



	class CurrentValueState : public etl::fsm_state<GuiMachine, CurrentValueState, StateId::CURRENT_VALUE, ButtonClickEvent, TimeoutEvent, TickEvent, DataEvent>
	{
		public:
			etl::fsm_state_id_t on_enter_state()
			{
				printf("Entered Current value state\r\n");
				//draw screen
				_refresh();
				globals::getDisplayHal().turnOn();

				return No_State_Change;
			}

			etl::fsm_state_id_t on_event(const ButtonClickEvent& event)
			{
				printf("Clicked in current\r\n");

				return StateId::GRAPH;
			}

			etl::fsm_state_id_t on_event(const TimeoutEvent& event)
			{
				printf("Go to sleep in current\r\n");

				return StateId::DEEP_SLEEP;
			}

			etl::fsm_state_id_t on_event(const TickEvent& event)
			{
				printf("Tick in Current value\r\n");
				//increment sys time by constant
				// periph::sys_time::increaseTime(5000);
				//read aht if needed and push it data -> global call to sensor service tick
				//go to sleep
				periph::sleep::lightSleepForMs<5000>();

				return No_State_Change;
			}

			etl::fsm_state_id_t on_event(const DataEvent& event)
			{
				printf("Data event in Current value\r\n");
				_refresh();

				return No_State_Change;
			}

			etl::fsm_state_id_t on_event_unknown(const etl::imessage& event)
			{
				return No_State_Change;
			}

		private:
			void _refresh()
			{
				globals::getDisplayWriter().addDrawAction([](){
					const auto val = globals::getSensor().getLastValue();
					char buff[64];
					sprintf(buff, "TEMP: %d.%d C", val.first / 100, val.first % 100);
					globals::getFontWriter().drawStr(0, 0, buff);

					sprintf(buff, "HUM: %d.%d %", val.second / 100, val.second % 100);
					globals::getFontWriter().drawStr(0, 16, buff);

					sprintf(buff, "BAT: %d, %d %", (int) globals::getBattery().readRaw(), (int) globals::getBattery().readPercents());
					globals::getFontWriter().drawStr(0, 32, buff);
				});
				globals::getDisplayWriter().flush();
				globals::getDisplayWriter().clearDrawActions();
			}
	};



	class GraphValueState : public etl::fsm_state<GuiMachine, GraphValueState, StateId::CURRENT_VALUE, ButtonClickEvent, TimeoutEvent, DataEvent>
	{
		public:
			etl::fsm_state_id_t on_enter_state()
			{
				printf("Entered Graph value state\r\n");
				_count = 0;

				return No_State_Change;
			}

			etl::fsm_state_id_t on_event(const ButtonClickEvent& event)
			{
				printf("Clicked in graph\r\n");

				return No_State_Change;
			}

			etl::fsm_state_id_t on_event(const TimeoutEvent& event)
			{
				printf("Go to sleep in graph\r\n");

				return StateId::DEEP_SLEEP;
			}

			etl::fsm_state_id_t on_event(const DataEvent& event)
			{
				std::copy_backward(_dataEntries.begin(), _dataEntries.end() - 1, _dataEntries.end());
				_dataEntries[0] = globals::getSensor().getLastValue();

				_refresh();

				return No_State_Change;
			}

			etl::fsm_state_id_t on_event_unknown(const etl::imessage& event)
			{
				return No_State_Change;
			}
		


		private:
			std::array<std::pair<int32_t, int32_t>, 10> _dataEntries;
			int _count = 0;



			void _refresh()
			{
				globals::getGraphWriter().setLabel((_count < 5) ? "TEMPERATURE C" : "HUMIDITY %");

				globals::getDisplayWriter().addDrawAction([this](){
					std::array<int32_t, 10> tmp;
					std::transform(_dataEntries.begin(), _dataEntries.end(), tmp.begin(), [this](const auto& e){ return (_count < 5) ? e.first : e.second; });
					globals::getGraphWriter().drawGraph({tmp.begin(), 10});
				});
				globals::getDisplayWriter().flush();
				globals::getDisplayWriter().clearDrawActions();

				_count = (_count + 1) % 10;
			}
	};
}
