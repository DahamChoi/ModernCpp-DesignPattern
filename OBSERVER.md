# 관찰자
*옵저버 패턴은 객체의 상태 변화를 관찰하는 관찰자들, 즉 옵저버들의 목록을 객체에 등록하여 상태 변화가 있을 때마다 메서드 등을 통해 객체가 직접 목록의 각 옵저버에게 통지하도록 하는 디자인 패턴이다.*

## 속성 관찰자
	struct Person
	{
		int age;
		Person(int age) : age{ age } {}
	};

사람의 나이가 바뀌는 것을 어떻게 알 수 있을까? 그냥은 알 수가 없다. 변경을 알아채기 위해서 폴링을 할수도 있다. 즉, 매 100msec마다 현재 나이를 읽어서 직전의 나이와 비교한다. 이러한 방법은 효과가 있기는 하지만 대단히 비효율적이다. 좀 더 똑똑한 방법이 필요하다.

Person의 age에 쓰기 작업이 될 때 정보를 받을 수 있다면 가장 효율적이다. 누구든 쉽게 그렇게 생각해낼 것이다. 그러한 방법을 구현하기 위한 유일한 방법을 set 멤버 함수를 두는 것이다.

이제 set_age가 호출되어 값이 바뀔 때 어떤 알림을 보낼 수 있으면 된다. 그런데 어떻게 알림을 보낼 수 있을까?

## Observer\<T\>

한 가지 접근 방법은 Person의 변화에 관심을 가지는 쪽에서 사용할 수 있도록 어떤 변경이 일어날 때마다 호출되는 멤버 함수(보통 콜백이라 부른다)를 정의하여 베이스 클래스에 두는 것이다. 클라이언트에서는 이 베이스를 상속받아 변경시점에 수행할 작업을 구현한다. 

	struct PersonListener
	{
		virtual void person_change(Person& p, const string& property_name, const ayn new_value) = 0;
	}

하지만 이러한 접근 방법은 Person 타입에 한정되어 있다. 객체의 속성 변경과 그 변경을 모니터링해야 하는 작업은 매우 흔한 일이다. 그때마다 해당 객체 타입에 종속된 아릶 전용 베이스 클래스를 정의하는 것은 너무나 번거롭다. 따라서 좀 더 일반화된 접근 방법이 필요하다. 아래의 Observer 정의를 보자.

	template<typename T> struct Observer
	{
		virtual void field_changed(T& source, const string& filed_name) = 0;
	}

filed_cahnge()의 파라미터들은 그 이름에서 그 의미를 쉽게 알 수 있다. source는 모니터링할 필드를 가진 객체의 참조이고 field_name은 그 필드의 이름이다. field_name이 단순 문자열이라는 부분이 마음에 들지 않을 것이다. 이렇게 비정규적인 방식은 나중에 리팩터링에서 변수명이 바뀔 때 누락되기 쉽다.

이 구현은 Person 클래스의 변경 사항을 모니터링할 수 있게 해준다. 예를 들어 age에 쓰기 작업이 있을 때마다 콘솔에 메시지를 출력하고 싶다면 아래와 같이 할 수 있다.

	struct ConsolePersonObserver : Observer<Person>
	{
		void field_changed(Person& source, const string& field_name) override
		{
			cout << "Person's " << field_name << " has changed to " << source.get_age() << endl;
		}
	};

이 구현은 유연하기 때문에 여러 다른 시나리오들도 지원할 수 있다. 예를 들어 복수의 클래스의 필드 값들을 모니터링 할 수 있다. 아래의 Person과 Creature 두 클래스를 한꺼번에 모니터링하는 예이다.

	struct ConsolePersonObserver : Observer<Person>, Observer<Creature>
	{
		void field_changed(Person& source, ...) { ... }
		void field_changed(Creature& source, ...) { ... }
	};

또 다른 방법은 std::any를 이용하여 제니릭으로 구현하는 것이다. 

## Observable\<T\>

다시 Person으로 돌아와 보자. Person을 모니터링이 가능한 Observable 클래스로 만들어 본다. Observable은 다음과 같은 책임을 갖게 된다.
 - Person의 변경을 모니터링 하려는 관찰자들은 private 리스트로 관리한다.
 - 관찰자가 Person의 변경 이벤트에 수신 등록 또는 해제(subscribe()/unsubscribe())할 수 있게 한다.
 - notify()를 통해 변경 이벤트가 발생했을 때 모든 관찰자에게 정보가 전달되도록 한다.

