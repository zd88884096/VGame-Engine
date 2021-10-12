#pragma once
#include "../common/common.h"
#include "memoryArena.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>
#include <queue>

using namespace vmath_memory_arena;

namespace vmath_large_memory {
	struct Node{
		unsigned char color;
		Node* left, * right, * parent;
		size_t size;
		void* memory;

		Node(unsigned char c, Node* l, Node* r, Node* p, size_t sz, void* mem) {
			color = c;
			left = l;
			right = r;
			parent = p;
			size = sz;
			memory = mem;
		}

		bool operator<(const Node& other)const {
			return size < other.size || (size == other.size && memory < other.memory);
		}

		bool operator>(const Node& other)const {
			return size > other.size || (size == other.size && memory > other.memory);
		}

		bool operator==(const Node& other) const {
			return size == other.size && memory == other.memory;
		}
	};

	struct RBTree {
		Node* m_root;
		Node* m_MaxNode;
		std::vector<Node*> m_PrintVector;
		void** chunk_array;
		size_t chunk_array_index;
		inline size_t pad(size_t size) {
			return (((size-1) >> 3) + 1) << 3;
		}

		inline Node* NodeGen(unsigned char c, Node* l, Node* r, Node* p, size_t sz, void* mem) {
			Node* node = MemoryArena::Instance().StaticAllocate<sizeof(Node), Node>();
			node->color = c;
			node->left = l;
			node->right = r;
			node->parent = p;
			node->size = sz;
			node->memory = mem;
			return node;
		}

		RBTree(){
			constexpr size_t initialBlockSize = 16 * 1048576 - 32;
			constexpr size_t highestBit = ((size_t)1) << 62;
			chunk_array = (void**)malloc(sizeof(void*) * 40);
			chunk_array_index = 0;
			m_root = NodeGen(1, NullNodeGen(), NullNodeGen(), NullNodeGen(), 1, nullptr);
			m_MaxNode = m_root;
			void* m_memory = MemoryArena::Instance().LargeAllocate<void>(initialBlockSize+sizeof(size_t)*8);
			chunk_array[chunk_array_index] = m_memory;
			
			size_t* m_tmp = static_cast<size_t*>(m_memory);
			m_tmp[0] = highestBit;
			m_tmp[1] = initialBlockSize + sizeof(size_t) * 2;
			m_tmp += 2;
			m_memory = static_cast<void*>(m_tmp);
			m_tmp += initialBlockSize / sizeof(size_t);
			m_tmp[0] = initialBlockSize + sizeof(size_t) * 2;
			m_tmp[1] = highestBit;
			insert(initialBlockSize, m_memory);
		}

		Node* NullNodeGen() {
			return NodeGen(1, nullptr, nullptr, nullptr, 0, nullptr);
		}

		void Left_Rotate(Node* node){
			Node* child = node->right;
			node->right = child->left;
			//if (child->left->size!=0) {
			child->left->parent = node;
			//}
			child->parent = node->parent;
			child->left = node;
			if (node->parent->size==0) {
				m_root = child;
			}
			else if (node->parent->left == node) {
				node->parent->left = child;
			}
			else {
				node->parent->right = child;
			}
			node->parent = child;
		}

		void Right_Rotate(Node* node) {
			Node* child = node->left;
			node->left = child->right;
			//if (child->right->size!=0) {
			child->right->parent = node;
			//}
			child->parent = node->parent;
			child->right = node;
			if (node->parent->size==0) {
				m_root = child;
			}
			else if (node->parent->left == node) {
				node->parent->left = child;
			}
			else {
				node->parent->right = child;
			}
			node->parent = child;
		}

		size_t height(Node* cur) {
			if (cur->size==0) {
				return 0;
			}
			return 1 + std::max(height(cur->left), height(cur->right));
		}

		bool height_test(Node* cur) {
			if (cur->size == 0) {
				return true;
			}
			size_t left_height = height(cur->left), right_height = height(cur->right);
			return height_test(cur->left) && height_test(cur->right) && (left_height * 2 + 1 >= right_height || right_height * 2 + 1 >= left_height);
		}

		size_t black_height(Node* cur) {
			if (cur->size == 0) {
				return 0;
			}
			return std::max(black_height(cur->left), black_height(cur->right)) + cur->color;
		}

