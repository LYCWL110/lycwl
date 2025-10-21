/*#include <iostream>
#include <vector>

using namespace std;

struct memstr {
    void* head;
    bool memp;
};

class mymemory {

public:
    vector<memstr> mes;
        mymemory(int a,int b) {
            block_size = a;
            block_count = b;

            for (int i =0;i<block_count;i++) {
                memstr mm;
                mm.head = malloc(block_size);
                mm.memp = false;
                mes.push_back(mm);
            }
        }
    void add(void*&p) {
            for (int i =0;i<mes.size();i++) {
                if (!mes[i].memp) {
                    mes[i].memp = true;
                    p = mes[i].head;
                }
            }
        }
    void del(void*p) {
            for (int i =0;i<mes.size();i++) {
            if (p == mes[i].head) {
                mes[i].memp = false;
            }
            }
        }

     ~mymemory() {
            for (int i =0;i<mes.size();i++) {
                if (mes[i].memp) {
                    free(mes[i].head);
                }
            }
        }

private:
    int block_size;
    int block_count;

};


int main() {

}*/


#include <iostream>

#
#include "cmake-build-debug/text.h"

struct Foo {
    int a;
    double b;
    Foo(int aa, double bb) : a(aa), b(bb) {
        std::cout << "Foo constructed\n";
    }
    ~Foo() {
        std::cout << "Foo destructed\n";
    }
};

int main() {
    Hashmapmemory pool(64 * 1024);


    Foo* f = pool.construct<Foo>(42, 3.14);
    std::cout << "f->a=" << f->a << " f->b=" << f->b << "\n";


    pool.destroy(f);


    void* p1 = pool.allocate(24);

    void* p2 = pool.allocate(10000);


    pool.deallocate(p1);
    pool.deallocate(p2);

    pool.dumpStats();

    return 0;
}
