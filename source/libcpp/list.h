#pragma once

#include <memory.h>

template <typename T>
class ListNode;
template <typename T>
class List;

template <typename T>
struct ListNode
{
	using ValueType = T;
	using SizeType = size_t;
	using ReferenceType = ValueType&;
	using ConstReferenceType = const ValueType&;
	using PointerType = ValueType*;
	using ConstPointerType = const ValueType*;

	ValueType value;
	ListNode* next;
	ListNode* prev;

public:
	ListNode() = default;
	ListNode(ValueType newValue) : value(newValue), next(nullptr), prev(nullptr) {}

	ReferenceType	   getValue() { return value; }
	ConstReferenceType getValue() const { return value; }
	PointerType		   get() { return &value; }
	ConstPointerType   get() const { return &value; }

	ListNode*		getNext() { return next; }
	const ListNode* getNext() const { return next; }
	ListNode*		getPrev() { return prev; }
	const ListNode* getPrev() const { return prev; }
};

template <typename T>
class List
{
	using ValueType = T;
	using SizeType = size_t;
	using ReferenceType = ValueType&;
	using ConstReferenceType = const ValueType&;
	using PointerType = ValueType*;
	using ConstPointerType = const ValueType*;

	using NodeType = ListNode<T>;
	NodeType* m_head;
	NodeType* m_tail;
	size_t	  m_size;

public:
	List() : m_head(nullptr), m_tail(nullptr), m_size(0) {}

	SizeType  size() const { return m_size; }
	bool	  empty() const { return m_size == 0; }
	NodeType* getFront() const { return m_head; }
	NodeType* getBack() const { return m_tail; }

	void addFront(NodeType* newNode)
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

	void addFront(ConstReferenceType value) { addFront(new NodeType(value)); }

	void addBack(NodeType* newNode)
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

	void addBack(const ValueType& value) { addBack(new NodeType(value)); }

	void clear()
	{
		NodeType* node;

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

	NodeType* at(SizeType index)
	{
		NodeType* current = m_head;
		for (SizeType i = 0; i < index; i++)
		{
			current = current->next;
		}
		return current;
	}

	void remove(NodeType* node)
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

	void removeFront()
	{
		if (m_head != nullptr)
		{
			remove(m_head);
		}
	}

	void removeBack()
	{
		if (m_tail != nullptr)
		{
			remove(m_tail);
		}
	}

	NodeType* find(ReferenceType value)
	{
		NodeType* node = m_head;
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

	NodeType* find(PointerType value)
	{
		NodeType* node = m_head;
		while (node != nullptr)
		{
			if (node->getValue() == *value)
			{
				break;
			}
			node = node->next;
		}
		return node;
	}

	bool contains(const ValueType& value) { return find(value) != nullptr; }

	void rotate(SizeType count)
	{
		if (count == 0 || count >= m_size || m_head == nullptr)
		{
			return;
		}

		for (SizeType i = 0; i < count; i++)
		{
			NodeType* temp = m_head;
			m_head = m_head->next;
			m_tail->next = temp;
			temp->prev = m_tail;
			m_tail = temp;
			m_tail->next = m_head;
			m_head->prev = m_tail;
		}
	}

	NodeType*		begin() { return getFront(); }
	const NodeType* begin() const { return getFront(); }
	NodeType*		end() { return getBack(); }
	const NodeType* end() const { return getBack(); }
};