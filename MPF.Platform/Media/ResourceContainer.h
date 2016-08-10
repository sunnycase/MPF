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

template<class T>
class ResourceContainer;

struct ResourceBase
{
	std::vector<std::weak_ptr<IDrawCallList>>& GetDependentDrawCallLists() noexcept { return _dependentDrawCallLists; }
	void AddDependentDrawCallList(std::weak_ptr<IDrawCallList>&& dcl) { _dependentDrawCallLists.emplace_back(std::move(dcl)); }
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
protected:
	struct ObjectStorage
	{
		static_assert(std::is_nothrow_destructible<T>::value, "T must be nothrow destructible.");

		ObjectStorage() noexcept
			:Inited(false)
		{

		}

		ObjectStorage(const ObjectStorage& other)
			:Inited(other.Inited)
		{
			if (Inited)
				new (_data) T(other.GetObject());
		}

		ObjectStorage(ObjectStorage&& other) noexcept
			:Inited(other.Inited)
		{
			if (Inited)
			{
				new (_data) T(std::move(other.GetObject()));
				other.Inited = false;
			}
		}

		ObjectStorage& operator=(const ObjectStorage& other)
		{
			Free();
			if (other.Inited)
			{
				new (_data) T(other.GetObject());
				Inited = true;
			}
			return *this;
		}

		ObjectStorage& operator=(ObjectStorage&& other) noexcept
		{
			Free();
			if (other.Inited)
			{
				new (_data) T(std::move(other.GetObject()));
				Inited = true;
				other.Inited = false;
			}
			return *this;
		}

		template<class ...P>
		void Create(P&&... params)
		{
			Free();
			new (_data) T(std::forward<P>(params)...);
			Inited = true;
		}

		T& GetObject()
		{
			assert(Inited);
			return *reinterpret_cast<T*>(_data);
		}

		const T& GetObject() const
		{
			assert(Inited);
			return *reinterpret_cast<T const*>(_data);
		}

		void Free() noexcept
		{
			if (Inited)
			{
				GetObject().~T();
				Inited = false;
			}
		}

		bool GetUsed() const noexcept { return Used; }

		void SetUsed() noexcept
		{
			Used = true;
		}

		void SetUnused() noexcept
		{
			Used = false;
		}

	private:
		alignas(alignof(T)) char _data[sizeof(T)];

		bool Used = false;
		bool Inited;
	};
public:
	ResourceContainer(size_t capacity = 231)
	{
		Enlarge(capacity);
	}

	virtual ~ResourceContainer()
	{
		for (auto&& item : _data)
			item.Free();
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
		auto& obj = _data.at(handle);
		obj.Create();
		obj.SetUsed();
		return handle;
	}

	void Enlarge(size_t up = 0)
	{
		const auto oldCapacity = _data.size();
		const auto upCapacity = up ? up : oldCapacity;
		const auto newCapacity = oldCapacity + upCapacity;
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
			auto& obj = _data[handle];
			obj.Free();
			obj.SetUnused();
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
		assert(value.GetUsed());
		return value.GetObject();
	}

	T & FindResource(UINT_PTR handle) noexcept
	{
		auto& value = _data[handle];
		assert(value.GetUsed());
		return value.GetObject();
	}

	bool TryFindResource(UINT_PTR handle, T const*& refer) const noexcept
	{
		auto& value = _data[handle];
		if (value.GetUsed())
		{
			refer = &value.GetObject();
			return true;
		}
		return false;
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
				auto next = _freeList.erase(it);
				if (next != _freeList.end())
					CombineFreeNode(next);
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
	std::vector<ObjectStorage> _data;
	std::list<FreeListEntry> _freeList;
	std::vector<UINT_PTR> _cleanupList;
};

END_NS_PLATFORM