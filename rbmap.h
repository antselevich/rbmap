#pragma once
#ifndef _RBMAP_
#define _RBMAP_
#include <map>
#include "rbtree.h"

_STD_BEGIN

template<
	class _Key_type, 
	class _Mapped_type, 
	class _Comparator = less<_Key_type>,
	class _Alloc = allocator<pair<const _Key_type, _Mapped_type> > >
class _rbTraits
{
public:
	//typedef pair<_Key_type, _Mapped_type> _Value_type;
	typedef pair<const _Key_type, _Mapped_type> value_type;
	typedef _Key_type key_type;
	typedef _Comparator compare;

	typedef typename _Alloc::template rebind<value_type>::other
		allocator_type;

	_rbTraits() 
		: comp()
	{
	}

	_rbTraits(compare ctor)
		: comp(ctor)
	{
	}

	template<class _Ty1, class _Ty2>
		static const key_type& _Key(const _STD pair<_Ty1, _Ty2>& _Val)
		{	// extract key from element value
		return (_Val.first);
		}

	_Comparator comp;

	class value_compare
		: public binary_function<value_type, value_type, bool>
		{
		friend class _rbTraits<_Key_type, _Mapped_type, _Comparator, _Alloc>;

	public:
		bool operator()(const value_type& _Left,
			const value_type& _Right) const
			{	// test if _Left precedes _Right by comparing just keys
			return (comp(_Left.first, _Right.first));
			}

		value_compare(compare _Pred)
			: comp(_Pred)
			{	// construct with specified predicate
			}


	protected:
		compare comp;	// the comparator predicate for keys
		};
	
};

template<class _Key_type, 
	class _Mapped_type, 
	class _Comparator = less<_Key_type>,
	class _Alloc = allocator<pair<const _Key_type, _Mapped_type> > >
class rbmap
		: public rbtree<_rbTraits<_Key_type, _Mapped_type, _Comparator> >
	{
		
	public:
	
		typedef rbmap<_Key_type, _Mapped_type, _Comparator, _Alloc> _My_map;
		typedef rbtree<_rbTraits<_Key_type, _Mapped_type, _Comparator> > _My_tree;
		typedef _Key_type key_type;
		typedef _Mapped_type mapped_type;
		typedef _Comparator key_compare;
		typedef typename _My_tree::compare compare;
		//typedef typename _My_tree::allocator_type allocator_type;
		//typedef typename _Mybase::size_type size_type;
	
		typedef typename _My_tree::iterator iterator;
		typedef typename _My_tree::const_iterator const_iterator;
		typedef typename _My_tree::reverse_iterator reverse_iterator;
		typedef typename _My_tree::const_reverse_iterator const_reverse_iterator;
		typedef typename _My_tree::value_type value_type;


		rbmap()
		: _My_tree(/*key_compare(), allocator_type()*/)
		{	// construct empty map from defaults
		}

		rbmap(const _My_tree& _Right)
		: _My_tree(_Right)
		{	// construct map by copying _Right
		}

	template<class _Iter>
		rbmap(_Iter _First, _Iter _Last)
		: _My_tree(/*key_compare(), allocator_type()*/)
		{	// construct map from [_First, _Last), defaults
		this->insert(_First, _Last);
		}

	/*template<class _Iter>
		rbmap(_Iter _First, _Iter _Last, const allocator_type& _Al)
		: _Mybase(_Al)
		{	// construct map from [_First, _Last), comparator, and allocator
		this->insert(_First, _Last);
		}*/

		_My_map& operator=(const _My_map& _Right)
		{	// assign by copying _Right
		_My_tree::operator=(_Right);
		return (*this);
		}

		rbmap(_My_map&& _Right)
		: _My_tree(_STD move(_Right))
		{	// construct map by moving _Right
		}

		_My_map& operator=(_My_map&& _Right)
		{	// assign by moving _Right
		_My_tree::operator=(_STD move(_Right));
		return (*this);
		}

		/*mapped_type &operator[](const key_type& _Keyval)
		{	// find element matching _Keyval or insert with default mapped
		iterator _Where = this->lower_bound(_Keyval);
		if (_Where == nilend()
			|| compare()(_Keyval, this->key(*_Where)))
			_Where = this->insert(_Where,
				value_type(_Keyval, mapped_type()));
		return ((*_Where).second);
		}*/

		mapped_type at(const key_type& _Keyval) // TODO &at, not at
		{	// find element matching _Keyval
			iterator _Where = this->lower_bound(_Keyval);
			key_type _Dataval =  this->key(*_Where);
			if (_Where == nilend() || compare()(_Keyval, _Dataval))
				_Xout_of_range("invalid map<K, T> key");
			return ((*_Where).second);
		}

	/*const mapped_type& at(const key_type& _Keyval) const
		{	// find element matching _Keyval
		const_iterator _Where = this->lower_bound(_Keyval);
		if (_Where == this->end()
			|| this->comp(_Keyval, this->_Key(_Where._Mynode())))
			_Xout_of_range("invalid map<K, T> key");
		return ((*_Where).second);
		}*/

	void rbswap(_My_map& _Right)
		{	// exchange contents with non-movable _Right
		_My_tree::rbswap(_Right);
		}

	void rbswap(_My_map&& _Right)
		{	// exchange contents with movable _Right
		_My_tree::rbswap(_STD move(_Right));
		}

	int height()
	{
		return _My_tree::height();
	}
		
	};

template<class _Key_type,
	class _Mapped_type,
	class _Comparator,
	class _Alloc> inline
	void swap(rbmap<_Key_type, _Mapped_type, _Comparator, _Alloc>& _Left,
		rbmap<_Key_type, _Mapped_type, _Comparator, _Alloc>& _Right)
	{	// swap _Left and _Right maps
	_Left.rbswap(_Right);
	}

template<class _Key_type,
	class _Mapped_type,
	class _Comparator,
	class _Alloc> inline
void swap(rbmap<_Key_type, _Mapped_type, _Comparator, _Alloc>& _Left,
		rbmap<_Key_type, _Mapped_type, _Comparator, _Alloc>&& _Right)
	{	// swap _Left and _Right maps
	typedef rbmap<_Key_type, _Mapped_type, _Comparator, _Alloc> _My_map;
	_Left.rbswap(_STD forward<_My_map>(_Right));
	}


template<class _Key_type,
	class _Mapped_type,
	class _Comparator,
	class _Alloc> inline
	void swap(rbmap<_Key_type, _Mapped_type, _Comparator, _Alloc>&& _Left,
		rbmap<_Key_type, _Mapped_type, _Comparator, _Alloc>& _Right)
	{	// swap _Left and _Right maps
	typedef rbmap<_Key_type, _Mapped_type, _Comparator, _Alloc> _My_map;
	_Right.rbswap(_STD forward<_My_map>(_Left));
	}


_STD_END

#endif // _RBMAP_