		bool black_height_test(Node* cur) {
			if (cur->size == 0) {
				return true;
			}
			return black_height_test(cur->left) && black_height_test(cur->right) && black_height(cur->left) == black_height(cur->right);
		}

		bool red_descendant_test(Node* cur) {
			if (cur->size == 0) {
				return true;
			}
			return red_descendant_test(cur->left) && red_descendant_test(cur->right) && (cur->color == 0 ? cur->color != cur->left->color && cur->color != cur->right->color : true);
		}

		int indexOf(Node* node) {
			for (size_t i = 0; i < m_PrintVector.size(); ++i) {
				if (m_PrintVector[i] == node) {
					return static_cast<int>(i);
				}
			}
			return -1;
		}

		void dfsNodes(Node* cur) {
			if (cur == nullptr) {
				return;
			}
			dfsNodes(cur->left);
			m_PrintVector.push_back(cur);
			dfsNodes(cur->right);
		}

		void print() {
			m_PrintVector.clear();
			dfsNodes(m_root);
			std::queue<Node*> q;
			q.push(m_root);
			while (!q.empty()) {
				int indent = -1, sz = q.size();
				for (int m = 0; m < sz; m++) {
					Node* cur = q.front();
					q.pop();
					int index = indexOf(cur), indentlen = index - indent - 1;
					indent = index;
					for (int i = 0; i < indentlen * 3; i++) {
						std::cout << " ";
					}
					std::cout << cur->size;
					if (cur->color == 0) {
						std::cout << "R";
					}
					else {
						std::cout << "B";
					}
					if (cur->size < 10) {
						std::cout << " ";
					}
					if (cur->left != nullptr) {
						q.push(cur->left);
					}
					if (cur->right != nullptr) {
						q.push(cur->right);
					}
				}
				std::cout << std::endl;
			}
			std::cout << "\n\n\n\n" << std::endl;
			m_PrintVector.clear();
		}

		void printAddr() {
			m_PrintVector.clear();
			dfsNodes(m_root);
			std::queue<Node*> q;
			q.push(m_root);
			while (!q.empty()) {
				int indent = -1, sz = q.size();
				for (int m = 0; m < sz; m++) {
					Node* cur = q.front();
					q.pop();
					int index = indexOf(cur), indentlen = index - indent - 1;
					indent = index;
					for (int i = 0; i < indentlen * 15; i++) {
						std::cout << " ";
					}
					std::cout << cur->memory;
					if (cur->color == 0) {
						std::cout << "R";
					}
					else {
						std::cout << "B";
					}
					if (cur->size < 10) {
						std::cout << " ";
					}
					if (cur->left->size!=0) {
						q.push(cur->left);
					}
					if (cur->right->size!=0) {
						q.push(cur->right);
					}
				}
				std::cout << std::endl;
			}
			std::cout << "\n\n\n\n" << std::endl;
			m_PrintVector.clear();
		}

		void printExternalFrag() {
			constexpr size_t highestBit = (static_cast<size_t>(1)) << 62, mask = highestBit - 1;
			m_PrintVector.clear();
			size_t free_nums = 0, free_size = 0, total_size = 0;
			for (size_t i = 0; i <= chunk_array_index; ++i) {
				free_size += 2 * sizeof(size_t);
				total_size += 2 * sizeof(size_t);
				size_t* mem = static_cast<size_t*>(chunk_array[i]);
				++mem;
				size_t size;
				while (size = (*mem)&mask) {
					if (*mem < highestBit) {
						std::cout << "Block Size: " << size << "	Memory Location: " << mem << std::endl;
						++free_nums;
						free_size += size;
					}
					else {
						free_size += sizeof(size_t) * 2;
					}
					total_size += size;
					mem += size / sizeof(size_t);
				}
			}
			std::cout << "\n\nTotal Free Blocks: " << free_nums << std::endl;
			std::cout << "\n\nTotal Free Memory in Bytes: " << free_size << std::endl;
			std::cout << "\n\nTotal Allocated Size: " << total_size << std::endl;
			std::cout << "\n\nFragmentation Rate: " << static_cast<double>(free_size) / static_cast<double>(total_size) << std::endl;
		}

