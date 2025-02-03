#include <stdint.h>

namespace std
{
	template <typename T>
	class LinkedList
	{
	public:
		class Node
		{
		protected:
			Node* next = nullptr;
			Node* prev = nullptr;
			T	  value{};

		public:
			friend class LinkedList;

			Node() = default;
			Node(T newValue) : value(newValue), next(nullptr), prev(nullptr) {}

			const T& getValue() const { return value; }
			T&		 getValue() { return value; }

			const Node* getNext() const { return next; }
			Node*		getNext() { return next; }
			const Node* getPrev() const { return prev; }
			Node*		getPrev() { return prev; }
		};

	private:
		Node*	 m_head = nullptr;
		Node*	 m_tail = nullptr;
		uint32_t m_size = 0;

	public:
		LinkedList() = default;
		~LinkedList() { /* clear(); */ }

		uint32_t size() const { return m_size; }
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

		void addFront(const T& value) { addFront(new Node(value)); }
	};
} // namespace std