# 코멘드
*커맨드 패턴이란 요청을 객체의 형태로 캡슐화하여 사용자가 보낸 요청을 나중에 이용할 수 있도록 메서드 이름, 매개변수 등 요청에 필요한 정보를 저장 또는 로깅, 취소할 수 있게 하는 패턴이다.*

## 시나리오
은행의 마이너스 통장을 생각해보자. 마이너스 통장은 현재 잔고와 인출 한도가 있다. 입금과 출금 동작을 위해 아래와 같이 deposit()과 withdraw() 멤버 함수를 만든다.

    class BankAccount
    {
    private:
    	int balance = 0;
    	int overdraft_limit = -500;
    
    public:
    	void deposit(int amount)
    	{
    		balance += amount;
    		std::cout << "deposited " << amount << ", balance is now " << balance << std::endl;
    	}
    
    	void withdraw(int amount)
    	{
    		if (balance - amount >= overdraft_limit)
    		{
    			balance -= amount;
    			std::cout << "withdraw " << amount << ", balance is now " << balance << std::endl;
    		}
    	}
    };

이 멤버 함수들을 직접 호출하여 입금을 할 수도 있고 출금을 할 수도 있다. 하지만 앞서 언급했듯이 금융 감사가 가능하도록 모든 입출금 내역을 기록해야 한다고 하자. 그런데 이러한 입출금 클래스는 이미 기존에 만들어져 검증되었고 동작 중이어서 수정할 수 없다고 가정하자.

## 커맨드 패턴의 구현
먼저 커맨드의 인터페이스를 정의한다.

    class Command
    {
    public:
    	virtual void call() const = 0;
    };

이 인터페이스를 이용해 은행 계좌를 대상으로 한 작업 정보를 캡슐화하는 BankAccountCommand를 정의할 수 있다.

    class BankAccountCommand : public Command
    {
    public:
    	enum Action { deposit, withdraw };
    
    private:
    	BankAccount& account;
    	Action action;
    	int amount;
    
    public:
    	BankAccountCommand(BankAccount& account, const Action action, const int amount)
    		: account(account), action(action), amount(amount) {}
    
    	void call() const override
    	{
    		switch (action)
    		{
    		case deposit:
    			account.deposit(amount);
    			break;
    		case withdraw:
    			account.withdraw(amount);
    			break;
    		}
    	}
    };

커멘드에는 다음과 같은 정보들이 포함된다.
 - 작업을 적용될 대상 계좌
 - 실행될 작업 종료, 해당 작업에 연관된 옵션과 변수들의 집합이 한 번의 선언으로 모두 정의된다.
 - 입금 또는 출금할 금액의 크기

클라이언트가 이러한 정보를 제공하면 커맨드를 입금 또는 인출 작업을 실행 할 수 있다. 이러한 접근 방법으로 커맨드를 만들고 커맨드가 지정하고 있는 계좌에 변경 작업을 가할 수 있다.

## 되돌리기(Undo) 작업
커맨드는 계좌에서 일어난 작업들에 대한 모든 정보를 담고 있기 때문에 어떤 작업에 의한 변경 내용을 되돌려서 그 작업이 행해지기 이전 상태의 계좌를 리턴할 수 있다.

먼저, 되돌리기 작업을 커맨드 인터페이스 추가 기능으로 넣을지 아니면 또 하나의 커맨드로서 처리할지 결정해야 한다. 여기서는 편의상 커맨드 인터페이스의 추가 기능으로서 구현하기로 한다. 이 부분은 디자인 차원에서의 의사 결정으로 1장에서 이야기한 인터페이스 분리 원칙을 따르는 것이 바람직하다. 예를 들어 커맨드들 중에 비가역적으로 적용되는 것들이 있다면 커맨드 인터페이스 자체에 되돌리기 인터페이스가 포함될 경우 클라이언트에게 혼란을 준다. 이런 경우에는 되돌리기가 가능한 커맨드와 일반 커맨드를 분리하는 것이 좋을 것이다.

    class Command
    {
    public:
    	virtual void call() = 0;
    	virtual void undo() = 0;
    };
	
	void undo() override
	{
		switch (action)
		{
		case withdraw:
			account.deposit(amount);
			break;
		case deposit:
			account.withdraw(amount);
		}
	}