이 모든 기능은 별도의 베이스 클래스로 쉽게 옮길 수 있다. 그렇게 함으로써 잠재적으로 일어날 수 있는 중복 구현을 예방한다.

	template<typename T> struct Observable
	{
		void notify(T& source, const string& name) { ... }
		void subscrible(Observer<T>* f) { observers.push_back(f); }
		void unsubscribe(Observer<T>* f) { ... }
	private:
		vector<Observer<T>*> observers;
	};

subscribe()는 간단하게 구현되었다. 관찰자 목록에 새로운 관찰자를 추가하는 작업밖에 없다. 관찰자 목록은  private으로 내부에서만 접근할 수 있게 통제된다. 이 목록은 상속받는 클래스에서도 접근할 수 없다. 이렇게 함으로써 관찰자 목록이 임의로 수정되는 것을 막는다.

다음으로 notify()를 구현한다. 기본 아이디어는 단순하다. 모든 관찰자를 순회하며 관찰자의 field_changed()를 차례로 호출한다.

	void notify(T& source, const string& name)
	{
		for(auto abs : observers)
			obs->field_changed(source, name);
	}

Obserable\<T\>를 상속받는 것만으로는 부족하다. 관찰 받는 클래스에서도 자신의 필드가 변경될 때마다 notify()를 호출해 주어야 한다.

예를 들어 set_age() 멤버 함수를 보자. 이 멤버 함수는 다음과 같은 세 가지 책임을 진다.
 - 필드의 값이 실제로 바뀌었는지 검사해야 한다. 만약 age의 값이 20살인 상태에서 동일한 20살로 값이 중복 설정될 때 대입 작업과 알림을 수행하는 것은 의미 없는 동작이다.
 - 필드에 적절한 값이 설정되어야 한다. 예를 들어 age의 값으로서 -1은 의미가 없다.
 - 필드의 값이 바뀌었을 때 올바른 인자로 notify를 호출한다.

이에 따라, set_age()는 다음과 같은 새로운 구현을 가진다.

	struct Person : Observable<Person>
	{
		void set_age(const int age)
		{
			if(this->age == age) return;
			this->age = age;
			notify(*this, "age");
		}
	private:
		int age;
	};


## 관찰자(Observer)와 관찰 대상(Observable)의 연결
이렇게 준비된 코드를 기반으로 하여 Person의 필드 값(속성) 변화에 대한 알림을 받아보자. 알림을 받기 위한 ㅗ간찰자는 다음과 같이 정의된다.

	struct ConsolePersonObserver : Observer<Person>
	{
		void field_changed(Person& source,
			const string& field_name) override
		{
			cout << "Person's " << field_name << " has changed to "
				<< source.get_age() << endl;
		}
	};

그리고 아래와 같이 이용할 수 있다.

	Person p{ 20 };
	ConsolePersonObserver cpo;
	p.subscribe(&cpo);
	p.set_age(21);
	p.set_age(22);

속성의 종속성, 스레드 안정성, 재진입 안정성과 같은 문제들을 고려할 필요가 없다면 여기까지의 이 구현들을 그대로 사용해도 괜찮다. 하지만 그러한 문제들을 무시할 수 있는 단순한 상황은 드물다. 이제 그러한 문제들까지 고려한 더 정교한 접근 방법을 살펴보자.

## 종속성 문제
가장 낮은 투표 가능 연령은 16세로 오스트리아, 브라질 등에서 채택하고 있다. 즉, 이들 국가에서는 16세가 되는 순간 권한이 생긴다. Person의 age필드 변화에 따라 투표 권한이 생겼음을 알리는 기능을 만들어 보자. 먼저 Person에 아래와 같은 속성 읽기 멤버 함수가 있다고 가정하자.

	bool get_can_vote() const { return age >= 16; }

get_can_vote() 멤버 함수는 전용 필드 멤버도 없고 대응되는 set 멤버 함수도 없다. 그럼에도 불구하고 notify()를 호출해 줘야 할 것만 같다. 어떻게 해야 할까? 투표 가능 여부가 바뀌었는지는 set_age()를 이용해 간접적으로 알 수 있다. 따라서 can_vote의 변경 여부에 대한 알림은 set_age() 안에서 수행될 필요가 있다. 이 부분은 아래와 같이 구현할 수 있다.

	void set_age(int value) const
	{
		if(age == value) return;

		auto old_can_vote = can_vote();
		age = value;
		notify(*this, "age");

		if(old_can_vote != can_vote())
			notify(*this, "can_vote");
	}

