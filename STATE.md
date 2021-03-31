# 상태
*상태 패턴은 객체 지향 방식으로 상태 기계를 구현하는 행위 소프트웨어 디자인 패턴이다. 상태 패턴을 이용하면 상태 패턴 인터페이스의 파생 클래스로서 각각의 상태를 구현함으로써, 또 패턴의 슈퍼클래스에 의해 정의되는 정의되는 메소드를 호출하여 상태 변화를 구현함으로써 상태 기계를 구현한다.*

## 상태 기반 상태 전이
가장 쉬운 예로 전등을 생각해보자. 전등을 꺼짐, 켜짐 두 가지 상태가 있다. 아무 상태로든 전이할 수 있는 모델을 만들어보자. 이 예는 상태 디자인 패턴의 전통적인 구현 방식을 반영하고는 있지만 권장하는 방법은 아니라는 점에 유의하자.

먼저 전등 스위치를 모델링해보자. 상태 값과 다른 상태로 전이할 수단을 가지면 된다.

	class LightSwitch
	{
		State * state;
	public:
		LightSwitch()
		{
			state = new OffState();
		}
		void set_state(State* state)
		{
			this->state = state;
		}
	};

이 정도면 충분해 보인다. 이제 상태를 정의해보자. 상태는 enum 값이 아니라 하나의 클래스로 정의한다.

	struct State
	{
		virtual void on(LightSwitch *ls)
		{
			cout << "Light is already on" << endl;
		}
		virtual void off(LightSwitch *ls)
		{
			cout << "Light is already off" << endl;
		}
	};

이 구현은 직관적인 것과는 거리가 멀다. 이해하려면 따로 설명이 필요하다. 전등을 켜고 끄는 시나리오에서는 그 상태를 별도의 클래스로 정의하는 것 자체가 그렇게 상식적이지 않다. 무엇이 문제일까?

1. State가 추상 타입이 아니다. 언뜻 생각하기에는 추상 타입의 상태에는 접근할 방법이(또는 이유 자체가) 없다고 생각할 수도 있지만 실제로는 그렇지않다.
2. State 자체적으로 상태 전이를 할 수 있도록 하고 있다. 이것은 전이에 대한 상식적인 개념과 거리가 멀다. 전등 스위치는 스위치가 상태를 변경시킨다. 상태 자체가 상태를 변경하지는 않는다. 마치 아이스크림 가게에 들렸다가 햄버거 가게에 가려 했는데 아이스크림 가게 자체가 햄버거 가게로 변신하는 것과 같다.
3. State::on/off 동작은 이미 그 상태가 있다는 것을 가정하고 있다. 이 부분은 직관적인 것을 떠나서 가장 큰 문제가 된다. 전체 예제의 구현에 이로 인한 영향이 미친다.