되돌리기가 추가된 Command인터페이스와 BankAccountCommand::undo()의 구현이다. 이 구현은 문제가 있다. 만약 은행 전체의 잔고보다 큰 금액을 출금하려 했다면 당연히도 실패했을 것이다. 하지만 그 작업을 되돌리기할 때는 실패한 작업이라는 사실을 확인하지 않는다. 즉, 되돌리기 작업 때문에 은행 자산만큼의 큰 금액이 입금되어 버릴 수 있다.

따라서, 출금 작업의 성공여부를 리턴하도록 withdraw()를 수정한다.

	bool withdraw(int amount)
	{
		if (balance - amount >= overdraft_limit)
		{
			balance -= amount;
			std::cout << "withdraw " << amount << ", balance is now " << balance << std::endl;
			return true;
		}
		return false;
	}

이제 BankAccountCommand 전체를 수정하여 아래와 같은 두 작업을 수행하게 한다.
 - 출금 때마다 success 플래그를 내부적으로 저장한다.
 - undo()가 호출될 때 이 플래그를 이용한다.
 
이제 플래그를 가졌으니 undo()의 구현을 개선할 수 있다. 이와 같은 수정으로 출금 커맨드의 되돌리기가 부작용을 일으키지 않게 되었다.

이 예제는 커맨드에 작업 관련 정보뿐만 아니라 임시 정보까지 저장할 수 있다는 것을 보여준다. 이러한 방식은 금융 거래 이력에 대한 감사 기능을 추가할 때도 유용하다.

## 컴포지트 커맨드
계좌 A에서 계좌 B로의 이체는 다음의 두 커맨드로 수행될 수 있다.
1. 계좌 A에서 $X만큼 출금
2. 계좌 B에 $X만큼 입금
두 커맨드를 각각 호출하는 대신 하나의 "계좌 이체" 커맨드로 감싸서 처리할 수 있다면 편리할 것이다. 이것은 컴포지트 패턴에서 지향하는 것과 동일하다.
컴포지트 커맨드의 골격을 만들어보자. vector\<BankAccountCommand\>을 상속받기로 한다. std::vector에 버추얼 소멸자가 없기 때문에 일반적으로는 바람직하지 않지만 이 에에서는 문제 되지 않는다.

	    class CompositeBankAccountCommand 
	    	: public std::vector<BankAccountCommand>, public Command
	    {
	    public:
    	CompositeBankAccountCommand(const std::initializer_list<value_type>& items)
    		: vector<BankAccountCommand>(items) {}
    
    	void call() override
    	{
    		for (auto& cmd : *this)
    			cmd.call();
    	}
    
    	void undo() override
    	{
    		for (auto it = rbegin(); it != rend(); ++it)
    			it->undo();
    	}
    };

위 코드에서 볼 수 있듯이 CompositeBankAccountCommand는 vector이면서도 Command이다. 즉, 컴포지트 디자인 패턴을 따르고 있다. 생성자는 편리한 initializer_list를 이용해 인자를 받고 있고 undo(), call() 두 개의 작업이 구현되어 있다.

    class MoneyTransferCommand : public CompositeBankAccountCommand
    {
    public:
    	MoneyTransferCommand(BankAccount& from, BankAccount& to, int amount)
    		: CompositeBankAccountCommand
    	{
    		BankAccountCommand{
    			from, BankAccountCommand::withdraw, amount}, 
    		BankAccountCommand{
    			to, BankAccountCommand::deposit, amount }
    	} {}
    };

