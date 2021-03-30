# 책임 사슬
***chain-of-responsibility pattern**은 명령 객체와 일련의 처리 객체를 포함하는 디자인 패턴이다. 각각의 처리 객체는 명령 객체를 처리할 수 있는 연산의 집합이고, 체인 안의 처리 객체가 핸들할 수 없는 명령은 다음 처리 객체로 넘겨진다. 이 작동방식은 새로운 처리 객체부터 체인의 끝까지 다시 반복된다.*

## 시나리오
어떤 컴퓨터 게임에 크리처들이 있다고 하자. 이 크리처들은 그 이름과 함께 공격력과 방어력 두 가지 값을 속성으로 가진다.

이제 게임이 진행되는 동안 크리처가 처리되어야 한다. 아이템을 습득할 수도 있고 마법에 당할 수도 있다. 어떤 경우든 공격력 또는 방어력 값이 이벤트에 맞게 변경되어야 한다. 이를 위해 CreatureModifier 를 호출한다.

더 나아가서, 한번에 여러 개의 이벤트가 발생하여 CreateModifer가 여러 번 호추로딜 수도 있다. 따라서 변경 작업을 크리처별로 쌓아 놓아 순서대로 속성이 변경될 수 있게 한다.

## 포인터 사슬

    class CreatureModifier
    {
    private:
    	CreatureModifier* next{ nullptr };
    
    protected:
    	Creature& creature;
    
    public:
    	explicit CreatureModifier(Creature& creature)
    		: creature(creature) {}
    
    	void add(CreatureModifier* cm)
    	{
    		if (next) next->add(cm);
    		else next = cm;
    	}
    
    	virtual void handle()
    	{
    		if (next) next->handle();
    	}
    };

CreatureModifier 구현은 책임 사슬의 전통적인 구현 방법을 보여준다. 위 코드에서는 여러 가지 일이 일어나고 있다. 하나씩 차례대로 살펴보자.
 - 이 클래스는 Creature의 참조를 넘겨받아 저장하고 변경할 준비를 한다.
 - 이 클래스는 실질적으로 하는 일이 많지 않지만 추상 클래스는 아니다. 모든 멤버가 구현되어 있다.
 - 멤버 변수 next는 부가적으로 현재의 변경 작업에 뒤따르는 또 다른 CreatureModifier를 가리킨다. 당연하지만 이 포인트의 실 구현체는  CreatureModifier를 상속받는 무엇이든 가능하다.
 - add() 멤버 함수는 다른 크리처에 대한 변경 작업을 현재의 변경 작업 사슬에 연결하여 추가한다. 이러한 연결은 재귀적으로 일어난다. 만약 현재 사슬이 nullptr을 가리키고 있으면 인자로 주어진 변경 작업을 그대로 설정하고 재귀 호출에는 진입하지 않는다.
 - handle() 멤버 함수는 단순히 사슬의 다음 항목을 처리한다. 이 함수는 그자체로는 아무런 작업을 수행하지 않는다. 이 함수는 목적에 따라 오버라디이 될 수도 있기 때문에 virtual로 선언되어 있다.

이 구현은 단지 연결 리스트에 항목을 추가하는 정도밖에는 특별할 게 없다. 하지만 이클래스를 상속받아 실질적인 작업들이 추가되기 시작하면 이 구현의 의미가 더 명확해진다. 예를 들어 크리처의 공격력을 두 배로 키우는 변경 작업이 다음과 같이 정의될 수 있다.

    class DoubleAttackModifier : public CreatureModifier
    {
    public:
    	DoubleAttackModifier(Creature& creature)
    		: CreatureModifier(creature) {}
    
    	void handle() override
    	{
    		creature.attack *= 2;
    		CreatureModifier::handle();
    	}
    };

이 클래스는 CreatureModifier를 상속받아 handle() 메서드 안에서 두 가지 작업을 한다. 하나는 공격력을 두 배로 키우는 것이고 다른 하느는 부모 클래스의 handle() 메서드를 호출하는 것이다. 부모의 handle()을 호출하는 부분이 매우 중요하다. 변경 작업의 사슬이 연이어질 수 있으려면 중간의 어느 클래스에서도 handle()의 구현부 마지막에서 부모의 handle()을 호출하는 것을 빠드리지 않아야 한다.