여하튼 on/off 각각에 해당하는 State를 정의해보자.

	struct OnState : State
	{
		OnState{ cout << "Light turned on" << endl; }
		void off(LightSwitch* ls) override;
	};

	struct OffState : State
	{
		OffState(){ cout << "Light turend off" << endl;
		void on(LightSwitch* ls) override;
	};

OnState::off와 OffState::on의 구현에서 상태 스스로 상태 전환을 할 수 있게 하고 있다. 즉, 아래와 같은 구현이 가능하다.

	void OnState::off(LightSwitch* ls)
	{
		cout << "Switch light off..." << endl;
		ls->set_state(new OffState());
		delete this;
	}

여기서 전환이 일어난다. 이 구현은 보통의 C++ 코드에서는 보기 힘든 delete this; 구문을 가지고 있다. 이 구문은 해당 객체가 이미 생성이 완료되었다는 대단히 위험한 가정을 하고 있다. 이 부분을 스마트 포인터를 이용해서 수정할 수는 있지만 포인터의 사용과 힙 메모리 할당(new *State() 구문)이 일어난다는 것 자체가 여기서 직접적인 상태 소멸이 일어남을 명확히 보여준다. 만약 상태가 소멸자를 가진다면 여기서 호출될 것이고 그 안에서 추가적인 정리 작업을 할 수도 있다.

당연하지만 전등 스위치를 통해서 상태 전환도 가능해야 한다. 따라서 아래와 같이 구현한다.

class LightSwitch
{
	...
	void on() { state->on(this); }
	void off() { state->off(this); }
};

이러한 구현을 이용하여 아래와 같은 시나리오를 실행 할 수 있다.

	LightSwitch ls;
	ls.on();
	ls.off();

이 접근 방법은 그다지 마음에 들지 않는다. 왜냐하면 직관적이지 않다. 목적하는 상태를 알려주면 그 상태로 바뀌는 것이 더 직관적이다. 마음에 들지는 않지만 상태가 상태를 바꾼다는 접근 방식은 "전통적인" 구현이다(GoF)

## 수작업으로 만드는 상태 머신
전화기를 위한 상태 머신을 정의해보자. 이 전화기는 스마트폰이 아니라 구식 전화기이다. 먼저, 전화기의 상태들을 나열하여 정의해본다.

	enum class State
	{
		off_hook,
		connecting,
		conneted,
		on_hold,
		on_hook
	};

이제 상태 간의 전이를 나열한다. 이번에도 enum 클래스로 정의한다.

    enum class Trigger
    {
    	call_dialed,
    	hung_up,
    	call_connected,
    	place_on_hold,
    	take_off_hold,
    	left_message,
    	stop_using_phone
    };

상태 머신에서 상태 간 전이가 어떤 규칙으로 이루어져야 하는지에 대한 정보를 어딘가에 저장해야 한다. 여기서는 map을 이용한다.

	map<State, vector<pair<Trigger, State>>> rules;

이 부분은 그다지 매끄럽지 않다. map의 키는 상태 전이의 출발 상태이고, 값을 트리거와 도착 상태 쌍들의 집합이다. 트리거와 도착 상태의 쌍들은 출발 상태에서 전이할 수 있는 상태들과 그 트리거에 대한 규칙이 된다.

이 다음과 같이 초기화할 수 있다.

	rules[State::off_hook] = {
		{Trigger::call_dialed, State::coneecting},
		{Trigger::stop_using_phone, State::on_hook}
	};

	rules[State::connecting] = {
		{Trigger::hung_up, State::off_hook},
		{Trigger::call_connected, State::connected}
	};

전이 규칙들 말고도 시작 상태가 필요하다. 그리고 상태 머신이 특정 상태에 도달한 후 멈추기를 바란다면 종료 상태도 필요할 수 있다.

	State currentState{ State::off_hook }, {exitState{ State::on_hook};

이러한 준비를 기반으로 하면 상태 머신의 구동에 별도의 컴포넌트를 만들지 않아도 된다. 즉, 전체적으로 관리 통제할 모듈이 필요 없다. 예를 들어 전화기와 사용자의 상호 작용 모델이 다음과 같이 구현될 수 있다.

	while(true)
	{
		cout << "The phone is currently " << currentState << endl;
	select_trigger:
		cout << "Select a trigger:" << endl;

		int i = 0;
		for(auto item : rules[currentState])
			cout << i++ << ". " << item.first << endl;

		int input;
		cin >> input;
		if(input < 0 || (input+1) > rules[currentState].size())
		{
			cout << "Incorrect option. Please try again." << endl;
			goto select_trigger;
		}

		currentState = rules[currentState][input].second;
		if(currentState == exitState) break;
	}

사용자에게 현재 상태에서 가용한 트리거 중 하나를 선택하게 하고, 트리거가 유효한 경우 앞서 생성한 map의 규칙에 따라 상태를 전이한다.

수작업으로 만들어진 상태 머신의 가장 큰 장점은 이해하기가 매우 쉽다는 점이다. 상태와 전이 각각이 평범한 enum 값들이고 상태 전이 규칙 집합이 단순한 std::map으로 정의된다. 그리고 출발 상태와 도착 상태가 단순 변수에 지나지 않는다.

## Boost.MSM을 이용한 상태 머신
실 세계의 상태 머신은 훨씬 더 복잡하다. 특정 상태에 도달했을 때 어떤 동작을 수행해야 하는 경우도 있다. 또는 조건부로 상태 전이를 해야 할 수도 있다. 즉, 어떤 상황에 대한 조건이 합치되어야만 특정 상태로의 전이가 수행되도록 통제해야 할 수도 있다.

Boost.MSM을 Boost에서 제공되는 상태 머신 라이브러리이다. 아래와 같이 CRTP 형태로 state_machine_def를 상속받아 사용한다.

	struct PhoneStateMachine : state_machine_def<PhoneStateMachine>
	{
		bool angry{ false };
		struct OffHook : state<> {};
		struct Connecting : state<>
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM&)
			{
				cout << "We are connecting..." << endl;
			}
			// ...
		}
	};

위 코드에서 볼 수 있듯이 특정 state로의 진입/탈출에 따른 추가 동작을 정의해 넣을 수도 있다.

그리고 상태 전이 시점에(상태에 도달한 시점이 아니라) 수행되어야 하는 동작도, 정의할 수 있다. 상태 전이도 하나의 클래스로 정의된다. 단 특별히 상속받아야 할 클래스는 없다. 대신 특정 시그니처(파리미터 타입 목록)를 가지는 operator()를 구현해야 한다.

	struct PhoneBeingDestoryed
	{
		template <class EVT, class FSM, class SourceState, class TargetState>
		void operator()(EVT const&, FSM&, SourceState&, TargetState&)
		{
			cout << "Phone breaks into a millon pieces" << endl;
		}
	};

이미 예상했듯이 주어진 인자들은 상태 머신과 출발 상태와 도착 상태에 대한 참조들이다.

