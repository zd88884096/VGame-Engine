#pragma once
#include "../common/common.h"
#include "memoryArena.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>

using namespace vmath_memory_arena;

namespace vmath_memory_structs {

	namespace detail
	{
		constexpr size_t PREV_ALLOC_LIMIT = 64;
		constexpr size_t FREE_BLOCKS_LIMIT = 254;
	}
	
	template <typename T, T blockSize>
	constexpr bool NotPowerOfTwo = (blockSize == 0) || (blockSize & (blockSize - 1)) != 0;

	struct Block {
		static constexpr size_t _MinimumSize = 2 * sizeof(size_t) + 2 * sizeof(Block*) + sizeof(void*);
		static constexpr size_t _PointerSize = 2 * sizeof(size_t) + 2 * sizeof(Block*);
		size_t size_front;
		Block* next;
		Block* prev;
		void* data;
	};

	template<size_t blockSize, unsigned char align>
	requires (!NotPowerOfTwo<unsigned char, align>)
	struct Chunk
	{
	private:
		std::byte* _ChunkHead;
	public:
		std::byte* _BlockHead;
		Chunk<blockSize, align>* prev;
		Chunk<blockSize, align>* next;
		unsigned char m_FirstAvailable, m_NumsAvailable;
		
		void Initialize() {
			constexpr size_t total_size = (280 + align + detail::FREE_BLOCKS_LIMIT * (((blockSize - 1) / align + 1) * align));
			constexpr size_t auxilliaryBlocks = (256 / align + 1);
			_ChunkHead = MemoryArena::Instance().StaticAllocate<total_size, std::byte>();
			assert(_ChunkHead != nullptr);
			_BlockHead = reinterpret_cast<std::byte*>((((reinterpret_cast<size_t>(_ChunkHead)) / align) + auxilliaryBlocks) * align);
			m_FirstAvailable = 0;
			m_NumsAvailable = detail::FREE_BLOCKS_LIMIT;
			next = nullptr;
			prev = nullptr;
			unsigned char i = 0;
			std::byte* p = _ChunkHead;
			for (; i < m_NumsAvailable; ++p) {
				*p = static_cast<std::byte>(++i);
			}
		}

		size_t indexOf(void* p)
		{
			constexpr size_t alignedSize = ((blockSize - 1) / align + 1) * align;
			assert(p != nullptr);
			assert(p >= _BlockHead && p < _BlockHead + detail::FREE_BLOCKS_LIMIT * alignedSize);
			assert((reinterpret_cast<size_t>(p) - reinterpret_cast<size_t>(_BlockHead)) % alignedSize == 0);
			return (reinterpret_cast<size_t>(p) - reinterpret_cast<size_t>(_BlockHead)) / alignedSize;
		}

		void* Allocate() {
			constexpr size_t alignedSize = ((blockSize - 1) / align + 1) * align;
			constexpr std::byte byte255 = static_cast<std::byte>(255);
			std::byte* presult = _BlockHead + (m_FirstAvailable * alignedSize);
			std::byte* index = indexLocation(m_FirstAvailable);
			m_FirstAvailable = static_cast<unsigned char>(*index);
			*index = byte255;
			--m_NumsAvailable;
			return presult;
		}

		bool Deallocate(void* p) {
			constexpr size_t alignedSize = ((blockSize - 1) / align + 1) * align;
			size_t index = indexOf(p);
			constexpr std::byte byte255 = static_cast<std::byte>(255);
			//std::cout << (size_t)m_FirstAvailable << " " << (size_t)m_NumsAvailable <<" " << index << std::endl;
			std::byte* indexPointer = indexLocation(index);
			if (*indexPointer == byte255) [[likely]] {
				*indexPointer = static_cast<std::byte>(m_FirstAvailable);
				m_FirstAvailable = index;
				++m_NumsAvailable;
				return true;
			}
			return false;
		}

