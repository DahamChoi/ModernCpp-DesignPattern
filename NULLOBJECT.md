# Null 객체
*null 객체는 참조 된 값이 없거나 정의 된 중립동작이 있는 객체이다. null 객체 디자인 패턴은 이러한 객체의 사용 및 동작을 설명한다.*

## 시나리오
아래와 같은 인터페이스를 가진 Logger 라이브러리를 사용한다고 가정하자.

	struct Logger
	{
		virtual ~Logger() = default;
		virtual void info(const string& s) = 0;
		virtual void warn(const string& s) = 0;
	};

이 라이브러를 이용하여 다음과 같이 은행 계좌의 동작들에 로깅 기능을 구현하려 한다.

	struct BankAccount
	{
		std::shared_ptr<Logger> lg;
		string name;
		int balance = 0;

		BankAccount(const std::shared_ptr<Logger>& logger, const string& name, int balance)
			: log{logger}, name{name}, balance{balance} {}
		
		void deposit(int amount)
		{
			balance += amount;
			log->info("Deposited $" + lexical_cast<string>(amount) +
				" to " + name ", balance is now $" + lexical_cast<string>(balance));
		}
	};

무엇이 문제일까? 로깅이 필요하다면 로깅 클래스를 구현해서 그냥 사용하면 된다.

	struct ConsoleLogger : Logger
	{
		void info(const string& s) override
		{
			cout << "INFO : " << s << endl;
		}
		void warn(const string& s) override
		{
			cout << "WARINING!!! " << s << endl;
		}
	};

하지만 상황에 따라 로깅을 해서는 안 되는 경우가 있다면 어떻게 해야 할까?

## Null 객체
BankAccount의 생성자를 다시 살펴보자.

	BankAccount(const shared_ptr<Logger>& logger, const string& name, int balance)

생성자의 인자로 Logger 객체를 넘겨받는다. 초기화되지 않은 shared_ptr\<Logger\>()를 전달해서 로깅 기능을  피할 수 있다고 가정하는 것은 안전하지 않다. 추가적인 문서로 설명이 되어 있지 않다면 BankAccount 내부적으로 Logger를 사용하기 전에 포인터를 검사하는지 알 수가 없다.

따라서 로깅 기능을 사용하지 않으면서도 BankAccount를 안전하게 생성할 수 있는 방법은 Logger의 Null객체를 전달하는 것이다. Null 객체는 그 객체가 가진 인터페이스 규약을 모두 준수하면서도 실제 동작은 하지 않는 객체이다.

struct NullLogger : Logger
{
	virtual info(const string& s) override {}
	virtual warn(const string& s) override {}
};

## shared_ptr는 Null객체가 아니다
shared_ptr를 비롯한 스마트 포인터들은 Null 객체가 아니라는 점을 명확히 짚고 넘어가자. Null 객체는 그 인터페이스가 약속에 따라 올바르게 동작하는 특성을 보존하면서도 실제로는 아무것도 하지 않는 객체이다. 하지만 Null 객체 대신 초기화되지 않은 스마트 포인터로 멤버 함수를 호출하면 크래시가 발생한다.

스마트 포인터는 멤버의 호출에 있어서 "안전"하게 동작할 방법이 없다. 즉, 초기화되지 않은 스마트 포인터 foo에 대해서 foo->bar()와 같은 호출이 저절로 "아무것도 안 하는" 더미 호출이 되도록 하는 방법은 없다. 그 이유는 스마트 포인터에서 역참조 연산자 *, 포인터 멤버 참조 연산자 ->가 하는 역할이 특별한 것이 없고 단순히 스마트 포인트가 감싸고 있는 일반 포인터에 직접 적용되기 때문이다.

