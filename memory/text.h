#include<iostream>
#include<unordered_map>
#include<vector>
#include<cstddef>
#include<cstdlib>
#include<cassert>
#include<new>
#include<utility>
#include <algorithm>
using namespace std;

class Hashmapmemory {
public:
    Hashmapmemory(size_t slabbyte = 64*1024): slabbytes_(slabbyte) {}// 声明一块内存大小
    ~Hashmapmemory() {
        for (auto &p : allocinfo_) {
            if (p.second.large && p.first) {
                std::free(p.first);
            }
        }

        // 释放所有 slab 的物理内存（小块由这些 slab 切分而来）
        for (auto &kv : slabs_) {
            for (auto &s : kv.second) {
                std::free(s.data);
            }
        }
    }


    void* allocate(size_t usersize);//分配
    void deallocate(void* ptr);//删除
    // 在内存池上进行 placement-new：构造对象并返回 T*
    template<typename T, typename... Args>
    T* construct(Args&&... args) {
        void* mem = allocate(sizeof(T));                          // 分配 T 所需字节
        return new (mem) T(std::forward<Args>(args)...);          // 原地构造
    }

    // 调用对象析构并将内存归还池
    template<typename T>
    void destroy(T* obj) {
        if (!obj) return;                                         // 空指针无操作
        obj->~T();                                                // 显式调用析构
        deallocate(static_cast<void*>(obj));                      // 归还内存
    }

    void dumpStats() const {
        std::cout << "===== MemoryPool stats =====\n";
        std::cout << "slab bytes    : " << slabbytes_<< "\n";                       // 每个 slab 的大小
        std::cout << "num allocations outstanding: " << allocinfo_.size() << "\n"; // 未释放块数（含大块/小块）
        std::cout << "size-classes (chunkSize : #slabs) :\n";                        // 每个大小类对应的 slab 数量
        for (auto &kv : slabs_) {
            std::cout << "  " << kv.first << " : " << kv.second.size() << "\n";
        }
        std::cout << "============================\n";
    }
private:
    struct slab {
        void* data;  //内存起始地址
        size_t totalbyties;//申请内存总大小
        size_t chunksize;//每个块的内存大小
        size_t chununks;//分配的块数
    };

    struct Allocinfo {
        size_t chunksize;//每个块的大小，用于分辨大小类
        size_t userchunk;//用户期待的内存大小
        bool large;//判断是否为内存池申请
    };
    size_t slabbytes_;//申请内存大小
    unordered_map<size_t,void*> freelist_;//每个大小类的头指针
    unordered_map<size_t,vector<slab>> slabs_;//记录申请的内存池
    unordered_map<void*,Allocinfo> allocinfo_;//记录申请的地址,大块为0

    static size_t roundpow2(size_t x);

     void creatslab(size_t chunksize);
};