# 플라이웨이트 패턴
***플라이웨이트 패턴**은 동일하거나 유사한 객체들 사이에 가능한 많은 데이터를 서로 공유하여 사용하도록 하여 메모리 사용량을 최소화하는 소프트웨어 디자인 패턴이다. 종종 오브젝트의 일부 상태 정보는 공유될 수 있는데, 플라이웨이트 패턴에서는 이와 같은 상태 정보를 외부 자료 구조에 저장하여 플라이웨이트 오브젝트가 잠깐동안 사용할 수 있도록 전달한다.*

## 사용자 이름
대규모 멀티 플레이가 지원되는 온라인 게임을 생각해보자. 예를 들어 John Smith라는 이름은 너무도 흔해서 두 명 이상이 있을 가능성이 매우 높다. 만약 그러한 이름들을 계속해서 각각 따로 저장해 나간다면 매번 11바이트의 공간을 낭비하게 된다. 대신에 이름을 한 번만 저장하고 같은 이름을 가진 사용자들이 그 이름을 포인터로 가지게 할 수 있다. 이것은 적지 않은 메모리 절감이다.

더 나아가서 이름을 "성"과 "이름"으로 분할하는 것도 고려해볼 만하다. 그렇게하면, 예를 들어 Fizgerald Smith라는 이름이 각각 "Fitzerald"과 "Smith"을 가리키는 두 개의 포인터로 표현된다.

	typedef uint32_t key;
	
	struct User{
		User(const string& first_name, const string& last_name)
			: first_name{add(first_name)}, last_name{add(last_name)} {}

	protected:
		key first_name, last_name;
		static bitmap<key, string> names;
		static key seed;
		static key add(const string& s){
			auto it = names.right.find(s);
			if(it == names.rights.end()){
				names.insert({++seed, s});
				return seed;
			}
			return it->second;
		}
	};

이 코드에서 볼 수 있듯이 생성자는 멤버 변수 first_name 과 last_name을 멤버함수 add()의 리턴 값으로 초기화한다. add()는 키/값 쌍을 names 데이터 구조에 추가한다. 이 코드는 "가져오거나 추가하기" 메커니즘의 표준적인 구현이다. 이제 key 타입의 protected 인덱스 변수가 아니라 실제 성과 이름을 제공하기 위해 적절한 get/set 멤버 함수를 만들어야 한다.

## Boost.Flyweight
앞서 예제에서는 플라이웨이트를 사실상 수작업으로 하나하나 만들었다. Boost 라이브러리를 사용하면 그러한 수작업을 덜 수 있다.  boost::flyweight 타입은 그 이름이 암시 하듯이 플라이웨이트를 위한 것이다. 이 타입을 이용하면 공간을 절약하기 위한 플라이웨이트를 쉽게 생성할 수 있다.

User 클래스의 이름 멤버 변수에 boost::flyweight을 이용하면 아래와 같이 너무나 간단하게 플라이웨이트가 적용된다.

	struct User{
		flyweight<string> firstname, last_name;
		...
	};

## 문자열 범위
std::string::substring()을 호출하면 새로운 문자열이 생성되어 리턴될까? 확실하지 않다. 만약 리턴받은 부분 문자열을 수정하고 싶다면 수정할 수 있다. 그러면 원본 문자열에 영향이 있을까? 어떤 프로그래밍 언어는(예를 들어 siwtf나 Rust) 부분 문자열을 리턴할 때 문자열 자체가 아니라 명시적으로 그 범위를 리턴한다. 이러한 방식은 리턴받은 범위의 문자열을 수정할 수 있게 하는 것과 더불어 메모리 사용량을 절약하기 위한 플라이웨이트 패턴을 구현하고 있는 것이다.

이와 동등한 기능으로 C++에서는 string_view를 통해 문자열 범위를 제공한다. 이에 더하여 array에 대해서도 그 어떤 복제도 피할 수 있도록 여러 확장 타입들이 제공된다. 이제 매우 단순한 문자열 범위 기능을 직접 만들어보자.

클래스에 많은 수의 알파벳 텍스트들이 저장되어 있고, 그 텍스트의 어떤 범위를 선택하여 대문자로 바꾸러 한다고 가정하자. 워드프로세서나 IDE에서의 문자 변환 기능과 유사하다. 단순하게 범위에 포함되는 모든 문자를 대문자로 만들 수도 있지만, 그런 방법 대신 원본 텍스트에는 변경을 가하지 않고 스트림 출력 연산자로 텍스트를 내보낼 때만 그렇게 하고 싶다고 하자.

## 섣부른 접근 방법
매우 단순하고 비효율적인 방법 중 하나는 모든 문자와 매칭되는, 즉 원본 텍스트와 같은 크기의 이진 배열을 만들어 두고 대문자로 만들 문자를 표시하는 것이다. 이 방법은 다음과 같이 구현될 수 있다.

	class FormattedText{
	private:
		string plainText;
		bool* caps;
	public:
		explicit FormattedText(const string& plainText)
			: plainText{plainText}{
			caps = new bool[plainText.length()];
		}
		~FormattedText(){	delete[] caps;	}
		void capitalize(int start, int end){
			for(int i = start; i <= end; ++i)
				caps[i] = true;
		}
		friend std::ostream& operator<<(std::ostream& os, const FormattedText& obj){
			string s;
			for(int i = 0; i < obj.plainText.length(); ++i){
				char c = obj.planText[i];
				s += (obj.caps[i] ? toupper(c) : c);
			}
			return os << s;
		}
	};

그다지 멋지지는 않지만 목적하는 동작은 해낸다. 하지만 범위에 대한 시작/끝 표시만으로 요구 조건을 나타내기에 충분함에도 불구하고, 텍스트의 모든 문자마다 이진 플래그를 만든다는 것은 매우 큰 낭비이다. 여기에 플라이웨이트 패턴을 적용하여 개선해보자.

## 플라이웨이트의 구현
먼저, 상위 클래스와 플라이웨이트 클래스를 정의한다. 플라이웨이트 클래스는 상위 클래스 안에서 중첩 클래스로서 정의한다. TextRange는 포맷을 적용할 범위의 시작과 끝점만 저장한다. 그리고 대문자/소문자와 같이 적용할 포맷의 속성 정보도 저장한다. 포맷을 적용할지 여부에 대한 판단은 멤버 함수 covers) 하나에만 의존한다. 이 함수는 주어진 위치의 문자가 포맷을 적용을 받을 필요가 있는 여부를 결정해준다.

이제 TextFormattedText를 위한 operator<<를 구현할 수 있다. 이 구현에서 각각의 문자에 대해 속하는 범위가 있는지 검사한다. 어떤 범위든 속하는 범위가 있다면 대문자로 바꾼다. 이구현은 앞서 섣부른 구현이 할 수 있는 것처럼 대문자 변환을 해내면서도 약간의 변화만으로 더 유연한 API를 제공한다.

[ModernCpp-DesignPattern/structural/flyweight at main · DahamChoi/ModernCpp-DesignPattern (github.com)](https://github.com/DahamChoi/ModernCpp-DesignPattern/tree/main/structural/flyweight)

## 요약
플라이웨이트 패턴은 기본적으로 공간 절약을 위한 테크닉이다. 플라이웨이트가 실제 구현되는 형태는 매우 다양할 수 있다. 어떤 경우에는 플라이웨이트를 API에 사용하기 위한 토큰으로서 전달받아 플라이웨이트가 어디서 생성되었든 관계없이 활용하고 수정하게 할 수도 있고 또는, 사용자도 모르게 암묵적으로 플라이웨이트가 활용될 수도 있다.