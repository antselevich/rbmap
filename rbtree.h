#ifndef __RBTREE__
#define __RBTREE__

#include <iostream>
#include <string>
#include <xfunctional>
#include <memory>
#include <stdexcept>

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,3)

 #pragma warning(disable: 4127)

using namespace std;

template <class _rbTraits>
class const_rbtree_iterator;

template <class _rbTraits>
class rbtree_iterator;

template <class _rbTraits>
class rbtree // rank balanced tree
{

	public:

		typedef typename _rbTraits::key_type key_type;
		typedef typename _rbTraits::compare compare;
		typedef typename _rbTraits::value_type value_type;
		typedef typename _rbTraits::compare compare;
		typedef typename _rbTraits::allocator_type allocator_type;

		typedef typename allocator_type::size_type size_type;
		typedef typename allocator_type::size_type size_type;
		typedef typename allocator_type::difference_type difference_type;
		typedef typename allocator_type::pointer pointer;
		typedef typename allocator_type::const_pointer const_pointer;
		typedef typename allocator_type::reference reference;
		typedef typename allocator_type::const_reference const_reference;
		
	private:

		value_type *_data; // contents
		rbtree *_left; // left subtree
		rbtree *_right; // right subtree
		rbtree *_parent; // pointer at parent
		rbtree *_spec; // special nill node
		bool _isNil; // true if it is absent;
		char _rank; // rank of the node

		static void know_your_parents(rbtree *x)
		{
			if (x->is_nil())
				return;
			x->_left->_parent = x;
			x->_right->_parent = x;
		}

		void rotate_right() // rotate right at the left child of this
		{
			rbtree *x = this->_left;
			swap(x->_data, this->_data);
			swap(x->_rank, this->_rank);
			this->_left = x->_left;
			x->_left = x->_right;
			x->_right = this->_right;
			this->_right = x;
			know_your_parents(x);
			know_your_parents(this);
		}

		void rotate_left() // rotate left at the right child of this
		{
			rbtree *y = this->_right;
			swap(y->_data, this->_data);
			swap(y->_rank, this->_rank);
			this->_right = y->_right;
			y->_right = y->_left;
			y->_left = this->_left;
			this->_left = y;
			know_your_parents(y);
			know_your_parents(this);
		}

		static void rotate_right_at(rbtree *q)
		{
			q->parent()->rotate_right();
		}

		static void rotate_left_at(rbtree *q)
		{
			q->parent()->rotate_left();
		}

		void promote() // promote node
		{
			_rank = rank() + 1;
		}
		
		void demote() // demote node
		{
			_rank = rank() - 1;
		}

		rbtree *find_node(value_type value) // find a node
		{
			if (is_nil())
			{
				return 0;
			}
			key_type data_key = key(*_data);
			key_type value_key = key(value);
			if (compare()(data_key, value_key))
			{
				return right()->find_node(value);
			}
			else if (compare()(value_key, data_key))
			{
				return left()->find_node(value);
			}
			else
			{
				return this;
			}
		}

		static int rank_dif(rbtree *r)
		{
			if (r->parent() == NULL)
				return 0;
			return r->parent()->rank() - r->rank();
		}

		template<class _Valty>
			rbtree *insert_it (_Valty &&value) // insert element into the tree
		{
			if (is_nil())
			{
				_data = new value_type(value);
				_left = new rbtree();
				_right = new rbtree();
				_isNil = false;
				_parent = NULL;
				_rank = 0;
				return this;
			}
			else
			{
				key_type data_key = key(*_data);
				key_type value_key = key(value);
				if (compare()(data_key, value_key)) 
				{
					if (!right()->is_nil())
					{
						return right()->insert_it(value);
					}
					else
					{
						_right = new rbtree(value);
						_right->_parent = this;
						return _right;
					}
				}
				else if (compare()(value_key, data_key))
				{
					if (!left()->is_nil())
					{
						return left()->insert_it(value);
					}
					else
					{
						_left = new rbtree(value);
						_left->_parent = this;
						return _left;
					}
				}
				else
				{
					_data = new value_type(value);
					return this;
				}
			}
		}

		void rebalance_after_insertion() // Rebalance this
		{
			rbtree *current = this;
			rbtree *parent = current->parent();
			if (parent == NULL || parent->is_nil() || rank_dif(current) != 0) // Stop
			{
				return;
			}
			bool left = parent->left() == current; // True if inserted is a left child of parent
			
			rbtree *sibling = left ? parent->right() : parent->left(); // Sibling of inserted
			sibling->_parent = parent;
			while (rank_dif(sibling) == 1) // Promotion
			{
				parent->promote();
				current = parent;
				parent = current->parent();
				
				// TODO ѕроверить, возможно, не понадобитс€
				//-----------------------------------------
				if (parent == NULL || parent->is_nil() || rank_dif(current) != 0)
				{
					return;
				}
				//-----------------------------------------

				left = parent->left() == current;
				sibling = left ? parent->right() : parent->left();
				sibling->_parent = parent;
			}
			rbtree *t = left ? current->right() : current->left();
			t->_parent = current;
			if (rank_dif(t) == 2) // Rotation
			{
				if (left)
				{
					rotate_right_at(current);
					parent->right()->demote();
				}
				else
				{
					rotate_left_at(current);
					parent->left()->demote();
				}
			}
			else if (rank_dif(t) == 1) // Double rotation
			{
				if (left)
				{
					rotate_left_at(t);
					rotate_right_at(t->parent());
				}
				else
				{
					rotate_right_at(t);
					rotate_left_at(t->parent());
				}
				parent->promote();
				parent->left()->demote();
				parent->right()->demote();
			}	
		}

