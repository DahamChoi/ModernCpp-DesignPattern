# 브릿지

***브리지 패턴**이란 구현부에서 추상층을 분리하여 각자 독립적으로 변형할 수 있게 하는 패턴이다.*

## Pimpl 관례
Pimpl은 Pointer to Implementation의 약자이다. 즉, 구현부를 포인터로 참조하는 관례를 뜻한다.

    struct Person{
    	std::string name;
    	void greet();
    	Person();
    	~Person();
    	class Personlmpl;
    	PersonImpl *impl;
    };

> 헤더부

    struct Person::PersonImpl{
    	void greet(Person* p);
    }
    Person::Person() : impl(new PersonImpl) {}
    Person::~Person(){ delete impl; }
    void Person::Person::greet(){ impl->greet(this); }
    void Person::PersonImpl::greet(Person* p){
    	printf("Hello %s", p->name.c_str());
    }

> 소스부

이 코드는 뭔가 이상하다. 대단히 단순한 클래스임에도 난해한 정의들이 들어간다. 필드 변수 name과 멤버 함수 greet()까지는 자연스럽지만 굳이 생성자와 소멸자가 필요할까? 그리고 PersonImpl은 무엇인가?

이 클래스의 이상함은 클래스의 구현부를 다른 클래스(PersonImpl)에 숨기고자 하는 의도에서 발생한다. 구현 클래스 PersonImpl을 헤더파일이 아닌 .cpp 파일에 정의하는 것이 핵심적이다. 왜 이렇게 할까? 왜 클래스를 이중으로 정의해서 포인터를 왔다 갔다 하는 것일까? 이 방식은 3가지 장점이 있다.
1. 클래스의 구현부의 상당 부분이 실제로 감춰질 수 있다. 만약 Person 클래스가 많은 수의 private/protected 멤버를 가진다면 헤더를 통해 모두 클라이언트에 노출된다. private/protected 한정자로 인해 실제 사용은 할 수 없지만 불필요하게 많은 정보가 노출되는 것은 피할 수가 없다. 하지만 Pimpl 관례를 이용하면 꼭 필요한 public 인터페이스만 노출할 수 있다.
2. 바이너리 호환성을 보증하기 쉬워진다. 숨겨진 구현 클래스에 대한 수정은 바이너리 호환성에 영향을 미치지 않는다.
3. 헤더 파일이 멤버 선언에 필요한 헤더들만 인클루드하게 할 수 있다. 즉, 구현부 때문에 클라이언트에서 필요 없는 헤더까지 인클루드해야 하는 상황을 피할 수 있다. 예를 들어 Person이 private 멤버로 vector\<string\>을 가진다고 하자. 어쩔 수 없이 Person.h 파일 안에 #include \<string\>을 해야 한다. 그리고 Person.h를 사용하는 모든 사용자는 강제로 \<vector\>와 \<string\>을 인클루드 하게 된다. 하지만 Pimpl 관례를 이용하면 Person.cpp에서만 해당 헤더들을 인쿨르드하면 된다.

이러한 장점들은 깨끗하고 안정적인 헤더를 유지할 수 있게 해준다. 디자인 패턴관점에서는 Pimpl을 브릿지 패턴의 좋은 예로 볼수 있다. 위 예에서는 멤버 변수 pimpl이 불투명 포인터이다. 즉, 포인터의 상세 사항이 숨겨져 있다. 이 포인터는 공개용 인터페이스와 숨겨야 할 .cpp 파일의 세부구현을 연결하는 브릿지 역할을 한다.

## 브릿지
Pimpl 관례는 브릿지 디자인 패턴의 매우 특별한 예일 뿐이다. 이제 일반적인 관점에서 브릿지 디자인 패턴이 무엇인지 살펴보자.

[ModernCpp-DesignPattern/structural/bridge at main · DahamChoi/ModernCpp-DesignPattern (github.com)](https://github.com/DahamChoi/ModernCpp-DesignPattern/tree/main/structural/bridge)

위의 코드에서 브릿지를 만든 다음 참조로서 Circle을 넘기면 draw()가 호출될 때 브릿지를 이용해 원을 그린다. 브릿지는 자신이 다루고 있는 것이 Circle인지 자신이 참조 변수로 접근되고 있다는 것도 알지 못한다.

## 요약
브릿지는 단순한 개념이다. 연결 끈이나 접착제처럼 두 가지를 함께 연결한다. 추상화(인터페이스)를 하면 어떤 컴포넌트가 다른 컴포넌트와 그 상세 구현을 전혀 알지 못하는 상태에서도 서로 연동할 수 있다.

브릿지 패턴에 연결되는 것들은 서로의 존재를 알아야만 한다. 예를 들어 Circle은 Renderer의 참조를 가져야 했고, 반대로 Renderer는 원을 어떻게 그리는지 알아야 했다. 이 부분은 Mediator 패턴과 대비되는 부분이다. 매개자 패턴은 서로를 직접적으로 알지 못하더라도 서로 연동할 수 있게 해준다.