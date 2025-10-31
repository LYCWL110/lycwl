#include "LRU.h"
lru::mylist::mylist(int k,int v)
{
    key = k;
    value = v;
    pre = nullptr;
    next = nullptr;
}

lru::lru(int capacity){
    n = capacity;
    L = new mylist(-1,-1);
    R = new mylist(-1,-1);
    L->next = R;
    R->pre = L;
}

void lru::remove(mylist* node){
    mylist* node1 = node;
    node1->pre->next = node1->next;
    node1->next->pre = node1->pre;
    hash.erase(node1->key);
}

void lru::insert(int key,int value){
    mylist* node = new mylist(key,value);
    node->next = R;
    node->pre = R->pre;
    R->pre->next = node;
    R->pre = node;
    hash[key] = node;
}

int lru :: get(int key){
    if(hash.find(key)!=hash.end()){
        mylist* node = hash[key];
        int a = node->value;
        remove(node);
        insert(key,a);
        return a;
    }else{
        return -1;
    }
}

void lru::put(int key,int value){
    if(hash.find(key)!=hash.end()){
        mylist* node = hash[key];
        remove(node);
        insert(key,value);
    }else{
        if(hash.size()==n){
            remove(L->next);
            hash.erase(L->next->key);
        }
        insert(key,value);
        
    }
}