		rbtree *delete_it(value_type value) // delete element from the tree
		{
			if (is_nil())
				return 0;
			key_type data_key = key(*_data);
			key_type value_key = key(value);
			if (compare()(data_key, value_key))
			{
				return right()->delete_it(value);
			}
			else if (compare()(value_key, data_key))
			{
				return left()->delete_it(value);
			}
			else
			{
				bool leftNull = left()->is_nil();
				bool rightNull = right()->is_nil();
				if (leftNull && rightNull)
				{
					//delete(_left);
					//delete(_right);
					this->_isNil = true;
					//this->_rank = -1;
					//return this;
				}
				else if (leftNull)
				{
					_data = right()->data();
					_rank = right()->rank(); //TODO ???
					delete(_right);
					_right = new _mytree();
					right()->_parent = this;
					//return right();
				}
				else if (rightNull)
				{
					_data = left()->data();
					_rank = left()->rank(); //TODO ???
					delete(_left);
					_left = new _mytree();
					left()->_parent = this;
					//return left();
				}
				else
				{
					rbtree *another = right()->most_left();
					value_type another_value = *(another->data());
					_data = new value_type(another_value);
					//_rank = another->rank(); //TODO ???
					return another->delete_it(another_value);
				}
				return this;
			}
		}


		public:

		void erase_node (rbtree *_Nod)
		{
			bool leftNull = _Nod->left()->is_nil();
			bool rightNull = _Nod->right()->is_nil();
			if (leftNull && rightNull) {
				delete(_Nod->left()); 
				delete(_Nod->right()); 
				delete(_Nod->data());
				_Nod->_rank = -1;
				_Nod->_isNil = true;
			}
			else if (leftNull)
			{
				_Nod->_data = _Nod->right()->data();
				_Nod->_rank = _Nod->right()->rank(); //TODO ???
				delete(_Nod->_right);
				_Nod->_right = new _mytree();
				_Nod->right()->_parent = _Nod;
			}
			else if (rightNull)
			{
				_Nod->_data = _Nod->left()->data();
				_Nod->_rank = _Nod->left()->rank(); //TODO ???
				delete(_Nod->_left);
				_Nod->_left = new _mytree();
				_Nod->left()->_parent = _Nod;
			}
			else
			{
				rbtree *another = _Nod->right()->most_left();
				value_type another_value = *(another->data());
				_Nod->_data = new value_type(another_value);
				//_rank = another->rank(); //TODO ???
				
				erase_node(another);
				return;
				//_Nod = another;
			}

			_Nod->rebalance_after_deletion();
		}

		void erase(const key_type& _Keyval) // TODO size_type!
		{
			_Pairii _Place = equal_range(_Keyval);
			 erase(_Place.first, _Place.second);
		}

		rbtree_iterator<_rbTraits> erase(rbtree_iterator<_rbTraits> _Begin, rbtree_iterator<_rbTraits> _End) // delete elements [_Begin..._End] from the tree TODO const_iterator, not iterator
		{
			while (_Begin != _End && !_Begin.pointer()->is_nil())
			{
				erase(_Begin++);
			}
			return _Begin;
		}

		rbtree_iterator<_rbTraits> erase(rbtree_iterator<_rbTraits> _Place) // delete element from the tree TODO const_iterator, not iterator
		{
			rbtree *_Nod = _Place.pointer();
			if (_Nod->is_nil())
				_DEBUG_ERROR("map/set erase iterator outside range");
			erase_node(_Nod);
			return _Place; // TODO ++
		}

