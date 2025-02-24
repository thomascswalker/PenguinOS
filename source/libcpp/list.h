#pragma once

#include <memory.h>

template <typename T>
class List
{
	using ValueType = T;
	using SizeType = size_t;
	using ReferenceType = ValueType&;
	using ConstReferenceType = const ValueType&;
	using PointerType = T*;
	using ConstPointerType = const T*;

	struct Node
	{
	protected:
		ValueType value;
		Node*	  next;
		Node*	  prev;

	public:
		friend class List;

		Node() = default;
		Node(ValueType newValue) : value(newValue), next(nullptr), prev(nullptr) {}

		ReferenceType	   getValue() { return value; }
		ConstReferenceType getValue() const { return value; }

		Node*		getNext() { return next; }
		const Node* getNext() const { return next; }
		Node*		getPrev() { return prev; }
		const Node* getPrev() const { return prev; }
	};

	Node*  m_head;
	Node*  m_tail;
	size_t m_size;

public:
	List() : m_head(nullptr), m_tail(nullptr), m_size(0) {}

	SizeType size() const { return m_size; }
	bool	 isEmpty() const { return m_size == 0; }
	Node*	 getFront() const { return m_head; }
	Node*	 getBack() const { return m_tail; }

	void addFront(Node* newNode)
	{
		if (m_head != nullptr)
		{
			newNode->next = m_head;
			m_head->prev = newNode;
			m_head = newNode;
			m_tail->next = m_head;
		}
		else
		{
			m_head = m_tail = newNode;
		}

		m_size++;
	}

	void addFront(ConstReferenceType value) { addFront(new Node(value)); }

	void addBack(Node* newNode)
	{
		if (m_tail != nullptr)
		{
			m_tail->next = newNode;
			newNode->prev = m_tail;
			newNode->next = m_head;
			m_tail = newNode;
			m_head->prev = m_tail;
		}
		else
		{
			m_head = m_tail = newNode;
		}

		m_size++;
	}

	void addBack(const T& value) { addBack(new Node(value)); }

	void clear()
	{
		Node* node;

		while (m_head != nullptr && m_size > 0)
		{
			node = m_head->next;
			delete m_head;
			m_head = node;
			m_size--;
		}

		m_head = nullptr;
		m_tail = nullptr;
	}

	Node* at(SizeType index)
	{
		Node* current = m_head;
		for (SizeType i = 0; i < index; i++)
		{
			current = current->next;
		}
		return current;
	}

	void remove(Node* node)
	{
		if (node == nullptr)
		{
			return;
		}

		if (m_size == 1)
		{
			clear();
			return;
		}

		if (node == m_head)
		{
			m_head = m_head->next;
			m_head->prev = m_tail;
			m_tail->next = m_head;
		}
		else if (node == m_tail)
		{
			m_tail = m_tail->prev;
			m_tail->next = m_head;
			m_head->prev = m_tail;
		}
		else
		{
			node->next->prev = node->prev;
			node->prev->next = node->next;
		}

		delete node;
		m_size--;
	}

	Node* find(const T& value)
	{
		Node* node = m_head;
		while (node != nullptr)
		{
			if (node->getValue() == value)
			{
				break;
			}
			node = node->next;
		}
		return node;
	}

	bool contains(const T& value) { return find(value) != nullptr; }

	Node*		begin() { return getFront(); }
	const Node* begin() const { return getFront(); }
	Node*		end() { return getBack(); }
	const Node* end() const { return getBack(); }
};