뭔가 잘못되었다. 이 코드는 원래 목적을 벗어나서 과도한 일을 하고 있다. age의 변화뿐만 아니라 그에 영향받는 can_vote의 변화까지 찾아서 알림을 생성한다. 이런 방식은 확장성이 없다. 만약 can_vote가 age뿐만 아니라 성별이나 지역에도 영향을 받는다면 어떻게 될까? 영향받는 필드의 set 멤버 함수마다 can_vote의 변화에 대한 알림 코드가 들어가야 한다. 필드가 많아지고 종속성이 복잡할 경우 이런식의 구현은 유지보수가 대단히 어려워진다.

위의 시나리오를 일반화하여 말하자면 can_vote가 속성 age에 종속성을 가지는 상황이다. 엑셀을 떠올려보면 이러한 종속성이 야기하는 어려움을 쉽게 이해할 수 있다. 엑셀에서 표로 계산을 할 때 서로 다른 많은 수식/값 셀들이 서로 참조하고 있으면 셀 하나만 변경되어도 많은 셀들이 다시 계산되어야 한다. 이 때 반드시 재계산해야 하는 종속된 셀을 알아내는 것은 쉬운 일이 아니다.

물론 이러한 속성 종속성에 따른 상호 영향 관계를 map\<string, vector\<string\>\>과 같은 용하여 추적/관리할 수도 있다. 즉, 어떤 속성에 영향을 주는 속성들의 목록 또는 반대로 어떤 속성이 영향을 받는 속성들의 목록을 저장하여 관리할 수 있다. 그런데 이러한 목록의 문제는 개발자 손으로 일일이 파악하여 기입하여야 한다는 것이다. 코드가 변경될 때마다 이러한 목록을 업데이트하는 일은 꽤나 소모적인 일이다.

## 수신 해제와 스레드 안전성
한 가지 제대로 다루지 않은 부분이 있다. 관찰자의 관찰 대상에 대한 응록을 어떻게 해제할 수 있을까? 당연히 떠오르는 방법은 관찰자 목록에서 해당 관찰자를 제거하는 것이다. 전체 시나리오에 걸쳐서 스레드가 하나만 사용된다면 아래와 같은 단순한 구현으로도 충분하다.

	void unsubscribe(Observer<T>* observer)
	{
		observers.erase(remove(observers.begin(), observers.end(), observ),
			observers.end());
	};

기술적으로 이러한 "삭제하고 이동하기" 관례가 틀린 것은 아니지만 단일 스레드 상황에서만 정상적인 동작이 보증된다. std::vector는 스레드 안전성이 없다. 따라서 subscribe()와 unsubscribe()가 동시에 호출된다면 의도하지 않은 결과가 발생할 수 있다. 왜냐하면 두 함수 모두 vector를 수정하려 들기 때문이다.

이 문제는 쉽게 해결할 수 있다. 관찰 대상 객체의 모든 작업에 아래와 같이 단순히 락을 걸면 된다.

	template<typename T>
	struct Observable
	{
		void notify(T& source, const string& name)
		{
			scoped_lock<mutex> lock{ mtx };
			...
		}
	
		void subscribe(Observer<T>* f)
		{
			scoped_lock<mutex> lock{ mtx };
			...
		}

		void unsubscribe(Observer<T>* o)
		{
			scoped_lock<mutex> lock{ mtx };
			...
		}
	private:
		vector<Observer<T>*> observers;
		mutex mtx;
	};

다른 괜찮은 방법으로 TPL/PPL과 같은 병렬 작업 라이브러리를 채용하여 std::vector 대신 스레드 안전성을 보증하는 concurrent_vector를 사용하는 것이다. 단, 이경우 항목의 순서가 보증되지는 않는다. 예를 들어 두 개의 관찰자 객체를 순서대로 추가하더라도 그 순서대로 알림이 온다는 보증은 없다. 하지만 락을 직접 관리하느라 소요되는 코딩과 디버깅 시간은 확실히 줄일 수 있다.