또 다른 흥미로운 예를 살펴보자. 크리처에 마법을 걸어 어떤 보너스도 받을 수 없게 만드는 변경 작업을 적용하고 싶다고 하자. 언뜻 생각하기에는 쉽게 구현하기 어려울 것 같다. 하지만 쉽게 할 수 있다. 왜냐하면 단순히 부모의 handle()을 호출하지 않기만 하면 되기 때문이다. 그렇게 하면 전체 책임 사슬의 호출이 생략된다.

## 브로커 사슬
앞서 살펴본 포인터를 이용한 사슬 예는 매우 인위적이다. 실제 게임에서는 크리처가 임의로 보너스를 얻거나 잃을 수 있어야 한다. 이어 붙이는 것만 가능한 연결 리스트로는 임의의 변경 작업을 지원할 수 없다. 게다가 일반적인 게임이라면 크리처의 상태를 영구적으로 변경하는것이 아니라 원본은 남겨두고 임시로 변경 작업이 적용되게 하고 싶을 것이다.

책임 사슬을 구현하는 또 다른 방법은 중앙 집중화된 컴포넌트를 두는 것이다. 이 컴포넌트는 게임에서 발생할 수 있는 모든 변경 작업의 목록을 관리하고 특정 크리처의 공격력 또는 방어력의 상태를 그간의 변경작업 이력이 모두 반영된 상태로 구할 수 있게 한다.

이러한 컴포넌트를 이벤트 브로커라 부른다. 왜냐하면 이 컴포넌트는 참여하는 모든 컴포넌트를 연결하는 역할을 하기 때문이다. 이것은 매개자 디자인 패턴이기도 하고, 모든 이벤트를 모니터링한 결과를 조회할 수 있게 하기 때문에 관찰자 디자인 패턴이기도 하다.

이제 이벤트 브로커를 하나 만들어 보자. 이번에도 게임을 예로 든다. 먼저 Game  클래스를 만든다. 이 클래스는 게임의 실행에 대한 모든 것을 담는다.

	class Game // 매개자
	{
		signal<void(Query&)> queries;
	};

상태 조회 명령을 전송하기 위해 Boost.Signals2 라이브러리를 사용한다. 이 라이브러리는 어떤 신호를 발생시키고 그 신호를 기다리고 있는 모든 수신처가 신호를 처리할 수 있게 한다.

어떤 크리처의 상태를 임의의 시점에 조회할 수 있게 하고 싶다고 하자. 단순히 크리처의 필드를 읽을 수도 있지만 문제가 있다. 크리처에 가해진 변경 작업이 모두 완료되어 결괏값이 확정된 이후에 읽어야 한다. 따라서 조회 작업을 별도의 객체에 캡슐화하여 처리하기로 한다.(커멘트 패턴) 이 객체는 다음과 같이 정의된다.

struct Query {
	string creature_name;
	enum Argument { attack, defense } argument;
	return result;
}

위 클래스는 어떤 크리처의 특정 상태 값에 대한 조회라는 개념을 캡슐화 하고 있다. 이 클래스를 사용하기 위해 필요한 것은 크리처의 이름과 조회할 상태 값의 종류뿐이다. Game::queries에서 변경 작업들을 적용하고 최종 결괏값을 리턴하기 위해 Query 객체에 대한 참조를 사용한다.

이제 Creature의 정의를 살펴보자. 앞서 보았던 정의와 거의 같다. 필드 변수에서의 차이점은 Game의 참조가 추가되었다는 것 하나뿐이다.

    class Creature
    {
    private:
    	Game& game;
    	int attack, defense;
    
    public:
    	std::string name;
    
    	Creature(Game& game, const std::string& name, int attack, int defense)
    		: game{ game }, name { name }, attack{ attack }, defense{ defense } {}
    
    	friend std::ostream& operator<<(std::ostream& os, Creature& creature)
    	{
    		os << "name : " << creature.name << std::endl;
    		os << "attack : " << creature.get_attack() << std::endl;
    		os << "defense : " << creature.defense << std::endl;
    		return os;
    	}
    
    	int get_attack() const
    	{
    		Query q{ name, Query::Argument::attack, attack };
    		game.queries(q);
    		return q.result;
    	}
    };

