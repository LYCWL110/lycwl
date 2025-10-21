#include "text.h"

#include<cassert>
#include<cstddef>
#include<cstdlib>
#include<iostream>
#include<new>
#include<unordered_map>
#include<utility>
#include<vector>
#include <algorithm>
using namespace std;

size_t Hashmapmemory::roundpow2(size_t x) {
    size_t p = 1;
    while (p<x) p<<=1;
    return p;
}

void Hashmapmemory:: creatslab(size_t chunksize) {
    size_t chunks = slabbytes_/chunksize;
    if (chunks ==0) chunks = 1;//计算需要多少块内存
    size_t allobytes = chunks*chunksize;
    //分配内存
    void* data = malloc(allobytes);
    if (!data) throw std::bad_alloc();

    slab s{data,allobytes,chunksize,chunks};
    slabs_[chunksize].push_back(s);//记录slab

    void *head = nullptr;//插入表头
    auto it = freelist_.find(chunksize);
    if (it != freelist_.end()) head = it->second;

    char* base = (char*)data;//标注
    for (size_t i=0;i<chunks;i++) {
        void* solt = base + i*chunksize;
        reinterpret_cast<void**>(solt)[0] = head;
        head = solt;
    }
    freelist_[chunksize] = head;
}

void *Hashmapmemory::allocate(size_t usersize) {
    if (usersize ==0)  return nullptr;

//找到所需内存大小
   size_t chunksize = Hashmapmemory::roundpow2(max(usersize,sizeof(void*)));
//寻找free中的表头
    if (usersize > slabbytes_ / 2) {
        void* p = std::malloc(usersize);
        if (!p) throw std::bad_alloc(); // 失败抛异常
        allocinfo_[p] = Allocinfo{0, usersize, true}; // 记录为大块
        return p;
    }

    void *head = nullptr;
    auto it = freelist_.find(chunksize);
    if (it != freelist_.end()) head = it->second;

    if (!head) {
        creatslab(chunksize);
        head = freelist_[chunksize];
      if (!head)  throw std::bad_alloc(); //检查是否正常申请内存

    }
    //弹出一个块，声明next存放下一指针的头地址，传给freelist
    void* next = reinterpret_cast<void**>(head)[0];
    freelist_[chunksize] = next;

    allocinfo_[head]= Allocinfo{chunksize,usersize,false};
     return head;
}

void Hashmapmemory::deallocate(void* data) {
    if (!data) return;
    auto it = allocinfo_.find(data);//确认是否存在
    if (it == allocinfo_.end()) {
        std::cerr << "[Error] deallocate: unknown or double-free pointer " << data << "\n";
        return; // 或者 throw std::runtime_error("deallocate: unknown pointer")
    }

    Allocinfo info = it->second;//获取large
    allocinfo_.erase(it);

    if (info.large) {
        free(data);//删除大块
        return;
    }
    void* curhead = nullptr;
    auto it2 = freelist_.find(info.chunksize);
    if (it2 != freelist_.end()) curhead = it2->second;//小块找到删除的地址

    reinterpret_cast<void**>(data)[0] = curhead;
    freelist_[info.chunksize] = data;

}