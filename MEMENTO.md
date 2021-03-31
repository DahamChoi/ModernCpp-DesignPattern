# 메멘토
*메멘토 패턴은 객체를 이전 상태로 되돌릴 수 있는 기능을 제공하는 디자인 패턴이다.*

## 은행 계좌
	class BankAccount
	{
		int balance = 0;
	public:
		explicit BankAccount(const int balance) : balance{ balance } {}

		Memento deposit(int amount)
		{
			balance += amount;
			return { balance };
		}

		void restore(const Memento& m)
		{
			balance = m.balance;
		}
	};

은행 계좌 예를 살펴보자. 이번에는 은행 계좌에 입금 기능만 있다고 가정한다. 즉 deposit() 멤버 함수만 존재한다. 그리고 이전에는 이 함수가 아무런 동작도 하지 않았지만 여기서는 Memento 객체를 리턴한다. Memento 객체는 계좌를 Memento 에 저장된 상태로 되돌릴 때 사용될 수 있다.

	class Memento
	{
		int balance;
	public:
		Memento(int balance) : balance{ balance } {}
		friend class BankAccount;
	};

Memento 클래스는 두 가지 특기할 점이 있다.
 - 불변 속성을 가진다. 저장된 잔고 값이 나중에 변경될 수 있다면 존재한 적 없는 과거 상태로 되돌리는 게 가능해져 버린다.
 - BankAccount를 friend로 선언한다. balance 필드 변수에 접근하기 위해 필요하다. friend로 선언하는 대신 Memento 클래스를 BankAccount 내부의 중첩 클래스로 만드는 방법도 있다.

이제 다음과 같이 계좌 상태를 기억해 두었다가 되돌릴 수 있다. 

	void meneto()
	{
		BankAccount ba { 100 };
		auto m1 = ba.deposit(50);
		auto m2 = ba.deposit(25);
		ba.restore(m1);
		ba.restore(m2);
	}

이 구현은 충분히 제 역할을 하지만 예외 상황 처리가 부족하다. 예를 들어 계좌를 만든 초기 상황으로는 돌아갈 수 가 없다. 왜냐하면 생성자에서는 balance의 값을 리턴할 방법이 없기 때문이다. 물론 포인터를 이용할 수는 있지만 코드가 지저분해진다.

## Undo와 Redo
BankAccount에서 생성되는 Memento를 모두 저장한다면 어떻게 될까? 이 경우 커맨드 디자인 패턴의 구현에서와 마찬가지로 되돌리기와 다시하기 작업이 가능해진다. 이제 Memento를 이용해서 Undo/Redo 기능을 어떻게 구현할 수 있는지 알아보자.

	class BankAccount2
	{
		int balance = 0;
		vector<shared_ptr<Memento>> changes;
		int current;
	public:
		explicit BankAccount(const int balance) : balance(balance)
		{
			change.emplace_back(make_shared<Memento>(balance));
			current = 0;
		}

		shared_ptr<Memento> depiosit(int amount)
		{
			balance += amount;
			auto m = make_shared<Memento>(balance);
			changes.push_back(m);
			++current;
			return m;
		}

		void restore(const shared_ptr<Memento>& m)
		{
			if(m)
			{
				balance = m->balance;
				changes.push_back(m);
				current = changes.size() - 1;
			}
		}

		shared_ptr<Memento> undo()
		{
			if(current > 0)
			{
				--current;
				auto m = changes[current];
				balance = m->balance;
				return m;
			}
			return {};
		}

		shared_ptr<Memento> redo()
		{
			if(current + 1 < changes.size())
			{
				++current;
				auto m = changes[current];
				balance = m->balance;
				return m;
			}
			return {};
		}
	};

이제 계좌 생성 초기 잔고 값이 저장되어 생성자에서 리턴할 수 없는 문제가 해결된다. 물론 생성자가 메멘토를 리턴할 수 있게 되는 것은 아니지만 되돌리기 작업에는 활용할 수 있다. 초기 상태로 되돌리기 위해 reset() 같은 전용 멤버 함수를 만들 수도 있고 메멘토의 저장소를 직접 읽을 수 있게 할 수도 있다. 이 부분은 구현하는 사람 마음대로이다.

위 코드에서 메멘토를 저장할 때 shared_ptr를 사용하고, 리턴할 때도 shared_ptr을 사용한다. 그리고 현재 위치를 나타내는 current 필드를 두어 방금 수행한 작업을 기준으로 뒤 또는 앞으로 순회할 수 있게 한다. 이것을 이용하여 되돌리기와 다시하기 작업을 쉽게 구현할 수 있다.

restore()함수는 앞서 보았던 것과는 크게 다르다. 첫 번째로, 인자로 받은 shared_ptr가 유효한지 검사한다. 이러한 검사를 하는 이유는 아무것도 하지 않는 기능을 제공하기 위해서이다. 이렇게 하면 어떤 선행 작업에서 아무것도 하지 않는 디폴트 값을 리턴할 수도 있다. 두 번째로 메멘토를 복구할 때 복구로 인한 변경 작업 자체도 변경 리스트에 추가하고 ㅇㅆ다. 이렇게 함으로써 방금 수행한 되돌리기를 다시 되돌리는 것도 가능하게 한다.

undo()함수를 살펴보자. 당연하지만, 변경 목록에 대한 현재 위치가 0보다 클 때만 undo()를 수행할 수 있다. 현재 위치가 0보다 크다면, 현재 위치를 한 칸 뒤로 옮기고 그 위치의 변경 사항을 얻어서 상태에 적용하고 그 변경을 리턴한다. 만약 이전 메멘토로 되돌릴 수 없다면 shared_ptr의 디폴트 생성자를 통해 공백 포인터 객체를 리턴한다. 공백 포이넡는 restore()에서 검사되어 무시된다. redo()도 비슷하게 구현된다.

## 요약
메멘토 디자인 패턴은 시스템의 상태를 이전으로 되돌리기 위한 토큰을 어떻게 정의하고 어떻게 관리할 수 있는지 보여준다. 보통 그러한 토큰은 시스템을 특정 상태로 옮기기에 딱 필요한 만큼의 작은 정보를 저장한다. 이러한 토큰들을 이용하여 시스템을 임의의 상태로 재설정하는 대신 시스템의 모든 정보를 저장해서 뒤또는 앞 상태로의 이동 과정을 하나하나 따져보고 통제할 수도 있다.


