# 매개자
*매개자 패턴은 어떻게 객체들의 집합이 상호작용하는지를 함축해놓은 객체를 정의한다. 매개자 패턴을 사용하면 객체 간 통신은 중재자 객체 안에 함축된다. 객체들은 더 이상 다른 객체와 서로 직접 통신하지 않으며 대신 중재자를 통해 통신한다. 이를 통신 객체 간 의존성을 줄일 수 있으므로 결합도를 감소시킨다.*

## 채팅 룸
인터넷 채팅 룸은 매개자 디자인 패턴이 적용될 수 있는 가장 전형적인 예이다. 

	struct Person
	{
		string name;
		ChatRoom* room = nullptr;
		vector<string> chat_log;

		Person(const string& name);
		
		void receive(const string& origin, const string& message);
		void say(const string& message) const;
		void pm(const string& who, const string& message) const;
	};

참여자는 이름(name), 대화 내용(chat_log), 채팅 룸 객체로의 참조를 가진다. 그리고 생성자와 세 개의 멤버 함수가 있다.
 - receive()는 메세지를 수신받는다. 보통 이러한 함수는 화면에 수신받은 메시지를 출력하고 대화 내용을 업데이트한다.
 - say()는 채팅 룸의 모든 참여자에게 메시지를 전송한다.
 - pm()은 개인 메시지 기능이다. 채팅 룸의 특정 참여자를 이름으로 지정하여 그 참여자에게만 메시지가 전송된다.

say()와 pm() 모두 채팅 룸에서 수행되어야 하는 작업을 중계하는 역할이다.

	struct ChatRoom
	{
		vector<Person*> people;

		void join(Person* p);
		void broadcast(const string& origin, const string& message);
		void message(const string& origin, const string& who, const string& message);
	};

포인터, 참조, shared_ptr 등등 어떤 방식으로 객체에 접근할지는 구현하는 사람의 취향에 달려있다. 하지만 std::vector를 사용할 경우 참조를 저장할 수 없다는 제약 사항이 따른다. 따라서 여기서는 포인터를 사용한다. ChatRoom의 API는 아래와 같이 단순하게 정의된다.
- join은 채팅 룸에 사용자가 입장할 수 있게 한다. 채팅 룸을 퇴장하는 API(levae())는 당장은 구현하지 않는다.
- boradcast()는 채팅 룸의 모든 참여자에게 메시지를 보낸다. 엄밀히 말하자면 보내는 사람 자기 자신에게는 보내지 않으므로 모든 참여자가 아니다.
- message()는 개인 메시지를 보낸다.

다음은 join의 구현이다.

	void ChatRoom::join(Person *p)
	{
		string join_msg = p->name + " joins the chat";
		broadcast("room", join_msg);
		p->room = this;
		people.push_back(p);
	}

전통적인 IRC 채팅 룸이 그렇듯이, 채팅 룸에 새로운 사용자가 입장하면 그 사실을 채팅 룸의 모든 사람에게 알린다. 이 경우 발신처는 채팅 룸에 들어온 개인이 아니라 채팅 룸 자체가 된다. 그다음 채팅 룸에 새로 들어온 Person의 room 포인터를 업데이트하고 채팅 룸의 참여자 목록에 Person의 포인터를 추가한다.

broadcast() 멤버 함수를 살펴보자. 여기서 태이룸의 모든 참여자에 대한 메시지 전송이 일어난다. 각 참여자는 메시지를 처리하기 위한 자기만의 Person::receive() 멤버 함수를 가지기 떄문에 아래와 같이 쉽게 broadcast()가 구현된다.

	void ChatRoom::broadcast(const string& origin, const string& message)
	{
		for(auto p : people)
			if(p->name != origin)
				p->receive(origin, message)
	}

마지막으로 채팅 룸에서의 개인 메시지 처리는 아래와 같이 message() 멤버 함수로 구현된다.

	void ChatRoom::message(const string& origin, const string& who, const string& message)
	{
		auto target = find_if(begin(people), end(poeple), [&](const Person * p) { return p->name == who });
		if(target != end(people))
		{
			(*target)->receive(origin, message)
		}
	}

참여자 목록에서 지정된 수신처를 탐색한 다음, 수신처의 참여자가 존재할 경우, 그 참여자에 메시지를 전달한다.

이제 채팅 룸 API가 준비되었으므로 Person의 say()와 pm() 멤버 함수는 아래와 같이 구현될 수 있다.

	void Person::say(const string& message) const
	{
		room->broadcast(name, message);
	}

	void Person::pm(const string& who, const string& message) const
	{
		room->message(name, who, message);
	}

receive() 멤버 함수는 수신되는 메시지를 화면에 출력하고 대화 내용에 추가하기에 적합한 위치이다.

	void Person::receive(const string& origin, const string& message)
	{
		string s { origin + ": " + message };
		cout << "[" << name <<"'s chat session] " << s << "\n"
		chat_log.emplace_back(s);
	}

## 매개자와 이벤트
이 채팅 룸 예에는 일관된 테마가 있다. 누군가 메시지를 올리면 참여자들은 알람을 받아야 한다. 어떤 이벤트를 가진 매개자라는 개념은 모든 참여자에게 해당된다. 참여자는 이벤트의 수신처로 등록할 수 있고, 이벤트를 발생시킬 수도 있다. 즉 알림이 발생하게 할 수 있다.