		void rebalance_after_deletion() // Rebalance at this
		{
			rbtree *deleted = this;
			rbtree *parent = deleted->parent();

			/*cout << "B! " << !(rank_dif(parent->left()) == 2 && rank_dif(parent->right()) == 2 && parent->rank() == 1) << endl;
			cout << "parent->left()->rank() : " << parent->left()->rank() << endl;
			cout << "parent->right()->rank() : " << parent->right()->rank() << endl;
			cout << "parent->rank() : " << parent->rank() << endl;

			cout << "rank_dif(parent->left()) == 2 : " << (rank_dif(parent->left()) == 2) << endl;
			cout << "rank_dif(parent->right()) == 2 : " << (rank_dif(parent->right()) == 2) << endl;
			cout << "parent->rank() == 1 : " << (parent->rank() == 1) << endl;
			*/
			if (parent == NULL || parent->is_nil()) // Stop
				return;
			if (rank_dif(deleted) != 3 && !(rank_dif(parent->left()) == 2 && rank_dif(parent->right()) == 2 && parent->rank() == 1)) // Stop
				return;
			bool left = parent->left() == deleted; // True if deleted is a left child of the parent

			rbtree *sibling = left ? parent->right() : parent->left();
						
			
			rbtree *t;
			rbtree *u;

			bool demotion = rank_dif(sibling) == 2;
			bool double_demotion;
			if (!sibling->is_nil())
			{
				t = left ? sibling->left() : sibling->right();
				u = left ? sibling->right() : sibling->left();
				double_demotion = rank_dif(t) == 2 && rank_dif(u) == 2;
			}
			else
			{
				double_demotion = false;
			}
			while (demotion || double_demotion) // Demotion, double demotion
			{
				if (demotion) // Demotion
				{
					parent->demote();
				}
				else // Double demotion
				{
					parent->demote();
					sibling->demote();
				}	
				deleted = parent;
				parent = deleted->parent();

				if (parent == NULL || parent->is_nil())
					return;
				if (rank_dif(deleted) != 3 && !(rank_dif(parent->left()) == 2 && rank_dif(parent->right()) == 2 && rank_dif(parent) == 1)) // Stop
					return;
				
				left = parent->left() == deleted;
				sibling = left ? parent->right() : parent->left();
				demotion = rank_dif(sibling) == 2;
				if (!sibling->is_nil())
				{
					t = left ? sibling->left() : sibling->right();
					u = left ? sibling->right() : sibling->left();
					double_demotion = rank_dif(t) == 2 && rank_dif(u) == 2;
				}
				else
				{
					double_demotion = false;
				}
				
			}

			if (rank_dif(u) == 1) // Rotation
			{
				if (left)
				{
					rotate_left_at(sibling);
					parent->left()->demote();
					if (parent->left()->right()->is_nil())
					{
						parent->left()->demote();
					}
				}
				else
				{
					rotate_right_at(sibling);
					parent->right()->demote();
					if (parent->right()->left()->is_nil())
					{
						parent->right()->demote();
					}
				}
				parent->promote();
			}
			else if (rank_dif(t) == 1 && rank_dif(u) == 2) // Double rotation
			{
				if (left)
				{
					rotate_right_at(t);
					rotate_left_at(sibling);
					parent->right()->demote();
					parent->left()->demote();
					parent->left()->demote();
				}
				else
				{
					rotate_left_at(t);
					rotate_right_at(sibling);
					parent->left()->demote();
					parent->right()->demote();
					parent->right()->demote();
				}
				parent->promote();
				parent->promote();
			}

		}

		rbtree *most_left()
		{
			if (left()->is_nil())
				return this;
			else
				return left()->most_left();
		}

		rbtree *most_right()
		{
			if (right()->is_nil())
				return this;
			else
				return right()->most_right();
		}


	
	public:		
		
		static key_type& key(value_type value)
		{
			return ((key_type&)_rbTraits::_Key(value));
		}
		typedef rbtree<_rbTraits> _mytree;

		typedef const_rbtree_iterator<_rbTraits> const_iterator;
		typedef rbtree_iterator<_rbTraits> iterator;
		typedef _STD reverse_iterator<iterator> reverse_iterator;
		typedef _STD reverse_iterator<const_iterator> const_reverse_iterator;

		typedef pair<iterator, bool> _Pairib;
		typedef pair<iterator, iterator> _Pairii;
		typedef pair<const_iterator, const_iterator> _Paircc;

		rbtree(value_type value) // constructor
		{
				_data = new value_type(value);
				_parent = NULL;
				_left = new rbtree();
				_right = new rbtree();
				_spec = new rbtree(true);
				_isNil = false;
				_rank = 0;
		}

		rbtree(/*const compare& _Parg, allocator_type _Al*/) // constructor of an empty tree
		{
			_isNil = true;
			_rank = -1;
			_parent = NULL;
			_spec = new rbtree(true);
		}

		rbtree(bool isSpec) // constructor of spec
		{
			if (isSpec)
			{
				_isNil = true;
				_rank = -1;
				_parent = NULL;
			}
		}

		rbtree(const rbtree &another) // copying constructor TODO comp!
		{
			_rank = another._rank;
			_parent = another._parent;
			_isNil = another._isNil;
			if (!_isNil)
			{
				_data = new value_type(*(another._data));
				if (another._right->is_nil())
				{
					_right = new rbtree();
				}
				else
				{
					_right = new rbtree(*(another._right));
				}

				if (another._left->is_nil())
				{
					_left = new rbtree();
				}
				else
				{
					_left = new rbtree(*(another._left));
				}
			}
		}

		rbtree operator=(const rbtree& other)
		{
			return new rbtree(other);
		}

		~rbtree() // destructor TODO написать
		{
			if (!is_nil())
			{
				delete(_left);
				delete(_right);
			}
		}

		value_type *data() { return _data; } // get contents
		rbtree *left() { return _left; } // get left subtree
		rbtree *right() { return _right; } // get right subtree

		bool is_nil() { return _isNil; }

		int rank() { // TODO убрать (хот€ бы в private)
			if (is_nil())
				return -1;
			return _rank; 
		}

