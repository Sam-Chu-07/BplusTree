#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;

int level;
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
        int pop(int value);

        redistribute(page* left_page, int replace_key);

    private:
        string page_type; 
        vector<int> key;
        page* parent;
        page *pre_page;
        page *next_page;
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
        
};



// --------------------------------------------------------- //
// - - - - - - - - - - - - - B+ TREE - - - - - - - - - - - - //
// --------------------------------------------------------- //
class Btree
{
    public:
        Btree(int input_order);

        Display(){level=0; display_tree(root);}

        Insert(int value);

        Delete(int value);

        Lookup(int value);

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
    int order = 2, input = 0;
    cout << "Hello!" << endl ;
    
    cin >> order;
    Btree tree(order);
    
    while(input != 8)
    {
        cout << endl;
        cout << "(1) Initialize (2) Attach (3) Bulkload (4) Lookup" << endl;
        cout << "(5) Insert     (6) Delete (7) Display  (8) Quit" << endl;
        cout << "Select an operation: ";
        cin >> input;
        switch (input)
        {
            case 4:
                cout << "Look up key = ";
                cin >> input;
                tree.Lookup(input);
                break;
            case 5:
                cout << "Inserting" << endl;
                cin >> input;
                while(input != -1){
                    tree.Insert(input);
                    cin >> input;
                }
                cout << "Insert finish" << endl;
                break;
            case 7:
                tree.Display();
                cout << "\n" <<endl;
                break;
            default:
                break;
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
// 建立一棵B+ Tree //
Btree::Btree(int input_order)
{
    page_size = input_order * 2;
    leaf_page *leaf = new leaf_page;
    root = leaf;

}

// 遞迴顯示整棵 B+ Tree //
Btree::display_tree(page* current)
{
    if (current->get_type() == "index"){
        
        index_page *index = dynamic_cast<index_page*>(current);
        for(int i=0 ; i<index->ptr.size(); i++){
            level ++;
            display_tree(index->ptr[i]);
            level --;
            if(i<index->key.size())
                cout << index->key[i] << "(" << level << ") ; ";
        }
    } else if (current->get_type() == "leaf"){
        
        leaf_page *leaf = dynamic_cast<leaf_page*>(current);
         cout << "{ ";
        for(int i=0 ; i<leaf->key.size() ; i++)
            cout << leaf->key[i] << " ";
         cout << "} ; " ;
    }
}

// 插入一個使用者輸入的數值 //
Btree::Insert(int value)
{
    cout << "Try to Insert value: " << value << endl;

    int position;
    page *current = root;  // 從根節點往下找適當的位置
    index_page *index;
    vector<int>::iterator itr;

    for(int x=0 ; x<10 ; x++)  // 先執行10次
    {
        if (current->get_type() == "leaf"){ // 目前為資料節點
            
            leaf_page *leaf = dynamic_cast<leaf_page*>(current);

            leaf->push(value); // 對資料節點塞入 使用者輸入的值
            if (leaf->key.size() > page_size){ // 檢查是否超過資料節點的容量

                cout << "\noverflow ! Leaf page size: " << current->key.size() << "\n" <<endl;
                leaf_page *temp = new leaf_page;    // 新增一個資料節點
                temp->parent = leaf->parent;        // 設定新節點的父節點
                temp->pre_page = leaf;              // 設定新節點的左節點
                temp->next_page = leaf->next_page;  // 設定新節點的右節點
                leaf->next_page = temp;             // 設定舊節點的右節點
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
    cout << "Insert Success !\n" << endl;
}


Btree::push_up(page* origin_page, page* new_page, int push_key)
{
    index_page *parent = dynamic_cast<index_page*>(origin_page->parent); // 父節點必為路標節點
    if (parent == nullptr){ // 本身為root層 需新增起始節點

        cout << "Create new root page" << endl;
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

        if (parent->key.size() > page_size){ // 檢查父節點是否超過容量

            cout << "\noverflow ! Index page size: " << parent->key.size() << "\n" <<endl;
            index_page* temp = new index_page;      // 新增路標節點
            temp->parent = parent->parent;          // 設定新節點之父節點
            temp->pre_page = parent;                // 設定新節點隻左節點
            temp->next_page = parent->next_page;    // 設定新節點隻右節點
            parent->next_page = temp;               // 設定舊節點之右節點
            temp->key.assign(parent->key.begin() + page_size/2, parent->key.end());
            parent->key.erase(parent->key.begin() + page_size/2, parent->key.end());
            temp->ptr.assign(parent->ptr.begin() + page_size/2 + 1, parent->ptr.end());
            parent->ptr.erase(parent->ptr.begin() + page_size/2 + 1, parent->ptr.end());

            for(int i=0 ; i<temp->ptr.size() ; i++) // 新增的路標節點 其子節點的parent 為該路標節點
                temp->ptr[i]->parent = temp;
            
            push_up(parent, temp, temp->key[0]);  // 繼續往上層送
            temp->key.erase(temp->key.begin(), temp->key.begin()+1); // 移除新增的節點最左邊的key 因為被往上移了
        }
    } 
}


// 移除一個使用者輸入的值 //
Btree::Delete(int value)
{
    cout << "Try to Delete value: " << value << endl;

    int position;
    page *current = root;  // 從根節點往下找尋欲刪除的值
    index_page *index;
    vector<int>::iterator itr;

    for(int x=0 ; x<10 ; x++)  // 先執行10次
    {
        if (current->get_type() == "leaf"){ // 目前為資料節點
            
            leaf_page *leaf = dynamic_cast<leaf_page*>(current);
            if (leaf->pop(value) == -1){   // 對資料節點移除 使用者輸入的值
                cout << "Error! Delete value: " << value << "Not Exist" << endl; // 報錯: 該值不存在
                return 0;
            }

            if (leaf->key.size() < page_size/2){ // 檢查資料節點是否空太多位置

                cout << "\nunderflow ! Leaf page size: " << leaf->key.size() << "\n" <<endl;
                
                if (leaf->next_page->parent == leaf->parent){ // 判斷右節點是否為兄弟
                    
                    leaf_page *right_page = dynamic_cast<leaf_page*>(leaf->next_page);

                    if (right_page->key.size() > page_size/2){ // 可以向右節點借一個key
                        
                        leaf->key.push_back(right_page->key[0]);  // 此節點加入右節點的第一個key
                        right_page->key.erase(right_page->key.begin(), right_page->key.begin()+1) ;
                        //leaf->parent

                        

                    } else {                                   // 無法向右節點借一個key

                    }


                } else if (leaf->pre_page->parent == leaf->parent){ // 判斷左節點是否為兄弟

                } else {cout << "Error! No sibling in this Leaf page\n" << endl;}
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


// 在 B+ Tree 中搜尋使用者輸入的數值 //
Btree::Lookup(int value)
{
    cout << "Try to Look up value: " << value << endl;
    int position;
    page *current = root;  // 從根節點往下搜尋
    index_page *index;
    vector<int>::iterator itr;

    for(int x=0 ; x<10 ; x++)  // 先執行10次
    {
        if (current->get_type() == "leaf"){ // 目前為資料節點
            
            leaf_page *leaf = dynamic_cast<leaf_page*>(current);
            itr = find(leaf->key.begin(), leaf->key.end(), value);
            if (itr != leaf->key.end())
                cout << "true" << endl;
            else
                cout << "false" << endl;
            break;

        } else if (current->get_type() == "index"){ // 目前為路標節點

            index = dynamic_cast<index_page*>(current);

            for( itr =index->key.begin() ; itr != index->key.end() ; itr++){ // 使用路標往下層繼續找尋目標值
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





page::page(){
    parent = nullptr;
    pre_page = nullptr;
    next_page = nullptr;
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

int page::pop(int value)
{
    vector<int>::iterator itr;
    for (itr = key.begin() ; itr != key.end() ; itr++){
        if (*itr == value){
            key.erase(itr, itr+1);
            return 1;
        }
    }
    return -1;
}

index_page::index_page()
{
    set_type("index");
    cout << "Create a Index Page" << endl;
}

index_page::redistribute(page* left_page, int replace_key)
{
    
    for(int i=0 ; i<key.size() ; i++){
        if (ptr[i] == leaf_page){
            key[i] = replace_key;
            cout << "Redistribute Success!" << endl;
            break;
        }
    }
}

leaf_page::leaf_page()
{
    set_type("leaf");
    cout << "Create a Leaf Page" << endl;
}
