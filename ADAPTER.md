# 어댑터

***어댑터 패턴**(Adapter pattern)은 클래스 인터페이스를 사용자가 기대하는 다른 인터페이스로 변환하는 패턴으로, 호환성이 없는 인터페이스 때문에 함께 동작할 수 없는 클래스들이 함께 작동하도록 해준다.*

## 시나리오
간단한 예를 하나 살펴보자. 픽셀을 그리는데 적합한 어떤 그리기 라이브러리가 있고 이 라이브러리를 사용해야만 그림을 그릴 수 있다고 하자. 그런데 선분, 사각형과 같은 기하학적 모양을 그려야 한다고 하자. 하나하나에 점을 찍는 방식으로 무엇이든 그릴 수는 있지만 기하학적 도형을 그리기에는 너무 저수준 작업이다. 따라서 기하학적 도형을 픽셀 기반 표현으로 바꾸어주는 어댑터가 필요하다.

## 어댑터
사각형 몇 개를 그려야 한다고 가정해 보자

	vector<shared_ptr<VectorObject>> vectorObject{
		make_shared<VectorRectangle>(10,10,100,100),
		make_shared<VectorRectangle>(30,30,60,60)
	}

이 객체들을 그리기 위해서는 사각형을 이루는 선분의 집합에서 각각의 선분마다 많은 수의 점이 변환되어야 한다. 이를 위해 별도의 클래스를 만든다. 이 클래스는 선분 하나를 점의 집합으로 만들어 저장하고 각 점들을 순회할 수 있도록 반복자로 노출한다.

	struct LineToPointAdapter{
	public:
		typedef vector<Point> Points;
		LineToPointAdapter(Line& line){	// TO DO }
		virtual Points::iterator begin(){ return points.begin(); }
		virtual Points::iterator end(){ return points.end(); }
	private:
		Points points;
	};

	for(auto& obj : vectorObjects){
		for(auto& line : *obj){
			LineToPointAdapter ltp{ line };
			DrawPoints(dc, ltp.begin(), ltp.end());
		}
	}

기하 도형에서 선분 집합을 정의하면 그 선분들로 LineToPointAdapter를 생성하여 점들의 집합으로 넘겨주고, 그점들을 순회할 수 있는 시작 반복자와 끝 반복자를 DrawPoints에 넘겨주어 그림을 그린다.

## 일시적 어댑터
안타깝게도 위 코드는 아름답지만 문제가 되는 상황이 있다. 화면이 업데이트 될 때마다 DrawPoints()가 불리는 경우를 생각해보자. 전혀 바뀐 것이 없더라도 도형의 선분들이 어댑터에 의해 반복적으로 점으로 변환된다. 이러한 비효율적인 중복 작업을 어떻게 피할 수 있을까?

한 가지 방법은 캐싱을 이용하는 것이다. 즉, 모든 Point를 애플리케이션이 기동할 때 미리 어댑터를 이용해 정의해 두고 재활용하는 것이다.

이제 반복 변환 작업은 없다. 하지만 잠깐 다시 생각해보자. 원본 vectorObjects가 바뀌어 선분을 다시 점으로 변환해야 하는 경우에는 어떻게 할 것인가? 미리 만들어 두고 재활용하는 방식을 사용할 수가 없다. 하지만 비효율적인 반복 변환 작업도 피해야 한다. 어떻게 해야 할까? 캐싱을 활용한다.

[ModernCpp-DesignPattern/structural/adapter at main · DahamChoi/ModernCpp-DesignPattern (github.com)](https://github.com/DahamChoi/ModernCpp-DesignPattern/tree/main/structural/adapter)