		rbtree *parent() { return _parent; } // TODO убрать (хот€ бы в private)

		rbtree *spec() { return _spec; }

		iterator specit() { return iterator(*this, _spec); }

		const_iterator specit() const 
		{ 
			return const_iterator(*this, _spec); 
		}

		iterator find(const key_type& _Keyval)
		{	// find an element in mutable sequence that matches _Keyval
			iterator _Place = lower_bound(_Keyval);
			iterator _None = specit();
			key_type this_key = key(*(_Place.pointer()->data()));
			return (_Place == _None || compare()(_Keyval, this_key) ? _None : _Place);
		}

		const_iterator find(const key_type& _Keyval) const
		{	// find an element in mutable sequence that matches _Keyval
			const_iterator _Place = lower_bound(_Keyval);
			const_iterator _None = specit();
			key_type this_key = key(*(_Place.pointer()->data()));
			return (_Place == _None || compare()(_Keyval, this_key) ? _None : _Place);
		}

		template<class _Valty>
			void insert1 (_Valty &&value) // insert and rebalance
		{
			rbtree *current = insert_it(_STD forward<_Valty>(value)); // insert value and put the inserted node to *current
			current->rebalance_after_insertion(); // Rebalance at current		
		}

		void remove (value_type value) // delete and rebalance
		{
			rbtree *deleted = delete_it(value); // delete_elem the node and put the place from where it has been deleted to *deleted
			if (deleted != 0)
				deleted->rebalance_after_deletion(); // Rebalance at deleted
			else
			{
				//TODO  ак-то сообщить, что нет такого элемента
			}
		}

		bool sch(value_type value)
		{
			return (find_node(value) != 0);
		}

		void clear()
		{
			delete(_left);
			delete(_right);
			delete(_data);
			_isNil = true;
		}

		void _Assign_rv(_mytree& _Right)
		{	// assign by moving _Right
		if (this == &_Right)
			;
		/*else if (get_allocator() != _Right.get_allocator())
			_Xinvalid_argument("invalid map/set<T> move assign");*/ //TODO
		else
			{	// clear this and steal from _Right
			clear();
			/*this->_Swap_all(_Right);
			_Swap_adl(this->comp, _Right.comp);
			_STD swap(this->_Myhead, _Right._Myhead);
			_STD swap(this->_Mysize, _Right._Mysize);*/
			this = new rbtree(&_Right);
			}
		}

		void rbswap(_mytree& _Right)
		{	// exchange contents with movable _Right
			_Assign_rv(_STD forward<_mytree>(_Right));
		}

		// TODO	в итераторах провер€ть, не пусто ли дерево

		iterator begin()
		{	// return iterator for beginning of mutable sequence
			if (is_nil())
				return (iterator(*this, this));
			return (iterator(*this, this->most_left()));
		}

		const_iterator begin() const
		{
			return const_iterator(*this, this->most_left());
		}

		iterator end()
		{	// return iterator for end of mutable sequence
			return (iterator(*this, this->most_right()));
		}

		const_iterator end() const
		{
			return const_iterator(*this, this->most_right());
		}

		iterator nilend()
		{	// return iterator to special empty node
			return (iterator(*this, this->spec()));
		}

		const_iterator nilend() const
		{
			return const_iterator(*this, this->spec());
		}

		reverse_iterator rbegin()
		{	// return iterator for beginning of reversed mutable sequence
			return (reverse_iterator(end()));
		}

		const_reverse_iterator rbegin() const
		{	// return iterator for beginning of reversed nonmutable sequence
			return (const_reverse_iterator(end()));
		}

		reverse_iterator rend()
		{	// return iterator for end of reversed mutable sequence
			return (reverse_iterator(begin()));
		}

		const_reverse_iterator rend() const
		{	// return iterator for end of reversed nonmutable sequence
			return (const_reverse_iterator(begin()));
		}

		const_iterator cbegin() const
		{	// return iterator for beginning of nonmutable sequence
			return (((const _mytree *)this)->begin());
		}

		const_iterator cend() const
		{	// return iterator for end of nonmutable sequence
			return (((const _mytree *)this)->end());
		}

		const_reverse_iterator crbegin() const
		{	// return iterator for beginning of reversed nonmutable sequence
			return (((const  _mytree *)this)->rbegin());
		}

		const_reverse_iterator crend() const
		{	// return iterator for ebd of reversed nonmutable sequence
			return (((const  _mytree *)this)->rend());
		}

		const_iterator lower_bound(const key_type& _Keyval) const
		{ // Find least node not less then _Keyval
			return const_iterator(*this, this->_Lbound(_Keyval));;
		}

		iterator lower_bound(const key_type& _Keyval)
		{ // Find least node not less then _Keyval
			return iterator(*this, this->_Lbound(_Keyval));;
		}

		const_iterator upper_bound(const key_type& _Keyval) const
		{ // Find greatest node not bigger then _Keyval
			return const_iterator(*this, this->_Ubound(_Keyval));;
		}

