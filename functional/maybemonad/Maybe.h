#pragma once

template <typename T>
class Maybe
{
public:
	T* context;
	Maybe(T* context) : context{ context } {}

	template<typename Func>
	auto With(Func evaluator);

	template<typename TFunc>
	auto Do(TFunc action)
	{
		if (context != nullptr) action(context);
		return *this;
	}
};


template <typename T>
Maybe<T> maybe(T* context)
{
	return Maybe<T>(context);
}

template<typename T>
template<typename Func>
inline auto Maybe<T>::With(Func evaluator)
{
	return context != nullptr ? maybe(evaluator(context)) : nullptr;
}
