# 템플릿 메서드
*템플릿 메서드 패턴은 동작 상의 알고리즘의 프로그램 뼈대를 정의하는 행위 디자인 패턴이다. 알고리즘의 구조를 변경하지 않고 알고리즘의 특정 단계들을 다시 정의할 수 있게 해준다.*

## 게임 시뮬레이션
보드 게임류는 대부분 서로 비슷하다. 게임을 시작하고, 플레이어가 차례로 돌악며 어떤 액션을 하고, 승자가 결정되면 게임이 종료된다. 체스, 바둑 등등 거의 대부분의 보드 게임이 이러한 형태를 따른다. 따라서 아래와 같은 알고리즘을 정의할 수 있다.

	class Game
	{
		void run()
		{
			start();
			while(!have_winner())
				take_turn();
			cout << "Player " << get_wintter() << "wins";
		}
	}

게임에서 실행하는 run() 메서드는 단지 다른 메서드들을 호출할 뿐이다. 그 메서드들은 퓨어 버추얼로 정의되어 있고 protected이기 때문에 다른 쪽에서 호출할 수는 없다.

	protected:
		virtual void start() = 0;
		virtual bool have_winner() = 0;
		virtual void take_turn() = 0;
		virtual int get_winner() = 0;

일반적으로 보자면 꼭 버출어이어먄 할 이유는 없다. 특히 리턴 값이 void인 경우는 더욱 그렇다. 예를 들어 만약 어떤 게임에 명시적인 start() 단계가 없다면 퓨어 버추얼인 start() 메서드는 인터페이스 분리 원칙의 위반을 유도하게 된다. 왜냐하면 필요 없는 멤버를 구현해야만 하기 때문이다. "전략" 디자인 패턴 절에서는 의도적으로 아무것도 하지 않는 버추얼 메서드를 만들었다. 하지만 템플릿 메서드를 사용할 때는 문제 여부가 불분명해진다.

이제 위 코드에 공통적인 기능을 더 추가해보자. 플레이어의 수와 현재 플레이어의 인덱스는 모든 게임에서 의미가 있을 것이다.

	class Game
	{
	public:
		explicit Game(iont number_of_players)
			: number_of_players(number_of_players){}
	protected:
		int current_player{ 0 };
		int number_of_players;
		...
	};

이제 Game 클래스를 확장하여 체스 게임을 구현할 수 있다.

	class Chess : public Game
	{
	public:
		explicit Chess() : Game{ 2 } {}
	protected:
		void start() override {}
		bool have_winner() override { return turns == max_turns; }
		void take_turn() override
		{
			turns++;
			current_player = (current_player + 1) % number_of_players;
		}
		int get_winner() override { return current_player; }
	private:
		int turns{ 0 }, max_turn{ 10 };
	};

체스는 두 명의 플레이어를 필요로 하기 때문에 생성자에서 2명을 파라미터로 전달한다. 필요한 함수들을 오버라이딩하면서 가상으로 10번의 말을 두고 승자를 가리도록 구현한다. 이 예제는 템플릿 메서드 디자인 패턴의 거의 모든 것을 담고 있다.

## 요약
전략 패턴에서는 콤포지션을 사용하여 동적 전략과 정적 전략으로 나뉘어졌지만 템플릿 메서드에서는 상속을 사용하기 때문에 정적인 방법만 존재한다. 이미 생성된 객체의 부모를 바꿀 방법은 없으므로 상속을 이용하면 결과적으로 정적인 방법이 된다.

템플릿 메서드 패턴에서는 설계 차원의 선택 사항이 한 가지뿐이다. 템플릿 메서드에서 이용될 메서드를 퓨어 버츄얼로 할 것이냐 실 구현체로 할 것이냐(공백 함수일 수도 있다)를 선택해야 한다. 자식 클래스에서 모든 메서드를 구현할 필요가 없다면, 실 구현체를 선택하고 아무것도 공백 함수를 두는 것이 더 편리 할 수 있다.