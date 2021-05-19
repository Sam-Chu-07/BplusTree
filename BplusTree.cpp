#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;

// ------------------------------------------------------ //
// - - - - - - - - - - - - 節點 - - - - - - - - - - - - - //
// ------------------------------------------------------ //
class page{
    public:
        page();
        virtual ~page(){}
        friend class Btree;

        set_type(string type){page_type = type;}
        string get_type(){return page_type;}
        int push(int value);

    private:
        string page_type; 
        vector<int> key;
        page* parent;
};

// --------------------------------------------------------- //
// - - - - - - - - - - - - - 路標節點 - - - - - - - - - - - - //
// --------------------------------------------------------- //
class index_page : public page
{
    public:
        index_page();
        friend class Btree;

    private:
        vector<page*> ptr;
};


// --------------------------------------------------------- //
// - - - - - - - - - - - - - 資料節點 - - - - - - - - - - - - //
// --------------------------------------------------------- //
class leaf_page : public page
{
    public:
        leaf_page();
        friend class Btree;

    private:
        leaf_page *pre_page;
        leaf_page *next_page;
};



// --------------------------------------------------------- //
// - - - - - - - - - - - - - B+ TREE - - - - - - - - - - - - //
// --------------------------------------------------------- //
class Btree
{
    public:
        Btree(int input_order);

        Display(){display_tree(root);}

        Insert(int value);

        push_up(page* origin_page, page* new_page, int push_key);

        display_tree(page* current);
            
    private:
        int page_size; // 兩倍的order
        page *root;
};




// ################################################# //
    // ---------------- 主要程式 ---------------- //
// ################################################# //
int main()
{
    cout << "Hello!" << endl ;
    int order = 2, input = 0;
    cin >> order;
    Btree tree(order);
    while(input != -1){
        cin >> input;
        if (input == 0){
            tree.Display();
            cout << endl;
        }else{
            tree.Insert(input);
        }
    }


    /*
    Btree tree(2);
    tree.Insert(5);
    tree.Insert(3);
    tree.Insert(9);
    tree.Insert(8);
    tree.Insert(1);
    */
    return 0;
}


// ################################################# //
    // ---------------- 函式定義 ---------------- //
// ################################################# //

Btree::Btree(int input_order)
{
    page_size = input_order * 2;
    leaf_page *leaf = new leaf_page;
    root = leaf;

}

Btree::display_tree(page* current)
{
    if (current->get_type() == "index"){
        
        index_page *index = dynamic_cast<index_page*>(current);
        for(int i=0 ; i<index->ptr.size(); i++){
            display_tree(index->ptr[i]);
            if(i<index->key.size())
                cout << index->key[i] << " ; ";
        }

    } else if (current->get_type() == "leaf"){
        
        leaf_page *leaf = dynamic_cast<leaf_page*>(current);
        for(int i=0 ; i<leaf->key.size() ; i++)
            cout << leaf->key[i] << " ";
         cout << "; " ;
    }
}


Btree::Insert(int value)
{
    cout << "Try to Insert value: " << value << endl;

    int position;
    page *current = root;
    index_page *index;
    vector<int>::iterator itr;

    for(int x=0 ; x<10 ; x++)  // 先執行10次
    {
        if (current->get_type() == "leaf"){ // 確認為資料節點
            
            leaf_page *leaf = dynamic_cast<leaf_page*>(current);

            leaf->push(value); // 塞入資料
            if (leaf->key.size() > page_size){ // 檢查是否超過資料節點的容量

                cout << "\noverflow ! current leaf page size: " << current->key.size() << "\n" <<endl;
                leaf_page *temp = new leaf_page;
                temp->parent = leaf->parent;
                temp->key.assign(leaf->key.begin() + page_size/2, leaf->key.end());
                leaf->key.erase(leaf->key.begin() + page_size/2, leaf->key.end());
                push_up(leaf, temp, temp->key[0]);
            }
            break;

        } else if (current->get_type() == "index"){ // 目前為路標節點

            index = dynamic_cast<index_page*>(current);

            for( itr =index->key.begin() ; itr != index->key.end() ; itr++){ // 使用路標找尋合適的位置
                if (*itr > value)
                    break;
            }

            if (itr != index->key.end())
                position = itr - index->key.begin();
            else
                position = index->ptr.size() - 1;

            current = index->ptr[position];

        } else { cout << "Error! page neither Index page nor Leaf page!!!" << endl; }
    }
}


Btree::push_up(page* origin_page, page* new_page, int push_key)
{
    index_page *parent = dynamic_cast<index_page*>(origin_page->parent); // 父節點必為路標節點
    if (parent == nullptr){ // 本身為root層 需新增起始節點

        cout << "Create new root level" << endl;
        index_page *temp = new index_page;
        temp->push(push_key);
        temp->ptr.push_back(origin_page);
        temp->ptr.push_back(new_page);
        origin_page->parent = temp;
        new_page->parent = temp;
        root = temp;

    } else { // 上層有父節點

        int position = parent->push(push_key); // 將key放入父節點，並回傳放入的位置(次序)
        vector<page*>::iterator itr = parent->ptr.begin();
        itr = parent->ptr.insert(itr + position + 1, new_page);

        if (parent->key.size() > page_size){ // 檢查是否超過父節點的容量

            cout << "\noverflow ! Index page size: " << parent->key.size() << "\n" <<endl;
            index_page* temp = new index_page;
            temp->parent = parent->parent;
            temp->key.assign(parent->key.begin() + page_size/2, parent->key.end());
            parent->key.erase(parent->key.begin() + page_size/2, parent->key.end());
            temp->ptr.assign(parent->ptr.begin() + page_size/2 + 1, parent->ptr.end());
            parent->ptr.erase(parent->ptr.begin() + page_size/2 + 1, parent->ptr.end());
            push_up(parent, temp, temp->key[0]);  // 繼續往上層送
            temp->key.erase(temp->key.begin(), temp->key.begin()+1);
        }
    } 

}


page::page(){
    parent = nullptr;
}

int page::push(int value)
{
    vector<int>::iterator itr;
    for (itr = key.begin() ; itr != key.end() ; itr++){
        if (*itr > value)
            break;
    }
    itr = key.insert(itr,value);
    return itr - key.begin();
}


index_page::index_page()
{
    set_type("index");
    cout << "Create a Index Page" << endl;
}


leaf_page::leaf_page()
{
    set_type("leaf");
    cout << "Create a Leaf Page" << endl;
}