		void* Allocate(size_t mem_sz);
		void Deallocate(void* memory);
		Node* find(size_t mem_sz, void* memory, Node* cur);
		void insert_add(Node* node, Node* cur);
		void insert_fix(Node* node);
		void insert(size_t mem_sz, void* memory);
		void transplant(Node* dest, Node* src);
		Node* maximum(Node* cur);
		Node* minimum(Node* cur);
		void remove(Node* node);
		void complete_remove(Node* node);
	};

	void* RBTree::Allocate(size_t mem_sz) {
		static size_t m_AllocMultiplier = 32;
		constexpr size_t highestBit = (static_cast<size_t>(1)) << 62;
		mem_sz = pad(mem_sz);
		if (mem_sz > m_MaxNode->size) {
			size_t blockSize = std::max(mem_sz * 2 + 8*sizeof(size_t), m_AllocMultiplier * 1048574 + 8*sizeof(size_t));
			m_AllocMultiplier <<= 1;
			void* mem = MemoryArena::Instance().LargeAllocate<void>(blockSize);
			++chunk_array_index;
			chunk_array[chunk_array_index] = mem;
			size_t restSize = blockSize - 4 * sizeof(size_t) - mem_sz;
			size_t* m_tmp = static_cast<size_t*>(mem);
			m_tmp[0] = highestBit;
			m_tmp[1] = mem_sz + 2 * sizeof(size_t) + highestBit;
			m_tmp += 2;
			mem = static_cast<void*>(m_tmp);
			m_tmp += mem_sz/sizeof(size_t);
			m_tmp[0] = highestBit+mem_sz+2*sizeof(size_t);
			m_tmp[1] = restSize;
			m_tmp += 2;
			size_t* m_end = m_tmp + restSize/sizeof(size_t) - 2;
			m_end[0] = restSize;
			m_end[1] = highestBit;
			insert(restSize - 2 * sizeof(size_t), static_cast<void*>(m_tmp));
			return mem;
		}
		Node* node = m_MaxNode;
		void* res = node->memory;
		if (node->size == mem_sz) [[unlikely]] {
			if (node->left->size == 0) {
				m_MaxNode = node->parent;
			}
			else {
				m_MaxNode = maximum(node->left);
			}
			size_t* m_tmp = static_cast<size_t*>(node->memory) - 1;
			m_tmp[0] |= highestBit;
			m_tmp += 1 + (mem_sz/sizeof(size_t));
			m_tmp[0] |= highestBit;
			remove(node);
			return res;
		}
		size_t blockSize = node->size + sizeof(size_t) * 2, returnBlockSize = mem_sz + sizeof(size_t) * 2, restSize = blockSize - returnBlockSize;
		size_t* m_tmp = static_cast<size_t*>(node->memory) - 1;
		m_tmp[0] = returnBlockSize+highestBit;
		m_tmp += 1 + mem_sz/sizeof(size_t);
		m_tmp[0] = returnBlockSize+highestBit;
		m_tmp[1] = restSize;
		m_tmp[restSize/sizeof(size_t)] = restSize;
		m_tmp += 2;
		if (node->left->size != 0) {
			Node* maxLeft = maximum(node->left);
			if (restSize > 2 * sizeof(size_t)) [[likely]] {
				restSize -= 2 * sizeof(size_t);
				node->size = restSize;
				if (*node > *maxLeft) {
					node->memory = static_cast<void*>(m_tmp);
				}
				else {
					m_MaxNode = maxLeft;
					remove(node);
					insert(restSize, static_cast<void*>(m_tmp));
				}
			}
			else {
				m_MaxNode = maxLeft;
				remove(node);
			}
		}
		else {
			if (restSize > 2 * sizeof(size_t)) [[likely]] {
				restSize -= 2 * sizeof(size_t);
				node->size = restSize;
				if (*node > *node->parent) {
					node->memory = static_cast<void*>(m_tmp);
				}
				else {
					m_MaxNode = node->parent;
					remove(node);
					insert(restSize, static_cast<void*>(m_tmp));
				}
			}
			else {
				m_MaxNode = node->parent;
				remove(node);
			}
		}
		return res;
	}

