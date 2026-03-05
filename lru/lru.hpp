#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include "utility.hpp"
#include "exceptions.hpp"
#include "class-integer.hpp"
#include "class-matrix.hpp"
#include <vector>
class Hash {
public:
	unsigned int operator () (Integer lhs) const {
		int val = lhs.val;
		return std::hash<int>()(val);
	}
};
class Equal {
public:
	bool operator () (const Integer &lhs, const Integer &rhs) const {
		return lhs.val == rhs.val;
	}
};

namespace sjtu {
template<class T> class double_list{
public:
	struct node {
		T data;
		node *prev, *next;

		node(const T &x, node *p = nullptr, node *n = nullptr)
			: data(x), prev(p), next(n) {}
		node() : prev(nullptr), next(nullptr) {}
	};

	node *head, *tail;
	int currentLength = 0;
public:
// --------------------------
	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	*/
	double_list(){
		head = new node();
		tail = new node();
		head->next = tail;
		tail->prev = head;
		currentLength = 0;
	}
	double_list(const double_list<T> &other){
		head = new node();
		tail = new node();
		head->next = tail;
		tail->prev = head;
		currentLength = 0;

		node *p = other.head->next;
		while (p != other.tail) {
			insert_tail(p->data);
			p = p->next;
		}
	}
	double_list& operator=(const double_list<T> &other) {
		if (this == &other) return *this;
		clear();

		node *p = other.head->next;
		while (p != other.tail) {
			insert_tail(p->data);
			p = p->next;
		}
		return *this;
	}
	~double_list(){
		if (currentLength != 0) {
			node *p = head->next;
			while (p != tail) {
				node *tmp = p;
				p = p->next;
				delete tmp;
			}
		}
		delete head;
		delete tail;
	}

	int size(){ return currentLength ; }

	void clear() {
		if (currentLength != 0) {
			node *p = head->next;
			while (p != tail) {
				node *tmp = p;
				p = p->next;
				delete tmp;
			}
			head->next = tail;
			tail->prev = head;
			currentLength = 0;
		}
	}

	class iterator{
	public:
    	node *ptr;
		const double_list* check;
		friend class double_list;
	    // --------------------------

		iterator(node* p = nullptr , const double_list* c = nullptr) : ptr(p) , check(c) {}
		iterator(const iterator &t){
			ptr = t.ptr;
			check = t.check;
		}
		//iterator(): ptr(nullptr) , check(nullptr){}
		~iterator() = default;


		iterator& operator++() {
			if (this->ptr == check->tail) {
				throw sjtu::index_out_of_bound("double list , it++");
			}
			ptr = ptr->next;
			return *this;
		}
		iterator operator++(int) {
			if (this->ptr == check->tail) {
				throw sjtu::index_out_of_bound("double list , it++");
			}
			iterator tmp = *this;
			ptr = ptr->next;
			return tmp;
		}

		iterator& operator--() {
			if (this->ptr == check->head || this->ptr == check->head->next) {
				throw sjtu::index_out_of_bound("double list , it--");
			}
			ptr = ptr->prev;
			return *this;
		}
		iterator operator--(int) {
			if (this->ptr == check->head || this->ptr == check->head->next) {
				throw sjtu::index_out_of_bound("double list , it--");
			}
			iterator tmp = *this;
			ptr = ptr->prev;
			return tmp;
		}
		/**
		 * if the iter didn't point to a value
		 * throw " invalid"
		*/
		T &operator*() const {
			if (!ptr || ptr == check->tail || ptr == check->head) {
				throw sjtu::invalid_iterator("double list , if the iter didn't point to a value,*");
			}
			return ptr->data;
		}
        /**
         * other operation
        */
		T *operator->() const noexcept {
			if (!ptr) {
				throw sjtu::invalid_iterator("double list , if the iter didn't point to a value,->").what();
			}
			return &ptr->data;
		}
		bool operator==(const iterator &rhs) const {
			return this->ptr == rhs.ptr;
    	}
		bool operator!=(const iterator &rhs) const {
			return this->ptr != rhs.ptr;
		}
	};
	/**
	 * return an iterator to the beginning
	 */
	iterator begin() const{
		return iterator(head->next,this);
	}
	/**
	 * return an iterator to the ending
	 * in fact, it returns the iterator point to nothing,
	 * just after the last element.
	 */
	iterator end() const{
		return iterator(tail,this);
	}
	iterator back() const {
		return iterator(tail->prev,this);
	}

	// int dis(iterator it) {
	// 	int i = 0;
	// 	iterator tmp = begin();
	// 	while (tmp != it) {
	// 		++i;
	// 		++tmp;
	// 		if (tmp == end()) {
	// 			throw sjtu::index_out_of_bound();
	// 		}
	// 	}
	// 	return i;
	// }
	/**
	 * if the iter didn't point to anything, do nothing,
	 * otherwise, delete the element pointed by the iter
	 * and return the iterator point at the same "index"
	 * e.g.
	 * 	if the origin iterator point at the 2nd element
	 * 	the returned iterator also point at the
	 *  2nd element of the list after the operation
	 *  or nothing if the list after the operation
	 *  don't contain 2nd elememt.
	*/
	iterator erase(iterator pos){
		node *p = pos.ptr;
		iterator ret(p->next,this);

		p->prev->next = p->next;
		p->next->prev = p->prev;

		delete p;
		--currentLength;
		return ret;
	}

	/**
	 * the following are operations of double list
	*/

	void move_to_head(node *p) {   //移动已有节点
		p->prev->next = p->next;
		p->next->prev = p->prev;
		p->next = head->next;
		p->prev = head;
		head->next->prev = p;
		head->next = p;
	}


	void insert_head(const T &val){
		node *beg = head->next;
		node *new_node = new node(val, head, beg);

		head->next = new_node;
		beg->prev = new_node;

		++currentLength;
	}

	void insert_tail(const T &val){
		node *p = tail->prev;
		node *new_node = new node(val, p, tail);

		tail->prev = new_node;
		p->next = new_node;

		++currentLength;
	}

	void delete_head(){
		node *p = head->next;

		head->next = p->next;
		p->next->prev = head;

		delete p;
		--currentLength;
	}

	void delete_tail(){
		node *p = tail->prev;

		tail->prev = p->prev;
		p->prev->next = tail;

		delete p;
		--currentLength;
	}
	/**
	 * if didn't contain anything, return true, 
	 * otherwise false.
	 */
	bool empty() const{
		return  currentLength == 0;
	}
};

template<
	class Key,
	class T,
	class Hash = std::hash<Key>, 
	class Equal = std::equal_to<Key>
> class hashmap{
public:
	using value_type = pair<const Key, T>;

	std::vector<double_list<value_type>> buckets;

	size_t current_size = 0;

	Hash hasher;
	Equal equal_op;

	static const size_t INIT_BUCKET_COUNT = 16;
	static constexpr double MAX_LOAD_FACTOR = 0.75;

	size_t get_bucket_index(const Key& key) const{

		size_t hash_code = hasher(key);

		return hash_code % buckets.size();
	}


// --------------------------

	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	*/
	hashmap() {
		buckets.resize(INIT_BUCKET_COUNT);
		current_size = 0;
	}
	hashmap(const hashmap &other): buckets(other.buckets), current_size(other.current_size),
		  hasher(other.hasher), equal_op(other.equal_op) {}

	virtual ~hashmap() {
		//hashmap<Key, T, Hash, Equal>::clear();
		// for (auto p : buckets) {
		// 	delete p.head;
		// 	delete p.tail;
		// }
	};

	hashmap & operator=(const hashmap &other){
		if (this == &other) return *this;
		buckets = other.buckets;
		current_size = other.current_size;
		hasher = other.hasher;
		equal_op = other.equal_op;
		return *this;
	}

	class iterator{


	public:

		const hashmap* check_map;
		int bucket_idx;
		typename double_list<value_type>::iterator list_it;
// --------------------------
        /**
         * the follows are constructors and destructors
         * you can also add some if needed.
        */
		iterator() : check_map(nullptr), bucket_idx(0) {}
		iterator(const hashmap* mp , int idx , typename double_list<value_type>::iterator it)
			:check_map(mp) , bucket_idx(idx) , list_it(it) {}
		iterator(const iterator &t) {
			check_map = t.check_map;
			bucket_idx = t.bucket_idx;
			list_it = t.list_it;
		}
		~iterator() = default;


        /**
		 * if point to nothing
		 * throw 
		*/
		value_type &operator*() const {
			if (!list_it.ptr) {
				throw sjtu::invalid_iterator();
			}
			return *list_it;
		}

        /**
		 * other operation
		*/
		value_type *operator->() const noexcept {
			if (!list_it.ptr) {
				throw sjtu::invalid_iterator();
			}
			return &(*list_it);
		}
		bool operator==(const iterator &rhs) const {
			return bucket_idx == rhs.bucket_idx && list_it == rhs.list_it;
    	}
		bool operator!=(const iterator &rhs) const {
			return bucket_idx != rhs.bucket_idx || list_it != rhs.list_it;
		}
	};



	virtual void clear(){
		if (current_size != 0) {
			for (auto &l : buckets) {
				l.clear();
			}
			current_size = 0;
		}
	}
	/**
	 * you need to expand the hashmap dynamically
	*/
	 void expand(){
		auto old_buckets = buckets;

		size_t new_bucket_count = buckets.size() * 2;
		buckets.clear();
		buckets.resize(new_bucket_count);
		current_size = 0;


		for (auto& p : old_buckets) {
			for (auto& node : p) {
				insert({node.first, node.second});
			}
		}
	}

    /**
     * the iterator point at nothing
    */
	iterator end()const {
		return iterator(this, buckets.size(), typename double_list<value_type>::iterator());	}
	/**
	 * find, return a pointer point to the value
	 * not find, return the end (point to nothing)
	*/
	iterator find(const Key &key)const{
		if (buckets[get_bucket_index(key)].empty()) {
			return end();
		}else {
			size_t idx = get_bucket_index(key);
			auto it = buckets[idx].begin();
			while (it != buckets[idx].end()) {
				if (equal_op((*it).first, key)) {
					return iterator(this, idx, it);
				}
				++it;
			}
			return end();
		}
	}
	/**
	 * already have a value_pair with the same key
	 * -> just update the value, return false
	 * not find a value_pair with the same key
	 * -> insert the value_pair, return true
	*/
	sjtu::pair<iterator,bool> insert(const value_type &value_pair){
		if (current_size / buckets.size() >= 1.0) {
			expand();
		}
		int tar_idx = get_bucket_index(value_pair.first);
		auto tmp_it = find(value_pair.first);
		if (tmp_it == end()) {
			buckets[tar_idx].insert_tail(value_pair);
			++current_size;
			return {iterator(this,tar_idx,--buckets[tar_idx].end()),true};
		}else {
			tmp_it->second = value_pair.second;
			return {tmp_it,false};
		}
	}
	/**
	 * the value_pair exists, remove and return true
	 * otherwise, return false
	*/
	bool remove(const Key &key){
		auto tmp_it = find(key);
		if (tmp_it == end()) {
			return false;
		}else {
			buckets[tmp_it.bucket_idx].erase(tmp_it.list_it);
			--current_size;
			return true;
		}
	}
};

// template<
// 	class Key,
// 	class T,
// 	class Hash = std::hash<Key>,
// 	class Equal = std::equal_to<Key>
// > class linked_hashmap :public hashmap<Key,T,Hash,Equal>{
//
// 	typedef pair<const Key, T> value_type;
// 	double_list<value_type*> entry_list;
// 	std::vector<double_list<typename double_list<value_type*>::iterator>> index_buckets;
// public:
//
//
// // --------------------------
// 	class const_iterator;
// 	class iterator{
// 	public:
// 		friend class linked_hashmap;
//
// 		typename double_list<value_type*>::iterator ptr;
//     // --------------------------
// 		iterator() = default;
// 		iterator(const iterator &other) : ptr(other.ptr) {}
// 		iterator(typename double_list<value_type*>::iterator p) : ptr(p) {}
//
// 		~iterator() = default;
//
// 		/**
// 		 * iter++
// 		 */
// 		iterator& operator++() { ++ptr; return *this; }
// 		iterator operator++(int) { iterator tmp = *this; ++ptr; return tmp; }
// 		iterator& operator--() { --ptr; return *this; }
// 		iterator operator--(int) { iterator tmp = *this; --ptr; return tmp; }
//
// 		/**
// 		 * if the iter didn't point to a value
// 		 * throw "star invalid"
// 		*/
// 		value_type* &operator*() const { return *ptr; }
// 		value_type* *operator->() const noexcept { return ptr.operator->(); }
//
// 		/**
// 		 * operator to check whether two iterators are same (pointing to the same memory).
// 		 */
// 		bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
// 		bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
// 		bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
// 		bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
//
// 		typename double_list<value_type*>::iterator get() {
// 			return ptr;
// 		}
// 	};
//
// 	class const_iterator {
// 		public:
//         	friend class linked_hashmap;
//
// 			typename double_list<value_type*>::iterator ptr;
//     // --------------------------
// 		const_iterator() = default;
// 		const_iterator(const iterator &other) : ptr(other.ptr) {}
// 		const_iterator(typename double_list<value_type*>::iterator p) : ptr(p) {}
// 		const_iterator(const_iterator &other) : ptr(other.ptr) {}
//
// 		~const_iterator() = default;
//
// 		const_iterator& operator++() { ++ptr; return *this; }
// 		const_iterator operator++(int) { iterator tmp = *this; ++ptr; return tmp; }
// 		const_iterator& operator--() { --ptr; return *this; }
// 		const_iterator operator--(int) { iterator tmp = *this; --ptr; return tmp; }
//
// 		/**
// 		 * if the iter didn't point to a value
// 		 * throw
// 		*/
//
// 		const value_type* &operator*() const { return *ptr; }
// 		const value_type* *operator->() const noexcept { return ptr.operator->(); }
//
// 		/**
// 		 * operator to check whether two iterators are same (pointing to the same memory).
// 		 */
// 		bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
// 		bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
// 		bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
// 		bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
// 	};
//
// 	linked_hashmap() {
// 		index_buckets.resize(8);
// 	}
// 	linked_hashmap(const linked_hashmap &other){
// 		this->buckets = other.buckets;
// 		this->current_size = other.current_size;
//
// 		entry_list = other.entry_list;
// 		index_buckets = other.index_buckets;
// 	}
// 	~linked_hashmap() override {
// 		linked_hashmap<Key, T, Hash, Equal>::clear();
// 	}
// 	linked_hashmap & operator=(const linked_hashmap &other) {
// 		this->clear();
// 		this->buckets = other.buckets;
// 		this->entry_list = other.entry_list;
// 		this->index_buckets = other.index_buckets;
//
// 		return *this;
// 	}
//
//  	/**
// 	 * return the value connected with the Key(O(1))
// 	 * if the key not found, throw
// 	*/
// 	T & at(const Key &key) {
// 		auto it = find(key);
// 		if (it == end()) {
// 			throw sjtu::runtime_error();
// 		}
// 		return *(*it).second;
// 	}
// 	const T & at(const Key &key) const {
// 		const auto it = find(key);
// 		if (it == end()) {
// 			throw sjtu::runtime_error();
// 		}
// 		return *(*it).second;
// 	}
// 	T & operator[](const Key &key) {
// 		auto it = find(key);
// 		if (it == end()) {
// 			throw sjtu::runtime_error();
// 		}
// 		return *(*it).second;
// 	}
// 	const T & operator[](const Key &key) const {
// 		const auto it = find(key);
// 		if (it == end()) {
// 			throw sjtu::runtime_error();
// 		}
// 		return *(*it).second;
// 	}
//
// 	/**
// 	 * return an iterator point to the first
// 	 * inserted and existed element
// 	 */
// 	iterator begin() { return iterator(entry_list.begin()); }
// 	const_iterator cbegin() const { return const_iterator(entry_list.begin()); }
// 	iterator end() { return iterator(entry_list.end()); }
// 	const_iterator cend() const { return const_iterator(entry_list.end()); }
//
// 	bool empty() const { return entry_list.empty(); }
// 	size_t size()  { return entry_list.size(); }
//
//     void clear () override{
// 		if (this->current_size != 0) {
// 			for (auto l : this->buckets) {
// 				l.clear();
// 			}
// 			this->current_size = 0;
// 		}
//
// 		entry_list.clear();
// 		this->buckets.clear();
// 		for (auto l : index_buckets) {
// 			l.clear();
// 		}
// 		index_buckets.clear();
// 	}
//
//  	/**
// 	 * insert the value_piar
// 	 * if the key of the value_pair exists in the map
// 	 * update the value instead of adding a new element，
//      * then the order of the element moved from inner of the
//      * list to the head of the list
// 	 * and return false
// 	 * if the key of the value_pair doesn't exist in the map
// 	 * add a new element and return true
// 	*/
// 	void expand () override{
// 		auto old_buckets = this->buckets;
// 		auto old_index_buckets = index_buckets;
//
// 		size_t new_bucket_count = this->buckets.size() * 2;
//
// 		this->buckets.clear();
// 		this->index_buckets.clear();
//
// 		this->buckets.resize(new_bucket_count);
// 		index_buckets.resize(new_bucket_count);
// 		this->current_size = 0;
//
//
// 		for (auto& p : old_buckets) {
// 			for (auto& node : p) {
// 				linked_hashmap::insert({node.first, node.second});
// 			}
// 		}
// 	}
//
// 	pair<iterator, bool> insert(value_type &value_pair) {
// 		if (this->current_size / this->buckets.size() >= 1.0) {
// 			expand();
// 		}
//
// 		int tar_idx = this->get_bucket_index(value_pair.first);
// 		auto tmp_it = hashmap<Key,T,Hash,Equal>::find(value_pair.first);
// 		if (tmp_it == hashmap<Key,T,Hash,Equal>::end()) {
// 			this->buckets[tar_idx].insert_tail(value_pair);
// 			entry_list.insert_tail(&value_pair);
// 			index_buckets[tar_idx].insert_tail(entry_list.back());
// 			++this->current_size;
// 			return {iterator(entry_list.back()),true};
// 		}else {
// 			int d = this->buckets[tar_idx].dis(tmp_it.list_it);
// 			typename double_list<value_type*>::iterator tmp = index_buckets[tar_idx].begin();
// 			while (d > 0) {
// 				++tmp;
// 				--d;
// 			}
// 			typename double_list<value_type*>::iterator tar_en_it = *tmp;
// 			entry_list.erase(tar_en_it);
// 			tmp_it->second = value_pair.second;
// 			entry_list.insert_tail(&value_pair);
// 			*tmp = entry_list.back();
//
// 			return (iterator(entry_list.back()),false);
// 		}
// 	}
//  	/**
// 	 * erase the value_pair pointed by the iterator
// 	 * if the iterator points to nothing
// 	 * throw
// 	*/
// 	void remove(typename linked_hashmap::iterator pos) {
// 		auto tar_key = (*pos)->first;
// 		entry_list.erase(pos.get());
//
// 		auto bu_it = hashmap<Key,T,Hash,Equal>::find(tar_key);
// 		int tar_idx = this->get_bucket_index(tar_key);
// 		int d = this->buckets[tar_idx].dis(bu_it);
// 		auto ib_it = index_buckets[tar_idx].begin();
// 		while (d > 0) {
// 			++ib_it;
// 			--d;
// 		}
//
// 		this->buckets[tar_idx].erase(bu_it);
// 		index_buckets[tar_idx].erase(ib_it);
// 	}
// 	/**
// 	 * return how many value_pairs consist of key
// 	 * this should only return 0 or 1
// 	*/
// 	size_t count(const Key &key) const {
// 		return find(key) == end() ? 0 : 1;
// 	}
// 	/**
// 	 * find the iterator points at the value_pair
// 	 * which consist of key
// 	 * if not find, return the iterator
// 	 * point at nothing
// 	*/
// 	iterator find(const Key &key) {
// 		auto it = hashmap<Key,T,Hash,Equal>::find(key);
// 		if (it == hashmap<Key,T,Hash,Equal>::end()) {
// 			return end();
// 		}else {
// 			int tar_idx = get_bucket_index(key);
// 			int d = this->buckets[tar_idx].dis(it);
// 			typename double_list<typename double_list<value_type*>::iterator>::iterator tmp = index_buckets[tar_idx].begin();
// 			while (d > 0) {
// 				++tmp;
// 				--d;
// 			}
// 			auto tar_it = *tmp;
//
// 			return iterator(tar_it);
// 		}
// 	}
//
// };

