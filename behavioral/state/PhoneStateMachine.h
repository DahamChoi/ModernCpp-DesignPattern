#pragma once

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <iostream>
#include <vector>
#include <string>

std::vector<std::string> state_names{
	"off hook's",
	"connecting's",
	"connected's",
	"on hold's",
	"destroyed's"
};

// transitions/events
struct CallDialed {};
struct HangUp {};
struct CallConnected {};
struct PlacedOnHold {};
struct TakenOffHold {};
struct LeftMessage {};
struct PhoneThrownIntoWall {};

class PhoneStateMachine : public boost::msm::front::state_machine_def<PhoneStateMachine>
{
public:
	bool angry{ false };
	struct OffHook : boost::msm::front::state<> {};
	struct Connecting : boost::msm::front::state<>
	{
		template<class Event, class FSM>
		void on_entry(Event const& evt, FSM&)
		{
			std::cout << "We are connecting..." << std::endl;
		}
	};
	struct Connected : boost::msm::front::state<> {};
	struct OnHold : boost::msm::front::state<> {};
	struct PhoneDestroyed : boost::msm::front::state<> {};

	struct PhoneBeingDestroyed
	{
		template<class EVT, class FSM, class SourceState, class TargetState>
		void operator()(EVT const&, FSM&, SourceState&, TargetState&)
		{
			std::cout << "Phone breaks into a million pieces" << std::endl;
		}
	};

	struct CanDestroyPhone
	{
		template <class EVT, class FSM, class SourceState, class TargetState>
		bool operator()(EVT const&, FSM& fsm, SourceState&, TargetState&)
		{
			return fsm.angry;
		}
	};

	struct transition_table : boost::mpl::vector <
		boost::msm::front::Row<OffHook, CallDialed, Connecting>,
		boost::msm::front::Row<Connecting, CallConnected, Connected>,
		boost::msm::front::Row<Connected, PlacedOnHold, OnHold>,
		boost::msm::front::Row<OnHold, PhoneThrownIntoWall, PhoneDestroyed, PhoneBeingDestroyed, CanDestroyPhone>
	> {};

	typedef OffHook initial_state;

	template <class FSM, class Event>
	void no_transition(Event const& e, FSM&, int state)
	{
		std::cout << "No transition from state " << state_names[state]
			<< " on event " << typeid(e).name() << std::endl;
	}
};