# 팩터리
*객체를 생성하기 위해 인터페이스를 정의하지만, 어떤 클래스의 인스턴스를 생성할지에 대한 결정은 서브클래스가 내리도록 합니다.*

## 시나리오

    struct Point{
    	Point(const float x, const float y)
    		: x{x}, y{y} {}
    	float x, y; };

직교좌표계의 좌표점 정보를 저장하려 한다고 하자.  다음과 같은 구현이 쉽게 떠오를 것이다. 여기까지는 전혀 문제 될 것이 없다. 하지만 극좌표계용 생성자를 추가하면 되지 않을까 생각할 수 있다. 하지만 안타깝게도 문제가 있다. 직교좌표계 생성자를 추가하면 되지 않을까 생각할 수 있다. 하지만 안타깝게도 문제가 있다. 직교 좌표계 생성자도 두 개의 flaot 값을 파라미터로 하기 때문에 극좌표계의 생성자와 구분할 수 없다. 한가지 단순한 구분 방법은 enum 타입 값을 파라미터에 추가하는 것이다.  이러한 생성자는 기능적으로 문제가 없지만 훌륭한 것과는 거리가 멀다. 이제 어떻게 개선할 수 있을지 알아보자.


## 팩터리 메서드

    struct Point {
    protected:
    	Point(const float x, const float y) : x{x}, y{y} {}
    public:
    	static Point NewCertesian(float x,float y){ return {x,y} }
    	static Point NewPolar(float r, float theta){ return {r * cost(theta), r * sin(theta) } };

이 예에서 생성자의 문제는 항상 타입과 같은 이름을 가진다는 것이다. 즉, 일반적인 함수 이름과 달리 생성자의 이름에는 추가적인 정보를 표시할 수가 없다. 그리고 생성자 오버로딩으로는 같은 float 타입인 x,y와 r,theta를 구분할 수 없다.
그러면 어떻게 해야 할까? 생성자를 protected로 하여 숨기고, 대신 Point객체를 만들어 리턴하는 static 함수를 제공하는 것은 어떨까?
여기서 각각의 static 함수들을 팩터리 메서드라고 부른다. 이러한 메서드가 하는 일은 Point객체를 생성하여 리턴하는 것뿐이다. 이제 좌표점을 생성할때 명료하게 할 수 있다.


## 팩터리

빌더와 마찬가지로 Point를 생성하는 함수들을 별도의 클래스에 몰아넣을 수 있다. 그러한 클래스를 팩터리라고 부른다.

    struct Point{
    	float x,y;
    	friend class PointFactory;
    private:
    	Point(float x, float y) : x(x), y(y) {} };
    
    struct PointFactory{
    	static Point NewCartesian(float x,float y){ return Point{ x, y };
    	static Point NewPolar(float r, float theta){ return Point{ r*cos(theta), r*sin(theta) };
    
    auto my_point = PointFactory::NewCertesian(3,4);

Point의 생성자는 private로 선언되어 사용자가 직접 생성자를 호출하는 경우가 없게 한다.
Point는 PointFactory를 friend 클래스로 선언한다. 이 부분은 팩터리가 Point의 생성자에 접근할 수 있게 하려는 의도이다.


## 내부 팩터리

내부 팩터리는 생성할 타입의 내부 클래스로서 존재하는 간단한 팩터리를 말한다.
내부 팩터리의 장점은 생성할 타입의 내부 클래스이기 때문에 private 멤버들에 자동적으로 자유로운 접근 권한을 가진다는 점이다.

    struct Point{
    private:
    	Point(float x,float y) : x(x), y(y) {}
    
	    struct PointFactory{
		private:
			PointFactory() {}
		public:
			static Point NewCertesian(float x,float y){ return { x,y }; }
			static Point NewPolar(float r,float theta){ return { r*cos(theta), r*sin(theta) };
		};
	public:
		float x, y;
		static PointFactory Factory;
	};

팩터리가 생성할 바로 그 클래스 안에 팩터리 클래스가 들어가 있다. 이러한 방법은 팩터리가 생성해야 할 클래스가 단 한종류일 때 유용하다. 하지만 팩터리가 여러 타입을 활용하여 객체를 생성해야 한다면 내부 팩터리 방식은 적합하지 않다.(객체 생성에 필요한 다른 타입들의 private 멤버에 접근하기는 사실상 불가능하다.)
    	

## 추상 팩터리

그동안 객체 한개를 생성하는 경우를 살펴보았다. 드물지만 여러 종류의 연관된 객체들을 생성해야 할 경우도 있다. 추상 팩터리는 그러한 경우를 위한 패턴이다. 

[modern_cpp_designpattern/creational/factory at main · DahamChoi/modern_cpp_designpattern (github.com)](https://github.com/DahamChoi/modern_cpp_designpattern/tree/main/creational/factory)


## 함수형 팩터리

이제 팩터리와 관련하여 마지막으로 알아볼 주제이다. 보통 팩터리라고 말할 때 다음의 두 가지 중 하나를 의미한다.

1. 객체를 어떻게 생성하는지 알고 있는 어떤 클래스
2. 호출했을 때 객체를 생성하는 함수

두 번째의 경우를 팩터리 메서드의 하나처럼 볼 수 있을 것 같지만 사실 다르다. 어떤 타입 T를 리턴하는 std::function을 어떤 함수의 인자로 넘겨서 객체를 생성하는 것이 두번째 경우에 해당하고 그냥 팩터리라고 부른다. 왜 팩터리 메서드라고 하지 않을까? 메서드는 어떤 클래스의 멤버 함수를 의미한다는 것을 떠올려보면 납득이 갈 것이다.
함수도 변수에 저장될 수 있다. 즉, 팩터리에 포인터를 저장하는 방법(추상 팩토리에서 구현했던 것처럼) 대신에 200ml의 음료를 생성하는 절차 자체를 팩터리가 내장하게 할 수 있다.


    class DrinkWithVolumeFactory
    {
    private:
    	map<string, function<unique_ptr<HotDrink>()>> factories;
    public:
    	DrinkWithVolumeFactory(){
    		factories["tea"] = [] {
    			auto tea = make_unique<Tea>();
    			tea->prepare(200);
    			return tea;
    		};
    	}
    	inline unique_ptr<HotDrink> make_drink(const string& name){ return factories[name](); }
    };




## 요약

- 팩터리 메서드는 생성할 타입의 멤버 함수로 객체를 생성하여 리턴한다. 이 메서드는 생성자를 대신한다.
- 팩터리는 별도의 클래스로서 목적하는 객체의 생성방법을 알고 있다. 클래스 대신 함수의 형태로 존재하며(예를 들어 std::function 등) 인자로서 사용될 수 있는 경우도 팩터리라고 부른다.
추상 팩터리는 그 이름이 암시하는 바와 같이 구현 클래스에서 상속받는 추상 클래스이다. 추상 팩터리는 어떤 타입 하나가 아니라 여러 타입의 패밀리를 생성 할 때 사용된다. 실제 프로그래밍을 할 때 추상 팩터리를 사용해야 할 경우는 극히 드물다.

팩터리를 생성자 호출에 대비해 몇 가지 중요한 장점이 있다.
- 팩터리는 객체의 생성을 거부할 수 있다. 생성자는 익셉션을 발생시키는 방법밖에 없지만 팩터리는 nullptr을 리턴하는 등의 방법으로 문제가 있는 경우 객체 생성에 자연스럽게 실패할 수 있다.
- 팩터리는 가독성 높은 명명이 가능하다. 생성자는 타입 이름과 같은 이름을 가질 수 밖에 없지만 팩터리는 용도를 잘 설명하는 이름을 부여할 수 있다.
- 단일 팩터리가 서로 다른 여러 타입의 객체를 생성할 수 있다.
- 팩터리에 다형성을 부여할 수 있다. 서브 클래스에서 인스턴스를 만들고 베이스 클래스에서 인스턴스의 참조나 포인터를 리턴할 수 있다.
- 팩터리는 캐싱과 같은 메모리 최적화 구현이 가능하다. 풀링(pooling)이나 싱글턴 패턴을 적용하기에 매우 자연스러운 코드 구조를 제공한다.

팩터리는 빌더와는 다르다. 일반적으로 팩터리는 객체를 한 번에 생성하지만, 빌더는 각 구성요소마다 필요한 정보를 제공하며 여러 단계를 거쳐 객체를 생성한다.