		void print() {
			std::byte* start = _ChunkHead;
			constexpr std::byte* limit = start + detail::FREE_BLOCKS_LIMIT;
			for (std::byte* p = start; p < limit; ++p) {
				std::cout << static_cast<size_t>(*p) << " ";
			}
			std::cout << " " << std::endl;
			std::cout << static_cast<size_t>(m_FirstAvailable) << " " << static_cast<size_t>(m_NumsAvailable) << std::endl;
		}

		std::byte* indexLocation(unsigned char index) {
			assert(index < detail::FREE_BLOCKS_LIMIT + 1);
			return index + _ChunkHead;
		}

		bool includes(void* p) {
			assert(p != nullptr);
			constexpr size_t alignedSize = ((blockSize - 1) / align + 1) * align;
			constexpr size_t blockSizeTotal = detail::FREE_BLOCKS_LIMIT * alignedSize;
			return p >= _BlockHead && p < _BlockHead + blockSizeTotal;
		}
	};

	template<size_t blockSize, unsigned char align>
	requires (!NotPowerOfTwo<unsigned char, align>)
	class FixedAllocator {
	private:
		typedef Chunk<blockSize, align> chunk_type;
		chunk_type* _ChunkListHead;
		chunk_type* m_AllocIndex;
		chunk_type* m_DeallocIndex, * m_DeallocChunk;
		chunk_type** m_ChunkHeadArray;
		size_t m_FreeBlocks, m_AllocateFromPrevLimit, m_ChunkArrayLength, m_NormalizedListLength;
	public:
		FixedAllocator() {
			chunk_type* newChunk =
				MemoryArena::Instance().StaticAllocate<sizeof(chunk_type), chunk_type>();
			assert(newChunk != nullptr);
			newChunk->Initialize();
			m_ChunkHeadArray = 
				MemoryArena::Instance().StaticAllocate<sizeof(chunk_type*)*1024, chunk_type*>();
			assert(m_ChunkHeadArray != nullptr);
			m_ChunkHeadArray[0] = newChunk;
			m_NormalizedListLength = 0;
			m_ChunkArrayLength = 1024;
			_ChunkListHead = newChunk;
			m_AllocIndex = m_DeallocIndex = m_DeallocChunk = newChunk;
			m_AllocateFromPrevLimit = detail::PREV_ALLOC_LIMIT;
			m_FreeBlocks = detail::FREE_BLOCKS_LIMIT;
		}
		void print();
		void* Allocate();
		void Deallocate(void* p);
	
	};

	template<size_t blockSize, unsigned char align>
	requires (!NotPowerOfTwo<unsigned char, align>)
	void FixedAllocator<blockSize, align>::print() {
		auto i = _ChunkListHead;
		while(i!=nullptr) {
			i->print();
			i = i->next;
		}
		std::cout << "m_DeallocIndex: ";
		if (m_DeallocIndex == nullptr) {
			std::cout << "nullptr" << std::endl;
		}
		else {
			std::cout << m_DeallocIndex << std::endl;
		}
	}

	template<size_t blockSize, unsigned char align>
	requires (!NotPowerOfTwo<unsigned char, align>)
	void FixedAllocator<blockSize, align>::Deallocate(void* p) {
		assert(p != nullptr);
		auto index_left = m_DeallocIndex;
		if (index_left->includes(p)) {
			if (index_left->Deallocate(p)) {
				++m_FreeBlocks;
				assert(m_FreeBlocks <= m_AllocateFromPrevLimit * 4);
			}
			return;
		}
		size_t start = 0, end = m_NormalizedListLength, mid = 0;
		while (start < end) {
			mid = (1 + start + end) / 2;
			if (m_ChunkHeadArray[mid]->_BlockHead <= p) {
				start = mid;
			}
			else {
				end = mid - 1;
			}
		}
		if(p<m_ChunkHeadArray[mid]){
			mid--;
		}
		assert(m_ChunkHeadArray[mid]->includes(p));
		if (m_ChunkHeadArray[mid]->Deallocate(p)) {
			m_FreeBlocks++;
		}
	}

