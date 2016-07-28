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
		_dependentDrawCallLists.clear();
	}

	std::vector<std::weak_ptr<IDrawCallList>>& GetDependentDrawCallLists() noexcept { return _dependentDrawCallLists; }
	void AddDependentDrawCallList(std::weak_ptr<IDrawCallList>&& dcl) { _dependentDrawCallLists.emplace_back(std::move(dcl)); }

	bool Used = false;
private:
	std::vector<std::weak_ptr<IDrawCallList>> _dependentDrawCallLists;
};

struct DECLSPEC_UUID("E3A7830B-60CA-43CB-ACA4-A82AC65A45E5") IResourceContainer : public IUnknown
{
	virtual UINT_PTR Allocate() = 0;
	virtual HRESULT RetireResource(UINT_PTR handle) noexcept = 0;
};

template<class T>
class ResourceContainerImplement : public WRL::Implements<WRL::RuntimeClassFlags<WRL::ClassicCom>, IResourceContainer>
{
	struct FreeListEntry
	{
		size_t start;
		size_t length;
	};
public:
	ResourceContainerImplement(size_t capacity = 231)
	{
		_data.resize(capacity);
		_freeList.emplace_front<FreeListEntry>({ 0, _data.size() });
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

	virtual HRESULT RetireResource(UINT_PTR handle) noexcept override
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

template<class T>
class ResourceContainer : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, ResourceContainerImplement<T>>
{
};

END_NS_PLATFORM