		iterator upper_bound(const key_type& _Keyval)
		{ // Find greatest node not bigger then _Keyval
			return iterator(*this, this->_Ubound(_Keyval));;
		}

		_Pairii equal_range(const key_type& _Keyval)
		{	// find range equivalent to _Keyval in mutable tree
			return (_Eqrange(_Keyval));
		}

		_Paircc equal_range(const key_type& _Keyval) const
		{	// find range equivalent to _Keyval in nonmutable tree
			return (_Eqrange(_Keyval));
		}

		rbtree *_Lbound(const key_type& _Keyval)
		{
			_mytree *_Nod = this;
			_mytree *_Result = _spec;
			while (!_Nod->is_nil())
			{
				key_type _Dataval = key(*(_Nod->data()));

				if (compare()(_Dataval, _Keyval))
				{
					_Nod = _Nod->right();
				}
				else
				{
					_Result = _Nod;
					_Nod = _Nod->left();
				}
			}
			return _Result;
		}

		rbtree *_Lbound(const key_type& _Keyval) const
		{
			_mytree *_Nod = this;
			_mytree *_Result = _spec;
			while (!_Nod->is_nil())
			{
				key_type _Dataval = key(*(_Nod->data()));

				if (compare()(_Dataval, _Keyval))
				{
					_Nod = _Nod->right();
				}
				else
				{
					_Result = _Nod;
					_Nod = _Nod->left();
				}
			}
			return _Result;
		}

		rbtree *_Ubound(const key_type& _Keyval)
		{
			_mytree *_Nod = this;
			_mytree *_Result = _spec;
			while (!_Nod->is_nil())
			{
				key_type _Dataval = key(*(_Nod->data()));
				if (compare()(_Keyval, _Dataval))
				{
					_Nod = _Nod->left();
				}
				else
				{
					_Result = _Nod;
					_Nod = _Nod->right();
				}
			}
			return _Result;
		}

		rbtree *_Ubound(const key_type& _Keyval) const
		{
			_mytree *_Nod = this;
			_mytree *_Result = _spec;
			while (!_Nod->is_nil())
			{
				key_type _Dataval = key(*(_Nod->data()));
				if (compare()(_Keyval, _Dataval))
				{
					_Nod = _Nod->left();
				}
				else
				{
					_Result = _Nod;
					_Nod = _Nod->right();
				}
			}
			return _Result;
		}

		_Pairii _Eqrange(const key_type& _Keyval)
		{
			_mytree *_Start = _spec;
			_mytree *_End = _spec;
			_mytree *_Root = this;
			_mytree *_Nod = _Root;
			key_type _Dataval;

			while (!_Nod->is_nil())
			{
				_Dataval = key(*(_Nod->data()));
				if (compare()(_Dataval, _Keyval))
				{
					_Nod = _Nod->right();
				}
				else
				{
					if (_End->is_nil() && compare()(_Keyval, _Dataval))
					{
						_End = _Nod;
					}
					_Start = _Nod;
					_Nod = _Nod->left();
				}
			}

			_Nod = _End->is_nil() ? _Root : _End->left();

			while (!_Nod->is_nil())
			{
				_Dataval = key(*(_Nod->data()));
				if (compare()(_Keyval, _Dataval))
				{
					_End = _Nod;
					_Nod = _Nod->left();
				}
				else
				{
					_Nod = _Nod->right();
				}
			}

			iterator First = iterator(*this, _Start);
			iterator Last = iterator(*this, _End);

			return _Pairii(First, Last);

		}



		_Paircc _Eqrange(const key_type& _Keyval) const
		{
			_mytree *_Start = _spec;
			_mytree *_End = _spec;
			_mytree *_Root = this;
			_mytree *_Nod = _Root;
			key_type _Dataval;

			while (!_Nod->is_nil())
			{
				_Dataval = key(*(_Nod->data()));
				if (compare()(_Dataval, _Keyval))
				{
					_Nod = _Nod->right();
				}
				else
				{
					if (_End->is_nil() && compare()(_Keyval, _Dataval))
					{
						_End = _Nod;
					}
					_Start = _Nod;
					_Nod = _Nod->left();
				}
			}

			_Nod = _End->is_nil() ? _Root : _End->left();

			while (!_Nod->is_nil())
			{
				_Dataval = key(*(_Nod->data()));
				if (compare()(_Keyval, _Dataval))
				{
					_End = _Nod;
					_Nod = _Nod->left();
				}
				else
				{
					_Nod = _Nod->right();
				}
			}


			const_iterator First = const_iterator(*this, _Start);
			const_iterator Last = const_iterator(*this, _End);

			return _Paircc(First, Last);

		}

		_Pairib insert(const value_type& _Val)
		{	// try to insert node with value _Val, favoring right side
			return (insert(_Val, false));
		}

