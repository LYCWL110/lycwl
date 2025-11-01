#include"lfu.h"

lfu::node::node(int k,int y,int f){
    key = k;
    value = y;
    freq = f;
    pre = nullptr;
    next = nullptr;

}

lfu::mylist::mylist(int f){
    freq = f;
    L = new node(-1,-1,-1);
    R = new node(-1,-1,-1);
    L->next = R;
    R->pre = L;
}

lfu::lfu(int capacity){
    n = capacity;
    size = 0;
   
    
}

void lfu::remove(node* nd){
    node* mynode = nd;
    mynode->pre->next = mynode->next;
    mynode->next->pre = mynode->pre;
}

void lfu::insert(node* nd){
    int freq = nd->freq;
    if(fre_list.find(freq) == fre_list.end()){
       fre_list[freq] = new mylist(freq);
    }
    mylist* mylistnode = fre_list[freq];

    node* pre = mylistnode->R->pre;
    node* next = mylistnode->R;
    pre->next = nd;
    nd->next = next;
    next->pre = nd;
    nd->pre = pre; 
    
}

int lfu::get(int key){
   if(key_table.find(key) != key_table.end()){
       node* mynode = key_table[key];
       remove(mynode);
       mynode->freq += 1;
       if(fre_list[size]->L->next == fre_list[size]->R){
           size++;
       }
       insert(mynode);
       return mynode->value;
   }else{
       return -1;
   }
}

void lfu::put(int key,int value){
    if(n==0) return ;

    if(get(key) ==-1) {
        key_table[key]->value = value;
    }else{
        if(key_table.size() == n){
            node*node = fre_list[size]->L->next;
            remove(node);
            key_table.erase(node->key);
        }
    }
    node* mynode =new node(key,value,1);
    key_table[key] = mynode;
    size =1;
    insert(mynode);


}