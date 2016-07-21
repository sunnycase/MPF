//
// MPF Platform
// Resource Container
// 作者：SunnyCase
// 创建时间：2016-07-20
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"
#include <vector>
#include <list>
#include <algorithm>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct ResourceBase
{
	void SetUsed()
	{
		Used = true;
	}

	void SetUnused()
	{
		Used = false;
	}

	bool Used = false;
};

template<class T>
class ResourceContainer : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IResourceContainer>
{
	struct FreeListEntry
	{
		size_t start;
		size_t length;
	};
public:
	ResourceContainer(size_t capacity = 231)
	{
		_data.resize(capacity);
		_freeList.emplace_front<FreeListEntry>({ 0, _data.size() });
	}

	UINT_PTR Allocate()
	{
		if (_freeList.empty()) Enlarge();
		assert(!_freeList.empty());
		auto& front = _freeList.front();
		assert(front.length);
		auto handle = front.start++;
		if (!--front.length)
			_freeList.erase(_freeList.begin());
		_data[handle].SetUsed();
		return handle;
	}

	void Enlarge()
	{
		const auto oldCapacity = _data.size();
		const auto newCapacity = oldCapacity * 2;
		_data.resize(newCapacity);
		if (!_freeList.empty())
		{
			auto& back = _freeList.back();
			if (back.start + back.length == _data.size())
			{
				back.length += newCapacity - oldCapacity;
				return;
			}
		}
		_freeList.emplace_back<FreeListEntry>({ oldCapacity, newCapacity - oldCapacity });
	}

	STDMETHODIMP RetireResource(UINT_PTR handle) override
	{
		try
		{
			_data[handle].SetUnused();
			_cleanupList.emplace_back(size_t(handle));
			return S_OK;
		}
		CATCH_ALL();
	}

	void CleanUp()
	{
		for (auto&& handle : _cleanupList)
		{
			if (!_freeList.empty())
				_freeList.emplace_front<FreeListEntry>(handle, 1);
			else
			{
				auto it = std::find_if(_freeList.begin(), _freeList.end(), [&](const FreeListEntry& entry) {return entry.start > handle; });
				CombineFreeNode(_freeList.insert(it, { handle, 1 }));
			}
		}
		_cleanupList.clear();
	}
private:
	void CombineFreeNode(typename std::list<FreeListEntry>::iterator it)
	{
		assert(it != _freeList.end());
		if (it != _freeList.begin())
		{
			auto left = std::prev(it);
			if (left->start + left->length == it->start)
			{
				left->length += it->length;
				CombineFreeNode(_freeList.erase(it));
				return;
			}
		}
		auto right = std::next(it);
		if (right != _freeList.end())
		{
			if(it->start + it->length == right->start)
			{
				it->length += right->length;
				_freeList.erase(right);
			}
		}
	}
private:
	std::vector<T> _data;
	std::list<FreeListEntry> _freeList;
	std::vector<size_t> _cleanupList;
};

END_NS_PLATFORM