		_Pairib insert(const value_type& _Val, bool _Leftish)
		{	// try to insert node with value _Val, on left if _Leftish
			rbtree *_Trynode = this;
			rbtree *_Placenode = this;
			bool _Addleft = true;	// add to left of head if tree empty
			key_type plkey;
			key_type trkey;
			key_type valkey = key(_Val);
			while (!_Trynode->is_nil())
			{	// look for leaf to insert before (_Addleft) or after
				_Placenode = _Trynode;
				trkey = key(*(_Trynode->data()));
				if (_Leftish)
					_Addleft = compare()(trkey, valkey);	// favor left end
				else
					_Addleft = compare()(valkey, trkey);	// favor right end
				_Trynode = _Addleft ? _Trynode->left() : _Trynode->right();
			}
			iterator _Place = iterator(*this, _Placenode);
			if (!_Addleft)
				;	// need to test if insert after is okay
			else if (_Place == begin())
			{
				return (_Pairib(_Insert(true, _Placenode, _Val), true));
			}
			else
				--_Place;	// need to test if insert before is okay

			plkey = key(*(_Place.pointer()->data()));
			
			if (compare()(plkey, valkey))
			{
				return (_Pairib(_Insert(_Addleft, _Placenode, _Val), true));
			}
			else
			{
				return (_Pairib(_Place, false));
			}
		}

		iterator insert(const_iterator _Place, const value_type& _Val)
		{	// try to insert node with value _Val using _Place as a hint
			const_iterator _Next;
			key_type valkey = key(_Val);
			key_type plkey;
			key_type mrkey;
			bool _Leftish = false;	// assume nearest point is end of sequence
			if (is_nil())
				return (_Insert(true, this, _Val));	// empty tree
			
			plkey = key(*(_Place.pointer()->data()));
			key_type nekey = key(*(((_Next = _Place)-1).pointer()->data()));
			key_type bekey = key(*(_Next.pointer()->data()));
			if (_Place == begin())
			{	// insert at beginning if before first element
				if (compare()(valkey, plkey))
					return (_Insert(true, _Place._Mynode(), _Val));
			}
			else if (_Where == _spec)
			{	// insert at end if after last element
				rbtree *mr = most_right();
				mrkey = key(*(mr->data()));
				if (compare()(mrkey, valkey))
					return (_Insert(false, mr, _Val));
			}
			else if (compare()(valkey, plkey)
				&& compare()(nekey, valkey))
			{	// insert before _Where
				_Place--;
				if (_Next.pointer()->right()->is_nil())
					return (_Insert(false, _Next.pointer(), _Val));
				else
					return (_Insert(true, _Place.pointer(), _Val));
			}
			else if (compare()(plkey, valkey)
				&& (++(_Place) == spec()
					|| compare()(valkey, bekey)))
			{	// insert after _Where
				if (_Place.pointer()->right()->is_nil())
					return (_Insert(false, _Place.pointer(), _Val));
				else
					return (_Insert(true, _Next.pointer(), _Val));
			}

			return (insert(_Val, _Leftish).first);
		}

		_Pairib _Linsert(rbtree *_Node, bool _Leftish)
		{
			const value_type& _Val = _Node->data();
			rbtree *_Trynode = this;
			rbtree *_Placenode = this;
			key_type trkey;
			key_type valkey = key(_Val);
			key_type plkey;
			bool _Addleft = true;	// add to left of head if tree empty

			while (!_Trynode->is_nil())
			{	// look for leaf to insert before (_Addleft) or after
				_Placenode = _Trynode;
				trkey = key(*(_Trynode->data()));
				if (_Leftish)
					_Addleft = compare()(trkey, valkey);	// favor left end
				else
					_Addleft = compare()(valkey, trkey);	// favor right end
				_Trynode = _Addleft ? _Trynode->left() : _Trynode->right();
			}

			iterator _Place = iterator(*this, _Placenode);
			plkey = key(*(_Place.pointer()->data()));
			if (!_Addleft)
				;	// need to test if insert after is okay
			else if (_Place == begin())
				return (_Pairib(_Insert(true, _Placenode, _Node), true));
			else
				--_Place;	// need to test if insert before is okay

			if (!_Place.pointer()->is_nil() && compare()(plkey, valkey))
				return (_Pairib(_Insert(_Addleft, _Placenode, _Node), true));
			else
			{	// duplicate, don't insert
				// TODO delete it
				return (_Pairib(_Place, false));
			}
		}

		iterator _Insert(const_iterator _Place, rbtree *_Node)
		{	// try to insert node at _Node using _Place as a hint
			const value_type& _Val = _Node->data();
			const_iterator _Next;
			bool _Leftish = false;
			key_type valkey = key(_Val);
			key_type plkey = key(*(_Place.pointer()->data()));
			key_type mrkey;
			if (_Place == begin())
			{	// insert at beginning if before first element
				if compare()(valkey, plkey)
					return (_Insert(true, _Place.pointer(), _Node));
			}
			else if (_Place == _spec)
			{	// insert at end if after last element
				rbtree *_mr = most_right();
				mrkey = key(*(_mr->data()));
				if (compare()(mrkey, valkey))
					return (_Insert(false, _mr, _Node));
			}
			
			else if (compare()(valkey, key(*(_Place.pointer()->data())))
				&& compare()(key(*((--(_Next = _Place)).pointer()->data())), key(_Val)))
			{	// insert before _Where
				if (_Next.pointer()->right()->is_nil())
					return (_Insert(false, _Next.pointer(), _Node));
				else
					return (_Insert(true, _Place.pointer(), _Node));
			}
			
			else if (compare()(plkey, val)
				&& (++(_Next = _Place) == _spec || compare()(valkey, plkey)))
			{	// insert after _Where
				if (_Place.pointer()->right()->is_nil())
					return (_Insert(false, _Place.pointer(), _Node));
				else
					return (_Insert(true, _Next._pointer(), _Node));
			}
			
			return (_Linsert(_Node, _Leftish).first);	// try usual insert
		}

