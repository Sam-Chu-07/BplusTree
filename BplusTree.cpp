#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;

int level;


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

        
       
    private:
        int page_size; // 兩倍的order

        // ------------------------------------------------------ //
        // - - - - - - - - - - - - 節點 - - - - - - - - - - - - - //
        // ------------------------------------------------------ //
        class page{
            public:
                string page_type; 
                vector<int> key;
                page* parent;
                page *pre_page;
                page *next_page;

                page();
                virtual ~page(){}
                int push(int value);
                int pop(int value);
                show();
        };

        // --------------------------------------------------------- //
        // - - - - - - - - - - - - - 路標節點 - - - - - - - - - - - - //
        // --------------------------------------------------------- //
        class index_page : public page
        {
            public:
                vector<page*> ptr;

                index_page();
                ~index_page();
                int key_index_between_ptr(page* x_page, page* y_page);
                redistribute(page* left_page, int replace_key);
        };

        // --------------------------------------------------------- //
        // - - - - - - - - - - - - - 資料節點 - - - - - - - - - - - - //
        // --------------------------------------------------------- //
        class leaf_page : public page
        {
            public:
                leaf_page();
                ~leaf_page();
        };

        page *root;

        push_up(page* origin_page, page* new_page, int push_key);

        pop_down(page* empty_page, page* merge_page);

        display_tree(page* current);
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
            case 6:
                cout << "Delete key = ";
                cin >> input;
                tree.Delete(input);
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
    if (current->page_type == "index"){
        
        index_page *index = dynamic_cast<index_page*>(current);
        for(int i=0 ; i<index->ptr.size(); i++){
            level ++;
            display_tree(index->ptr[i]);
            level --;
            if(i<index->key.size())
                cout << index->key[i] << "(" << level << ") ; ";
        }
    } else if (current->page_type == "leaf"){
        
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
        if (current->page_type == "leaf"){ // 目前為資料節點
            
            leaf_page *leaf = dynamic_cast<leaf_page*>(current);

            leaf->push(value); // 對資料節點塞入 使用者輸入的值
            if (leaf->key.size() > page_size){ // 檢查是否超過資料節點的容量

                cout << "\noverflow ! Leaf page size: " << current->key.size() << "\n" <<endl;
                leaf_page *temp = new leaf_page;      // 新增一個資料節點
                temp->parent = leaf->parent;          // 設定新節點的父節點指標
                temp->pre_page = leaf;                // 設定新節點的左節點指標
                temp->next_page = leaf->next_page;    // 設定新節點的右節點指標
                if (leaf->next_page != nullptr)
                    leaf->next_page->pre_page = temp; // 設定舊節點之右邊節點的左節點指標
                leaf->next_page = temp;               // 設定舊節點的右節點指標
                
                temp->key.assign(leaf->key.begin() + page_size/2, leaf->key.end());
                leaf->key.erase(leaf->key.begin() + page_size/2, leaf->key.end());
                push_up(leaf, temp, temp->key[0]);
            }
            break;

        } else if (current->page_type == "index"){ // 目前為路標節點

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
    cout << "Insert Success !" << endl;
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
            temp->parent = parent->parent;          // 設定新節點之父節點指標
            temp->pre_page = parent;                // 設定新節點隻左節點指標
            temp->next_page = parent->next_page;    // 設定新節點隻右節點指標
            if (parent->next_page != nullptr)
                parent->next_page->pre_page = temp; // 設定舊節點之右邊節點的左節點指標
            parent->next_page = temp;               // 設定舊節點之右節點指標
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
        if (current->page_type == "leaf"){ // 目前為資料節點
            
            leaf_page *leaf = dynamic_cast<leaf_page*>(current);
            if (leaf->pop(value) == -1){   // 對資料節點移除目標值
                cout << "Error! Key: " << value << " Not Exist" << endl; // 報錯: 該值不存在
                cout << "Delete Fail !" << endl;
                return -1;
            }

            if (leaf->key.size() < page_size/2){ // 檢查資料節點是否剩餘過多空間

                cout << "\nunderflow ! Leaf page size: " << leaf->key.size() << "\n" <<endl;
                
                // 判斷左節點是否為兄弟 且不為 NULL //
                if (leaf->pre_page != nullptr && leaf->pre_page->parent == leaf->parent){

                    cout << "Left leaf page is sibling" <<  endl;
                    leaf_page *left_page = dynamic_cast<leaf_page*>(leaf->pre_page);
                    index_page *parent = dynamic_cast<index_page*>(leaf->parent);
                    
                    // 可以向左節點借一個key //
                    if (left_page->key.size() > page_size/2){
                        
                        cout << "borrow a key fom left leaf page" << endl;
                        leaf->key.insert(leaf->key.begin(), left_page->key.back()); // 將左節點的最後一個key 放入此資料節點的第一個位置
                        left_page->key.pop_back(); // 移除左節點最後一個key
                        parent->redistribute(left_page, leaf->key[0]); 
                        break; // 結束 Underflow 檢查與處理

                    // 無法向左節點借一個key 且 右節點為NULL或不為兄弟  //
                    } else if (leaf->next_page == nullptr || leaf->next_page->parent != leaf->parent){
                        
                        cout << "merge to left leaf page" << endl;
                        // 將這個資料節點內所有的key都丟到左節點
                        for( ; leaf->key.size() > 0 ; ){
                            left_page->key.push_back(leaf->key.front());
                            leaf->key.erase(leaf->key.begin(), leaf->key.begin()+1);
                        }
                        pop_down(leaf, left_page);
                        break; // 結束 Underflow 檢查與處理
                    }
                }

                // 判斷右節點是否為兄弟 且 不為 null//
                if (leaf->next_page != nullptr && leaf->next_page->parent == leaf->parent){ 
                    
                    cout << "Right leaf page is sibling" << endl;
                    leaf_page *right_page = dynamic_cast<leaf_page*>(leaf->next_page);
                    index_page *parent = dynamic_cast<index_page*>(leaf->parent);

                    // 可以向右節點借一個key //
                    if (right_page->key.size() > page_size/2){    
                        
                        cout << "borrow a key fom right leaf page" << endl;
                        leaf->key.push_back(right_page->key[0]);  // 將右節點的第一個key 放入此節點中
                        right_page->key.erase(right_page->key.begin(), right_page->key.begin()+1); // 移除右節點第一個key
                        parent->redistribute(leaf, right_page->key[0]); 
                    
                    } else {  // 無法向右節點借一個key 因此只能向右做合併 //

                        cout << "merge to right leaf page" << endl;
                        // 將這個資料節點內所有的key都丟到右節點
                        for( itr = right_page->key.begin() ; leaf->key.size() > 0 ; ){
                            itr = right_page->key.insert(itr, leaf->key.back());
                            leaf->key.pop_back();
                        }
                        pop_down(leaf, right_page);
                    }

                } else {cout << "Error! No sibling in this Leaf page\n" << endl;}
            }
            break; // 結束 Underflow 檢查與處理

        } else if (current->page_type == "index"){ // 目前為路標節點 繼續往下找

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

        } else { cout << "Error! This page neither Index page nor Leaf page!!!" << endl; }
    }
    cout << "Delete Success !" << endl;
}