## 개선된 디자인
만약 BankAccount를 내 마음대로 바꿀 수 있다면 어떻게 할까? 좀 더 쉽게 인터페이스를 바꿀 수 있을까? 아래와 같이 몇 가지 아이디어가 있을 수 있다.
 - 모든 포인터 사용처마다 포인터의 유효성 검사를 추가한다. 이렇게 하면 BankAccount 입장에서는 올바르게 동작할 것이다. 하지만 라이브러리의 사용자 입장에서는 여전히 혼란스럽다. 포인터가 null이어도 된다는 사실을 문서 등의 다른 방법으로 사용자가 알아야만 한다
 - 디폴트 인자를 추가한다. 즉 const shared_ptr<Logger>& logger = no_logging과 같이 설정한다. 여기서 no_logging은 shared_ptr의 디폴트 생성자 객체로 BankAccount 안에 멤버로서 가지고 있는다. 당연하게도 이렇게 하면 로깅 객체가 사용하는 곳마다 shared_ptr가 감싸고 있는 포인터가 null인지 여부를 검사를 해야한다.
 - optional 타입을 사용한다. 이 방법은 관례적으로도 올바르고 의도적으로도 목적하는 바와 합치한다. 하지만 optional<shared_tr<T>>을 전달하는 번거로움과 optional의 공백 여부를 확인하는 작업들이 추가되어야 한다.

## 묵시적인 Null 객체
또 다른 급진적인 아이디어가 있다. 호출과 집행 두 가지 절차로 나누어 로깅을 처리하는 것이다. 즉, 인터페이스에 종속된 부분(호출)과 실제 로그를 저장하는 부분(집행)을 구분한다. 다음의 OptionalLogger 정의를 보자

	struct OptionalLogger : Logger {
		shared_ptr<Logger> impl;
		static shared_ptr<Logger> no_logging;
		Logger(const shared_ptr<Logger>& logger) : impl{logger} {}
		virtual void info(const string& s) override{
			if(impl) impl->info(s);
		}
		virtual void warn(const string&s) override{
			if(impl) impl->warn(s);
		}
	};

	shared_ptr<Logger> BankAccount::no_logging{};

이제 호출 부분을 구현 부분으로부터 추상화하였다. 따라서 BankAccount의 생성자를 다음과 같이 재정의할 수 있다.

	shared_ptr<OptionalLogger> logger;
	BankAccount(const string& name, int balance, const shared_ptr<Logger>& logger = no_logging)
		: log{make_shared<OptionalLogger>(logger)}, name{name}, balance{balance} {}

이 코드는 꽤 기교적이다. 인자로 넘겨받는 로깅 객체를 그대로 사용하지 않고 안전한 사용을 보증하는 OptionalLogger로 감싼다. 이렇게 함으로써 디폴트 인자 값(no_logging)이 사용되든 사용자가 null 을 넘기든 관계없이 로딩 객체가 유효할 때만 로깅 호출이 일어난다.

## 요약
Null 객체 패턴은 API 설계에서 발생하는 문제를 상기시킨다. 의존하는 객체들에 어떤 종류의 가정을 할 수 있을까? 일반 포인터든 스마트 포인터든 포인터를 넘겨 받는다면 그 포인터의 모든 사용차머다 유효성 검사를 해야 할 책임이 있는 걸까?

만약 책임이 없다고 느낀다면 클라이언트로서 유일한 대응 방법은 인터페이스에 내재된 규약을 준수하되 실제 동작은 하지 않는 Null 객체를 구현하는 것이다. 그런데 이 방법은 멤버 함수에 대해서만 효과가 있다. 만약 멤버 변수를 사용하고 있다면 Null 객체로 올바른 동작을 보증하기 어렵다.

전달되는 객체 인자에 적극적으로 Null 객체를 활용하고 싶다면 명시적으로 그렇게 해야 한다. 파라미터 타입에 std::optional을 지정하거나, 자체적인 Null 객체를 디폴트 값에 지정하여 힌트를 주어야 한다. 또는 해당 인자에 기대되는 값이 무엇인지 문서를 통해 명확히 할 수도 있다.