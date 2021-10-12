#pragma once
#include "../common/common.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

size_t MEGA_BYTE = 1048576;
size_t Init_Size = MEGA_BYTE * 128;
size_t GIGA_BYTE = 1024 * MEGA_BYTE;
namespace vmath_memory_arena {
	struct Space {
		std::byte* _SpaceHead, * m_SpaceCurrent, * _SpaceTail;
		//size_t total_size;
		
		Space(size_t size)
		{
			_SpaceHead = static_cast<std::byte*>(malloc(size));
			m_SpaceCurrent = _SpaceHead;
			_SpaceTail = _SpaceHead + size;
			assert(_SpaceHead != nullptr && _SpaceTail != nullptr);
		}
		
		bool within(size_t allocate_size) {
			return m_SpaceCurrent + allocate_size < _SpaceTail;
		}

		std::byte* Allocate(size_t allocate_size) {
			if (within(allocate_size)) {
				std::byte* result = m_SpaceCurrent;
				m_SpaceCurrent += allocate_size;
				return result;
			}
			else {
				return nullptr;
			}
		}
	};
	struct SpaceAllocator {
		Space** _SpaceArray;
		//Space sp;
		size_t renewal_size, index;
		
		SpaceAllocator()
		{
			_SpaceArray = (Space**)malloc(40 * sizeof(Space*));
			index = 0;
			renewal_size = Init_Size;
			_SpaceArray[index] = new Space(renewal_size);
		}
		
		std::byte* Allocate(size_t allocate_size) {
			std::byte* initial_request = _SpaceArray[index]->Allocate(allocate_size);
			if (initial_request != nullptr) 
			{
				return initial_request;
			}
			else {
				renewal_size = std::max(allocate_size, renewal_size ) * 2;
				Space Space_New(renewal_size);
				index++;
				_SpaceArray[index] = &Space_New;
				return Space_New.Allocate(allocate_size);
			}
		}
	};
	class MemoryArena {
		SpaceAllocator DynamicSpaceAllocator, StaticSpaceAllocator, LargeSpaceAllocator;
		MemoryArena() 
		{

		}
	public:
		static MemoryArena& Instance()
		{
			static MemoryArena instance;
			return instance;
		}
		
		template <typename T>
		T* DynamicAllocate(size_t allocate_size) {
			return reinterpret_cast<T*>(DynamicSpaceAllocator.Allocate(allocate_size));
		}

		template <typename T>
		T* LargeAllocate(size_t allocate_size) {
			return reinterpret_cast<T*>(LargeSpaceAllocator.Allocate(allocate_size));
		}

		template <size_t allocate_size, typename T>
		T* StaticAllocate() {
			return reinterpret_cast<T*>(StaticSpaceAllocator.Allocate(allocate_size));
		}

		void DynamicRelease() {
			Space** arr = DynamicSpaceAllocator._SpaceArray;
			for (size_t i = 0; i < DynamicSpaceAllocator.index; ++i) {
				free(arr[i]->_SpaceHead);
			}
		}

		void LargeRelease() {
			Space** arr = LargeSpaceAllocator._SpaceArray;
			for (size_t i = 0; i < LargeSpaceAllocator.index; ++i) {
				free(arr[i]->_SpaceHead);
			}
		}

		void StaticRelease() {
			Space** arr = StaticSpaceAllocator._SpaceArray;
			for (size_t i = 0; i < StaticSpaceAllocator.index; ++i) {
				free(arr[i]->_SpaceHead);
			}
		}

		void freeAll() {
			LargeRelease();
			DynamicRelease();
			StaticRelease();
		}
		/*void ReInitialize() {
			Space** arr = DynamicSpaceAllocator._SpaceArray;
			for (size_t i = 1; i <= DynamicSpaceAllocator.index; ++i) {
				free(arr[i]->_SpaceHead);
			}
			DynamicSpaceAllocator.renewal_size = Init_Size;
			DynamicSpaceAllocator.index = 0;
			arr[0]->m_SpaceCurrent = arr[0]->_SpaceHead;


			arr = StaticSpaceAllocator._SpaceArray;
			for (size_t i = 1; i <= StaticSpaceAllocator.index; ++i) {
				free(arr[i]->_SpaceHead);
			}
			StaticSpaceAllocator.renewal_size = Init_Size;
			StaticSpaceAllocator.index = 0;
			arr[0]->m_SpaceCurrent = arr[0]->_SpaceHead;

			arr = LargeSpaceAllocator._SpaceArray;
			for (size_t i = 1; i <= LargeSpaceAllocator.index; ++i) {
				free(arr[i]->_SpaceHead);
			}
			LargeSpaceAllocator.renewal_size = Init_Size;
			LargeSpaceAllocator.index = 0;
			arr[0]->m_SpaceCurrent = arr[0]->_SpaceHead;

		}*/
	};
}