	void RBTree::Deallocate(void* memory) {
		constexpr size_t highestBit = (static_cast<size_t>(1)) << 62, mask = highestBit - 1;
		size_t* m_left = static_cast<size_t*>(memory) - 1;
		size_t m_TotalSize = m_left[0]&mask;
		size_t* m_right = m_left + m_TotalSize/sizeof(size_t);
		--m_left;
		size_t sizeLeft = m_left[0];
		size_t sizeRight = m_right[0];
		if (!(highestBit & sizeLeft) && sizeLeft>16) {
			m_TotalSize += sizeLeft;
			m_left = m_left + 2 - sizeLeft/sizeof(size_t);
			Node* node = find(sizeLeft - 2 * sizeof(size_t), static_cast<void*>(m_left), m_root);
			complete_remove(node);
			--m_left;
		}
		else {
			++m_left;
		}
		if (!(highestBit & sizeRight) && sizeRight>16) {
			m_TotalSize += sizeRight;
			++m_right;
			Node* node = find(sizeRight - 2 * sizeof(size_t), static_cast<void*>(m_right), m_root);
			complete_remove(node);
			m_right += sizeRight/sizeof(size_t) - 2;
		}
		else {
			--m_right;
		}
		m_left[0] = m_TotalSize;
		++m_left;
		m_right[0] = m_TotalSize;
		insert(m_TotalSize - 2 * sizeof(size_t), static_cast<void*>(m_left));
	}

	Node* RBTree::find(size_t mem_sz, void* memory, Node* cur) {
		if (mem_sz < cur->size || (mem_sz == cur->size && memory < cur->memory)) {
			return find(mem_sz, memory, cur->left);
		}
		else if (mem_sz != cur->size || memory != cur->memory) {
			return find(mem_sz, memory, cur->right);
		}
		return cur;
	}

	void RBTree::insert_add(Node* newNode, Node* curNode) {
		if (*newNode < *curNode) {
			if(curNode->left->size != 0){
				insert_add(newNode, curNode->left);
			}
			else {
				newNode->left = curNode->left;
				newNode->right = NullNodeGen();
				curNode->left = newNode;
				newNode->parent = curNode;
			}
		}
		else {
			if (curNode->right->size!=0) {
				insert_add(newNode, curNode->right);
			}
			else {
				newNode->left = curNode->right;
				newNode->right = NullNodeGen();
				curNode->right = newNode;
				newNode->parent = curNode;
			}
		}
	}

	void RBTree::insert_fix(Node* node) {
		Node* uncle, *gp, *p;
		bool side;
		while (node->parent->size!=0 && node->parent->color == 0) {
			p = node->parent;
			gp = node->parent->parent;
			if (side = (p == gp->left)) {
				uncle = gp->right;
			}
			else {
				uncle = gp->left;
			}
			if (uncle->size!=0 && uncle->color == 0) {
				p->color = 1;
				uncle->color = 1;
				gp->color = 0;
				node = gp;
			}
			else if(side){
				if (node == p->right) {
					node = node->parent;
					Left_Rotate(node);
				}
				node->parent->color = 1;
				node->parent->parent->color = 0;
				Right_Rotate(node->parent->parent);
			}
			else {
				if (node == p->left) {
					node = node->parent;
					Right_Rotate(node);
				}
				node->parent->color = 1;
				node->parent->parent->color = 0;
				Left_Rotate(node->parent->parent);
			}
		}
		m_root->color = 1;
	}

	void RBTree::insert(size_t mem_sz, void* memory) {
		Node* newNode = NodeGen(0, nullptr, nullptr, nullptr, mem_sz, memory);
		insert_add(newNode, m_root);
		insert_fix(newNode);
		m_MaxNode = *newNode > *m_MaxNode ? newNode : m_MaxNode;
	}

	void RBTree::transplant(Node* dest, Node* src) {
		if (dest->parent->size == 0) {
			m_root = src;
		}
		else if (dest == dest->parent->left) {
			dest->parent->left = src;
		}
		else {
			dest->parent->right = src;
		}
		src->parent = dest->parent;
	}

	Node* RBTree::maximum(Node* cur) {
		while (cur->right->size!=0) {
			cur = cur->right;
		}
		return cur;
	}

	Node* RBTree::minimum(Node* cur) {
		while (cur->left->size!=0) {
			cur = cur->left;
		}
		return cur;
	}

