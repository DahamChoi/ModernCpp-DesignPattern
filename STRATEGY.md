# 전략
*전략 패턴은 실행 중에 알고리즘을 선택할 수 있게 하는 디자인 패턴이다.*

## 동적 전략
앞으로 살펴볼 예제의 목표는 단숞나 텍스트의 목록을 아래의 두 가지 포맷으로 랜더링하는 것이다.

	enum class OuputFormat
	{
		markdown,
		html
	};

"전략"의 골격은 아래의 베이스 클래스로 정의된다.

	struct ListStrategy
	{
		virtual void start(ostringstream& oss) {};
		virtual void add_list_item(ostringstream& oss, const string& item) {}
		virtual void end(ostringstream& oss) {};
	};

이제 텍스트 처리 컴포넌트를 보자. 이 컴포넌트는 목록 처리를 위한 전용 멤버 함수 apeend_list()를 갖는다.	

	struct TextStrategy
	{
		void append_list(const vector<string> items)
		{
			list_strategy->start(oss);
			for(auto& item : items)
				list_strategy->add_list_item(oss, item);
			list_strategy->end(oss);
		}
	private:
		ostringstream oss;
		unique_ptr<ListStrategy> list_strategy;
	};

위 코드에서 oss는 결과가 출력될 버퍼이다. append_list()는 목록을 렌더링하는 과정을 정의할 뿐만 아니라 렌더링에 적용할 전략도 가진다.

위 코드에는 컴포지션이 사용되고 있다. 컴포지션은 이러한 골격 알고리즘을 실체화된 구현으로 만들어주는 방법 중 하나이다.

컴포지션 대신 add_list_item()과 같은 버추얼 멤버 함수를 두어 하위 클래스에서 오버라이딩하게 할 수도 있다. 그러한 방법은 템플릿 메서드 패턴이라고 한다.

목록 렌더링 전략을 어떻게 구현하는지 살펴보자. 먼저 HtmlListStrategy는 다음과 같이 구현될 수 있다.

	struct HtmlListStrategy : ListStrategy
	{
		void start(ostringstream& oss) override
		{
			oss << "<ul>" << endl;
		}

		void end(ostringstream& oss) override
		{
			oss << "</ul>" << endl;
		}

		void add_list_item(ostringstream& oss, const string& item) override
		{
			oss << "<li>" << item << "</li>" << endl;
		}
	};

오버라이딩해야 할 멤버 함수들을 구현함으로써 목록을 처리하는 과정의 골격 사이 사이를 살로 채운다. MarkdownListStrategy 전략도 비슷한 방식으로 구현한다. 그런데 Markdown 방식에서는 열림/닫힘 태그가 필요 없기 때문에 add_list_item() 멤버 함수의 오버라이딩만 구현하면 된다.

	struct MarkdownListStrategy : ListStrategy
	{
		void add_list_item(ostringstream& oss, const string& item) override
		{
			oss << " * " << item << endl;
		}
	};

이제 TextProcessor를 이용하여 서로 다른 전략에 목록을 입력하여 서로 다른 렌더링 결과를 얻을 수 있다. 예를 들어 다음과 같이 이용할 수 있다.

	TextProcessor tp;
	tp.set_output_format(OutputFormat::markdown);
	tp.append_list({"foo","bar","baz"})l;
	cout << tp.str() << endl;

전략을 런타임에 목적에 따라수도 있다. 이 때문에 이 구현을 "동적 전략"패턴이라고 부른다. set_output_format() 함수에서 전략의 선택이 이루어진다. 이 함수의 구현은 아래와 같이 단순하다.

	void set_output_format(const OutputFormat format)
	{
		switch(format)
		{	
		case OutputFormat::markdown:
			list_stragy = make_unique<MarkdownListSTrategy>();
			break;
		case OutputFormat::html:
			list_strategy = make_unique<HtmlListSTrategy>();
			break;
		}
	}


## 정적 전략
템플릿의 마법 덕분에 어떤 전략이든 자동으로 타입에 맞추어 적용할 수 있다. 아래와 같이 TextProcessor 클래스에 약간의 수정만 가하면 된다.

	template<typename LS>
	struct TextProcessor
	{
		void append_list(const vector<string> items)
		{
			list_strategy.start(oss);
			for(auto& item : items)
				list_strategy.add_list_item(oss, item);
			list_strategy.end(oss);
		}

	private:
		ostringstream oss;
		LS list_strategy;
	};

위 코드에서 정적으로 전략이 연결될 수 있게 하는 부분은 템플릿 파라미터 LS가 거의 전부이다. 그리고 앞서 포인터로 사용하던 부분을 타입 LS를 사용하도록 바꾸었다. append_list()의 결과는 동일하다.

	TextProcessor<MarkdownListStrategy> tpm;
	tpm.append_list({"foo","bar","baz"});
	cout << tpm.str() << endl;

	TextProcessor<HtmlListStrategy> tph;
	tph.append_list({"foo","bar","baz"});
	cout << tph.str() << endl;

위 예제의 출력 결과는 동적 전략의 예와 동일하다. 하지만 두 개의  TextProcessor 인스턴스를 만들고 있다는 점을 유의하자. 목록 렌더링 전략마다 개별적으로 인스턴스를 가져야만 한다.

## 요약
전략 디자인 패턴은 알고리즘의 골격만을 정의하고 세부 구현은 컴포지션으로서 특정 전략을 선택적으로 채워 넣을 수 있게 한다. 이 접근 방법은 아래와 같이 두가지 실현 방법이 있다.
 - 동적 전략은 사용될 전략을 단순히 포인터 또는 참조로서 가진다. 전략을 바꾸고 싶을 때는 참조를 변경하면 된다.
 - 정적 전략은 컴파일 시점에 전략이 선택되어 고정되도록 한다. 나중에 전략을 바꿀 수 없다.

둘 중 어느 것을 이용해야 할까? 동적 전략은 객체가 생성된 이후에도 전략을 재설정할 수 있다. 텍스트 출력 형태를 설정하는 UI 메뉴가 있다고 하자. 출력 형태를 바꿀 수 있는 TextProcess를 사용하는 것이 편리할까 아니면 두 개의 객체 변수 TextProceesor\<MarkdownStrategy\>와 TextProcessor\<HtmlStrategy\>를 바꿔 사용하는 것이 편리할까? 이 부분은 전적으로 개발자의 선택사항이다.

마지막으로 적용할 수 있는 전략의 목록을 제한할 수도 있따는 것을 언급하고 넘어간다. 일반화된 LsitStrategy 인자 대신, 적용 가능한 전략의 목록을 지정하여 std::variant로 전략을 전달할 수도 있다.