베이스 클래스의 생성자를 재사용해 두 개의 커맨드로 "계좌 이체 커맨드" 객체를 초기화하고 있다. 그리고 베이스 클래스의 call()/undo() 구현도 재사용하다.

하지만 앞서 이야기되었듯이 문제가 있다. 베이스 클래스의 구현은 명령이 실패할 수도 있다는 것을 고려하지 않고 있다. 만약 계좌 A에서의 출금이 실패했다면 계좌 B로의 입급이 실행되어서는 안 된다. 이때는 전체 명령 사슬이 취소되어야 한다.

이러한 예외 처리를 지원하려면 아래와 같이 큰 변화가 필요하다.
 - Command에 success 플래그를 추가한다.
 - 각 작업의 수행마다 성공, 실패 여부를 기록한다.
 - 성공한 명령에 대해서만 되돌리기(undo) 명령이 수행될 수 있도록 한다.
 - 명령의 되돌림을 주의 깊게 수행하는 클래스 DependentCompositeCommand를 커맨드 클래스 간에 둔다.

각 커맨드가 호출될 때, 직전의 커맨드가 성공적으로 수행되었을 때만 실제 수행되게 한다. 그렇지 않은 경우 success 플래그를 false로 세팅하고 커멘드를 실행 하지 않는다.

undo() 멤버 함수를 오버라이딩할 필요는 없다. 왜냐하면 개별 커멘드가 자체적으로 자신의 success 플래그를 확인하고 이 플래그가 true일 때만 되돌리기 작업을 실행하기 때문이다.

구성하고 있는 모든 커맨드가 성공적으로 실행되어야만 성공하는 더 강한 조건의 컴포지트 커맨드를 생각해볼 수도 있다. 예를 들어 계좌 이체를 할 때 출금은 성공했더라도 입급이 실패했다면 전체를 취소하는 것이 올바를 것이다.

## 명령과 조회의 분리
명령과 조회를 분리(Command Query Separation, CQS)한다는 개념은 어떤 시스템에서의 작업을 크게 보았을 때 다음의 두 종류 중 한 가지로 분류될 수 있다는 것에서 제안되었다.
 - 명령 : 어떤 시스템의 상태 변화를 야기하는 작업 지시들로 어떤 결괏값의 생성이 없는 것
 - 조회 : 어떤 결괏값을 생성하는 정보 요청으로, 그 요청을 처리하는 시스템의 상태 변화를 일으키지 않는 것

직접적으로 상태에 대한 읽기, 쓰기 작업을 노출하고 있는 객체라면 그러한 것들을 private로 바꾸고 각 상태에 대한 get/set 멤버 함수들 대신 단일 인터페이스로 바꿀 수 있다. 예를 들어 체력과 빠르기 두 개의 속성을 가지는 크리처 클래스가 있다고 할 때, 각 속성에 대한 get/set/메서드 대신 아래처럼 단일한 커맨드 인터페이스를 제공할 수 있다.

	class Creature
	{
		int strength, agility;
	public:
		Creature(int strength, agility) :
			strenth{ strenth }, agility{ agility } {}
		
		void process_command(const CreatureCommand& cc);
		int process_query(const CreatureQuery& q) const;
	};

위 코드에는 get/set 멤버 함수가 없다. 대신 두개의 API 멤버 함수 process_command()와 process_query()가 있다. Creature가 제공해야 하는 속성과 기능이 아무리 늘어나더라도 이 API 두 개만으로 처리된다. 즉 커맨드만으로 크리처와의 상호작용을 수행할 수 있다. 이 API들은 가각 전용 클래스로 정의되고 enum 타입 클래스 CreatureAbility와 연관된다.

	enum class CreatureAbility{ strength, agility };
	
	struct CreatureCommand
	{
		enum Action{ set, increaseBy, decreaseBy } action;
		CreatureAbility ability;
		int amount;
	};

	struct CreatureQuery
	{
		CreatureAbility ability;
	};