attack, defense 변수가 private으로 선언된다는 점을 눈여겨보자. 이 값들이 private이라는 것은 변경 작업들이 반영된 최종 값이 별도의 get 멤버 함수로 얻어져야 한다는 것을 의미한다. 예를 들어 get_attack()과 같이 이용된다.

여기서 마법 같은 일이 일어난다. 단순한 값을 리턴하거나 포인터 기반의 정적 책임 사슬을 이용하는 대신, 목적하는 인자로 Query 객체를 만든 다음 Game::queries에 넘기면 등록된 수신처들이 각각 조회 객체를 검사하여 자신이 처리 가능한 경우 결과를 채워준다.(등록된 수신처들 모두 조회 객체에 접근하여 값을 바꿀 수 있다)

이제 변경 작업을 구현해보자. 이번에도 베이스 클래스를 만든다. 단, 이번에는 handle() 메서드가 없다. 변경 작업의 베이스 클래스는 그다지 흥미로울 부분이 없다. 사실, 전혀 사용하지 않을 수도 있다. 이 클래스가 하는 일은 생성자가 올바른 인자로 호출되는 것을 보증하는 역할 뿐이다. 하지만 앞서 베이스 클래스를 이용하는 방식을 사용했으므로 그 관계를 따르기로 한다. 이제 CreatureModifer를 상속받아 변경 작업 클래스가 실제로 어떻게 구현될 수 있는지 보자.

    class DoubleAttackModifier : public CreatureModifier
    {
    private:
    	boost::signals2::connection conn;
    
    public:
    	DoubleAttackModifier(Game& game, Creature& creature)
    		: CreatureModifier(game, creature)
    	{
    		conn = game.queries.connect([&](Query& q)
    		{
    				if (q.creature_name == creature.name &&
    					q.argument == Query::Argument::attack)
    					q.result *= 2;
    		});
    	}
    
    	~DoubleAttackModifier()
    	{
    		conn.disconnect();
    	}
    };

위 코드에서 볼 수 있듯이 생성자와 소멸자에서 주요 작업들이 수행되고 추가적인 메서드는 필요하지 않다. 생성자에서는 Game에 대한 참조를 통해 Game::queries에 접근하여, 공격력을 두 배 증가시키는 람다 함수를 조회 이벤트에 연결한다. 이 람다 함수는 먼저 몇가지 확인 작업을 해야 한다. 인자로 주어진 것이 작업을 적용할 수 있는 적절한 크리처인가? 조회 이ㅔㅂㄴ트가 공격력에 대한 것이 맞는가? 이 두 가지 확인에 관련된 정보는 모두 Query의 참조에 들어 있다. 그리고 변경할 초깃값도 함께 들어 있다. 그리고 변경할 초깃값도 함께 들어 있다.

나중에 객체가 소멸되었을 때 이벤트의 연결을 해제할 수 있도록 연결 정보도 저장해야 한다. 그렇게 함으로써 변경 작업이 임시적으로 적용되고 유효한 조건을 벗어났을 때 더 이상 적용되지 않게 할 수 있다.

이러한 것들을 모두 포함하면 다음과 같은 구현이 만들어진다.

     Game game;
       	Creature goblin{ game, "Boblin", 2, 2 };
       	std::cout << goblin;
       	{
       		DoubleAttackModifier dam{ game, goblin };
       		std::cout << goblin;
       	}
       	std::cout << goblin;

무슨 일이 일어나고 있는 걸까? 변경 작업이 적용되기 전 고블린의 공격력과 벙어력은 각각 2,2이다. 그다음 게임 중에 공격력이 2배로 커지는 범위에 들어오면(코드상에서는 중괄호 블록 안) DoubleAttackModifier를 적용한다. 그러면 고블린의 공격력과 벙어력이 각각 4,2가 된다. 그리고 그 범위를 벗어나면 자동으로 변경 작업의 소멸자가 호출되어 브로커와의 연결을 끊고 변경 작업이 더 이상 적용되지 않게 된다. 즉, 범위 밖에서는 고블린의 공격력과 벙어력이 각각 2,2로 원래대로 돌아간다.

## 요약
책임 사슬은 컴포넌트들이 어떤 명령이나 조회 작업을 차례로 처리할 수 있게 하는 매우 단순한 디자인 패턴이다. 책임 사슬의 가장 단순한 구현은 포인터 사슬이다. 이론적으로 포인터 사슬은 보통의 vector나 list로 대체될 수 있다.