# 인터프리터 
*인터프리터 패턴은 한 언어에서 문들을 평가하는 방법을 규정하는 디자인 패턴이다. 기본 개념은 특화된 컴퓨터 언어에서 각 기호마다 클래스를 갖는 것이다. 언어 내 한 문의 구문 트리는 컴포지트 패턴의 인스턴스이며 클라이언트의 문을 평가(해석)하기 위해 사용된다.*

## 개요
인터프리터 디자인 패턴의 목적은 그 이름이 의미하듯이 입력 데이터를 해석하는 것이다. 텍스트 입력을 대상으로 할 때가 많지만 꼭 텍스트에 한정되지는 않는다. 다음은 인터프리터를 이용해 처리되는 몇 가지 익숙한 예이다.
 - 42나 1.234e12와 같은 숫자 리터널은 바이너리로 저장하면 효율적이다. 그렇게 저장히기 위해 C++에서는 Boost.LexicalCast와 같은 고급 라이브러리를 이용할 수 있다. 원한다면 C API인 atof()를 이용할 수도 있다.
 - 텍스트에서 어떤 패턴을 찾고 싶을 때 정규 표현식을 이용하면 매우 편리하다. 사실 정규 표현식은 특정 목적을 위한 완전 별개의 언어이다(이런 경우를 Domain Specific Language(DSL)라고 부른다) 당연하지만 정규 표현식을 실제 실행하기 위해서는 적절하게 해석되어야 한다.
 - CSV, XML, JSON 또는 더 복잡한 경우를 포함하여 어떤 형태로든 구조화된 데이터는 실제 사용되기 위해 인터프리터가 필요하다.
 - 인터프리터 응용의 정점에는 바로 프로그래밍 언어가 있다. C나 파이썬 같은 프로그래밍 언어의 컴파일러 또는 인터프리터는 코드를 실제 실행할 수 있는 형태로 바꾸기 위해 코드를 이해하는 작업이 필요하다.

## 산술 표현식의 계산
3+(5-4)와 같은 매우 단순한 수식 표현을 구문을 분석해야 한다고 하자. 수식이 덧셈, 뺄셈, 괄호만 이용해서 표현된다고 가정한다. 우리가 만들 프로그램은 이러한 수식을 읽고 계산하여 최종 결괏값을 출력해야 한다.

### 렉싱
표현식을 해석하는 첫 번째 단계는 렉싱이라고 불린다. 렉싱은 문자열 입력을 토믄이라 불리는 단위로 나누어 나열한다. 토큰은 어떤 문법상에서 의미를 가지는 최소 단위이다. 산술 표현식은 토큰들의 나열로 변환될 수 있어야 한다. 이 예에서는 다음과 같은 것들이 토큰이 된다.
- 정수
- 연산자(덧셈 기호, 뺄셈 기호)
- 괄호(열림 또는 닫힘)

따라서 다음과 같은 구조로 정의할 수 있다.

	class Token
	{
	public:
		enum Type { interger, plus, minus, lparen, rparen } type;
		std::string text;

		explicit Token(Type type, const std::string& text)
			: type{ type }, text{ text } {}

		friend std::ostream& operator<<(std::ostream& os, const Token& obj)
		{
			return os << "'" << obj.text << "'";
		}
	};
		
단순한 enum 값이 토큰은 아니다. 토큰은 값이 아니라 분류에 가깝다. 덧셈, 뺄셈 기호는 토큰으로 분류되는 순간 값도 정해진다고 볼 수 있지만, 숫자는 토큰으로 분류되더라도 텍스트에서 해당하는 부분이 별도로 저장되어야 값을 보존할 수 있다.

표현식이 std::string으로 주어져 있을 때, 텍스트를 토큰으로 분할한 결과를 vector\<Token\>으로 리턴하는 것으로 렉싱 절차를 정의할 수 있다.

	std::vector<Token> lex(const std::string& input)
	{
	    std::vector<Token> result;

	    for (int i = 0; i < input.size(); ++i)
	    {
	        switch (input[i])
	        {
	        case '+':
	            result.push_back(Token{ Token::plus, "+" });
	            break;
	        case '-':
	            result.push_back(Token{ Token::minus, "-" });
	            break;
	        case '(':
	            result.push_back(Token{ Token::lparen, "(" });
	            break;
	        case ')':
	            result.push_back(Token{ Token::rparen, ")" });
	            break;
	        default:
	            std::ostringstream buffer;
	            buffer << input[i];
	            for (int j = i + 1; j < input.size(); ++j)
	            {
	                if (isdigit(input[j]))
	                {
	                    buffer << input[j];
	                    ++i;
	                }
	                else
	                {
	                    result.push_back(Token{ Token::interger, buffer.str() });
	                    break;
	                }
	            }
	            break;
	        }
	    }

	    return result;
	}

