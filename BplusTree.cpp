#include <iostream>
#include <vector>
using namespace std;



class Btree
{
    public:
        Btree(int input_order);



    // ---- B+ Tree pivate ---- // 
    private:
        int order;
        
    class page{
        public:
            page();
            virtual ~page(){}
            int capacity(){return key.size();}
            
        private:
            vector<int> key;
    };

    class index_page : public page
    {
        public:
            index_page(Btree btree);

        private:
            page **ptr;
    };

    class leaf_page : public page
    {
        public:
            leaf_page(Btree btree);

        private:
            leaf_page *pre_page;
            leaf_page *next_page;
    };

    page *root;
};

int main()
{
    cout << "Hello!" << endl ;
    Btree tree(5);
    return 0;
}



// ################ 函式定義 ################ //

Btree::Btree(int input_order){
    order = input_order;
    root = new leaf_page(*this);
    leaf_page *leaf = dynamic_cast<leaf_page*>(root);

}


Btree::page::page(){

}


Btree::index_page::index_page(Btree btree){
    ptr = new page*[btree.order + 1];
    cout << "Create a Index Page with key size "<< btree.order << endl;
}


Btree::leaf_page::leaf_page(Btree btree){
    cout << "Create a Leaf Page with key size "<< btree.order << endl;
}