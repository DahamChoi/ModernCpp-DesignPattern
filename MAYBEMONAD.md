# 라면 모나드(Maybe Monad)
C++ 에서는 다른 많은 언어들과 마찬가지로 어떤 값이 존재하지 않는다는 것을 여러가ㅣㅈ ㅏㅂㅇ법으로 표현할 수 있다. 보통 다음의 방법들이 사용된다.
 - nullptr에 의미를 부여하여 존재하지 않음을 표시
 - shared_ptr와 같은 스카트 포인터를 이용하여 존재 여부를 검사할 수 있게 함
 - STL 라이브러리의 std::optional\<T\>를 이용하여 값이 존재할 때는 타입 T의 값을 저장하고, 존재하지 않을 때는 std::nullpot를 저장

nullptr로 값의 부재를 표기하기로 했다고 하자. 개인 정보를 관리하는 애플리케이션이라면 주소가 기입된 경우도 있고 없는 경우도 있을 것이다. 이러한 모델은 다음과 같이 Person과 Address로 정의할 수 있다. 여기서 house_name이 옵셔널하게 된다.

	struct Address {
		string* house_name = nullptr;
	};

	struct Person {
		Address* address = nullptr;
	};

우리가 하려는 것은 Person이 주어졌을 때 주소 정보를 안전하게 출력하는 것이다. 당연하게도 house_name의 값이 존재할 때만 출력해야 한다. 전통적인 C++에서는 아래와 같이 구현할 것이다.

	void print_house_name(Person* p)
	{
		if(p != nullptr && p->address != nullptr &&
			p->address->house_name != nullptr)
			cout << *p->address->house_name << endl;
	}

위 코드는 nullptr에 접근하지 않기 위해 객체의 내부 구조를 파고들어 가는 과정을 보여준다. 이러한 침습적인 과정을 "~라면 모나드"(Maybe Moand에서 Monad는 함수형 언어에서 유래된 용어로 불필요하게 난해한 것으로 악명이 높다. 여기서는 단순히 어떤 type을 의미하는 것으로 해석하면 충분하다)를 이용해 함수형으로 표현할 수 있다.

"~라면 모나드"를 만들기 위해 새로운 타입 Maybe\<T\>를 정의한다. 이 타입은 드릴-다운 작업 와중에 사용되는 임시 객체의 타입으로 이용된다.

	template <typename T> struct Maybe {
		T* context;
		Maybe(T *context) : context(context) {}
	};

Maybe 타입은 그냥 포인터를 감싸는 것 정도로만 보인다. 그리고 쓸모ㅗ인다. 왜냐하면 Person* p가 주어져 있을 때 Maybe(p)를 만들 수가 없다. 생성자로 넘겨지는 인자로부터 템플릿 파라미터의 클래스를 추론할 수가 없기 때문이다. 이 예의 경우, 아래와 같이 전역 편의 함수를 추가로 만들어 템플릿 인자를 추론할 수 있게 해야 한다.

	template<typename T> Maybe<T> maybe(T* context)
	{
		return Maybe<T>(context);
	}

이제 Maybe에 다음과 같은 동작을 수행하는 멤버 함수를 추가하고 싶다.
 - context != nullptr;이면 객체에 대한 드릴-다운을 진행하고
 - 아니라면, 즉 nullptr라면 아무것도 하지 않는다.

드릴-다운 작업은 아래와 같이 템플릿 파라미터 Func으로 추상화된다.

	template<typename Func>
	auto With(Func evaluator)
	{
		return context != nullptr ? maybe(evalutor(context)) : nullptr;
	}

위 코드는 고차원 함수의 예를 보여준다. 고차원 함수란 함수를 넘겨받는 함수를 말한다. 이 함수는 evaluator라는 함수를 넘겨받는다. evaluator는 현재의 context가 null이 아니라면 context에 대해 호출될 수 있는 함수이다. 호출된 후에는 또 다른 Maybe로 감싸질 수 있는 포인터를 리턴한다. 이러한 트릭은 With()의 호출을 체인으로 엮을 수 있게 한다.

비슷한 방식으로 또 다른 멤버 함수를 만든다. 이번에는 단순히 주어진 함수를 context에 대해 호출한다. 함수에  context를 넘겨주기 전에 관례적으로 아무런 변경을 가하지 않는다.

	template<typename TFunc>
	auto Do(TFunc action)
	{
		if (context != nullptr) action(context);
		return *this;
	}

이제 모든 준비가 끝났다. 앞서의 print_house_name() 함수를 "~라면 모나드"를 이용하여 구현하면 아래와 같이 된다.

	void print_house_name(Person* p)
	{
		auto z = maybe(p)
			.With([](auto x){ return x->address; })
			.With([](auto x){ return x->house_name; })
			.Do([](auto x){ cout < *x << endl; });
	}

여기에서 몇 가지 짚어볼 것들이 있다. 첫 번째로, "~라면 모나드"를 이용하여 흐름식 인터페이스를 만들어 늑, 함수 호출이 필요한 지점들이 사슬을 이루며 구성되고 있다. 이러한 코드가 어떻게 가능한지는 쉽게 이해할 수 있다. 각 연산(Width, Do 등)이 *this나 new Maybe\<T\> 타입을 리턴하고 있기 때문에 가능하다. 두 번째로, 드릴-다운의 각 과정이 람다 함수를 이용해 추상화되고 있는 방식도 눈여겨볼 가치가 있다.

아마도 짐작하고 있듯이 이러한 접근 방식은 성능상 오버헤드가 있다. 하지만 이 오버헤드는 컴파일러가 코드 최적화를 어떻게 하느냐에 달려 있기 때문에 어느정도 일지 예측하기 어렵다. 그리고 이 코드는 완벽한 것과는 거리가 멀다. 왜냐하면 \[\]\(auto x\) 부분이 거치장 스럽다. maybe(p).With{it->address}와 같이 축약하여 표현할 수 있다면 이상적일 것이다.(Kotlin,Swift는 이 기능을 지원한다)