		iterator _Insert(bool _Addleft,  rbtree *_Placenode, const value_type& _Val)
		{	// add value next to _Placenode, to left if _Addleft
			rbtree *_Newnode = new rbtree();
			_Newnode->_data = new value_type(_Val);
			_Newnode->_left = new rbtree();
			_Newnode->_right = new rbtree();
			return (_Insert(_Addleft, _Placenode, _Newnode));
		}

		iterator _Insert(bool _Addleft, rbtree *_Placenode, rbtree *_Newnode)
		{
			//TODO проверить, умещаетс€ ли в пам€ти!
			_Newnode->_parent = _Placenode;
			if (_Placenode->is_nil())
			{
				_Placenode->_rank = 0;
				_Placenode->_isNil = false;
				_Placenode->_parent = NULL;
				_Placenode->_data = new value_type(*(_Newnode->_data));
				if (_Newnode->_right->is_nil())
				{
					_Placenode->_right = new rbtree();
				}
				else
				{
					_Placenode->_right = new rbtree(*(_Newnode->_right));
				}

				if (_Newnode->_left->is_nil())
				{
					_Placenode->_left = new rbtree();
				}
				else
				{
					_Placenode->_left = new rbtree(*(_Newnode->_left));
				}
				_Newnode = _Placenode;
			}
			else if (_Addleft)
			{
				_Placenode->_left->_isNil = false;
				_Placenode->_left->_rank = 0;
				_Placenode->_left->_data = new value_type(*(_Newnode->data()));
				if (_Newnode->_right->is_nil())
				{
					_Placenode->_left->_right = new rbtree();
				}
				else
				{
					_Placenode->_left->_right = new rbtree(*(_Newnode->_right));
				}

				if (_Newnode->_left->is_nil())
				{
					_Placenode->_left->_left = new rbtree();
				}
				else
				{
					_Placenode->_left->_left = new rbtree(*(_Newnode->_left));
				}
				_Placenode->_left->_parent = _Placenode;
				_Newnode = _Placenode->_left;
			}
			else
			{
				_Placenode->_right->_isNil = false;
				_Placenode->_right->_rank = 0;
				_Placenode->_right->_data = new value_type(*(_Newnode->data()));
				if (_Newnode->_right->is_nil())
				{
					_Placenode->_right->_right = new rbtree();
				}
				else
				{
					_Placenode->_right->_right = new rbtree(*(_Newnode->_right));
				}

				if (_Newnode->_left->is_nil())
				{
					_Placenode->_right->_left = new rbtree();
				}
				else
				{
					_Placenode->_right->_left = new rbtree(*(_Newnode->_left));
				}
				_Placenode->_right->_parent = _Placenode;
				_Newnode = _Placenode->_right;
			}
			_Newnode->rebalance_after_insertion();
			return (iterator(*this, _Newnode));
		}

		template<class _Iter> 
			void insert(_Iter _First, _Iter _Last)
		{	// insert [_First, _Last) one at a time
			for (; _First != _Last; ++_First)
			{	// insert element as lvalue
				const value_type& _Val = *_First;
				insert(end(), _Val); // TODO wrong end!
			}
		}

		int height()
		{
			if (is_nil())
				return 0;
			else
				return max(left()->height(), right()->height()) + 1;
		}
		friend class const_rbtree_iterator<_rbTraits>;
		friend class rbtree_iterator<_rbTraits>;
};

template <class _rbTraits>
class const_rbtree_iterator
{
	friend class rbtree<_rbTraits>;
public:
	
	typedef typename _rbTraits::key_type key_type;
	typedef typename _rbTraits::value_type value_type;
	typedef typename _rbTraits::compare compare;

	rbtree<_rbTraits> *_tree;
	rbtree<_rbTraits> *_pointer;

	const_rbtree_iterator(rbtree<_rbTraits> & tree, rbtree<_rbTraits> *pointer)
      : _tree(&tree), _pointer(pointer)
    { }

	const_rbtree_iterator()
    { }

	key_type key(value_type value)
	{
		return rbtree<_rbTraits>::key(value);
	}

	typedef const_rbtree_iterator<_rbTraits> _myconstiter;

	rbtree<_rbTraits> *pointer()
	{
		return _pointer;
	}

	value_type operator *()
    {
		// TODO ѕровер€ть, не пусто ли дерево
		return *(_pointer->data());
	}

	bool operator==(const _myconstiter& other)
    {
		return _pointer == other._pointer;
    }

