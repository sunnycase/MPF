//
// Tomato
// Free List
// 
// (c) SunnyCase 
// 创建日期 2016-08-28
#include "stdafx.h"
#include "../inc/FreeList.h"
#include <algorithm>

using namespace NS_PLATFORM;

FreeList::FreeList(size_t initialSize)
	:_capacity(initialSize)
{
	if (initialSize)
		_freeEntries.emplace_back<FreeEntry>({ 0, initialSize });
}

bool FreeList::TryAllocate(size_t count, size_t& offset)
{
	for (auto it = _freeEntries.begin(); it != _freeEntries.end(); ++it)
	{
		if (it->length >= count)
		{
			offset = it->offset;
			if (it->length == count)
				it = _freeEntries.erase(it);
			else
			{
				it->offset += count;
				it->length -= count;
			}
			return true;
		}
	}
	return false;
}

void FreeList::Enlarge(size_t amount)
{
	if (!amount)return;
	if (!_freeEntries.empty())
	{
		auto& back = _freeEntries.back();
		if (back.offset + back.length == _capacity)
		{
			back.length += amount;
			return;
		}
	}
	_freeEntries.emplace_back<FreeEntry>({ _capacity, amount });
	_capacity += amount;
}

void FreeList::Retire(size_t offset, size_t length)
{
	if (_freeEntries.empty())
		_freeEntries.emplace_front<FreeEntry>({ offset, length });
	else
	{
		auto it = std::find_if(_freeEntries.begin(), _freeEntries.end(), [offset](const FreeEntry& entry) {return entry.offset > offset; });
		CombineFreeNode(_freeEntries.insert(it, { offset, length }));
	}
}

void FreeList::CombineFreeNode(typename std::list<FreeEntry>::iterator it)
{
	assert(it != _freeEntries.end());
	if (it != _freeEntries.begin())
	{
		auto left = std::prev(it);
		if (left->offset + left->length == it->offset)
		{
			left->length += it->length;
			auto next = _freeEntries.erase(it);
			if (next != _freeEntries.end())
				CombineFreeNode(next);
			return;
		}
	}
	auto right = std::next(it);
	if (right != _freeEntries.end())
	{
		if (it->offset + it->length == right->offset)
		{
			it->length += right->length;
			_freeEntries.erase(right);
		}
	}
}