	template<size_t blockSize, unsigned char align>
	requires (!NotPowerOfTwo<unsigned char, align>)
	void* FixedAllocator<blockSize, align>::Allocate() {
		if (m_AllocIndex->m_NumsAvailable)
		{
			--m_FreeBlocks;
			assert(m_FreeBlocks >= 0);
			return m_AllocIndex->Allocate();
		}
		//std::cout << m_FreeBlocks << std::endl;
			//when we have 25% or more blocks free, we go to the chunks already available to allocate
			//otherwise, we create a new chunk
		if (m_FreeBlocks >= m_AllocateFromPrevLimit) {
			--m_FreeBlocks;
			assert(m_FreeBlocks >= 0);
			auto i = m_DeallocChunk;
			while (i && !i->m_NumsAvailable) i = i->next;
			
			if (i) {
				m_DeallocChunk = i;
				return i->Allocate();
			}

			i = _ChunkListHead;
			while (i && !i->m_NumsAvailable) i = i->next;
			assert(i != nullptr);
			m_DeallocChunk = i;
			return i->Allocate();
		}
		else {
			chunk_type* newChunk =
				MemoryArena::Instance().StaticAllocate<sizeof(chunk_type), chunk_type>();
			assert(newChunk != nullptr);
			newChunk->Initialize();
			++m_NormalizedListLength;
			size_t curListLength = m_NormalizedListLength;
			if (m_ChunkArrayLength == curListLength) {
				chunk_type** tmp = m_ChunkHeadArray;
				m_ChunkHeadArray = MemoryArena::Instance().DynamicAllocate<chunk_type*>(2 * curListLength * sizeof(chunk_type*));
				assert(m_ChunkHeadArray != nullptr);
				for (size_t i = 0; i < curListLength; ++i) {
					m_ChunkHeadArray[i] = tmp[i];
				}
				m_ChunkArrayLength <<= 1;
				m_ChunkHeadArray[curListLength] = newChunk;
			}
			else [[likely]] {
				m_ChunkHeadArray[curListLength] = newChunk;
			}
			newChunk->prev = m_AllocIndex;
			m_AllocIndex->next = newChunk;
			m_AllocIndex = newChunk;
			m_AllocateFromPrevLimit += detail::PREV_ALLOC_LIMIT;
			m_FreeBlocks += detail::FREE_BLOCKS_LIMIT-1;
			return m_AllocIndex->Allocate();
		}
	}

	template <size_t maxObjectSize>
	class SmallObjAllocator {
	public:
		template<size_t NumBytes, unsigned char align>
		void* Allocate()
		{
			if constexpr (align == 16)
			{
				if constexpr (NumBytes == 16) return m_FixedAllocator_16_16.Allocate();
				else if constexpr (NumBytes == 32) return m_FixedAllocator_32_16.Allocate();
				else if constexpr (NumBytes == 48) return m_FixedAllocator_48_16.Allocate();
				else if constexpr (NumBytes == 64) return m_FixedAllocator_64_16.Allocate();
				else if constexpr (NumBytes == 80) return m_FixedAllocator_80_16.Allocate();
				else if constexpr (NumBytes == 128) return m_FixedAllocator_128_16.Allocate();
				else if constexpr (NumBytes == 256) return m_FixedAllocator_256_16.Allocate();
				else return nullptr;
			}
			else {
				if constexpr (NumBytes == 16) return m_FixedAllocator_16_32.Allocate();
				else if constexpr (NumBytes == 32) return m_FixedAllocator_32_32.Allocate();
				else if constexpr (NumBytes == 48) return m_FixedAllocator_48_32.Allocate();
				else if constexpr (NumBytes == 64) return m_FixedAllocator_64_32.Allocate();
				else if constexpr (NumBytes == 80) return m_FixedAllocator_80_32.Allocate();
				else if constexpr (NumBytes == 128) return m_FixedAllocator_128_32.Allocate();
				else if constexpr (NumBytes == 256) return m_FixedAllocator_256_32.Allocate();
				else return nullptr;
			}

		}

