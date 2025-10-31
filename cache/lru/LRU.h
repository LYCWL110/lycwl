#include<map>
#include<unordered_map>
#include<malloc.h>
using namespace std;
class lru{
    private:
    struct mylist{
        int key;
        int value;
        mylist*pre;
        mylist*next;
        mylist(int k,int v);
    };

    mylist*L;
    mylist*R;

    int n;
    unordered_map<int,mylist*>hash;

    void remove(mylist*node);
    void insert(int key,int value); 

    public:
    lru(int capacity);
    int get(int key);
    void put(int key,int value);
};