C++에서는 언어 차원에서의 이벤트 기능이 제공되지 않는다. 따라서 Boost.Signals2 라이브러리를 이용한다. Boost.Signals2는 이벤트의 활용에 필수적인 기능들을 제공한다. 단, 사용되는 용어에 약간의 차이가 있다. 이벤트를 시그널(알림을 발생시키는 객체), 수신처를 슬롯(알림을 처리하는 함수)이라 부른다.

채팅 룸 예제로 돌아가기 전에 더 단순한 예로 축구 게임을 살펴보자. 축구에는 선수와 코치가 있다. 선수가 득점을 하면 코치는 칭찬을 한다. 이때 누가 골을 넣었는지, 몇 골이나 넣었는지에 대한 정보가 전달되어야만 한다.

이러한 정보를 전달하기 위해, 이벤트 데이터를 일반화한 베이스 클래스를 아래와 같이 정의할 수 있다.

	struct EventData
	{
		virtual ~EventData() = default;
		virtual void print() const = 0;
	}

이벤트 내용을 콘솔로 내용을 출력할 수 있도록 print() 함수가 추가되었다. 이제 이 클래스를 상속받아 골 득점 정보를 저장하는 클래스를 만들 수 있다.

	struct PlayerScoredData : EventData
	{
		string player_name;
		int goals_scored_so_far;

		PlayerScoredData(const string& player_name, const int goals_scored_so_far)
			: player_name(player_name), goals_socred_so_far(goals_scored_so_far) {}

		void print() const override
		{
			cout << player_name << " has scored! (their " << goals_scored_so_far << " goal)" << endl;
		}
	};

다시 한번 매개자를 만든다. 그런데 이번에는 아무런 동작이 없다. 이벤트 기반 구조에서는 매개자가 직접 일을 수행하지 않는다.

	struct Game
	{
		signal<void(EventData*)> events;	//	Observer
	}

사실 이벤트를 전역 변수로 이벤트를 Game 클래스를 두지 않는 밥법도 있다. 하지만 그런 극단적인 방법은 사용하지 않는다. 명시적으로 Game객체의 참조가 컴포넌트에 주입되는 코드가 있으면 이벤트와의 종속 관계가 분명하게 드러나는 장점이 있다.

이제 Player 클래스를 만들 수 있다. Plkayer는 선수 이름, 게임에서의 골 득점 그리고 당연하게도 매개자인 GAme의 참조를 가진다.

	struct Player
	{
		string name;
		int goals_scored = 0;
		Game& game;

		Player(const string& name, Game& game)
			: name(name), game(game) {}

		void score()
		{
			goals_socred++;
			PlayerScoredData ps{name, goals_scored};
			game.events(&ps);
		}
	};

Player::score() 멤버 함수를 눈여겨 보자. 이 함수는 이벤트를 이용해 PlayerScoredData를 생성하고 모든 이벤트의 수신처로 등록된 객체들에 알림을 전송한다. 이 이벤트를 받을 객체를 만들어보자. 코치라면 당연히 골 이벤트를 받고 싶어 할 것이다.

	struct Coach
	{
		Game& game;
		explict Coach(Game& game) : game(game)
		{
			game.events.connect([](EventData* e)
			{
				PlayerScoredData* ps = dynamic_cast<PlayerScoredData*>(e);
				if(ps && ps->goals_scored_so_far < 3)
				{
					cout << "coach says : well done, " << ps->player_name << endl;
				}
			});
		}
	};

Coach 클래스의 구현은 단순하다. 코치의 이름 조차도 저장하지 않는다. 생성자에서 game.events에 수신 등록을 할 뿐이다. game에서 어떤 이벤트가 발생하면 아게 되어 필요한 처리를 할 수 있다. 여기서는 골 득점 이벤트 발생 시 처리할 내용을 람다 함수로 지정한다. 이 람다 함수를 이벤트 슬롯이라고 부를 수도 있다.

람다 함수의 인자 타입이 EventData*임에 유의하자. 골 득점 이벤트에 대해서만 처리를 하고 싶지만 어떤 이벤트가 올지 알 수 없다. 따라서 dynamic_cast를 이용해 목적하는 골 득점 이벤트 타입이 수신되었는지 확인하다.

마법 같은 부분들은 모두 설정 단계에서 일어난다. 각각의 이벤트마다 명시적으로 슬롯을 모두 나열할 필요는 없다. 처리하고 싶은 이벤트에 대해서만 처리해도 된다. 아래는 클라이언트의 예이다.

	Game game;
	Player playeR{ "Sam", game };
	Coach coach{ game };

	player.score();

## 요약
매개자 디자인 패턴은 시스템 내 컴포넌트 모두가 참조할 수 있는 어떤 중간자를 컴포넌트 간에 서로 직접적으로 참조하지 않더라도 커뮤니티케이션을 할 수 있게 한다는 것을 기본 아이디어로 한다. 매개자를 통해 직접적인 메모리 접근 대신 식별자(이름, 번호, 경로 등)로 커뮤니케이션을 할 수 있게 한다.

매개자의 가장 단순한 구현 형태는 멤버 변수로 리스트를 두고 그 리스트를 검사하여 필요한 항목만 선택적으로 처리하는 함수를 만드는 것이다.

매개자의 좀 더 정규한 구현 형태는 시스템에서 발생하는 이벤트들에 대해서 이벤트를 받길 원하는 객체가 컴포넌트 간에 전달되는 메시지를 이벤트로써 다룰 수 있게 한다. 이러한 구현에서는 어떤 컴포넌트가 시스템에서 소멸되거나 더 이상 이벤트를 처리없을 때 손쉽게 수신 해제를 할 수 있다.