마지막으로 "보호 조건"이 있다. 보호 조건은 상태 전이를 수행하기 전에 유효한 전이인지 아닌지 검사하는 것으로 생각할 수 있다. 예를 들기 위해 앞서 정의한 멤버 변수 bool angry;를 이용한다. 이 변수를 보호 조건으로서 MSM에서 사용하기 위해서는 아래와 같이 클래스와 operator()로 감싸야 한다.

	struct CanDestoryPhone
	{
		template<class EVT, class FSM, class SourceState, class TargetState>
		bool operator()(EVT const&, FSM& fsm, SourceState&, TargetState&)
		{
			return fsm.angry;
		}
	};

위 코드는 CanDestoryPhone으로 이름 붙여진 보호 조건이다. 나중에 상태 머신을 정의할 때 사용한다.

상태 머신의 규칙을 정의하기 위해, Boost.MSM은 MPL(메타프로그래밍 라이브러리)를 사용한다. 그중에서도 mpl::vector를 사용하여 상태 전이 테이블을 만든다. mpl::vector를 사용하여 상태 전이 테이블을 만든다. mpl::vector에 저장될 각 항목은 row로 감싸지며 row 안에서의 항목들은 순서대로 아래와 같은 의미를 가진다.

 - 출발 상태
 - 상태 전이
 - 도착 상태
 - 부가적인 동작(옵션)
 - 부가적인 보호 조건(옵션)

이렇게 준비된 것들을 이용하여 아래와 같은 전화 걸기 규칙을 정의할 수 있다.

	struct transition_table : mpl::vector<
		row<OffHook, CallDialed, Connecting>,
		row<Connecting, CallConnected, Connected>,
		row<Connected, PlacedOnHold, OnHold>,
		row<OnHold, PhoneTrownIntoWall, PhoneDestoryed, PhoneBeingDestroyed, CanDestroyPhone>
	> {};

위 코드에서 CallDiable와 같이 상태 전이를 나타내는 클래스는 상태 머신 클래스밖에서 정의되며 특별히 다른 클래스를 상속받지 않아도 된다. 상태 전이 클래스의 내부는 공백이어도 되지만 반드시 타입을 가져야만 한다.

위 코드에서 transition_table의 마지막 row는 흥미로운 부분이다. 보호 조건 CanDestoryPhone이 만족될 때만 수행될 수 있다. 그리고 PhoneThrownIntoWall이 수행되었을 때만 PhoneBeingDestroyed의 동작이 수행된다.

이제 몇 가지만 더 추가하면 된다. 첫 번째로, 시작 조건을 추가한다. Boost.MSM을 사용하고 있기 때문에 어떤 변수가 아니라 아래와 같이 typedef로 시작 조건이 정의된다.

	typedef OffHook initial_state;

마지막으로 그 어떤 상태 전이도 불가능할 때 수행할 동작을 정의해야 한다. 막다른 골목에 다다른 상황은 언제든 발생할 수 있다. 예를 들어 전화기가 망가졌다면 더 이상 아무것도 할 수 없다.

	template<class FSM, class Event>
	void no_transition(Event const& e, FSM&, int state)
	{
		cout << "No transition from state " << state_names[state] << " on event " << typeid(e).name() << endl;
	}

Boost.MSM은 상태 머신을 프런트엔드와 백엔드 두 부분으로 나누고 있다. 프런트 엔드는 지금껏 상태 머신을 정의하는 코드들이고 백엔드는 상태 머신을 생성하여 구동하는 부분이다. 백엔드 API를 이용하여 앞서 정의한 상태 머신을 생성할 수 있다.

	msn::back::state_machine<PhoneStateMachine> phone;

이 모든 것을 통합하면, 다음과 같은 시나리오를 실행할 수 있다.

	phone.process_event(CallDialed{});
	phone.process_event(CallConnected{});
	phone.process_event(PlacedOnHold{});
	
이러한 방법으로 상용 수준의 복잡한 상태 머신을 만들 수 있다.

## 요약
첫 번째로, Boost에서 두 종류의 상태 머신 라이브러리를 지원한다는 것을 알아두자. 하나는 여기서 살펴본 Boost.MSM이고 다른 하나는 Boost.Statechart이다. Boost말고도 많은 상태 머신 라이브러리가 있을 것이다.

두 번째로, 단순한 상태 머신 이상의 고차원 상태 머신들이 많이 있다는 것을 염두에 두자. 예를 들어 많은 라이브러리들이 "계층적 상태 머신"을 지원한다. 계층적 상태 머신이란, 예를 들어 몸이 아픈 상태는 감기, 근육통 등 수많은 하위 계층의 상태들을 가질 수 있다. 즉, 감기 상태에 있다면 당연하게도 아픈 상태이기도 하다.

마지막으로 현대의 상태 머신은 과거의 전통적인 상태 머신 디자인 패턴과 비교해 훨씬 더 진보되어 있다는 점을 짚고 넘어가자. 전통적인 방식은 중복된 API 문제, 자기 자신을 삭제하는 문제 등 바람직하지 않은 특성들을 가진다. 물론 동작은 하지만 비직관적이고 번거롭다.