## 재진입성(Reentrancy)
앞서의 구현은 언제든 호출될 수 있는 세 가지 주요 멤버 함수에 락을 추가하여 일정 수준의 스레드 안전성을 확보했다. 하지만 여전히 문제가 있다. 다음과 가ㅗ를 생각해보자. 운전면허 관제를 위한 컴포넌트 TrafficAdministration가 있다고 하자. 이 컴포넌트는 운전면허 시험 응시 기준에 합치하는지 모든 사람의 연령을 모니터링 한다. 그리고 어떤 사람의 나이가 기준 연령 17세에 도달했다면 모니터링을 중단하기 위해 알림 수신 등록을 해제한다.

	struct TrafficAdministration : Observer<Person>
	{
		void TrafficAdministration::field_chagned(
			Person& source, const string& field_name) override
		{
			if(field_name == "age")
			{
				if(source.get_age() < 17)
					cout << "Whoa there, you are not old enough to drive!" << endl;
				else
				{
					cout << "We no longer care!" << endl;
					source.unsubscribe(this);
				}
			}
		}
	};

이 시나리오는 문제를 일으킨다. 17세에 도달했을 때 다음과 같은 호출이 연이어 일어난다.

	notify() -> field_changed() -> unsubscribe()

여기서 unsubscribe()의 실행이 이미 앞아서 락이 점유된 이후에 일어나고 있다. 이 때문에 이미 점유된 락을 다시 점유하려 들게 된다. 이러한 문제를 재진입문제라고 한다. 이 문제에 대한 해결책으로 몇 가지 서로 다른 방법이 있을 수 있다.

 - 첫 번째 해결책은 그냥 이러한 상황을 금지하는 것이다. 그러면 최소한 이 상황에서만큼은 문제는 피할 수 있다. 이 상황에서 재진입이 일어난다는 것은 매우 자명하다.
 - 또 다른 방법은 컬렉션에서 항목을 삭제한느 작업 자체를 우회적으로 처리하는 것이다. 즉, 다음과 비슷한 방법으로 항목이 삭제된 것과 같은 효과를 낸다.

//

	void unsubscribe(Observer<T>* o)
	{
		auto it = find(observers.begin(), observers.end(), o);
		if(it != observers.end())
			*it = nullptr;	// cannot do this for a set
	}

이렇게 수정할 경우 당연하게도 notify()에서 추가적인 검사가 필요하다.

	void notify(T& source, const string& name)
	{
		for(auto obs : observers)
			if(obs)
				obs->field_changed(source, name);
	}

위 방법은 notify()와 subscribe()에서 일어나는 락 충돌 문제는 해결하지만, 예를 들어 subscribe()와 unsubscribe()가 병렬로 동시에 호출되어 양쪽에서 수정을 시도할 떄 발생할 수 있는 문제는 해결하지 못한다. 그 부분에 또 락을 추가하고 관리해야 할 것이다.

또 다른 가능한 방법은 notify() 안에서 컬렉션 전체를 복제하여 사용하는 것이다. 여전히 락이 필요하기는 하지만 알림을 보내는 단계에서 락을 점유할 필요는 없어진다. 즉, 아래와 같이 할 수 있다.

	void notify(T& source, const string& name)
	{
		vector<Observer<T>*> observers_copy;
		{
			lock_guard<mutext_t> lock{ mtx };
			observers_copy = observers;
		}
		for(auto obs : obsers_copy)
			if(obs)
				obs->field_changed(source, name)
	}

위 구현에서 여전히 락을 사용하기는 하지만 field_changed()를 호출하는 시점에는 락이 해제된 상태가 된다. 락은 vector를 복제하는 동안만 사용된다(락 해제는 락 변수가 스코프를 벗어날 때 소멸자를 통해 자동으로 수행된다) 메모리를 중복으로 사용하기는 하지만 이 예에서 vector는 포인터들을 저장하고 있기 때문에 점유하는 메모리양이 문제 될 만큼 크지는 않다.

마지막으로 mutex를 recursive_mutext로 바꾸는 방법이 있다. 대부분의 경우, 개발자들은 recursive_mutext를 좋아하지 ㅏㄴ지 성능적인 문제 때문이라기보다는, 많은 상황이 코드 설계에 약간의 주의만 기울이면 재귀적인 특성이 없는 보통의 mutex를 사용하면서도 문제가 없게 할 수 있기 때문이다.

아직 논의되지 않은 현실적인 이슈들이 남아있다.
 - 같은 관찰자가 두 번 추가되면 어떤 일이 일어나는가?
 - 만약 중복된 관찰자가 허용된다며 unsubscribe()가 호출되었을 때 등록된 모든 관찰자를 제거해야 할까?
 - vector가 아닌 다른 컨테이너를 사용하면 동작 방식에 어떤 영향을 미칠까? 예를 들어 중복 등록을 방지하기 위해 std::set이나 boost::unordered_set을 사용한다면 통상적인 작업들에 있어서 어떤 차이가 생길까?
 - 관찰자들 간에 서로 우선순위가 매겨져 있다면 어떻게 해야 할까?