	void RBTree::remove(Node* node) {
		unsigned char original = node->color;
		Node* replace = node, * replace_child;
		replace_child = node->left;
		transplant(node, node->left);
		replace = replace_child;
		if (original == 1) {
			Node* sibling;
			while (replace->color == 1 && replace != m_root) {
				if (replace == replace->parent->left) {
					sibling = replace->parent->right;
					if (sibling->color == 0) {
						replace->parent->color = 0;
						sibling->color = 1;
						Left_Rotate(replace->parent);
						sibling = replace->parent->right;
					}
					if (sibling->left->color == 1 && sibling->right->color == 1) {
						sibling->color = 0;
						replace = replace->parent;
					}
					else {
						if (sibling->right->color == 1) {
							sibling->color = 0;
							sibling->left->color = 1;
							Right_Rotate(sibling);
							sibling = replace->parent->right;
						}
						sibling->color = replace->parent->color;
						replace->parent->color = 1;
						sibling->right->color = 1;
						Left_Rotate(replace->parent);
						replace = m_root;
					}
				}
				else {
					sibling = replace->parent->left;
					if (sibling->color == 0) {
						replace->parent->color = 0;
						sibling->color = 1;
						Right_Rotate(replace->parent);
						sibling = replace->parent->left;
					}
					if (sibling->left->color == 1 && sibling->right->color == 1) {
						sibling->color = 0;
						replace = replace->parent;
					}
					else {

						if (sibling->left->color == 1) {
							sibling->color = 0;
							sibling->right->color = 1;
							Left_Rotate(sibling);
							sibling = replace->parent->left;
						}
						sibling->color = replace->parent->color;
						replace->parent->color = 1;
						sibling->left->color = 1;
						Right_Rotate(replace->parent);
						replace = m_root;
					}
				}
			}
			replace->color = 1;
		}
	}

	void RBTree::complete_remove(Node* node) {
		unsigned char original = node->color;
		Node* replace = node, * replace_child;
		if (node->left->size == 0) {
			replace_child = node->right;
			transplant(node, node->right);
		}
		else if (node->right->size == 0) {
			replace_child = node->left;
			transplant(node, node->left);
		}
		else {
			replace = minimum(node->right);
			original = replace->color;
			replace_child = replace->right;
			if (replace->parent == node) {
				replace_child->parent = replace;
			}
			else {
				transplant(replace, replace->right);
				replace->right = node->right;
				replace->right->parent = replace;
			}
			transplant(node, replace);
			replace->left = node->left;
			replace->left->parent = replace;
			replace->color = node->color;
		}
		replace = replace_child;
		if (original == 1) {
			Node* sibling;
			while (replace->color == 1 && replace != m_root) {
				if (replace == replace->parent->left) {
					sibling = replace->parent->right;
					if (sibling->color == 0) {
						replace->parent->color = 0;
						sibling->color = 1;
						Left_Rotate(replace->parent);
						sibling = replace->parent->right;
					}
					if (sibling->left->color == 1 && sibling->right->color == 1) {
						sibling->color = 0;
						replace = replace->parent;
					}
					else {
						if (sibling->right->color == 1) {
							sibling->color = 0;
							sibling->left->color = 1;
							Right_Rotate(sibling);
							sibling = replace->parent->right;
						}
						sibling->color = replace->parent->color;
						replace->parent->color = 1;
						sibling->right->color = 1;
						Left_Rotate(replace->parent);
						replace = m_root;
					}
				}
				else {
					sibling = replace->parent->left;
					if (sibling->color == 0) {
						replace->parent->color = 0;
						sibling->color = 1;
						Right_Rotate(replace->parent);
						sibling = replace->parent->left;
					}
					if (sibling->left->color == 1 && sibling->right->color == 1) {
						sibling->color = 0;
						replace = replace->parent;
					}
					else {

						if (sibling->left->color == 1) {
							sibling->color = 0;
							sibling->right->color = 1;
							Left_Rotate(sibling);
							sibling = replace->parent->left;
						}
						sibling->color = replace->parent->color;
						replace->parent->color = 1;
						sibling->left->color = 1;
						Right_Rotate(replace->parent);
						replace = m_root;
					}
				}
			}
			replace->color = 1;
		}
	}
}