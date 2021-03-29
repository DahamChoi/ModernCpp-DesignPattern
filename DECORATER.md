# 데코레이터 패턴
***데코레이터 패턴**이란 주어진 상황 및 용도에 따라 어떤 객체에 책임을 덧붙이는 패턴으로, 기능 확장이 필요할 때 서브클래싱 대신 쓸 수 있는 유연한 대안이 될 수 있다.*

## 시나리오
여러 가지 개선 작업이 필요한 상황을 생각해보자. 도형을 나타내는 클래스 Shape가 기존에 존재하고 있었고 이를 상속받아 색상이 있는 도형(ColoredShape)와 투명한 도형(TransparentShape)를 추가했다고 하자. 그리고 나중에 두 가지 특성을 모두 필요로 하는 ColoredTransparentShape를 만들었다고 하자. 결과적으로 두 가지 기능을 추가하기 위해 클래스를 3개 만들었다. 이런 식이면 기능이 하나 더 추가될 경우 7개의 클래스를 만들어야 할 수도 있다. 세 가지 추가 기능을 두 가지의 도형에 적용하려면 상속받아 만들어야 할 클래스가 14개까지 늘어난다.

평범한 상속으로는 효율적으로 새로운 기능을 도형에 추가할 수가 없다는 것을 알 수 있다. 따라서 접근 방법을 달리해 컴포지션을 활용한다. 컴포지션은 데커레이터 패턴에서 객체들이 새로운 기능을 확장할 때 활용되는 메커니즘이다. 이 접근 방식은 다시 두 가지 서로 다른 방식으로 나누어진다.

- 동적 컴포지션 : 참조를 주고받으면서 런타임에 동적으로 무언가를 합성할 수 있게 한다. 이 방식은 최대한의 유연성을 제공한다. 예를 들어 사용자 입력에 따라 런타임에 반응하여 컴포지션을 만들 수 있다.
- 정적 컴포지션 : 템플릿을 이용하여 컴파일 시점에 추가기능이 합성되게 한다. 이것은 코드 작성 시점에 대한 정확한 추가 기능 조합이 결정되어야만 한다는 것을 암시한다. 즉, 나중에 수정될 수 없다.


## 동적 데커레이터
도형에 색을 입히려 한다고 가정해보자. 상속 대신 컴포지션으로 ColoredShape를 만들 수 있다. 이미 생성된 Shape 객체의 참조를 가지고 새로운 기능을 추가한다.

	struct ColoredShape : Shape{
		Shape& shape;
		string color;
		// ... 이하 구현 생략
	};

코드에서 볼 수 있듯이 ColoredShape은 그 자체로서도 Shape이다. ColoredShpae은 다음과 같이 이용될 수 있다.

	Circle circle{0.5f};
	ColoredShape redCircle{circle, "red"};
	cout << redCircle.str();

만약 여기에 도형이 투명도를 가지게 하고 싶다면 마찬가지 방법으로 다음과 같이 쉽게 구현할 수 있다.

	struct TransparentShape : Shape{
		Shape& shape;
		uint8_t transparency;
		// ... 이하 구현 생략
	};

이제 0..255 범위의 투명도를 지정하면 그것을 퍼센티지로 출력해주는 새로운 기능이 추가되었다. 이러한 기능은 그 자체만으로는 사용할 수 없고 적용할 도형 인스턴스가 있어야만 한다.

하지만 편리하게도 ColoredShape와 TransparentShape를 합성하여 색상과 투명도 두 기능 모두 도형에 적용되도록 할 수 있다.

	TransparentShape myCircle{
		ColoredShape{
			Circle{23}, "green"
		}, 64
	};

위 코드에서 볼 수 있듯이 모두 즉석으로 생성하고 있다. 하지만 문제도 있다. 비상식적인 합성도 가능하다. 같은 데커레이터를 중복해서 적용해버릴 수도 있다. 예를 들어 ColoredShape{ColordShape{...}}과 같은 합성은 비상식적이지만 동작한다. 이렇게 중복된 합성은 "빨간색이면서 노락색이다"와 같은 모순된 상황을 야기한다.]