	bool operator!=(const _myconstiter& other)
    {
		return _pointer != other._pointer;
    }

	/*bool operator<(const _myconstiter& other)
    {
		return compare()(key(_pointer->data()), key(other._pointer->data()));
    }

	bool operator>(const _myconstiter& other)
    {
		return compare()(key(other._pointer->data()), key(_pointer->data()));
    }*/

	_myconstiter operator=(const _myconstiter& other)
    {
      _tree = other._tree;
      _pointer = other._pointer;
      return (*this);
    }

	_myconstiter& operator++()
    { // preincrement
		if (!_pointer->is_nil() && !_pointer->right()->is_nil())
		{
			_pointer = _pointer->right()->most_left();
		}
		else
		{
			rbtree <_rbTraits> *_ptr = _pointer;
			bool left_child = (!_ptr->is_nil()) && (_ptr->parent()) && (_ptr == _ptr->parent()->left());
			bool right_isnil = !left_child || (!_ptr->is_nil() && _ptr->parent()->right()->is_nil());
			while (!_ptr->is_nil() && _ptr->parent() && !left_child && right_isnil)
			{
				_ptr = _ptr->parent();
				if (!_ptr->parent())
				{
					left_child = false;
					right_isnil = true;
					break;
				}
				left_child = _ptr == _ptr->parent()->left();
				right_isnil = !left_child || _ptr->parent()->right()->is_nil();
			}
			if (left_child)
			{
				_pointer = _ptr->parent();
			}
			else if (!right_isnil)
			{
				_pointer = _ptr->parent()->right()->most_left();
			}
		}
		return *this;
    }

	_myconstiter operator++(int)
	{ // postincrement
		_myconstiter _tmp = *this;
		++*this;
		return _tmp;
	}

	_myconstiter& operator--()
    { // predecrement
		if (!_pointer->is_nil() && !_pointer->left()->is_nil())
		{
			_pointer = _pointer->left()->most_right();
		}
		else
		{
			rbtree <_rbTraits> *_ptr = _pointer;
			bool right_child = !_ptr->is_nil() && (_ptr == _ptr->parent()->right());
			bool left_isnil = !right_child || (!_ptr->is_nil() && _ptr->parent()->left()->is_nil());
			while (!_ptr->is_nil() && _ptr->parent() && !_ptr->parent()->is_nil() && !right_child && left_isnil)
			{
				_ptr = _ptr->parent();
				if (!_ptr->parent() || _ptr->parent()->is_nil())
				{
					right_child = false;
					left_isnil = true;
					break;
				}
				right_child = _ptr == _ptr->parent()->right();
				left_isnil = !right_child || _ptr->parent()->left()->is_nil();
			}
			if (right_child)
			{
				_pointer = _ptr->parent();
			}
			else if (!left_isnil)
			{
				_pointer = _ptr->parent()->left()->most_right();
			} 
		}
		return *this;
    }

	_myconstiter operator--(int)
	{ // postdecrement
		_myconstiter _tmp = *this;
		--*this;
		return _tmp;
	}

	_myconstiter operator+(int delta)
    {
		_myconstiter _tmp = *this;
		for (int i = 0; i < delta; i++)
			++_tmp;
		return _tmp;
    }

	_myconstiter operator-(int delta)
    {
		_myconstiter _tmp = *this;
		for (int i = 0; i < delta; i++)
			--_tmp;
		return _tmp;
    }

	_myconstiter operator+=(int delta)
    {
		for (int i = 0; i < delta; i++)
			++*this;
		return *this;
    }

	_myconstiter operator-=(int delta)
    {
		for (int i = 0; i < delta; i++)
			--*this;
		return *this;
    }

};

template <class _rbTraits>
class rbtree_iterator : public const_rbtree_iterator<_rbTraits> // TODO reference, pointer, etc.
{
public:
		typedef typename _rbTraits::key_type key_type;
		typedef typename _rbTraits::value_type value_type;
		typedef typename _rbTraits::compare compare;
		typedef rbtree_iterator<_rbTraits> _myiter;
		typedef const_rbtree_iterator<_rbTraits> _mybase;

		rbtree_iterator()
		{ }

		rbtree_iterator(rbtree<_rbTraits> & tree, rbtree<_rbTraits> *pointer)
			: _mybase(tree, pointer)
		{ }

		rbtree<_rbTraits> *pointer()
		{
			return _mybase::pointer();
		}

		value_type operator *()
		{
			// TODO ѕровер€ть, не пусто ли дерево
			return *(_pointer->data());
		}

		_myiter& operator++()
		{	// preincrement
			++(*(_mybase *)this);
			return (*this);
		}

		_myiter operator++(int)
		{	// postincrement
			_myiter _Tmp = *this;
			++*this;
			return (_Tmp);
		}

		_myiter& operator--()
		{	// predecrement
			--(*(_mybase *)this);
			return (*this);
		}

		_myiter operator--(int)
		{	// postdecrement
			_myiter _Tmp = *this;
			--*this;
			return (_Tmp);
		}

};


 #pragma warning(pop)
 #pragma pack(pop)

#endif //__RBTREE__