	template<
	class Key,
	class T,
	class Hash = std::hash<Key>,
	class Equal = std::equal_to<Key>
> class linked_hashmap : public hashmap<Key,T,Hash,Equal>{
public:
	typedef pair<const Key, T> value_type;

	double_list<value_type> entry_list;


	struct IndexNode {
		Key key;
		typename double_list<value_type>::node* entry_ptr;
		IndexNode() : entry_ptr(nullptr) {}
		IndexNode(const Key& k, typename double_list<value_type>::node* p) : key(k), entry_ptr(p) {}
	};
	std::vector<double_list<IndexNode>> index_buckets;
	size_t lhm_size;
	Hash lhm_hasher;
	Equal lhm_equal;

	static const size_t LHM_INIT_BUCKET = 16;
	static constexpr double LHM_MAX_LOAD = 0.75;

	size_t lhm_bucket_index(const Key& key) const {
		return lhm_hasher(key) % index_buckets.size();
	}

	// 在 index_buckets 中查找,返回 entry_list 中对应的 node*
	typename double_list<value_type>::node* index_find(const Key& key) const {
		if (index_buckets.empty()) return nullptr;
		size_t idx = lhm_bucket_index(key);
		auto it = index_buckets[idx].begin();
		while (it != index_buckets[idx].end()) {
			if (lhm_equal((*it).key, key)) {
				return (*it).entry_ptr;
			}
			++it;
		}
		return nullptr;
	}

	// 在 index_buckets 中插入
	void index_insert(const Key& key, typename double_list<value_type>::node* ptr) {
		if (index_buckets.empty()) index_buckets.resize(LHM_INIT_BUCKET);
		size_t idx = lhm_bucket_index(key);
		index_buckets[idx].insert_tail(IndexNode(key, ptr));
	}

	// 在 index_buckets 中删除
	void index_remove(const Key& key) {
		if (index_buckets.empty()) return;
		size_t idx = lhm_bucket_index(key);
		auto it = index_buckets[idx].begin();
		while (it != index_buckets[idx].end()) {
			if (lhm_equal((*it).key, key)) {
				index_buckets[idx].erase(it);
				return;
			}
			++it;
		}
	}

	// 更新 index_buckets 中某个 key 的 entry_ptr
	void index_update_ptr(const Key& key, typename double_list<value_type>::node* new_ptr) {
		if (index_buckets.empty()) return;
		size_t idx = lhm_bucket_index(key);
		auto it = index_buckets[idx].begin();
		while (it != index_buckets[idx].end()) {
			if (lhm_equal((*it).key, key)) {
				(*it).entry_ptr = new_ptr;
				return;
			}
			++it;
		}
	}

	// index_buckets 扩容
	void index_expand() {
		size_t new_size = index_buckets.size() * 2;
		std::vector<double_list<IndexNode>> new_buckets(new_size);
		for (size_t i = 0; i < index_buckets.size(); ++i) {
			auto it = index_buckets[i].begin();
			while (it != index_buckets[i].end()) {
				size_t new_idx = lhm_hasher((*it).key) % new_size;
				new_buckets[new_idx].insert_tail(*it);
				++it;
			}
		}
		index_buckets = std::move(new_buckets);
	}

// --------------------------
	class const_iterator;
	class iterator{
	public:
		typename double_list<value_type>::iterator ptr;
		friend class linked_hashmap;

    // --------------------------
		iterator() = default;
		iterator(const iterator &other) : ptr(other.ptr) {}
		iterator(typename double_list<value_type>::iterator p) : ptr(p) {}
		~iterator() = default;

		iterator& operator=(const iterator &other) {
			ptr = other.ptr;
			return *this;
		}

		iterator operator++(int) {
			iterator tmp = *this;
			++ptr;
			return tmp;
		}
		iterator &operator++() {
			++ptr;
			return *this;
		}
		iterator operator--(int) {
			iterator tmp = *this;
			--ptr;
			return tmp;
		}
		iterator &operator--() {
			--ptr;
			return *this;
		}

		value_type &operator*() const {
			if (!ptr.ptr || !ptr.check || ptr.ptr == ptr.check->tail || ptr.ptr == ptr.check->head) {
				throw sjtu::invalid_iterator();
			}
			return *ptr;
		}
		value_type *operator->() const noexcept {
			return ptr.operator->();
		}

		bool operator==(const iterator &rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator!=(const iterator &rhs) const {
			return ptr != rhs.ptr;
		}
		bool operator==(const const_iterator &rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator!=(const const_iterator &rhs) const {
			return ptr != rhs.ptr;
		}
	};

	class const_iterator {
	public:
		typename double_list<value_type>::iterator ptr;
		friend class linked_hashmap;

    // --------------------------
		const_iterator() = default;
		const_iterator(const iterator &other) : ptr(other.ptr) {}
		const_iterator(const const_iterator &other) : ptr(other.ptr) {}
		const_iterator(typename double_list<value_type>::iterator p) : ptr(p) {}
		~const_iterator() = default;

		const_iterator& operator=(const const_iterator &other) {
			ptr = other.ptr;
			return *this;
		}
		const_iterator& operator=(const iterator &other) {
			ptr = other.ptr;
			return *this;
		}

		const_iterator operator++(int) {
			const_iterator tmp = *this;
			++ptr;
			return tmp;
		}
		const_iterator &operator++() {
			++ptr;
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator tmp = *this;
			--ptr;
			return tmp;
		}
		const_iterator &operator--() {
			--ptr;
			return *this;
		}

		const value_type &operator*() const {
			if (!ptr.ptr || !ptr.check || ptr.ptr == ptr.check->tail || ptr.ptr == ptr.check->head) {
				throw sjtu::invalid_iterator();
			}
			return *ptr;
		}
		const value_type *operator->() const noexcept {
			return ptr.operator->();
		}

		bool operator==(const iterator &rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator!=(const iterator &rhs) const {
			return ptr != rhs.ptr;
		}
		bool operator==(const const_iterator &rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator!=(const const_iterator &rhs) const {
			return ptr != rhs.ptr;
		}
	};

	linked_hashmap() : lhm_size(0) {
		index_buckets.resize(LHM_INIT_BUCKET);
	}

	linked_hashmap(const linked_hashmap &other) : lhm_size(0) {
		index_buckets.resize(LHM_INIT_BUCKET);

		auto it = other.entry_list.begin();
		while (it != other.entry_list.end()) {
			insert(*it);
			++it;
		}
	}

	~linked_hashmap() {
		//linked_hashmap<Key, T, Hash, Equal>::clear();
	}

	linked_hashmap & operator=(const linked_hashmap &other) {
		if (this == &other) return *this;
		clear();
		auto it = other.entry_list.begin();
		while (it != other.entry_list.end()) {
			insert(*it);
			++it;
		}
		return *this;
	}

	T & at(const Key &key) {
		auto node_ptr = index_find(key);
		if (!node_ptr) {
			throw sjtu::index_out_of_bound();
		}
		return node_ptr->data.second;
	}

	const T & at(const Key &key) const {
		auto node_ptr = index_find(key);
		if (!node_ptr) {
			throw sjtu::index_out_of_bound();
		}
		return node_ptr->data.second;
	}

	T & operator[](const Key &key) {
		auto node_ptr = index_find(key);
		if (!node_ptr) {
			throw sjtu::index_out_of_bound();
		}
		return node_ptr->data.second;
	}

	const T & operator[](const Key &key) const {
		auto node_ptr = index_find(key);
		if (!node_ptr) {
			throw sjtu::index_out_of_bound();
		}
		return node_ptr->data.second;
	}

	iterator begin() {
		return iterator(entry_list.begin());
	}
	const_iterator cbegin() const {
		return const_iterator(entry_list.begin());
	}
	iterator end() {
		return iterator(entry_list.end());
	}
	const_iterator cend() const {
		return const_iterator(entry_list.end());
	}

	bool empty() const {
		return lhm_size == 0;
	}

	void clear(){
		entry_list.clear();
		for (size_t i = 0; i < index_buckets.size(); ++i) {
			index_buckets[i].clear();
		}
		index_buckets.clear();
		index_buckets.resize(LHM_INIT_BUCKET);
		lhm_size = 0;

		hashmap<Key,T,Hash,Equal>::clear();
	}

	size_t size() const {
		return lhm_size;
	}


	pair<iterator, bool> insert(const value_type &value) {
		if (index_buckets.empty()) index_buckets.resize(LHM_INIT_BUCKET);

		auto existing = index_find(value.first);

		if (existing) {
			existing->data.second = value.second;
			entry_list.move_to_head(existing);

			return pair<iterator, bool>(iterator(typename double_list<value_type>::iterator(existing, &entry_list)), false);
		} else {
			if ((double)(lhm_size + 1) / index_buckets.size() > LHM_MAX_LOAD) {

				index_expand();

			}

			entry_list.insert_head(value);
			auto new_node = entry_list.head->next;
			index_insert(value.first, new_node);
			hashmap<Key,T,Hash,Equal>::insert(value);

			++lhm_size;

			return pair<iterator, bool>(iterator(typename double_list<value_type>::iterator(new_node, &entry_list)), true);
		}
	}


	void remove(iterator pos) {
		if (pos == end()) {
			throw sjtu::invalid_iterator();
		}
		Key key = (*pos).first;
		index_remove(key);
		entry_list.erase(pos.ptr);
		hashmap<Key,T,Hash,Equal>::remove(key);

		--lhm_size;
	}

	size_t count(const Key &key) const {
		return index_find(key) != nullptr ? 1 : 0;
	}

	iterator find(const Key &key) {
		auto node_ptr = index_find(key);
		if (!node_ptr) {
			return end();
		}
		return iterator(typename double_list<value_type>::iterator(node_ptr, &entry_list));
	}
};


class lru{
    using lmap = sjtu::linked_hashmap<Integer,Matrix<int>,Hash,Equal>;
    using value_type = sjtu::pair<const Integer, Matrix<int> >;

    mutable lmap cache;
    int capacity;

public:
    lru(int size) : capacity(size) {}

    ~lru() {}


    void save(const value_type &v) const {
        auto existing = cache.index_find(v.first);
        if (existing) {
            existing->data.second = v.second;
            cache.entry_list.move_to_head(existing);
        } else {
            if ((int)cache.size() >= capacity) {
            	//尾部最不常用
                auto tail_node = cache.entry_list.tail->prev;
                auto tail_key = tail_node->data.first;
                cache.index_remove(tail_key);
                cache.entry_list.erase(
                    typename double_list<value_type>::iterator(tail_node, &cache.entry_list)
                );
                cache.lhm_size--;
                cache.hashmap<Integer,Matrix<int>,Hash,Equal>::remove(tail_key);
            }
            cache.entry_list.insert_head(v);
            auto new_node = cache.entry_list.head->next;
            cache.index_insert(v.first, new_node);
            cache.lhm_size++;
            cache.hashmap<Integer,Matrix<int>,Hash,Equal>::insert(v);

            if ((double)(cache.lhm_size + 1) / cache.index_buckets.size() > 0.75) {
                cache.index_expand();
            }
        }
    }


    Matrix<int>* get(const Integer &v) const {
        auto node_ptr = cache.index_find(v);
        if (!node_ptr) {
            return nullptr;
        }
        //标记为最近使用
        cache.entry_list.move_to_head(node_ptr);
        return &(node_ptr->data.second);
    }


    void print() {
        auto it = cache.entry_list.begin();
        while (it != cache.entry_list.end()) {
            std::cout << (*it).first.val << " " << (*it).second;
            ++it;
        }
    }
};
}

#endif