## 정적 데커레이터
예제 시나리오에서 Circle에는 resize() 멤버 함수가 있다. 이 함수는 Shape 인터페이스와 관계가 없다. 당연하게도 인위적으로 상황을 만들기 위해 그렇게 둔 것이다. 이미 짐작했듯이 resize()는 Shape 인터페이스에 없기 때문에 데커레이터에서 호출할 수가 없다.

다소 난해한 테크닉을 사용해야 하기는 하지만 가능하다. 템플릿과 상속을 활용하되 여기에서의 상속은 가능한 조합의 수가 폭발적으로 증가하지 않는다. 보통의 상속 대신 믹스인 상속이라 불리는 방식을 이용한다. 믹스인 상속은 템플릿 인자로 받은 클래스를 부모 클래스로 지정하는 방식을 말한다.

	template <typename T> struct ColoredShape : T {
		static_assert(is_base_of<Shape, T>::value, "Template argument must be a Shape");
		string color;
		
		template<typename..Args>
		ColoredShape(const string& color, Args ...args)
			: T(std::forward<Args>(args)...), color{ color } {}
		// ...이하 구현 생략
	};

기본 아이디어는 다음과 같다. 새로운 클래스 ColoredShape을 만들고 템플릿 인자로 받은 클래스를 상속받게 한다. 이때 템플릿 파리미터를 제약할 방법은 없다. 어떤 타입이든 올 수 있다. 따라서 static_assert를 이용해 Shape이외의 타입을 지정되는 것을 막는다.

ColoredShape<T>와 TransparentShape<T>의 구현을 기반으로 하여 색상이 있는 투명한 도형을 아래와 같이 합성할 수 있다.

	ColoredShape<TransparentSHape<Square>> square{"blue", 50, 3};

위에 구현된 생성자는 임의 개수의 인자를 받을 수 있다. 앞쪽 인자는 색상 값을 초기화하는데 이용되고 나머지 인자들은 그 인자가 어떻게 구성되었느냐와 관계없이 단순한 상위 클래스에 전달된다.

생성자들에 전달되는 인자의 타입과 개수, 순서가 맞지 않으면 컴파일 에러가 발생하기 때문에 올바르게 맞춰질 수밖에 없다. 클래스 하나에 디폴트 생성자를 추가하면 파라미터의 설정에 훨씬 더 융통성이 생긴다. 하지만 인자 배분에 혼란과 모호성이 발생할 수 있다는 점을 염두에 두어야 한다.

이러한 생성자들은 explicit 지정자를 부여하지 않도록 주의해야 한다. 만약 그렇게 할 경우, 복수의 데커레이터를 합성할 때 C++의 복제 리스트 초기화 규칙 위반 에러를 만나게 된다.