덧셈, 뺄셈 기호처럼 확정적인 토큰은 파싱하기 쉽다. 사실 그런 경우에는 토큰을 map\<BinaryOperation, char\>으로 저장하는 것이 더 편리하다. 하지만 숫자를 파싱하는 것은 쉽지 않다. 예를 들어 1을 만났을 때 연이어 오는 문자가 있는지 기다려야 한다. 이를 위해 별도의 처리 루틴을 만든다.

### 파싱
파싱은 토큰의 나열을 의미 있는 단위로 바꾼다. 보통 "의미 이는 단위"는 객체 지향적인 데이터 구조를 말한다. 그러한 데이터 구조 위에 모든 타입을 감싸는, 즉, 토큰 정의 트리에서 최상단에 추상 부모 타입을 두면 편리한 경우가 많다. 

	class Element
	{
	public:
		virtual int eval() const = 0;
	};

	class Integer : public Element
	{
	public:
		int value;

		explicit Integer(const int value) : value{ value } {}

		int eval() const override { return value; }
	};

만약 어떤 Element가 Integer가 아니라면 반드시 연산자여야 한다. 이 예에서의 덧셈, 뺄셈은 모두 이항 연사자이므로 모든 연산자 항목은 두 개의 하위 항목을 가진다.

	class BinaryOperation : public Element
	{
	public:
		enum Type { addition, subtraction } type;
		std::shared_ptr<Element> lhs, rhs;

		int eval() const override
		{
			return (type == addition) ?
				lhs->eval() + rhs->eval() : lhs->eval() - rhs->eval();
		}
	};

이 파싱 과정이 해야 할 일은 토큰의 나열을 표현식에 대한 이진 트리로 바꾸는 것이다. 앞서의 준비를 이용해 아래와 같이 구현할 수 있다.

	std::shared_ptr<Element> parse(const std::vector<Token>& tokens)
	{
	    auto result = std::make_unique<BinaryOperation>();
	    bool have_lhs = false;
	    for (size_t i = 0; i < tokens.size(); i++)
	    {
	        auto token = tokens[i];
	        switch (token.type)
	        {
	        case Token::interger:
	        {
	            int value = atoi(token.text.c_str());
	            auto integer = std::make_shared<Integer>(value);
	            if (!have_lhs) {
	                result->lhs = integer;
	                have_lhs = true;
	            }
	            else
	            {
	                result->rhs = integer;
	            }
	        }
	            break;
	        case Token::plus:
	            result->type = BinaryOperation::addition;
	            break;
	        case Token::minus:
	            result->type = BinaryOperation::subtraction;
	            break;
	        case Token::lparen:
	            int j = i;
	            for (; j < tokens.size(); j++)
	                if (tokens[j].type == Token::rparen)
	                    break;

	            std::vector<Token> subexpression(&tokens[i + 1], &tokens[j]);
	            auto element = parse(subexpression);
	            if (!have_lhs)
	            {
	                result->lhs = element;
	                have_lhs = true;
	            }
	            else
	            {
	                result->rhs = element;
	            }
	            i = j;
	            break;
	        }
	    }

	    return result;
	}

have_lhs 변수가 하려는 것은 트리를 만드는 것이다. 이 때 트리의 뿌리는 BinaryExpression이어야 한다. BinaryExpression은 정의에 따라 왼쪽과 오른쪽 항목들을 가진다. 그런데 숫자의 경우 표현식의 왼쪽에 위치해야 할지 오른쪽에 위치해야 할지 알수가 없다. 따라서 have_lhs 변수를 두어 어느쪽에 위치해야 하는지 기록해두어 활용한다.

이제 토큰별로 하나씩 살펴보자. 먼저 숫자의 경우 생성 코드로 넘겨져 텍스트로 표현된 정수가 숫자 값으로 변한된다.

덧셈, 뺄셈 토큰은 단순히 현재 처리중인 연산의 타입을 결정하기 때문에 처리하기 쉽다.

