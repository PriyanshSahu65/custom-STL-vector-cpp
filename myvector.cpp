// Custom STL-style vector implementation (~95%)
// Built with allocator, iterator, deep copy

#include <iostream>
#include <stdexcept>
#include <new>
#include <memory>
using namespace std;

// custome allocator class
template <typename T>
class myallocator {
public:
    using value_type = T; // typedef T value_type;


    myallocator(){};

    T* allocate(size_t n){
        return static_cast<T*>(::operator new(n*sizeof(T)));
    }

    void deallocate(T* ptr,size_t /*n*/){
        ::operator delete(ptr);
    }

    void construct(T* ptr, const T& val){
        new(ptr) T(val); //placement new 
    }

    void destroy(T* ptr){
        ptr->~T(); //destruct
    }
};

// My custome vector class ----------
template<typename T, typename Alloc = myallocator<T>>
class vector {
private:
    T* data;
    size_t sz;
    size_t cap;
    Alloc alloc;

    void resize(size_t new_cap) {
        
        // T* new_data = new T[new_cap]; // new array wi new cap
        T* new_data = alloc.allocate(new_cap);

        for (size_t i = 0; i < sz; ++i) {

            // new_data[i] = data[i]; <-- when using new;

            alloc.construct(&new_data[i], data[i]);
            alloc.destroy(&data[i]);

        }

        //  delete[] data; //del array of data wi []
        
        alloc.deallocate(data,cap);

        data = new_data;
        cap = new_cap;

    }

public:
    vector() : data(nullptr), sz(0), cap(0) {}

    vector(size_t n){
        
        // data = new T[n];
        
        data = alloc.allocate(n);
        
        sz=cap=n;
        for(size_t i=0;i<n;++i){
            
            //data[i] = T(); //def ini
        
            alloc.construct(&data[i],T());
        }
    }

    vector(size_t n, const T& val){

        //data = new T[n];
        data = alloc.allocate(n); //bought the plot

        sz=cap=n;
        for(size_t i=0;i<n;++i){

            // data[i] = val;
            alloc.construct(&data[i],val); // <-- Constructed the house
        }
    }

    vector(const vector& other){
        sz = other.sz;
        cap = other.cap;

        // data = new T[cap];
        data = alloc.allocate(cap);
        
        for(size_t i=0;i<sz;i++){
            
            // data[i]=other.data[i];
            alloc.construct(&data[i],other.data[i]);

        }
    }

    ~vector() {

        // delete[] data;
        for(size_t i = 0; i<sz; ++i){
            alloc.destroy(&data[i]);
        }
        alloc.deallocate(data,cap);
    }

    void push_back(const T& val) {
        if (sz == cap) {
            size_t new_cap = (cap == 0) ? 1 : cap * 2;
            resize(new_cap);
        }

        //data[sz++] = val;
        alloc.construct(&data[sz++],val);
    }

    void pop(){
        if(sz==0){
            throw out_of_range("Vector is emplty");
        }
        // sz--;
        alloc.destroy(&data[--sz]);
    }

    bool empty() const {return sz==0;}

    void clear(){
        for(size_t i =0;i<sz;++i){
            alloc.destroy(&data[i]);
        }
        sz=0; //we are keeping capacity here
    }

    T& front(){
        if(sz==0){
            throw out_of_range("Vector is emptty!");
        }else{
            return data[0];
        }
    }

    T& back(){
        if(sz==0){
            throw out_of_range("Vector is emptty!");
        }else{
            return data[sz-1];
        }
    }

    T& operator[](size_t index) {
        if (index >= sz)
            throw out_of_range("Index out of range");
        return data[index];
    }

    vector& operator=(const vector& other){
        if(this != &other){// mesa not assigned to mesa-- wesa go deepcopy

            for(size_t i=0;i<sz;++i){
                // Destroy current ele
                alloc.destroy(&data[i]);
            }

            //deloc curr mem
            alloc.deallocate(data,cap);

            //alloc and cpy from other
            sz = other.sz;
            cap = other.cap;
            data =alloc.allocate(cap);

            for(size_t i =0;i<sz;++i){
                alloc.construct(&data[i],other.data[i]);
            }
        }
        return *this;
    }

    void reserve(size_t new_cap) {
        if (new_cap > cap) { // safty check 
            T* new_data = alloc.allocate(new_cap);

            for (size_t i = 0; i < sz; ++i) {
                alloc.construct(&new_data[i], data[i]);
                alloc.destroy(&data[i]);
            }

            alloc.deallocate(data, cap);

            data = new_data;
            cap = new_cap;
        }
    }

    size_t size() const { return sz; }
    size_t capacity() const { return cap; }

    template <typename... Args>// any++ datatypes
    void emplace_back(Args&&... args){ // any para
        if(sz==cap){
            size_t new_cap = (cap==0)?1:cap*2;
            resize(new_cap);
        }

        // below: forwarding the constructor argument to const inplace
        alloc.construct(&data[sz],T(std::forward<Args>(args)...));
        ++sz;
    }


// My custome itterator class ----------
    class iterator{
    private:
        T* ptr; // a ptr to data;
    public:
        iterator(T* p) : ptr(p) {};

        T& operator*() const {return *ptr;}; //dereference
        iterator& operator++() {// ++it , &-> no dupli, inplace modification (as ref)
            ++ptr;
            return *this;
        }
        // (int) <-- system passes a 0 its a Overload 
        iterator operator++(int){// it++ Has to return a old value for computation not just ++ it
            iterator temp = *this; 
            ++ptr;
            return temp;
        }
        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }
        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }
    };

    iterator begin() const {
        return iterator(data);
    }
    iterator end() const{
        return iterator(data+sz);
    }
};

int main(){

//  ₍^. .^₎⟆    ~95% memberfx of original STL-vector

/* 
    Constructors:
    1. vector()
    2. vector(size_t n)
    3. vector(size_t n, const T& val)
    4. vector(const vector& other)
*/

/*
    Assignment:
    1. vector& operator=(const vector& other)
*/

/*
    Modifiers:
    1. void push_back(const T& val)
    2. void emplace_back(Args&&... args)
    3. void pop()
    4. void clear()
    5. void reserve(size_t new_cap)
*/

/*
    Element Access:
    1. T& operator[](size_t index)
    2. T& front()
    3. T& back()
*/

/*
    Capacity:
    1. size_t size() const
    2. size_t capacity() const
    3. bool empty() const
*/

/*
    Iterators:
    1. iterator begin() const
    2. iterator end() const
*/

}