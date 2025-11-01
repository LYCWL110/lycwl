#include <unordered_map>
using namespace std;
class lfu{
private:
    struct node{
        int key;
        int value;
        int freq;
        node* pre;
        node* next;
        node(int k,int y,int f);
    };
    struct mylist{
        node*L;
        node* R;
        int freq;
        mylist(int f);
    };



    int n;
    int size = 0;
    unordered_map<int,node*> key_table;
    unordered_map<int,mylist*>fre_list;
    
public:
    lfu(int capacity);
    void remove(node*node);
    void insert(node*node);
    int get(int key);
    void put(int key,int value);

};