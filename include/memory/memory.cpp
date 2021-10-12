#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include "memory.h"

using namespace vmath_memory_block;

//initialize playground memory
static const size_t allocated_space = 1048576;

//log2(allocated_space)+1
static const size_t seg_lists_length = 21;

static void* _HeapStart;
static Block **seg_lists;

Block* block_initialize(void* start, size_t size, Block* next_ptr, Block* prev_ptr){
        Block* block = (Block*)start;
        size_t padded_payload = (size + _WordSize - 1) & ~(_WordSize - 1);
        size_t total_size = padded_payload+Block::_PointerSize;
        block->size_front = size;
        *((size_t*)((size_t)start+total_size-sizeof(size_t))) = size;
        block->next = next_ptr;
        block->prev = prev_ptr;
        block->data = (void*)((size_t)start+Block::_PointerSize-sizeof(size_t));
        return block;
}

void initialize_seg_lists(){
	_HeapStart = malloc(allocated_space);
	if(_HeapStart==nullptr){
		std::cout<<"Fail to Initialize Heap from vmathlibrary/include/memory/memory.cpp"<<std::endl;
	}
	*((size_t*)(_HeapStart)) = ((size_t)1)<<63;
	*((size_t*)((size_t)_HeapStart+allocated_space-sizeof(size_t))) = ((size_t)1)<<63;
	_HeapStart=(void*)((size_t)_HeapStart+sizeof(size_t));


	//mark all words as free
	for(size_t i=0;i<allocated_space;i+=sizeof(size_t)){
		*((size_t*)((size_t)_HeapStart+i)) = 0;
	}

	//initialize seg_lists
	seg_lists = (Block**)malloc(sizeof(Block*)*seg_lists_length);
	if(seg_lists==nullptr){
        	std::cout<<"Fail to Initialize seg_lists from vmathlibrary/include/memory/memory.cpp"<<std::endl;
	}

	for(size_t i=0;i<seg_lists_length-1;i++){
		seg_lists[i]=nullptr;
	}

	//add the block of allocated_space to seg_lists
	seg_lists[seg_lists_length-1]=block_initialize(_HeapStart, allocated_space - Block::_PointerSize - 2*sizeof(size_t), nullptr, nullptr);
}

//size of block after padding
inline size_t padded_size(size_t size){
	return Block::_PointerSize+((size + _WordSize - 1) & ~(_WordSize - 1));
}

/*
Block* vmath_request(size_t size){
	//to be implemented
	size_t start = get_memory_tail();
	size_t allocate_size = padded_size(size);
	_Allocated = allocate_size+start;
	if(start+allocate_size>allocated_space){
		std::cout<<"Out of Memory from vmathlibrary/include/memory/memory.cpp"<<std::endl;
		return nullptr;
	}
	
	//to be changed for seg list
	Block* block = block_initialize((void*)(start+_HeapStart), size, nullptr, nullptr);
	
	//add the case in which the last block is free
	return block;
}
*/


//index of list that depends on the total size of block
//size 0-1 -> index 0
//size 2-3 -> index 1
//size 4-7 -> index 2
//size 8-15 -> index 3, etc...

inline size_t list_number(size_t size_total){
	return static_cast<size_t>(std::log10(size_total)/std::log10(2));
}

//malloc for vmath
void* vmath_malloc(size_t size){
	size_t total_size = padded_size(size);
	size_t list_index = list_number(total_size);
	Block* current_list = seg_lists[list_index];
	Block* iterator = current_list;
	while(list_index<seg_lists_length)
	{
		while(iterator!=nullptr&&iterator->size_front<total_size){
			iterator = iterator->next;
		}
		if(iterator!=nullptr)
		{
			//merge free blocks at front or/and back of the current block;
			Block* cur = iterator;
			void* start = cur;
			size_t merged_size_total = *((size_t*)cur);
			size_t prev_size = *((size_t*)(cur-sizeof(size_t)));
			if(!prev_size&(((size_t)1)<<63)){
				start == (void*)((size_t)start - prev_size);
				merged_size_total+=prev_size;
				*((size_t*)cur)=0;
				*((size_t*)(cur-sizeof(size_t)))=0;

				Block* prev_block = (Block*)start;
				size_t prev_list_index = list_number(prev_size);
				if(prev_block->prev==nullptr){
					seg_lists[prev_list_index]=prev_block->next;
					if(prev_block->next!=nullptr)
						prev_block->next->prev=nullptr;
				}
				else{
					if(prev_block->next!=nullptr)
						prev_block->next->prev=prev_block->prev;
					prev_block->prev->next=prev_block->next;
				}
			}
			size_t next_size = *((size_t*)(cur+*((size_t*)cur)));
			if(!next_size&(((size_t)1)<<63)){
				merged_size_total+=prev_size;
				*((size_t*)(cur+*((size_t*)cur)))=0;
				*((size_t*)(cur+*((size_t*)cur)-sizeof(size_t)))=0;

				Block* next_block = (Block*)(cur+*((size_t*)cur));
				size_t next_list_index = list_number(next_size);
				if(next_block->prev==nullptr){
					seg_lists[next_list_index]=next_block->next;
					if(next_block->next!=nullptr){
						next_block->next->prev=nullptr;
					}
				}
				else{
					if(next_block->next!=nullptr)
						next_block->next->prev=next_block->prev;
					next_block->prev->next=next_block->next;
				}
			}
			
			//allocate memory block to return
			Block* result = block_initialize(start, size, nullptr, nullptr);
			result->size_front = total_size+(((size_t)1)<<63);
			*((size_t*)(result+total_size-sizeof(size_t)))=total_size+(((size_t)1)<<63);
			
			//modify the leftovers
			size_t leftover_total_size = merged_size_total-total_size;
			if(leftover_total_size<Block::_MinimumSize){
				*((size_t*)((size_t)start+total_size))=leftover_total_size;
				*((size_t*)((size_t)start+merged_size_total-sizeof(size_t)))=leftover_total_size;
			}
			else{
				size_t leftover_list_index = list_number(leftover_total_size);
				Block* leftover = block_initialize((void*)((size_t)start+total_size), leftover_total_size, nullptr, seg_lists[leftover_list_index]); 
				if(seg_lists[leftover_list_index]!=nullptr)
                                	seg_lists[leftover_list_index]->prev = leftover;
				seg_lists[leftover_list_index]=leftover;
			}
			return result->data;

		}
		else
			list_index++;
	}
	//reshuffle the allocated memory to clear out external fragmentation?

	std::cout<<"Out of Free Memory in vmath_malloc from vmathlibrary/include/memory/memory.cpp"<<std::endl;
	return nullptr;
}
int main(){
	std::cout<<"0"<<std::endl;
}

