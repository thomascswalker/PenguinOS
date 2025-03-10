#pragma once

#include <compare.h>
#include <cstdlib.h>
#include <pair.h>

template <typename KeyType, typename T, typename Compare = std::Less<KeyType>>
class Map

{
public:
	using ValueType = Pair<KeyType, T>;
	using SizeType = size_t;

private:
	struct Node
	{
		ValueType data;
		Node*	  left;
		Node*	  right;
		Node*	  parent;

		Node(const ValueType& d) //
			: data(d), left(nullptr), right(nullptr), parent(nullptr)
		{
		}
	};

	enum Color
	{
		Red,
		Black
	};

	Node*	 m_root;
	SizeType m_size;
	Compare	 m_compare;

	void clear(Node* node)
	{
		if (!node)
		{
			return;
		}
		clear(node->left);
		clear(node->right);
		node->~Node();
		std::free(node);
	}

	Node* minimum(Node* n) const
	{
		while (n && n->left)
		{
			n = n->left;
		}
		return n;
	}

public:
	Map() : m_root(nullptr) {}
	~Map() { clear(m_root); }

	SizeType size() const { return m_size; }

	Pair<ValueType*, bool> insert(const ValueType& value)
	{
		Node* y = nullptr;
		Node* x = m_root;

		while (x)
		{
			y = x;
			if (m_compare(value.a, x->data.a))
			{
				x = x->left;
			}
			else if (m_compare(x->data.a, value.a))
			{
				x = x->right;
			}
			else
			{
				return Pair(&x->data, false); // Key already exists
			}
		}

		Node* z = static_cast<Node*>(std::malloc(sizeof(Node)));
		if (!z)
		{
			// Out of memory
			return Pair(nullptr, false);
		}

		// Placement new
		new (z) Node(value);
		z->parent = y;
		if (!y)
		{
			m_root = z;
		}
		else if (m_compare(z->data.a, y->data.a))
		{
			y->left = z;
		}
		else
		{
			y->right = z;
		}
		// Fixup
		m_size++;
		return Pair(&z->data, true);
	}
};