위 코드에서 볼 수 있듯이 명령 객체는 어떤 멤버 변수를 어떻게 얼마만큼 바꿀 것인가를 지시한다. 조회 객체는 조회할 대상만 지정한다. 여기서는 조회 결과가 함수 리턴 값으로 전달되는 것으로 가정하고 조회 객체 자체에 따로 저장하지는 않는다.하지만 앞서 보았듯이 만약 다른 객체가 이 객체에 영향을 미친다면 조회 객체에 값을 저장할 필요가 생길 수도 있다.

다음은 process_command()의 정의다.

	void Creature::process_command(const CreatureCommand &cc)
	{
		int* ability;
		switch(cc.ability)
		{
		case CreatureAbility::strength:
			ability = &strength;
			break;
		case CreatureAbility::agility;
			ability = &agility;
			break;
		}
		switch(cc.action)
		{
		case CreatureCommand::set:
			*ability = cc.amount;
			break;
		case CreatureCommand::increaseBy:
			*ability += cc.amount;
			break;
		case CreatureCommand::decreaseBy:
			*ability -= cc.amount;
			break;
		}
	}

process_query()의 정의는 더 단순하다.

	int Creature::process_query(const CreatureQuery &q) const
	{
		switch(q.ability)
		{
			case CreatureAbility::strength: return strnegth;
			case CreatureAbility::agility: return agility;
		}
	}

명령과 조회에 로깅이 필요하거나 객체가 유지해야 한다면 위 두 멤버 함수만 수정하면 된다. 유일한 문제는 익숙한 get/set방식의 API를 고집하는 클라이언트를 어떻게 지원하느냐이다.

다행이도 아래와 같이 process_...()멤버 함수의 호출을 감싸고 적절히 인자를 전달함으로써 get/set함수를 쉽게 만들 수 있다.

	void Creature::set_strenth(int value)
	{
		process_command(CreatureCommand{
			CreatureCommand::set, CreatureAbility::strength, value
		});
	}

	int Creature::get_strength() const
	{
		process_query(CreatureQuery{CreatureAbility::strength});
	}

위 코드는 명령과 조회를 분리를 구현하는 시스템의 내부에서 무슨 일이 일어나고 있는지 매우 간명하게 보여주는 예이다. 더불어, 객체의 모든 인터페이스를 명령과 조회 두 종류로 구분하는 방법을 보여주고 있기도 하다.
		

## 요약
커맨드 디자인 패턴은 단순하다. 인자를 전달하여 메서드를 호출하는 직접적인 방법으로 객체에 일을 시키는 대신, 작업 지시 내용을 감싸는 특별한 객체를 두어 객체와 커뮤니케이션하게 한다.

어떤 경우에는 객체에 변화를 일으키거나 어떤 작업을 시키는 것이 아니라 단순히 대상 객체로부터 어떤 정보를 얻고 싶을 수도 있다. 이러한 경우를 처리하는 커맨드 객체를 조회 객체라고 부른다. 많은 경우 조회 객체는 메서드의 리턴 값으로 정보를 전달하는 불변 객체이다. 하지만 13장의 책임 사슬 예에서 보았듯이 리턴하는 결과를 다른 컴포넌트가 변경할 수 있어야 하는 경우도 있다. 단, 이때도 컴포넌트 자체가 변경되지는 않고 리턴되는 결과만 변경된다.

커맨드 디자인 패턴은 많은 UI 시스템에서 "복사하기", "붙여넣기"와 같은 익숙한 동작들을 캡슐화하는 데 사용되고 있다. 커맨드는 여러 경로로 실행될 수 있다. 예를 들어 "복사하기"는 메뉴에서 실행될 수도 있고, 툴바 아이콘에서 실행될 수도 있고, 키보드 단축키로 실행될 수도 있다. 그리고 여러 작업들의 시퀸스를 저장해 두었다가 단일 명령으로 일괄처리 할 수도 있다.