이제 왼쪽 괄호를 살펴보자. 모든 괄호가 아니라 왼쪽 괄호이다. 이 예에서는 오른쪽 괄호를 명시적으로 찾지 않는다. 대신, 오른쪽 괄호, 즉 닫힘 괄호를 만나면 그 괄호 쌍으로 구분되는 부분 표현식을 꺼내어 재귀적으로 parse()를 적용한다. 그리고 현재 처리 중인 표현식의 왼쪽 또는 오른쪽 항목으로 설정한다.

실제 상황에서는 훨씬 더 많은 예외처리가 필요하다. 중첩 괄호뿐만 아니라 닫힘 괄호가 누락되는 것과 같은 잘못된 표현식에 대한 처리도 해야 한다. 정말 괄호가 누락되었다면 어떻게 처리해야 할까? 익셉션을 발생시켜야 할까? 아니면 표현식 마지막에 닫힘 괄호가 있는 것으로 삼고 남아 있는 표현식을 계속 파싱해야 할까? 아니면 다른 방법이 있을까?

C++ 언어 자체에 대한 경험으로 미루어볼 때 의미 있는 파싱 에러 메세지를 출력하는 것은 대단히 어려운 일이다. 사실, 뭔가 빠드린 것이 있을 때 렉서나 파서는 무언가 의미있는 것을 만날 때까지 잘못된 코드를 그냥 넘겨버린다. 특히 정적 분석 도구들이 이러한 접근법을 취한다. 그렇게 해야만 실시간으로 타이핑하고 미완성 코드에서도 의미 있게 동작할 수 있다.

## Boost.Spirit을 이용한 파싱
실제 상황에서는 무언가 복잡한 것을 처리해야 할 때 손으로 일일이 파서를 구현하지는 않는다. 물론 아주 단순한 데이터 포맷이라면 (예를 들어 XML이나 JOSN 같은) 직접 파서를 구현하는 일이 그리 어렵지 않다. 하지만 나만의 도메잍 그화 언어(DSL)나 프로그래밍 언어를 만들어야 한다면 바닥부터 구현하는 방식은 실행하기 어렵다.

Boost.Spirit은 파서를 만드는 데 도움을 주는 라이브러리이다. 그렇게 직관적이지는 않지만 파서를 생성할 수 있는 간결한 API를 제공한다. 이 라이브러리에서는 렉싱과 파싱 단계를 명시적으로 구분하지는 않는다. 대신 어떤 텍스트 구조가 내가 정의한 타입의 객체에 어떻게 매핑되는지 정의하는 방식이다.

### 추상 구문 트리
먼저, 추상 구문 트리를 정의해야 한다. 이를 위해 방문자 디자인 패턴을 지원하는 베이스 클래스를 만든다. 트리 구조의 순회가 핵심 작업이기 떄문에 방문자 패턴이 유용하다.

strcut ast_element
{
	virtual ~est_element() = default;
	virtual void accept(ast_element_visitor& visitor) = 0;
}

이 인터페이스는 "트론" 언어의 서로 다른 여러 코드 구성을 정의하는 데 사용된다. 예를 들어,

	struct property : ast_element
	{
		vector\<wstring\> names;
		type_specification type;
		bool is_constan{ false };
		wstring default_value;

		void accept(ast_element_vistor& visitor) override
		{
			visitor.visit(*this);
		}
	};

위 코드는 속성을 정의한다. 속성은 4가지 정보를 가지며 각각의 외부에서 접근 가능한 필드에 저장된다. 위 코드에 사용된 type_specification은 그자체가 또 하나의 ast_element이다.

BAST의 모든 클래스는 Boost.Fusion을 적용해야 한다. Boost.Fusion은 또 다른 Boost 라이브러리로 컴파일 타임 합성과 런타임 알고리즘을 지원한다. 아래와 같이 단순하게 적용할 수 있다.

	BOOST_FUSION_ADAPT_STRUCT(
		tlon::property,
		(std::vector\<std::wstring\>, names),
		(tlon::type_sepcification, type),
		(bool, is_constant),
		(std::wstring, default_value)
	);

