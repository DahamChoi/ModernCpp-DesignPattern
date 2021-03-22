# modern_cpp_designpattern
모던 C++ 디자인 패턴(저 : 드미트리 네스터룩) 학습

# 개요

## 몇가지 개념

### 이상한 재귀 템플릿 패턴(Curiously Recurring Template Pattern, CRTP)

> 자기 자신을 베이스 템플릿 인자로 상속받는 것

    struct Foo : SomeBase<Foo> { ... }

> 베이스 클래스의 구현부에서 타입이 지정된 this포인터를 사용할 수 있게 된다. 예를 들어 SomeBase를 상속받는 모든 서브 클래스들이 begin()/end() 메서드 쌍을 구현한다고 가정해 보자. SomeBase의 메서드 안에서 서브 클래스의 객체를 순회할 수 있을까? CRTP를 적용하면 this를 서브 클래스 타입으로 캐스팅할 수 있다.

    template<typename Derived>
    struct SomeBase{
    	void foo(){
    		for(auto& item : *static_cast<Derived*>(this)){ ... } } }

### 첨가(Mixin) 상속

> C++ 에서는 클래스를 정의할 때 자기 자신을 템플릿 인자로 할 수 있다. 

# 생성패턴


# 구조 패턴

# 행태 패턴

