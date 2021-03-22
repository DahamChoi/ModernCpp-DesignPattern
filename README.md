# modern_cpp_designpattern 
모던 C++ 디자인 패턴(저 : 드미트리 네스터룩) 학습, 해당 책을 공부하며 코드를 이 레포스트리에 정리하고 학습한 내용을 READ.MD에 작성하고자 한다.

## 몇가지 개념 

### 이상한 재귀 템플릿 패턴(Curiously Recurring Template Pattern, CRTP)

    struct Foo : SomeBase { ... }

 > 자기 자신을 베이스 템플릿 인자로 상속받는 것 

    template struct SomeBase{
	    void foo(){
		    for(auto& item : *static_cast<Derived*>(this)){ ... }}};

> 베이스 클래스의 구현부에서 타입이 지정된 this포인터를 사용할 수 있게 된다. 예를 들어 SomeBase를 상속받는 모든 서브 클래스들이 begin()/end() 메서드 쌍을 구현한다고 가정해 보자. SomeBase의 메서드 안에서 서브 클래스의 객체를 순회할 수 있을까? CRTP를 적용하면 this를 서브 클래스 타입으로 캐스팅할 수 있다. 

### 첨가(Mixin) 상속

    template struct Mixin : T { ... }

 > C++ 에서는 클래스를 정의할 때 자기 자신을 템플릿 인자로 할 수 있다. 이러한 방식을 첨가 상속(mixin inheritance)이라고 한다. 첨가 상속을 이용하면 계층적으로 여러 타입을 합성할 수 있다. 예를 들어 Foo<Bar\<Baz\>> x;와 같이 변수를 선언하면 새로운 타입 FooBarBaz를 따로 구현하지 않고도 세 가지 타입의 속성을 가지게 할 수 있다.

### 속성

    class Person{ 
	    int age_;
	public:
		int get_age() const { return age_; }
		void set_age(int value) { age_ = value; }
		__declspec(property(get=get_age,put=set_age)) int age; };

 > get/set 메서드를 가지는 클래스 내부 변수를 보통 그 클래스의 속성이라 부른다. 많은 프로그래밍 언어들이 속성을 언어 자체의 내장 기능으로 제공한다. 하지만 C++은 그렇지 않다. 하지만 컴파일러에서 비표준적인 방법으로 지원하고 있다.

## SOLID 디자인 원칙

### 단일 책임 원칙(Single Reponsibility Principle, SRP)
> **각 클래스는 단 한가지의 책임을 부여받아, 수정할 이유가 단 한가지여야 한다.**  SRP를 위배하는 안티 패턴의 극단적인 예로 전지전능 객체가 있다. 전지전능 객체는 가능한 많은 기능을 담아 하나의 괴물 같은 클래스를 이룬다. 

### 열림-닫힘 원칙(Open-Closed Principle, OCP)
> **타입이 확장에는 열려 있지만 수정에는 닫혀 있어야 한다.** 

### 리스코프 치환 원칙(Liskov Subsitution Principle, LSP)
> **어떤 자식 객체에 접근할 때 그 부모 객체의 인터페이스로 접근하더라도 아무런 문제가 없어야 한다.** 즉, 자식객체를 그 부모 객체와 동등하게 취급할 수 있어야 한다.

### 인터페이스 분리 원칙(Interface Sergregation Principle, ISP)
> **필요에 따라 구현할 대상을 선별할 수 있도록 인터페이스를 별개로 두어야 한다**

### 의존성 역전 원칙(Dependency Inversion Principle, DIP)
> **상위 모듈이 하위 모듈에 종속성을 가져서는 안 된다. 양쪽 모두 추상화에 의존해야 한다.**  **추상화가 세부 사항에 의존해서는 안된다. 세부 사항이 추상화에 의존해야 한다.** 즉, 종속성이 실 구현 타입이 아니라 인터페이스 또는 부모 클래스에 있어야 한다는 것을 말한다.

# 생성 패턴
 > 생성 패턴들은 객체의 생성과 관련된 공통적인 접근 방법들을 담는다.

## C++의 기본적인 부분

### 스택 할당
> 스택 메모리를 할당받아 객체를 생성하는 것을 말한다. 이러한 객체는 스코프를 벗어날 때 자동으로 소멸된다. 명시적으로 객체를 소멸시키지 않는다면 객체가 대입된 변수가 스코프에서 벗어나는 마지막 순간에 해당 객체의 소멸자가 호출된다.

### 힙 할당
> 원시 포인터를 이용하여 Foo\* foo = new Foo와 같이 객체를 생성하면 힙 메모리에 객체가 할당된다. 이렇게 인스턴스를 생성하면 메모리 해제를 비롯한 뒷정리를 누가 할 것인지가 불분명한 문제가 생긴다. 이 부분은 온전히 개발자에게 맡겨져 있다.

