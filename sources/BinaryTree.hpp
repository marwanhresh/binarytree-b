#pragma once

#include <iostream>
#include <queue>
#include <stdexcept>

namespace ariel{
	template <class T>
	class BinaryTree
	{
	private:

		class node
		{
		public:
			T data;
			node *left, *right;
			node* parent;
			node(const T& data, node *left, node *right, node* parent=nullptr) :
				data(data), left(left), right(right), parent(parent){}
		};

		node* root;

		node* findNode(node* root, const T& value)
		{
			if (root == nullptr){
				return nullptr;
			}

			if (root->data == value){
				return root;
			}
			node* l = findNode(root->left, value);
			if (l){
				return l;
			}
			node* r = findNode(root->right, value);
			if (r){
				return r;
			}
			return nullptr;
		}

	public:
		BinaryTree()
		{
			root = nullptr;
		}

		~BinaryTree(){
			deleteAll(root);
		}

		node* cloneTree(node* n){
			if (n == nullptr){
				return nullptr;
			}

			node *newRoot = new node(n->data, nullptr, nullptr, n->parent);
			newRoot->left = cloneTree(n->left);
			newRoot->right = cloneTree(n->right);

			return newRoot;
		}

		BinaryTree(const BinaryTree& other){
			*this = other;
		}

		BinaryTree& operator=(const BinaryTree& other){
			if (this != &other)
			{
				if(this->root != nullptr){
					deleteAll(this->root);
					this->root = nullptr;
				}
				this->root = cloneTree(other.root);
			}
			return *this;
		}


		BinaryTree(BinaryTree&& other) noexcept{
			*this = std::move(other);
		}

		BinaryTree& operator=(BinaryTree&& other) noexcept{
			if (this != &other)
			{
				root = other.root;
				other.root = nullptr;
			}
			return *this;
		}

		void deleteAll(node* n){
			if (n == nullptr){
				return;
			}
			if (n->left){
				deleteAll(n->left);
			}
			if (n->right){
				deleteAll(n->right);
			}
			delete n;
		}

		BinaryTree& add_root(const T& value)
		{
			if (root == nullptr)
			{
				root = new node(value, nullptr, nullptr);
			}
			else
			{
				root->data = value;
			}
			return *this;
		}

		BinaryTree& add_left(const T& value, const T& left_value)
		{
			node *n = findNode(root, value);
			if(n == nullptr){
				throw std::logic_error("element not found");
			}
			if (n->left == nullptr)
			{
				n->left = new node(left_value, nullptr, nullptr, n);
			}
			else
			{
				n->left->data = left_value;
			}
			return *this;
		}

		BinaryTree& add_right(T value, T right_value)
		{
			node *n = findNode(root, value);
			if(n == nullptr){
				throw std::logic_error("element not found");
			}
			if (n->right == nullptr)
			{
				n->right = new node(right_value, nullptr, nullptr, n);
			}
			else
			{
				n->right->data = right_value;
			}
			return *this;
		}



		friend std::ostream& operator<<(std::ostream& o, const BinaryTree<T>& b){
			printWithLevels(o, b.root);
			return o;
		}

		static void printWithLevels(std::ostream& o, node* p, int indent = 0)
		{
			if (p != nullptr) {
				if (indent != 0) {
					for (int i = 0; i < indent; i++){
						o << ' ';
					}
				}
				o << p->data << "\n";
				if (p->left)
				{
					printWithLevels(o, p->left, indent + 4);
				}
					
				if (p->right){
					printWithLevels(o, p->right, indent + 4);
				}
					

			}
		}


		class iterator{
		private:
			node* current_node;
		public:
			iterator(){
				current_node = nullptr;
			}

			node* getCurrentNode(){
				return current_node;
			}

			void setCurrentNode(node* n){
				current_node = n;
			}

			T& operator*() const {
				return current_node->data;
			}

			T* operator->() const {
				return &current_node->data;
			}

			bool operator==(const iterator& rhs) const {
				return current_node == rhs.current_node;
			}

			bool operator!=(const iterator& rhs) const {
				return current_node != rhs.current_node;
			}

