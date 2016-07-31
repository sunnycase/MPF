//
// MPF Platform
// Resource Container
// 作者：SunnyCase
// 创建时间：2016-07-20
//
#pragma once
#include "../../inc/common.h"
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct IDrawCallList;

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

	std::vector<std::weak_ptr<IDrawCallList>>& GetDependentDrawCallLists() noexcept { return _dependentDrawCallLists; }
	void AddDependentDrawCallList(std::weak_ptr<IDrawCallList>&& dcl) { _dependentDrawCallLists.emplace_back(std::move(dcl)); }

	bool Used = false;
private:
	std::vector<std::weak_ptr<IDrawCallList>> _dependentDrawCallLists;
};

struct IResourceContainer
{
	virtual UINT_PTR Allocate() = 0;
	virtual HRESULT RetireResource(UINT_PTR handle) noexcept = 0;
};

template<class T>
class ResourceContainer : public IResourceContainer
{
	struct FreeListEntry
	{
		size_t start;
		size_t length;
	};
public:
	ResourceContainer(size_t capacity = 231)
	{
		Enlarge(capacity);
	}

	virtual UINT_PTR Allocate() override
	{
		if (_freeList.empty()) Enlarge();
		assert(!_freeList.empty());
		auto& front = _freeList.front();
		assert(front.length);
		auto handle = front.start++;
		if (!--front.length)
			_freeList.erase(_freeList.begin());
		new (&_data.at(handle)) T();
		_data[handle].SetUsed();
		return handle;
	}

	void Enlarge(size_t up = 0)
	{
		const auto oldCapacity = _data.size();
		const auto upCapacity = up ? up : oldCapacity;
		const auto newCapacity = oldCapacity + upCapacity;
		_data.resize(newCapacity);
		std::for_each(_data.begin() + oldCapacity, _data.end(), [](auto& item)
		{
			item.~T();
			item.SetUnused();
		});
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

	virtual HRESULT RetireResource(UINT_PTR handle) noexcept override
	{
		try
		{
			_data[handle].~T();
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
			if (_freeList.empty())
				_freeList.emplace_front<FreeListEntry>({ handle, 1 });
			else
			{
				auto it = std::find_if(_freeList.begin(), _freeList.end(), [&](const FreeListEntry& entry) {return entry.start > handle; });
				CombineFreeNode(_freeList.insert(it, { handle, 1 }));
			}
		}
		_cleanupList.clear();
	}

	T const & FindResource(UINT_PTR handle) const noexcept
	{
		auto& value = _data[handle];
		assert(value.Used);
		return value;
	}

	T & FindResource(UINT_PTR handle) noexcept
	{
		auto& value = _data[handle];
		assert(value.Used);
		return value;
	}

	const std::vector<UINT_PTR> GetCleanupList() const noexcept
	{
		return _cleanupList;
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
			if (it->start + it->length == right->start)
			{
				it->length += right->length;
				_freeList.erase(right);
			}
		}
	}
protected:
	std::vector<T> _data;
	std::list<FreeListEntry> _freeList;
	std::vector<UINT_PTR> _cleanupList;
};

END_NS_PLATFORM