### unique 포인터(unique_ptr)
> "독점" 포인터라고도 한다. 이 스마트 포인터는 힙에 할당된 포인터를 받아, 더 이상 참조하는 곳이 없을 때 자동으로 소멸 작업이 이루어지도록 관리해준다. unique포인터는 "독점"이라는 이름이 의미하듯이 복제할 수 없고 다른 함수에 넘겨줄 수도 없다. 단, 원본과 그 제어권까지 통째로 이양하는 것은 가능하다.

### shared 포인터(shared_ptr)
> "공유" 포인터라고도 한다. 이 스마트 포인터는 힙에 할당된 포인터를 받아 코드 안의 다른 곳과 포인터를 공유하고 그 어느 부분에서도 더 이상 참조하는 곳이 없을 때 자동으로 소멸되도록 관리해준다.

### 약한 포인터(weak_ptr)
> "간접" 포인터라고도 한다. 이 포인터는 소유 개념이 없다. 단지 shared_ptr에 의해 관리되는 포인터를 간접적으로 참조한다. 실제 객체에 참조할 때는 shared_ptr로 변환해야만 한다. 이 포인터는 shared_ptr간에 순환 참조가 일어나는 것을 막기 위해 사용된다.

## 빌더
> 빌더 패턴은 생성이 까다로운 객체를 쉽게 처리하기 위한 패턴이다. 즉, 생성자 호출 코드 단 한줄로 생성할 수 없는 객체를 다룬다. 그러한 타입의 객체들은 다른 객체들의 조합으로 구성되거나, 상식적인 것을 벗어난 까다로운 로직을 필요로 한다. 이러한 객체를 생성하는 코드는 따로 분리되어 관리될 필요가 있다.

### 시나리오
> 웹페이지를 그리기 위한 컴포넌트들을 생성해야 한다고 하자. 먼저, 단순하게 단어를 나타내는 항목 두 개를 html의 비순차 리스트 태그로 출력해 보자.

    struct HtmlElement{
    	string name;
    	string text;
    	vector<HtmlElement> elements;
    	HtmlElement() {}
    	HtmlElement(const string& name, const string& text) : name(name), text(text) {}
    	string str(int indent = 0) const { ... } };
    
    string words[] = { "hello", "world" };
    HtmlElement list("ul", ""};
    for(auto w : words)	list.elements.emplace_back(HtmlElement("li",w));
    printf(list.str().c_str());

> 이 코드는 OOP에 기반하여 항목 리스트를 표현한다. 양식을 제어하기가 좀 더 쉬우면서도 목적하는 출력을 할 수 있다. 하지만 각각의 HtmlElement를 생성하는 작업이 그렇게 편리하다고 볼 수는 없다. 이 부분을 빌더 패턴을 활용하여 개선 할 수 있다.

### 단순한 빌더

    struct HtmlBuilder{
    	HtmlElement root;
    	HtmlBuilder(string root_name){ root.name = root_name; }
    	void add_child(string child_name, string child_text){
    		HtmlElement e{ child_name, child_text };
    		root.elements.emplace_back(e); }
    	string str(){ return root.str(); }
    
    HtmlBuilder builder{ "ul" };
    builder.add_child("li", "hello");
    builder.add_child("li", "world");
    cout << builder.str() << endl;

> 빌더 패턴은 단순하게 개별 객체의 생성을 별도의 다른 클래스에 위임한다. 위 코드는 빌더 패턴의 첫 번째 구현 시도이다. HtmlBuilder는 HTML 구성 요소의 생성만을 전담하는 클래스이다. add_child() 메서드는 현재 요소에 자식 요소를 추가하는 목적으로 사용된다. 그런데 add_child() 메서드는 리턴 값은 사용되는 곳 없이 void로 선언되어 있다. 리턴 값을 활용하면 좀 더 편리한 흐름식 인터페이스(fluent interface) 스타일의 빌더를 만들 수 있다.

### 흐름식 빌더

    HtmlBuilder& add_child(string child_name, string child_text){
    	HtmlElement e{ child_name, child_text };
    	root.elements.emplace_back(e);
    	return *this;
    }
    
    builder.add_child("li","hello").add_child("li","world");

> 빌더 자기 자신이 참조로서 리턴되기 때문에 메서드들이 꼬리를 무는 호출이 가능해진다. 이러한 형태로 호출하는 것을 흐름식 인터페이스(fluent interface)라고 부른다. 

### 의도 알려주기

### 그루비-스타일 빌더

### 컴포지트 빌더

### 요약




