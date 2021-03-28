# 싱글턴
 *"소프트웨어 디자인 패턴"에서 **싱글턴 패턴**(Singleton pattern)을 따르는 클래스는, 생성자가 여러 차례 호출되더라도 실제로 생성되는 객체는 하나이고 최초 생성 이후에 호출된 생성자는 최초의 생성자가 생성한 객체를 리턴한다. 이와 같은 디자인 유형을 **싱글턴 패턴**이라고 한다. 주로 공통된 객체를 여러개 생성해서 사용하는 DBCP(DataBase Connection Pool)와 같은 상황에서 많이 사용된다.*

## 전역 객체로서의 싱글턴
싱글턴 디자인 패턴은 어떤 특정 컴포넌트의 인스턴스가 애플리케이션 전체에서 단 하나만 존재해야 하는 상황을 처리하기 위해 고안되었다. 

이를 구현하기 위한 뻔한 아이디어는 static 전역 객체를 두는 것이다. static 전역 객체의 문제점은 각각의 컴파일 단위 바이너리들에서 초기화 순서가 정의되어 있지 않다는 것이다. 이 부분은 까다로운 문제를 일으킨다. static 전역 객체가 여러 개 사용된다면 어느 한 모듈에서 전역 객체를 참조할 때 그 전역 객체가 참조하는 또 다른 전역 객체가 아직 초기화된 상태가 아닐 수 있다. 그리고 사용자가 전역 객체가 있따는 사실을 어떻게 알 수 있느냐 하는 문제도 있다.

사용자가 조금 더 알기 쉽도록, 피룡한 객체를 리턴하는 전역 함수를 제공하는 방법이 있따.

    Database& get_database()
    {
    	static Database database;
    	return database;
    }

이 함수를 호출하면 데이터베이스에 접근할 수 있는 참조를 얻을 수 있다. 하지만, 위 함수의 스레드 안정성이 C++ 이상 버전에서만 보증된다는 점에 유의해야 한다. static 객체를 초기화하는 코드 앞뒤로 컴파일러가 락을 삽입하여 초기화 와중에 동시에 다른 스레드에서 접근하는 것을 방지해 주는지 확인해야 한다.

몰론, 이 방식이 문제를 일으킬 가능성이 다분하다. 만약 데이터베이스의 소멸자에서 다른 싱글턴 모듈을 참조한다면 프로그램이 크래시될 가능성이 높다. 이 부분은 소프트웨어 설계 철학 차원의 질문을 야기한다. 싱글턴이 다른 싱글턴을 참조해도 되는가?


## 전통적인 구현
위 구현은 한가지 매우 중요한 것을 빠드리고 있다. 객체가 추가로 생성되는 것을 막는 장치가 없다. static 전역 변수로 관리한다고 해서 인스턴스의 추가 생성을 막을 수는 없다.

이에 대한 쉬운 대책은 생성자 안에 static 카운터 변수를 두고 값이 증가될 경우 익셉션을 발생시키는 것이다. 이 방법은 문제를 더욱 어렵게 만든다. 인스턴스가 한 개보다 더 많이 만들어지려 할 때 익셉션을 발생시켜 여러 개의 인스턴스가 생기는 것을 막을 수 있지만 사용자 관점에서는 인터페이스상으로는 생성자가 단 한번만 호출되어야 한다는 것을 알 수가 없다. 사용자가 명시적으로 생성하는 것을 막는 방법은 앞서 그랬듯이 생성자를 private로 선언하고 인스턴스를 리턴받기 위한 멤버 함수를 만드는 것이다. 이 멤버 함수는 유일하게 하나만 존재하는 인스턴스를 리턴한다.

    struct Database{
    protected:
    	Database() { ... }
    public:
    	static Database& get(){
    		static Database* database = new Database();
    		return *database;
    	}
    	Database(Database const&) = delete;
    	Database(Database&&) = delete;
    	Database& operator=(Database const&) = delete;
    	Database& operator=(Database &&) = delete;
    };

위 코드에서 생성자를 숨기고 복제/이동 생성자/연산자를 삭제하는 방법을 눈여겨보자. Database가 다른 static 또는 전역 객체에 종속적이고, 소멸자에서 그러한 객체를 참조하고 있다면 위험하다. static 객체, 전역 객체의 소멸 순서는 결정적이지 않기 때문에 접근하는 시점에 이미 소멸되어 있을 수도 있다.

또 위 코드에서는 get() 함수에서 힙 메모리 할당으로 객체를 생성하게 한다. 이렇게 함으로써 전체 객체가 아니라 포인터만 static으로 존재할 수 있다. 위 구현은 Database가 프로그램이 종료될 때까지 살아있어 소멸자의 호출이 필요 없는 것으로 가정하고 있다. 따라서 참조 대신 포인터를 사용해 public 생성자가 호출될 일이 없게 한다. 그리고 위 코드는 메모리 누수를 일으키지 않는다.

### 멀티스레드 안정성
싱글턴 초기화 방법들은 C++11 버전 이상부터 스레드 세이프가 가능하다. 즉, 두개의 스레드 동시에 get()을 호출하더라도 Database가 두 번 생성되는 일은 없다. 하지만 C++ 이전 버전에서는 이중 검증 락킹(double-checked locking) 방법으로 생성자를 보호해야 한다. 다음은 이에 대한 전형적인 구현이다.


## 싱글턴의 문제

    class Database
    {
    public:
    	virtual int get_population(const std::string& name) = 0;
    };

이 Database를 상속받는 싱글턴 구현 클래스 SingletonDatabase가 있다고 하자. 앞서 지적했듯이, 싱글턴 패턴은 다른 싱글턴 컴포넌트에서 또 다른 싱글턴을 사용할 때 나타난다. 예를 들어 위 코드를 기반으로 하여, 서로 다른 여러 도시의 인구수의 합을 계산하는 싱글턴 컴포넌트 SingletonRecordFinder를 만들었다고 가정해보자.

