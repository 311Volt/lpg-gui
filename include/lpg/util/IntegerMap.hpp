/*

This header-only container is intended for use when assigning small
numerical IDs to objects. It is optimized for fast lookups and provides
automatic assignment of the smallest possible key when inserting.

Insertion - O(1)
Insertion with auto key - O(log n)
Deletion - O(log n) amortized
Lookup - O(1)
Memory - O(n) in the value of the largest key

*/

#ifndef LPG_INTEGER_MAP_H
#define LPG_INTEGER_MAP_H

#include <vector>
#include <queue>
#include <list>
#include <iterator>
#include <type_traits>
#include <stdexcept>
#include <cstdint>
#include <optional>
#include <queue>

namespace lpg {

	template<typename KeyT, typename ValueT>
	class IntegerMap {
		static_assert(std::is_integral<KeyT>::value);

		using ListT = std::list<ValueT>;
	public:

		class iterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type = int64_t;
			using value_type = std::pair<KeyT, ValueT>;
			using pointer = std::pair<KeyT, ValueT>*;
			using reference = std::pair<KeyT, ValueT>&;

			iterator(IntegerMap* im, KeyT idx)
				: im(im), idx(idx) {}
			
			value_type operator*() const
				{return {idx, im->at(idx)};}
			
			iterator& operator++()
			{
				do {
					idx++;
				} while(idx < im->size() && !im->contains(idx));
				return *this;
			}
			
			iterator operator++(int)
				{return ++iterator(*this);}

			friend bool operator==(const iterator& a, const iterator& b)
				{return a.im==b.im && a.idx==b.idx;}
			
			friend bool operator!=(const iterator& a, const iterator& b)
				{return a.im!=b.im || a.idx!=b.idx;}
			
			
		private:
			IntegerMap* im;
			KeyT idx;
		};

		IntegerMap() {}
		~IntegerMap() {}

		const ValueT& operator[](KeyT key) const
		{
			return *pVec[key];
		}

		const ValueT& at(KeyT key) const
		{
			checkState();
			auto ret = pVec.at(key);
			if(!ret) {
				throw std::out_of_range("no element found with this key");
			}
			return *ret;
		}

		bool contains(KeyT key) const
		{
			return (key < pVec.size() && pVec[key]);
		}

		ValueT* ptr(KeyT key) const noexcept
		{
			if(contains(key)) {
				return pVec[key];
			}
			return nullptr;
		}

	private:
		void free(KeyT key)
		{
			if(contains(key) && key < itVec.size() && pVec[key]) {
				li.erase(itVec[key]);
                                pVec[key] = nullptr;
			}
		}

		void resizeFor(KeyT key)
		{
			checkState();
			if(key >= pVec.size()) {
				pVec.resize(key+1, nullptr);
				itVec.resize(key+1);
			}
		}

		void reserve(KeyT key)
		{
			resizeFor(key);
			free(key);
		}

		KeyT regKeyValuePair(KeyT key, typename ListT::iterator it)
		{
			itVec[key] = it;
			pVec[key] = &(*it);
			return key;
		}
	public:

		KeyT insert(KeyT key, const ValueT& value)
		{
			reserve(key);
			return regKeyValuePair(key, li.insert(li.end(), value));
		}

		KeyT insert(const ValueT& value)
		{
			return insert(getLowestFreeKey(), value);
		}

		KeyT emplace(KeyT key, ValueT&& value)
		{
			reserve(key);
			return regKeyValuePair(key, li.emplace(li.end(), std::move(value)));
		}

		KeyT emplace(ValueT&& value)
		{
			return emplace(getLowestFreeKey(), std::move(value));
		}

		std::vector<KeyT> keys()
		{
			std::vector<KeyT> ret;
			for(KeyT i=0; i<size(); i++) {
				if(contains(i)) {
					ret.push_back(i);
				}
			}
			return ret;
		}

		KeyT size() const
		{
			return pVec.size();
		}

		iterator begin()
		{
			return iterator(this, 0);
		}

		iterator end()
		{
			return iterator(this, size());
		}

		void shrinkToFit()
		{
			checkState();
			using OffT = typename std::vector<ValueT*>::difference_type;
			for(OffT i=OffT(itVec.size())-1; i>=0 && !pVec[i]; i--) {
				pVec.pop_back();
				itVec.pop_back();
			}
		}

		void erase(KeyT key)
		{
			checkState();
			free(key);
			freeKeys.push(key);
			shrinkToFit();
		}

		void clear()
		{
			li.clear();
			itVec.clear();
			pVec.clear();
			freeKeys = {};
		}

		void checkState() const
		{
			if(pVec.size() != itVec.size()) {
				throw std::runtime_error("pVec and itVec have different sizes (this should never happen)");
			}
		}

		KeyT getLowestFreeKey()
		{
			KeyT ret = pVec.size();
			if(freeKeys.size()) {
				ret = freeKeys.top();
				freeKeys.pop();
			}
			return ret;
		}
	private:

		ListT li;
		std::vector<ValueT*> pVec;
		std::vector<typename ListT::iterator> itVec;

		std::priority_queue<KeyT, std::vector<KeyT>, std::greater<KeyT>> freeKeys;
	};

}

#endif // LPG_INTEGER_MAP_H
