#pragma once

template <typename T>
struct QueueNode
{
	using ValueType = T;
	ValueType  value;
	QueueNode* next;
	QueueNode(ValueType data) : value(data), next(nullptr) {}
};

template <typename T>
class Queue
{
	using ValueType = T;
	using ConstValueType = const T;
	using RefType = T&;
	using ConstRefType = const T&;
	using PointerType = T*;
	using ConstPointerType = const T*;

	using NodeType = QueueNode<T>;

	NodeType* m_head;
	NodeType* m_tail;

public:
	Queue() : m_head(nullptr), m_tail(nullptr) {}

	void enqueue(ConstValueType data)
	{
		NodeType* newNode = new NodeType(data);
		if (m_tail)
		{
			m_tail->next = newNode;
		}
		else
		{
			m_head = newNode;
		}
		m_tail = newNode;
	}

	void dequeue()
	{
		if (empty())
		{
			return;
		}

		NodeType* temp = m_head;
		m_head = m_head->next;
		if (!m_head)
		{
			m_tail = nullptr;
		}
		delete temp;
	}

	PointerType front() const { return m_head ? &m_head->value : nullptr; }
	PointerType back() const { return m_tail ? &m_tail->value : nullptr; }

	bool empty() const { return m_head == nullptr; }
};