[ModernCpp-DesignPattern/structural/decorater at main · DahamChoi/ModernCpp-DesignPattern (github.com)](https://github.com/DahamChoi/ModernCpp-DesignPattern/tree/main/structural/decorater)

## 함수형 데커레이터
데커레이터 패턴은 클래스를 적용 대상으로 하는 것이 보통이지만 함수에도 동등하게 적용될 수 있다. 예를 들어 코드에 문제를 일으키는 특정동작이 있다고 하자. 그 동작이 수행될 때마다 모든 상황을 로그로 남겨 엑셀에 옮겨다가 상태를 분석하고 싶다고 하자. 로그를 남기기 위해 보통 해당 동작의 앞뒤로 로그를 남긴다.

이러한 방식은 익숙하고 잘 동작한다. 하지만 이해 관계를 분리한다는 디자인 철학 관점에서는 바람직하지 않다. 로깅 기능을 분리하여 다른 부분과 얽히지 않고 재사용과 개선을 마음 편하게 할 수 있다면 훨씬 더 효과적이다.

이렇게 하는 데에는 여러 접근 방법이 있을 수 있다. 한 가지 방법은 문제의 동작과 관련된 전체 코드 단위를 통째로 어떤 로깅 컴포넌트에 람다로서 넘기는 것이다. 아래는 이러한 방법을 위한 로깅 컴포넌트의 정의이다.

	struct Logger{
		function<void()> func;
		string name;
		Logger(const function<void()>& func, const string& name) : func{func}, name{name} {}
		void operator()() const{
			cout << "Entering " << name << endl;
			func();
			cout << "Exiting " << name << endl;
		}
	};
	
	Logger([]() { cout << "Hello" << endl; }, "HelloFunction")();

코드 블록을 std::function으로서 전달하지 않고 템플릿 인자로 전달할 수도 있다. 이 경우 로깅 클래스가 다음과 같이 조금 달라진다.

	template <typename Func>
	struct Logger2{
		Func func;
		string name;
		Logger2(const Func& func, const string& name) : func{func}, name{name} {}
		void operator()() const{
			// ..이하동일

	template <typename Func> auto make_logger2(Func func, const string& name){
		return Logger2<Func>{ func, name };
	}

Logger2 클래스는 Logger와 사용 방법이 완전히 같다. 그래서 정확히 좋아졌는가? 데커레이터를 생성할 수 있는 기반이 마련되었다는 데 의미가 있다. 즉, 임의의 코드 블록을 데커레이션 할 수 있고 데커레이션 된 코드 블록을 필요할 때 호출할 수도 있다.

이제 좀 더 어려운 경우를 보자. 로그를 남기고 싶은 함수의 리턴 값을 넘겨야 한다면 어떻게 해야 할까? Logger를 다음과 같이 바꿔보자.

    template <typename R, typename... Args>
    class Logger3
    {
    private:
    	std::function<R(Args...)> func;
    	std::string name;
    
    public:
    	Logger3(std::function<R(Args...)> func, const std::string& name)
    		: func{ func }, name{ name } {}
    
    	R operator() (Args ...args)
    	{
    		std::cout << "Entering " << name << std::endl;
    		R result = func(args...);
    		std::cout << "Exiting " << name << std::endl;
    		return result;
    	}
    };
    
    template<typename R, typename... Args>
    auto make_logger3(R(*func)(Args...), const std::string& name)
    {
    	return Logger3<R, Args...>(func, name);
    }

위 코드에서 템플릿 인자 R은 리턴 값의 타입을 의미한다. 그리고 Args는 이미 여러 번 보았던 파라미터 팩이다. 이 데커레이터는 앞서와 마찬가지로 함수를 가지고 있다가 필요할 때 호출할 수 있게 해준다. 유일하게 다른 점은 operator()가 R타입의 리턴값을 가진다는 점이다. 즉, 리턴값을 잃지 않고 받아낼 수 있다.


## 요약
데커레이터 패턴은 열림-닫힘 원칙을 준수하면서도 클래스에 새로운 기능을 추가할 수 있게 해준다. 데커레이터의 핵심적인 특징은 데커레이터들을 합성할 수 있다는 것이다. 객체 하나에 복수의 데커레이터들을 순서와 무관하게 적용할 수 있다. 데커레이터는 다음과 같은 패턴의 종류가 있다.
 - 동적 데커레이터 : 데커레이션할 객체의 참조를 저장하고(객체의 전체값을 저장할 수도 있다) 런타임에 동적으로 합성할 수 있다. 대신 원본 객체가 가진 멤버들에 접근할 수 없다.
 - 정적 데커레이터 : 믹스인 상속을 이용해 컴파일 시점에 데커레이터를 합성한다. 이 방법은 런타임 융통성을 가질 수 없다. 즉, 런타임에 객체를 다시 합성할 수가 없다. 하지만 원본 객체의 멤버들에 접근할 수 있는 장점이 있다. 그리고 생성자 포워딩을 통해 객체를 완전하게 초기화할 수 있다.
 - 함수형 데커레이터 : 코드 블록이나 특정 함수에 다른 동작을 덧씌워 합성할 수 있다.



		
