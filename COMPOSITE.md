# 컴포지트 패턴
***컴포지트 패턴**이란 객체들의 관계를 트리 구조로 구성하여 부분-전체 계층을 표현하는 패턴으로, 사용자가 단일 객체와 복합 객체 모두 동일하게 다루도록 한다.*

## 배열에 기반한 속성
컴포지트 디자인 패턴은 보통 전체 클래스를 대상으로 적용된다. 하지만 그러한 설명에 들어가기 전에 클래스의 속성 수준에서 어떻게 사용될 수 있는지 먼저 살펴보자. "속성"이라는 용어는 클래스의 필드 멤버는 물론이고 API를 통해서 사용자에게 그 필드가 노출되는 방식을 포괄하여 지칭한다.

	class Creature{
		int strength, agility, intelligence;
	public:
		int get_strength() const { return strength; }
		void set_strength(int strength){ this->strength = strength; }
		// 다른 get/set 함수들...
		int sum() const{ return strength + agility + intelligence; }
		double average() const{ return sum() / 3.0; }
		int max() const { return ::max(::max(strength,agility), intelligence);
	};

어떤 컴퓨터 게임에 크리처들을 만들려고 한다고 하자. 여기서 크러처들의 특성 값들에 대한 통계를 내야 한다고 생각해보자. 위의 구현은 몇가지 이유로 그다지 바람직하지 않다.
- 전체 합계를 계산할 때 필드 중 하나를 빠드리기 쉽다.
- 평균을 계산할때 상숫값 3.0이 사용되고 있다. 이 값은 필드의 개수가 바뀔 때 함계 바뀌어야 해서 의도치 않은 종속성을 야기한다.
- 최대 값을 구할 때 모든 필드 값 쌍마다 std::max()를 반복 호출해야 한다.

이 코드는 별로 좋지 못하다. 여기에 또 다른 속성이 추가된다고 상상해보자. sum(), average(), max()를 비롯한 모든 집합적 계산 코드가 일일이 리팩터링되어야 한다. 좀 더 우아하게 처리할 방법이 없을까? 그러한 방법으로 배열 기반 속성이 있다.

배열 기반 속성은 다음과 같은 접근 방법을 말한다. 먼저, 필요한 속성 종류에 대한 enum 타입 멤버를 정의한다. 그리고 적절한 크기의 배열을 생성한다.

	class Creature{
		enum Abilities { str, agl, intl, countt };
		array<int, count> abilities;
	};

위의 enum 정의는 enum 항목의 개수를 나타내기 위해 count라는 항목을 추가로 가지고 있다. 여기서 enum 클래스가 아니라 enum 타입을 이용한다는 점을 눈여겨보자. enum 타입을 이용하면 enum 항목을 다루기가 조금 더 수월해진다. 이제 다음과 같이 코드를 구성할 수 있다.

	int get_strength() const { return abilities[str]; }
	void set_strength(int value{ abilities[str] = value; }
	
	int sum() const{
		return accumulate(abilities.begin(), abilities.end(), 0);
	}
	double average() const{
		return sum() / (double)count;
	}
	int max() const {
		return *max_element(abilities.begin(), abilities.end());
	}

## 그래픽 객체의 그루핑
파워포인트 같은 응용 프로그램을 생각해보자. 서로 다른 객체들을 드래그로 여러 개 선택해서 마치 하나의 객체처럼 다루는 작업을 생각해보자. 객체 하나를 더 선택해서 그룹에 추가할 수도 있다. 객체를 화면에 그리는 렌더링 작업도 마찬가지이다. 개별 그래픽 객체를 랜더링할 수도 있고 여러 개의 도형을 하나의 그룹으로 렌더링 할 수도 있다.

	struct GrahphicObject{
		virtual void draw() = 0;
	};

이러한 사용방식은 쉽게 구현할수 있다. 위와 같은 인터페이스 하나면 충분하다. 이 클래스의 이름에서 GraphicObject가 한 개의 그래픽 객체만 나타낸다고 생각했을 수 있다. 하지만 여러 개의 사각형과 원들이 모인 그래픽 객체들도 집합적으로 하나의 그래픽 객체를 나타날 수 있다. 여기서 컴포지트 디자인 패턴이 드러난다. 여러 개의 그래픽 객체를 가지는 GraphicObject를 정의할 수 있다. 당연하게도 이러한 집합적 관계는 재귀적으로 무한히 반복될 수 있다.

	struct Group : GraphicObject{
		std::string name;
		std::vector<GrahpicObject*> objects;

		explicit Group(const std::string& name) : name{ name } {}
		void draw() override{
			std::cout << "Group " << name.c_str() << "contains:" << std::endl;
			for(auto&& o : objects)
				o->draw();
		}
	};

개별 원 객체든 그룹 그래픽 객체든 draw() 함수를 구현하고 있는 그릴 수 있는 도형이다. 그룹 그래픽 객체는 그것을 구성하는 vector에 보관하여 스스로를 렌더링할 때 이용할 수도 있다. 그리고 그룹 그래픽 객체 자체도 다른 그룹에 속할 수 있다. 이제 다음과 같이 API를 이용할 수 있다.

	Group root("root");
	Circle c1, c2;
	root.objects.push_back(&c1);

	Group subgroup("sub");
	subgroup.objects.push_back(&c2);

	root.objects.push_back(&subgroup);

	root.draw();

이 예제는 인위적인 커스텀 인터페이스 정의가 사용되기는 했지만 가장 단순한 형태의 컴포지트 디자인 패턴 구현으로 볼 수 있다.


## 뉴럴 네트워크
구현 클래스에 일일이 개별 연결 함수를 만드는 대신, 베이스 클래스에 연결 함수를 만드록 다중 상속을 이용한다.

	template<typename Self>{
		template<typename T> void connect_to(T& other){
			for(Neuron& from : *static_cast<Self*>(this)){
				for(Neuron& to : other){
					from.out.push_back(&to);
					to.in.push_back(&from);
				}
			}
		}
	};

connect_to()가 어떻게 동작하는지 상세히 보자. 코드에서 볼 수 있듯이 connect_to()는 타입 T에 대한 템플릿 멤버 함수이다. 지정받은 타입T의 인자 other에 대해 *this를 순회하며 other의 뉴런을 연결한다. 여기에는 약간의 트릭이 있다. *this는 SomeNeurons& 타입이기 때문에 그냥 순회할 수가 없다. 따라서 실제 타입 Self로 캐스팅해야 한다.

이 부분이 SomeNeurons&을 템플릿 클래스로 선언할 수 밖에 없는 이유이다. 실제 타입 Self는 나중에 구현 클래스에서 SomeNeurons를 상속받을 때 struct Neuron : SomeNeurons\<Neuron\>과 같이 템플릿 인자로 지정된다. 이러한 트릭은 연결함수를 각 조합 케이스 별로 일일이 만드는 대신 치러야 할 약간의 비용이다.

[ModernCpp-DesignPattern/structural/composite at main · DahamChoi/ModernCpp-DesignPattern (github.com)](https://github.com/DahamChoi/ModernCpp-DesignPattern/tree/main/structural/composite)

## 요약
컴포지트 디자인 패턴은 개별 객체와 컬렉 객체 모두에 동일한 인터페이스를 사용할 수 있게 해준다. 명시적으로 인터페이스 멤버를 두어도 되고,  덕-타이핑을 적용해도 된다. 또는 범위 기반 for 루프를 이용하여 객체의 타입이 가지는 계층적인 의미를 훼손하지 않고 begin()/end() 멤버 함수만 의존성을 가지게 할 수도 있다.

단일 객체를 컬렉션처럼 보이게 해주는 키는 begin()/end() 멤버 함수들에 있다.