여기에서 SingletonRecordFinder의 문제는 SingletonDatabase에 밀좁하게 의존한다는 데서 발생한다. SingletonRecordFinder에 대한 단위 테스트를 한다고 생각해보자. 매번 바뀌는 데이터를 그때마다 테스트 코드를 수정해 삽입해야 한다. 실제 데이터 대신 더미 데이터를 사용하고 싶다면 어떻게 해야 할까? 이러한 설계에서는 더미 컴포넌트를 사용하는 것이 불가능하다. 이러한 유연성 부족은 싱글턴의 전영적인 단점이다.

[modern_cpp_designpattern/creational/singleton at main · DahamChoi/modern_cpp_designpattern (github.com)](https://github.com/DahamChoi/modern_cpp_designpattern/tree/main/creational/singleton)


## 싱글턴과 제어 역전(Inversion of Control)
어떤 컴포넌트를 명시적으로 싱글턴으로 만드는 것은 과도하게 깊은 종속성을 유발한다. 이 때문에 싱글턴 클래스를 다시 일반 클래스로 만들 때 그 사용처 깊숙이 손대야 하며 많은 수정 비용이 든다. 또 다른 방법은 클래스의 생성 소멸 시점을 직접적으로 강제하는 대신 IoC 컨테이너에 간접적으로 위임하는 것이다.

    auto injector = di::make_injector(
    	di::bind<IFoo>.to<Foo>.in(di::singleton),
    	// 기타 설정 작업들...
   	);

종속성 주입 프레임워크 Boost.DI를 이용하면 위와 같이 싱글턴 컴포넌트를 IoC 관례에 맞추어 정의할 수 있다.

위 코드에서는 타입 이름에 앞첨자 I를 붙여 인터페이스 목적의 타입임을 나타내고 있다. 여기서 di::bind 코드가 의미하는 바는, IFoo 타입 변수를 멤버로 가지는 컴포넌트가 생성될 때마다 IFoo 타입 멤버 변수를 Foo의 싱글턴 인스턴스로 초기화한다는 것을 뜻한다.

여러 개발자들의 경험칙에 따르면, 이렇게 종속성 주입 컨테이너를 활용하는 방식만이 바람직한 싱글턴 패턴의 구현 방법이다. 이렇게 하면 싱글턴 객체를 뭔가 다른 것으로 바꾸어야 할 때 코드 한군데만 수정하면 된다. 즉, 컨테이너 설정 코드만 수정하면 원하는 데로 객체를 바꿀 수 있다. 추가적인 장점으로 싱글턴을 직접 구현할 필요 없이 Boost.DI 프레임워크에서 자동으로 처리해 주기 때문에 싱글턴 로직을 잘못 구현할 오류의 여지를 없애준다. 게다가 Boost.DI는 스레드 세이프를 지원한다.

## 모노스테이트(Monostate)

    class Printer
    {
    	static int id;
    public:
    	int get_id() const { return id; }
    	void set_id(int value) { id = value; }
    };

모노스테이트는 싱글턴 패턴의 변형이다. 모노스테이트는 겉보기에는 일반 클래스와 동일하나 동작은 싱글턴처럼 한다. 언뜻 보기에는 보통 클래스처럼 보이지만 static 데이터를 이용하여 get/set 멤버 함수가 구현되어 있다.

무언가 괜찮은 트릭으로 보이지만 문제가 있다. 사용자는 일반 클래스인 것으로 알고 Printer의 인스턴스를 만들지만 실제로는 모든 인스턴스가 같은 데이터를 바라본다. 이 사실을 사용자가 어떻게 알 수 있겠는가?

모노스테이트 방식은 어느 수준에서는 잘 동작하고 몇 가지 장점도 있다. 예를 들어 상속받기가 쉬워 다형성을 활용할 수 있다. 그리고 생존 주기도 적절히 잘 정의된다. 모노스테이트의 가장 큰 장점은 시스템에서 사용 중인 이미 존재하는 객체를 이용할 수 있다는 점이다. 복수의 객체가 존재하는 것이 문제 되지 않는다면, 대규모 구조 변경 없이 기존 코드를 조금만 수정하여 기존에 잘 동작하던 기능에 변화를 일으키지 않고서도 싱글턴 특성을 추가할 수 있다.

모노스테이트 방식의 단점음 명확하다. 이 방식은 코드 깊숙이 손을 댄다(일반 객체를 모노스테이트로 변환하는 것은 간단하지 않다) 그리고 static멤버를 사용하기 때문에 실제 객체가 인스턴스화되어 사용되는지와 관계없이 항상 메모리를 차지한다. 마지막으로 모노스테이트의 가장 큰 단점은 클래스의 필드들이 항상 get/set멤버 함수를 통해서만 접근되는 것으로 가정하는 점이다. 만약 직접 노출되어 접근되는 필드가 있다면 리팩터링 작업이 재앙이 될 수 있다.

## 요약
신중하게만 사용된다면 싱글턴 패턴 자체가 나쁜 것은 아니다. 하지만 일반적으로는 테스트와 리팩터링 용이성을 해칠 수 있다. 싱글턴을 꼭 사용해야만 한다면 직접적인 사용법(SomeComponent.getInstance().foo()를 호출하는 식의)을 피한다. 대신 종속성을 주입하는 방식(생성자의 인자 등)으로 모든 종속성이 전체 코드의 한곳에서 관리 될 수 있는 형태(IoC 컨테이너 등)을 활용하는 것이 바람직하다.