## Boost.Signals2를 이용한 관찰자
많은 라이브러리들이 관찰자 패턴의 구현을 제공하고 있다. 그 중 가장 유명한 것은 Boost.Signals2 라이브러리일 것이다. 이 라이브러리는 기본적으로 C++표준 라이브러리의 std::signal에 해당되는 타입을 제공한다(이벤트라고 불리기도 한다) 이 시그널은 함수나 람다를 전달하여 수신 등록을 할 수 있다. 수신 해제를 할 수도 있고 알림을 받을 수도 있다.

Boost.Signals2를 이용한다면 Observer<T>를 아래와 같이 정의할 수 있다.

	template<typename T>
	struct Obserable
	{
		signal<void(T&, const string&)> property_changed;
	};

그리고 아래와 같이 호출할 수 있다.

	struct Person : Obserable<Person>
	{
		...
		void set_age(const int age)
		{
			if(this->age == age) return;
			this->age = age;
			property_changed(*this, "age");
		}
	}

Boost.Signals2 API의 사용 방법은 직접 signal을 사용하는 것이다. 물론 좀 더 사용하기 쉽도록 signal을 감싸서 편의 API를 만들 수도 있다.

	Person p{123};
	auto conn = p.property_changed.connect([](Person&, const string& prop_name)
	{
		cout << prop_name << " has been changed" << endl;
	});
	p.set_age(20); // name has been changed

	conn.disconnect();

connect()에서 리턴되는 객체는 알림이 더 이상 필요 없을 때 수신 등록을 해제하는데 이용할 수 있다.

## 요약
이 장에서 보여진 예제들은 관찰자 패턴에 대한 설명뿐만 아니라, 실제 필요한 것보다 과도하게 준비하는 오버 엔지니어링이 어떻게 일어날 수 있는지도 보여준다.

관찰자 패턴의 구현에서 이루어지는 주요한 디자인 선택 사항들을 정리하면 다음과 같다.

- 어떤 정보를 모니터링 할 수 있게 하여 커뮤니케이션을 도울 것인지 결정해야 한다. 예를 들어 필드 속성의 변경을 모니터링 할 수 있게 한다면 그 속성의 이름, 그리고 필요하다면 이전/이후 값을 관찰자에게 제공해야 한다. 단, 값이 전용 타입을 가진다면 타입의 전달에서 어려움을 겪을 수 있다.
- 클래스 전체가 관찰자로서 등록되어야 하는가? 아니면 이벤트를 처리할 함수를 등록하는 것만으로 충분한가?
- 관찰자의 알림 수신 해제는 어떻게 처리되어야 하나?
	- 만약 수신해제 기능이 필요 없다면 다행이다. 이 경우 관찰자의 구현에 많은 시간을 절약할 수 있다. 목록에서 항목을 삭제할 때 발생할 수 있는 재진입시나리오 등 까다로운 문제를 처리하지 않아도 된다.
- 명시적으로 unsubscribe() 함수를 제공해야 한다면 그 함수 안에서 "삭제 하고 이동하기" 관례를 사용하지 않는 것이 바람직하다. 대신 삭제할 항목을 표시만 해두고 실제 삭제는 나중에 하는 것이 까다로운 시나리오에 대응하기에 좋다.
- 일반 포인터(null일 수도 있다)를 주고받는 것이 마음에 들지 않는다면 weak_ptr의 사용을 고려해볼 수 있다.
- Observer\<T\>의 멤버 함수들이 서로 다른 여러 스레드에서 호출될 가능성이 있는가? 만약 그렇다면 수신 등록 목록을 보호해야만 한다.
	- 모든 관련 함수마다 scoped_lock을 넣거나
	- 스레드 안전성이 보증되는 컬렉션을 사용한다. 예를 들어 TBB/PPL의 concurrent_vector를 사용한다. 대신 항목 간의 순서는 보증되지 않는다.
- 동일한 관찰자가 복수의 수신 등록을 할 수 있어야 하는가? 만약 그렇다면 std::set은 사용할 수 없다.

안타깝게도, 관찰자 패턴에 있어서 모든 경우를 만족시켜서 바로 가져다 쓸 수 있는 이상적인 라이브러리는 존재하지 않는다. 어떤 구현을 사용하든 일정 부분 타협이 뒤따른다.


		
			
