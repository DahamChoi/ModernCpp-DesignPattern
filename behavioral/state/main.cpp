#include "PhoneStateMachine.h"

int main()
{
	boost::msm::back::state_machine<PhoneStateMachine> phone;

	auto info = [&]()
	{
		auto i = phone.current_state()[0];
		std::cout << "The phone is currently " <<
			state_names[i] << std::endl;
	};

	info();
	phone.process_event(CallDialed{});
	info();
	phone.process_event(CallConnected{});
	info();
	phone.process_event(PlacedOnHold{});
	info();
	phone.process_event(PhoneThrownIntoWall{});
	info();

	phone.process_event(CallDialed{});
	info();
}