Boost.Spirit은 std::vector나 std::optional과 같은 공용 데이터 타입을 파싱하는 데에는 아무런 문제가 없다. 하지만 다형성을 처리하는 데에는 조금 문제가 있다. 따라서 AST를 상속관계로 구성하기보다는 아래와 같이 variant를 사용하는 것이 Boost.Spirit을 사용하기에 유리하다.

	typedef varient<function_body, propery, function_signature> class_member;

### 파서
Boost.Spirit은 규칙의 집합으로서 파서를 정의할 수 있게 한다. 규칙을 정의하는 문법은 정규 표현식이나 BNF표기법과 유사하다. 단, 연산자가 심볼 뒤가 아니라 앞에 위치한다는 것만 다르다. "트론"을 위해 마음대로 정한 문법 규칙이다.

	class_declaration_rule %=
		lit(L"class ") >> +(alnum) % '.'
		>> -(lit(L"(") >> -parameter_declaration_Rule % ',' >> lit(")"))
		>> "{"
		>> "*(function_body_rule | property_rule | function_signature_rule)
		>> "}";

이 규칙은 클래스 선언이 "class"로 시작해야 함을 지정하고 있다. 그다음 하나 이상의 단어가 "."으로 구분되어(% 연산자의 여갛ㄹ) 붙을 수 있다. 각 단어는 하나 이상의 알파벳 또는 숫자로 구성될 수 있다(+(alnum) 부분). 그 결과는 이미 짐작했듯이 vector에 매핑된다. 연이어서, 중괄호 다음에 0개 이상의 함수 또는 속성 또는 시그니처의 정의가 올 수 있다. 이러한 것들이 매핑되는 필드들은 앞서 정의한 variant에 의해서 결정된다.

당연하게도 , 어떤 항목은 전체 AST 항목들의 계층에서 뿌리가 된다. 이예에서는 그러한 뿌리 항목을 파일이라 부른다. 아래는 파일을 파싱하는 그 결과를 보기 좋게 출력하는 함수이다.

	template<typename TLanguagePrinter, typename Iterator>
	wstring parse(Iterator first, Iterator last)
	{
		using spirit::qi::phrase_parse;

		file f;
		file_parser<wstring::const_iterator> fp{};
		auto b = phrase_parse(first, last, fp, space, f);
		if(b)
			return TLanguagePrinterP{.pretty_print(f);
		return wstring(L"FAIL");
	}

위 코드에서 TLanguagePrinter 타입은 방문자 패턴에서의 방문자 역할을 한다. TLanguagePRinter는 트론 언어를 C++와 같은 다른 언어로 변환하는데 사용될 수 있다.

### 프린터
언어를 파싱한 다음에는 컴파일을 할 수도 있지만 다른 언어로 변환할 수도 있다. 전체 AST 계층에 accept() 멤버 함수를 구현해 두었기 때문에 쉽게 할 수 있다.

유일하게 어려운 부분은 variant 타입을 어떻게 처리할 것인가이다. 가변 타입은 특별한 방문자를 필요로 한다. std::variant를 사용했다면 std::visit()가 호출된다. 하지만 이 예에서는 boost::variant를 사용했기 때문에 boost::accept_visitor()가 호출되고, 이 함수를 사용하려면 static_visitor를 상속받은 클래스를 넘겨주어야 한다. 넘겨줄 클래스는 모든 가능한 타입에 대해서 함수 호출 연산자가 오버로딩되어 있어야 한다. 아래는 이러한 클래스를 구현한 것이다.

	struct default_value_visitor : static_visitor<>
	{
		cpp_printer& printer;

		explicit default_value_visitor(cpp_printer& printer)
			: printer{ printer } {}

		void operator()(const basic_type& bt) const
		{
			printer.buffer << printer.default_value_for(bt.name);
		}

		void operator()(const tuple_signature& ts) const
		{
			for(auto& e : ts.elements)
			{
				this->operator()(e.type);
				printer.buffer << ", ";
			}
			printer.backtrack(2);
		}
	};

이렇게 정의한 다음 accept_visitor(foo, default_Value_Visitor{...})를 호출하면 variant에 저장된 객체의 타입에 맞추어 오버로딩된 함수가 올바르게 호출된다.

## 요약
사실 인터프리터 디자인 패턴은 그리 흔하게 사용되지는 않는다. 파서를 직접 만들어야 할 상황이 그렇게 많지 않다. 인터프리터는 컴퓨터 과학의 한 분야를 이루는 주제로 별개의 책으로 다루어도 부족하다.