Btree::pop_down(page* empty_page, page* merge_page)
{
    index_page *parent = dynamic_cast<index_page*>(empty_page->parent);    // 父節點必為路標節點
    
    vector<page*>::iterator itr;
    itr = find(parent->ptr.begin(), parent->ptr.end(), empty_page);        // 回傳空節點指標之位置
    int position = parent->key_index_between_ptr(empty_page, merge_page);  // 回傳兩指標間key的index

    parent->ptr.erase(itr, itr+1);                                                         // 移除空節點之指標
    parent->key.erase(parent->key.begin() + position, parent->key.begin() + position + 1); // 移除兩指標間的key值
    
    if (empty_page->next_page == merge_page){             // 判斷左為空節點 右為合併節點
        merge_page->pre_page = empty_page->pre_page;      // 設定合併節點的左節點指標
        if (empty_page->pre_page != nullptr)
            empty_page->pre_page->next_page = merge_page; // 設定空節點之左邊節點的 右節點指標 
    } else {                                              // 判斷左為合併節點 右為空節點
        merge_page->next_page = empty_page->next_page;    // 設定合併節點的右節點指標
        if (empty_page->next_page != nullptr)
            empty_page->next_page->pre_page = merge_page; // 設定空節點之右邊節點的 左節點指標 
    }
    
    delete empty_page;   // 刪除此空的節點

    if (parent == root && parent->key.size() == 0){ // 父節點為根節點(Root) 且 父節點內已經沒有key了
        
        cout << "Root Page key size: 0, pointer size: " << parent->ptr.size() << endl;
        cout << "Delete current Root Page!\n" << endl;
        if (parent->ptr[0] != merge_page)
            cout << "Error! remain pointer in root not point to the merge page\n" << endl;
        delete root;
        root = merge_page;

    } else if (parent->key.size() < page_size/2){ // 父節點內剩餘太多空間


        cout << "\nunderflow ! Index page size: " << parent->key.size() << "\n" <<endl;


        // 判斷左節點是否為兄弟 且不為 NULL //
        if (parent->pre_page != nullptr && parent->pre_page->parent == parent->parent){
            
            cout << "Left index page is sibling" <<  endl;
            leaf_page *left_page = dynamic_cast<leaf_page*>(parent->pre_page);
            index_page *parent = dynamic_cast<index_page*>(parent->parent);

        }

        // 可以向左節點借一個key //

        // 無法向左節點借一個key 且 右節點為NULL或不為兄弟  //
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
        if (current->page_type == "leaf"){ // 目前為資料節點
            
            leaf_page *leaf = dynamic_cast<leaf_page*>(current);
            itr = find(leaf->key.begin(), leaf->key.end(), value);
            if (itr != leaf->key.end()){
                if (leaf->pre_page != nullptr)
                    leaf->pre_page->show();
                cout << ";\n";
                leaf->show();
                //cout << "true" << endl;
                cout << ";\n";
                if (leaf->next_page != nullptr)
                    leaf->next_page->show();
            }
            else
                cout << "false" << endl;
            break;

        } else if (current->page_type == "index"){ // 目前為路標節點

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





Btree::page::page(){
    parent = nullptr;
    pre_page = nullptr;
    next_page = nullptr;
}

int Btree::page::push(int value)
{
    vector<int>::iterator itr;
    for (itr = key.begin() ; itr != key.end() ; itr++){
        if (*itr > value)
            break;
    }
    itr = key.insert(itr,value);
    return itr - key.begin();
}

int Btree::page::pop(int value)
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

Btree::page::show(){

    if (page_type == "index"){
        cout << "Index Page" << endl;
    }else if (page_type == "leaf"){
        cout << "Leaf Page" << endl;
    }else{
        cout << "Error page type\n" << endl;
        return 0;
    }
    cout << "key size: " << key.size() << endl;
    for(int i=0 ; i<key.size() ; i++){
        cout << key[i] << " ";
    }
    cout << endl;
}


Btree::index_page::index_page()
{
    page_type = "index";
    cout << "Create a Index Page" << endl;
}

Btree::index_page::~index_page()
{
    if (key.size() != 0)
        cout << "Error! Delete a Index page with key size not Zero" << endl;
    else{
        cout << "Delete a Index Page between [ " << endl;
        if (pre_page == nullptr)
            cout << " NULL ";
        else
            for(int i=0 ; i<pre_page->key.size() ; i++)
                cout << pre_page->key[i] << " ";
        cout << "] and [ ";

        if (next_page == nullptr)
            cout << " NULL ";
        else
            for(int i=0 ; i<next_page->key.size() ; i++)
                cout << next_page->key[i] << " ";
        cout << "]" << endl;
    }
}

int Btree::index_page::key_index_between_ptr(page* x_page, page* y_page)
{
    for(int i=0 ; i<key.size() ; i++){
        if ((ptr[i] == x_page && ptr[i+1] == y_page) || (ptr[i] == y_page && ptr[i+1] == x_page))
            return i;
    }
    cout << "Error! No key value between given pointer\n" << endl;
    return -1;
}


Btree::index_page::redistribute(page* left_page, int replace_key)
{
    for(int i=0 ; i<key.size() ; i++){
        if (ptr[i] == left_page){
            key[i] = replace_key;
            cout << "Redistribute Success!" << endl;
            break;
        }
    }
}

Btree::leaf_page::leaf_page()
{
    page_type = "leaf";
    cout << "Create a Leaf Page" << endl;
}

Btree::leaf_page::~leaf_page()
{
    if (key.size() != 0)
        cout << "Error! Delete a Leaf page with key size not Zero" << endl;
    else{
        cout << "Delete a Leaf Page between [ " << endl;
        if (pre_page == nullptr)
            cout << " NULL ";
        else
            for(int i=0 ; i<pre_page->key.size() ; i++)
                cout << pre_page->key[i] << " ";
        cout << "] and [ ";

        if (next_page == nullptr)
            cout << " NULL ";
        else
            for(int i=0 ; i<next_page->key.size() ; i++)
                cout << next_page->key[i] << " ";
        cout << "]" << endl;
    }
}