		template<size_t NumBytes, unsigned char align>
		void Deallocate(void* p)
		{
			if constexpr (align == 16) {
				if constexpr (NumBytes == 16) m_FixedAllocator_16_16.Deallocate(p);
				else if constexpr (NumBytes == 32) m_FixedAllocator_32_16.Deallocate(p);
				else if constexpr (NumBytes == 48) m_FixedAllocator_48_16.Deallocate(p);
				else if constexpr (NumBytes == 64) m_FixedAllocator_64_16.Deallocate(p);
				else if constexpr (NumBytes == 80) m_FixedAllocator_80_16.Deallocate(p);
				else if constexpr (NumBytes == 128) m_FixedAllocator_128_16.Deallocate(p);
				else if constexpr (NumBytes == 256) m_FixedAllocator_256_16.Deallocate(p);
			}
			else {
				if constexpr (NumBytes == 16) m_FixedAllocator_16_32.Deallocate(p);
				else if constexpr (NumBytes == 32) m_FixedAllocator_32_32.Deallocate(p);
				else if constexpr (NumBytes == 48) m_FixedAllocator_48_32.Deallocate(p);
				else if constexpr (NumBytes == 64) m_FixedAllocator_64_32.Deallocate(p);
				else if constexpr (NumBytes == 80) m_FixedAllocator_80_32.Deallocate(p);
				else if constexpr (NumBytes == 128) m_FixedAllocator_128_32.Deallocate(p);
				else if constexpr (NumBytes == 256) m_FixedAllocator_256_32.Deallocate(p);
			}
		}
		void Release();
		void print();
	private:
		FixedAllocator<16, 16> m_FixedAllocator_16_16;
		FixedAllocator<32, 16> m_FixedAllocator_32_16;
		FixedAllocator<48, 16> m_FixedAllocator_48_16;
		FixedAllocator<64, 16> m_FixedAllocator_64_16;
		FixedAllocator<80, 16> m_FixedAllocator_80_16;
		FixedAllocator<128, 16> m_FixedAllocator_128_16;
		FixedAllocator<256, 16> m_FixedAllocator_256_16;
		FixedAllocator<16, 32> m_FixedAllocator_16_32;
		FixedAllocator<32, 32> m_FixedAllocator_32_32;
		FixedAllocator<48, 32> m_FixedAllocator_48_32;
		FixedAllocator<64, 32> m_FixedAllocator_64_32;
		FixedAllocator<80, 32> m_FixedAllocator_80_32;
		FixedAllocator<128, 32> m_FixedAllocator_128_32;
		FixedAllocator<256, 32> m_FixedAllocator_256_32;
	};

	template <size_t maxObjectSize>
	void SmallObjAllocator<maxObjectSize>::Release() {
		MemoryArena::Instance().StaticRelease();
		MemoryArena::Instance().DynamicRelease();
	}
	template <size_t maxObjectSize>
	void SmallObjAllocator<maxObjectSize>::print() {
		std::cout << "FixedAllocator for 16 bytes" << std::endl;
		m_FixedAllocator_16_16.print();

		std::cout << "FixedAllocator for 32 bytes" << std::endl;
		m_FixedAllocator_32_16.print();

		std::cout << "FixedAllocator for 48 bytes" << std::endl;
		m_FixedAllocator_48_16.print();

		std::cout << "FixedAllocator for 64 bytes" << std::endl;
		m_FixedAllocator_64_16.print();

		std::cout << "FixedAllocator for 80 bytes" << std::endl;
		m_FixedAllocator_80_16.print();
	}
}