			virtual iterator& operator++() = 0;

		};


		class inorder_iterator : public iterator{
		public:
			inorder_iterator(node* ptr) {
				if (ptr == nullptr){
					return;
				}

				node* lastLeft = ptr;
				while (lastLeft->left != nullptr){
					lastLeft = lastLeft->left;
				}

				this->setCurrentNode(lastLeft);
			}

			inorder_iterator& operator++(){
				node* next = this->getCurrentNode();
				//right child
				if (next->right != nullptr){
					next = next->right;
					//leftmost
					while (next->left != nullptr){
						next = next->left;
					}
				}
				//no right child we go up
				else{
					while (next->parent != nullptr && next == next->parent->right){
						next = next->parent;
					}
					next = next->parent;
				}
				this->setCurrentNode(next);
				return *this;
			}

			inorder_iterator operator++(int){
				inorder_iterator tmp = *this;
				++(*this);
				return tmp;
			}
		};

		inorder_iterator begin_inorder(){
			return inorder_iterator(root);
		}

		inorder_iterator end_inorder(){
			return inorder_iterator(nullptr);
		}

		class preorder_iterator : public iterator{
		public:
			preorder_iterator(node* ptr) {
				if (ptr == nullptr){
					return;
				}

				this->setCurrentNode(ptr);
			}

			preorder_iterator& operator++(){
				node* next = this->getCurrentNode();
				//left child
				if (next->left != nullptr){
					next = next->left;
				}
				//right child
				else if (next->right != nullptr){
					next = next->right;
				}
				//leaf, go up
				else{
					if (next->parent != nullptr){
						bool goUp = true;
						// if we were at the left child go up and to the right
						if (next->parent->left == next){
							next = next->parent;
							// we have right child
							if (next->right != nullptr){
								next = next->right;
								goUp = false;
							}
						}
						//if we were at the right child or had no right child, go upper to preorder predecessor
						if (goUp){
							while (next->parent != nullptr && (next == next->parent->right || next->parent->right == nullptr)){
								next = next->parent;
							}
							next = next->parent;
							if (next != nullptr && next->right != nullptr){
								next = next->right;
							}
						}
					}
					//root
					else{
						next = nullptr;
					}
				}
				this->setCurrentNode(next);
				return *this;
			}

			preorder_iterator operator++(int){
				preorder_iterator tmp = *this;
				++(*this);
				return tmp;
			}
		};

		preorder_iterator begin_preorder(){
			return preorder_iterator(root);
		}

		preorder_iterator end_preorder(){
			return preorder_iterator(nullptr);
		}

		class postorder_iterator : public iterator{
		public:
			postorder_iterator(node* ptr) {
				if (ptr == nullptr){
					return;
				}

				node* next = ptr;
				//travel to deepest child
				while (next->left != nullptr || next->right != nullptr){
					while (next->left != nullptr){
						next = next->left;
					}
					while (next->right != nullptr){
						next = next->right;
					}
				}

				this->setCurrentNode(next);
			}
			
			postorder_iterator& operator++(){
				node* next = this->getCurrentNode();
				//in case of root
				if (next->parent == nullptr){
					next = nullptr;
				}
				//if we were the right child or there was no right child we go up
				else if (next == next->parent->right || next->parent->right == nullptr){
					next = next->parent;
				}
				else{
					next = next->parent;
					//if we have right child
					if (next->right != nullptr){
						next = next->right;
						//go right until we have left child or no more right to go
						while (next->right != nullptr && next->left == nullptr){
							next = next->right;
						}
						// go to leftmost
						while (next->left != nullptr){
							next = next->left;
						}
					}
				}
				this->setCurrentNode(next);
				return *this;
			}

			postorder_iterator operator++(int){
				postorder_iterator tmp = *this;
				++(*this);
				return tmp;
			}
		};

		postorder_iterator begin_postorder(){
			return postorder_iterator(root);
		}

		postorder_iterator end_postorder(){
			return postorder_iterator(nullptr);
		}

		inorder_iterator begin(){
				return inorder_iterator(root);
			}

		inorder_iterator end(){
			return inorder_iterator(nullptr);
		}


	};
}