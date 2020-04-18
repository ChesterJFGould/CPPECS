#pragma once

#include <iostream>
#include <cstdlib>
#include <EASTL/vector.h>
#include "EASTLFix.h"

#define MEMORYPOOL_DECLARATION(className)		\
public:											\
	static bool InitMemoryPool(int pageSize);	\
	static void DestroyMemoryPool();			\
	static void* operator new(size_t size);		\
	static void operator delete(void* p);		\
												\
private:										\
	static ECS::MemoryPool<className> *memoryPool;

#define MEMORYPOOL_IMPLEMENTATION(className)								\
bool className::InitMemoryPool(int pageSize)								\
{																			\
	className::memoryPool = new ECS::MemoryPool<className>(pageSize);		\
	return className::memoryPool->Init();									\
}																			\
																			\
void className::DestroyMemoryPool()											\
{																			\
	delete className::memoryPool;											\
}																			\
																			\
void* className::operator new(size_t size)									\
{																			\
	return className::memoryPool->Alloc();									\
}																			\
																			\
void className::operator delete(void* p)									\
{																			\
	className::memoryPool->Free(p);											\
}																			\
																			\
ECS::MemoryPool<className>* className::memoryPool = nullptr;

namespace ECS
{
	using namespace std;

	template <class DataType>
	class MemoryPool
	{
	public:
		MemoryPool(size_t pageSize);
		~MemoryPool();
		
		bool Init();
		void* Alloc();
		void Free(void* p);
		
	private:
		union Element
		{
			Element *Next;
			DataType Data;
			
			Element();
			~Element();
		};
		
		Element *freeList;
		eastl::vector<Element*> pages;
		size_t pageSize;
	};

	template <class DataType>
	MemoryPool<DataType>::MemoryPool(size_t pageSize) : pageSize(pageSize)
	{
		
	}

	template <class DataType>
	bool MemoryPool<DataType>::Init()
	{
		cout << "New MemoryPool with element size: " << sizeof(Element) << endl;
		
		MemoryPool<DataType>::Element *current = new MemoryPool<DataType>::Element[this->pageSize];
		
		this->pages.push_back(current);
		
		this->freeList = current;
		
		for (size_t i = 1; i < pageSize; i++)
		{
			current->Next = current + 1;
			current = current->Next;
		}
		
		current->Next = nullptr;
		
		return false;
	}

	template <class DataType>
	MemoryPool<DataType>::~MemoryPool()
	{
		cout << "pool cleanup" << endl;
		for (Element *p : this->pages)
		{
			delete[] p;
		}
	}

	template <class DataType>
	void* MemoryPool<DataType>::Alloc()
	{
		// Alloc a new page
		if (this->freeList == nullptr)
		{
			Element *current = new Element[this->pageSize];
		
			this->pages.push_back(current);
		
			this->freeList = current;
		
			for (size_t i = 1; i < pageSize; i++)
			{
				current->Next = current + 1;
				current = current->Next;
			}
			
			current->Next = nullptr;
		}
		
		// Take head of free list
		Element *element = this->freeList;
		
		// Remove node from free list
		this->freeList = element->Next;
		
		return (void*)element;
	}

	template <class DataType>
	void MemoryPool<DataType>::Free(void* p)
	{
		Element* element = static_cast<Element*>(p);
		
		element->Next = this->freeList;
		
		this->freeList = element;
	}

	template <class DataType>
	MemoryPool<DataType>::Element::Element()
	{
		
	}

	template <class DataType>
	MemoryPool<DataType>::Element::~Element()
	{
		
	}
}

