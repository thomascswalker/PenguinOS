#pragma once

#include <cstdlib.h>

class ICallable
{
public:
	virtual ~ICallable() = default;
	virtual void invoke() = 0;
};

template <typename Callable, typename RetValue, typename... Args>
class CallableWrapper : public ICallable
{
public:
	CallableWrapper(Callable&& callable) : m_callable(std::forward<Callable>(callable)) {}
	RetValue invoke(Args... args) override { m_callable(std::forward<Args>(args)...); }

private:
	Callable m_callable;
};

template <typename RetValue, typename... Args>
class Function
{
	ICallable* m_callable;

public:
	// Default constructor
	Function() = default;

	// Constructor to wrap a callable object
	template <typename Callable>
	Function(Callable&& callable)
	{
		m_callable =
			new CallableWrapper<Callable, RetValue, Args...>(std::forward<Callable>(callable));
	}

	// Move constructor
	Function(Function&& other) noexcept = default;

	// Move assignment operator
	Function& operator=(Function&& other) noexcept = default;

	// Deleted copy constructor and copy assignment operator
	Function(const Function&) = delete;
	Function& operator=(const Function& other) = delete;

	// Invoke the stored callable object
	void operator()(Args... args)
	{
		if (m_callable)
		{
			m_callable->invoke(std::forward<Args>(args)...);
		}
	}
};
