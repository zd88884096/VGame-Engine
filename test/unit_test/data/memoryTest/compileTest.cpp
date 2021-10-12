#include "../../../../include/memory/memory.h"
#include "../../../../include/memory/LargeMemory.h"
#include "../../../../include/memory/memoryArena.h"
#include "../../../utility/timer.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <time.h>
#include <vector>

using namespace vmath_test;
using namespace vmath_memory_structs;
using namespace vmath_large_memory;
using namespace vmath_memory_arena;

class Pair {
public:
	size_t a;
	void* b;
	Pair(size_t ap, void* bp) {
		a = ap;
		b = bp;
	}
};

void mem_print(size_t* start, size_t num) {
	constexpr size_t highestBit = (static_cast<size_t>(1)) << 62, mask = highestBit - 1;
	size_t blockSize = start[0] & mask;
	for (size_t i = 0; i < num; ++i) {
		std::cout << "Block " << i << ":" << std::endl;
		std::cout << "leftSize " << (start[0] & mask) << " rightSize " << (start[blockSize / sizeof(size_t) - 1] & mask) << std::endl;
		std::cout << "leftAlloc " << !!(start[0] & highestBit) << " rightAlloc " << !!(start[blockSize / sizeof(size_t) - 1] & highestBit) << "\n" << std::endl;
		start += blockSize / sizeof(size_t);
	}
}
int main()
{
	srand(time(NULL));
	Timer timer;
	RBTree tree;
	size_t testEpoch = 500000;
	size_t sizeMax = 128;
	size_t mallocSize = 2000;
	std::cout << "\n\nNumber of allocations: " << testEpoch << "\n" << "mallocSize " << mallocSize << "\n" << std::endl;
	std::vector<Pair*> pair_vec;
	std::vector<void*> mems;
	std::cout << std::boolalpha;
	SmallObjAllocator<80> small_one;

	

	timer.millisecondsElapsedf();
	for (size_t k = 0; k < testEpoch; ++k) {
		size_t size = rand() % 4;
		if (size > 0) {
			void* mem = small_one.Allocate<128, 32>();
			mems.push_back(mem);
		}
		else if (mems.size() > 0) {
			for (size_t i = 0; i < mems.size(); ++i)
				small_one.Deallocate<128,32>(mems[i]);
			mems.clear();
		}
	}
	float elapsedTime = timer.millisecondsElapsedf();
	pair_vec.clear();
	MemoryArena::Instance().DynamicRelease();

	//time subtraction purpose
	timer.millisecondsElapsedf();
	for (size_t k = 0; k < testEpoch; ++k) {
		
		size_t size = rand() % 4;
		if (size > 0) {
			void* mem = nullptr;
			mems.push_back(mem);
		}
		else if (mems.size() > 0) {
			for (size_t i = 0; i < mems.size(); ++i)
			mems.clear();
		}

	}

	float elapsedTime2 = timer.millisecondsElapsedf();
	std::cout << "Pool Allocator Elapsed Time: " << elapsedTime - elapsedTime2 << std::endl;


	mems.clear();
	timer.millisecondsElapsedf();
	for (size_t k = 0; k<testEpoch; ++k) {
		size_t size = rand() % 4;
		if (size > 0) {
			void* mem = tree.Allocate(mallocSize);
			//void* mem = small_one.Allocate<16, 32>();
			/*double* d = (double*)mem;
			d[0] = 1.0;
			d[1] = 2.0;
			d[2] = 3.0;
			d[3] = 4.0;
			d[4] = 1.0;
			d[5] = 2.0;
			d[6] = 3.0;
			d[7] = 4.0;
			d[8] = 1.0;
			d[9] = 2.0;
			d[10] = 3.0;
			d[11] = 4.0;
			d[12] = 4.0;*/
			mems.push_back(mem);
		}
		else if(mems.size()>0){
			for(size_t i=0;i<mems.size();++i)
				tree.Deallocate(mems[i]);
			mems.clear();
		}
	}
	elapsedTime = timer.millisecondsElapsedf();
	for (size_t i = 0; i < mems.size(); ++i)
		tree.Deallocate(mems[i]);
	mems.clear();
	MemoryArena::Instance().freeAll();
	pair_vec.clear();



	//time subtraction purpose
	timer.millisecondsElapsedf();
	for (size_t k = 0; k < testEpoch; ++k) {
		size_t size = rand() % 4;
		if (size > 0) {
			void* mem = nullptr;
			mems.push_back(mem);
		}
		else if (mems.size() > 0) {
			for (size_t i = 0; i < mems.size(); ++i);
			mems.clear();
		}
	}
	elapsedTime2 = timer.millisecondsElapsedf();
	
	std::cout << "RBTree Elapsed Time: " << elapsedTime-elapsedTime2<< std::endl;
	mems.clear();
	
	
	timer.millisecondsElapsedf();
	for (size_t k = 0; k < testEpoch; ++k) {
		size_t size = rand() % 4;
		if (size > 0) {
			void* mem = malloc(mallocSize);
			mems.push_back(mem);
		}
		else if (mems.size() > 0) {
			for (size_t i = 0; i < mems.size(); ++i)
				free(mems[i]);
			mems.clear();
		}
	}

	elapsedTime = timer.millisecondsElapsedf();

	timer.millisecondsElapsedf();
	for (size_t k = 0; k < testEpoch; ++k) {
		size_t size = rand() % 4;
		if (size > 0) {
			void* mem = nullptr;
			mems.push_back(mem);
		}
		else if (mems.size() > 0) {
			for (size_t i = 0; i < mems.size(); ++i);
			mems.clear();
		}
	}
	
	elapsedTime2 = timer.millisecondsElapsedf();

	std::cout << "Malloc Elapsed Time: " <<  elapsedTime - elapsedTime2 << std::endl;

	mems.clear();
	std::cout << "Printing Initial Tree: \n\n" << std::endl;
	tree.print();
	std::cout << "Unit Test 1\n\nAllocating 6 blocks of size 32 and freeing block 1 and 3 (0-indexed)\n" << std::endl;
	for (size_t i = 0; i < 6; ++i) {
		mems.push_back(tree.Allocate(32));
	}
	tree.Deallocate(mems[1]);
	tree.Deallocate(mems[3]);
	size_t* m_tmp = (size_t*)mems[0] - 1;
	mem_print(m_tmp, 6);
	tree.print();

	std::cout << "\nUnit Test 2\n\nFreeing block at 2\n" << std::endl;
	tree.Deallocate(mems[2]);
	m_tmp = (size_t*)mems[0] - 1;
	mem_print(m_tmp, 6);
	tree.print();

	std::cout << "\nUnit Test 3\n\nFreeing all remaining blocks\n" << std::endl;
	tree.Deallocate(mems[0]);
	tree.Deallocate(mems[4]);
	tree.Deallocate(mems[5]);
	m_tmp = (size_t*)mems[0] - 1;
	mem_print(m_tmp, 1);
	tree.print();

	std::cout << "\nUnit Test 4\n\nNow Allocating a 10MB Chunk and see if it is usable\n" << std::endl;
	std::cout << "writing 0.1 and 0.2 alternatively into the memory and calculate the sum\n" << std::endl;
	std::cout << "Expected: 196608\n\n" << std::endl;
	double* m_tmp2 = (double*)tree.Allocate(10 * 1048576);
	double sum = 0.0;
	for (size_t i = 0; i < 10 * 1048576 / 16; ++i) {
		m_tmp2[i * 2] = 0.1;
		m_tmp2[i * 2 + 1] = 0.2;
		sum += m_tmp2[i * 2] + m_tmp2[i * 2 + 1];
	}
	std::cout << "Result: " << sum << "\n\n" << std::endl;
	tree.print();
	tree.Deallocate(m_tmp2);
	tree.print();
	mems.clear();

	std::cout << "\nUnit Test 5\n\nNow Allocate the whole 16MB Chunk to see how the memory behaves\n\n" << std::endl;
	for (size_t i = 0; i < 262143; i++) {
		mems.push_back(tree.Allocate(48));
	}
	tree.print();
	std::cout << "\nNow randomly Deallocate 20 blocks of size 48 and print the tree\n\nIndices: " << std::endl;
	for (size_t i = 0; i < 20; i++) {
		size_t index = i * 10000 + rand() % 10000;
		tree.Deallocate(mems[index]);
		std::cout << index << " ";
	}
	std::cout << "\n\n" << std::endl;
	tree.print(); 
	tree.printExternalFrag();

	std::cout << "\nNow Allocate 10 of those free blocks\n\n" << std::endl;
	for (size_t i = 0; i < 10; i++) {
		tree.Allocate(48);
	}
	tree.print();
	tree.printExternalFrag();
	//tree.print();
	/*srand(time(NULL));
	size_t testEpoch = 3000000;
	Timer timer;
	SmallObjAllocator<80> SmallObjAllocator_One;
	FixedAllocator<80, 32> FixedAllocator_One;
	Chunk<80, 32> Chunk_One;
	std::puts("Test Start!");
	double sum = 0.0;
	double* a = nullptr;
	timer.millisecondsElapsedf();
	std::vector<double*> vec;
	for(size_t i=0;i<testEpoch;++i){
		a = (double*)SmallObjAllocator_One.Allocate<32,32>();
		//std::cout << "CheckPoint 1 " << i << std::endl;
		//std::cout << a << " " << (size_t)FixedAllocator_One._ChunkListTail->_NumsAvailable << " " << (size_t)FixedAllocator_One._FreeBlocks << std::endl;
		vec.emplace_back(a);
		a[0]=2.0;
		a[1]=3.0;
		a[2]=4.0;
		a[3]=5.0;
		sum+=a[0];
		sum+=a[1];
		sum+=a[2];
		sum+=a[3];
		a = (double*)SmallObjAllocator_One.Allocate<32, 32>();
		vec.emplace_back(a);
		a[0] = 2.0;
		a[1] = 3.0;
		a[2] = 4.0;
		a[3] = 5.0;
		sum += a[0];
		sum += a[1];
		sum += a[2];
		sum += a[3];
		//SmallObjAllocator_One.Deallocate<32, 32>(a);
		SmallObjAllocator_One.Deallocate<32, 32>(vec[rand() % vec.size()]);
	}
	//SmallObjAllocator_One.Release();
	float elapsedTime = timer.millisecondsElapsedf();
	a = (double*)SmallObjAllocator_One.Allocate<32, 32>();
	std::puts("Test End!");
	//std::cout << elapsedTime << std::endl;
	std::cout << "Pull Allocator Time Elapsed: " << elapsedTime << std::endl;
	std::vector<double*> vec2;
	timer.millisecondsElapsedf();
	for(int i=0;i<testEpoch;++i){
		a = (double*)malloc(64);
		vec2.emplace_back(a);
		a[0]=2.0;
                a[1]=3.0;
                a[2]=4.0;
                a[3]=5.0;
                sum+=a[0];
                sum+=a[1];
                sum+=a[2];
                sum+=a[3];
				a = (double*)malloc(64);
				vec2.emplace_back(a);
				a[0] = 2.0;
				a[1] = 3.0;
				a[2] = 4.0;
				a[3] = 5.0;
				sum += a[0];
				sum += a[1];
				sum += a[2];
				sum += a[3];
				double* b = vec2[rand() % vec2.size()];
				free(a);
	}
	elapsedTime = timer.millisecondsElapsedf();
	std::cout << "Malloc Time Elapsed: " << elapsedTime<< std::endl;*/
}

