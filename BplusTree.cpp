#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;


// --- B+ Tree --- //
class Btree
{
    public:
        Btree(int input_order);

        Insert(int value);



    

    // --- Private 部分 --- //
    private:
        int page_size; // 兩倍的order

        // ------------------------------------------------------ //
        // - - - - - - - - - - - - 節點 - - - - - - - - - - - - - //
        // ------------------------------------------------------ //
        class page{
            public:
                page();
                virtual ~page(){}

                int capacity(){return key.size();}
                set_type(string type){page_type = type;}
                string get_type(){return page_type;}
                push(int value);
                
            private:
                string page_type; 
                vector<int> key;
        };

        // --------------------------------------------------------- //
        // - - - - - - - - - - - - - 路標節點 - - - - - - - - - - - - //
        // --------------------------------------------------------- //
        class index_page : public page
        {
            public:
                index_page(Btree btree);

            private:
                page **ptr;
        };


        // --------------------------------------------------------- //
        // - - - - - - - - - - - - - 資料節點 - - - - - - - - - - - - //
        // --------------------------------------------------------- //
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




// ################################################# //
    // ---------------- 主要程式 ---------------- //
// ################################################# //
int main()
{
    cout << "Hello!" << endl ;
    Btree tree(2);
    tree.Insert(5);
    tree.Insert(3);
    tree.Insert(9);
    tree.Insert(8);
    tree.Insert(1);
    return 0;
}


// ################################################# //
    // ---------------- 函式定義 ---------------- //
// ################################################# //

Btree::Btree(int input_order)
{
    page_size = input_order * 2;
    leaf_page *leaf = new leaf_page(*this);
    root = leaf;
    //leaf_page *leaf = dynamic_cast<leaf_page*>(root);

}

Btree::Insert(int value)
{
    cout << "Try to Insert value: " << value << endl;

    page *temp = root;

    if (temp->get_type() == "leaf"){ // 確認為資料節點
        
        temp->push(value);
        if (temp->capacity() > page_size){ // 檢查是否塞得進去
            cout << "overflow ! current size: " << temp->capacity() << endl;
        }

    }



}



Btree::page::page(){

}

Btree::page::push(int value)
{
    key.push_back(value);
    sort(key.begin(), key.end());
    for (int i = 0; i < key.size(); i++) {
        cout << key[i] << " ";
    }
    cout << endl;
}



Btree::index_page::index_page(Btree btree)
{
    ptr = new page*[btree.page_size*2 + 1];
    set_type("index");
    cout << "Create a Index Page with key size "<< btree.page_size << endl;
}


Btree::leaf_page::leaf_page(Btree btree)
{
    set_type("leaf");
    cout << "Create a Leaf Page with key size "<< btree.page_size << endl;
}
