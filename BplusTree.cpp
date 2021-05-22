#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
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

        BulkLoading();
       
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
                
                push_up(page* new_page, Btree* btree);
                pop_down(page* empty_page, page* merge_page, Btree* btree);
                int key_index_between_ptr(page* x_page, page* y_page);
        };

        // --------------------------------------------------------- //
        // - - - - - - - - - - - - - 資料節點 - - - - - - - - - - - - //
        // --------------------------------------------------------- //
        class leaf_page : public page
        {
            public:
                leaf_page();
                ~leaf_page();

                overflow(Btree* btree);
                underflow(Btree* btree);
        };

        page *root;

        display_tree(page* current);
};




// ################################################# //
    // ---------------- 主要程式 ---------------- //
// ################################################# //
int main()
{
    int order, input;
    bool quit = false;
    cout << "Hello!" << endl ;
    
    cin >> order;
    Btree tree(order);
    
    while(quit == false)
    {
        cout << endl;
        cout << "(1) Initialize (2) Attach (3) Bulkload (4) Lookup" << endl;
        cout << "(5) Insert     (6) Delete (7) Display  (8) Quit" << endl;
        cout << "Select an operation: ";
        cin >> input;
        switch (input)
        {
            case 3:
                cout << "Bulkloading key sequence: ";
                tree.BulkLoading();
                break;
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
            case 8:
                quit = true;
                break;
            default:
                break;
        }
    }
    return 0;
}


// ################################################# //
    // ---------------- 函式定義 ---------------- //
// ################################################# //
// 建立一棵B+ Tree //
Btree::Btree(int input_order)
{
    page_size = input_order * 2;
    root = nullptr;
}


// 遞迴顯示整棵 B+ Tree //
Btree::display_tree(page* current)
{
    if (current == nullptr){

        cout << "Empty B+ Tree";
        
    }else if (current->page_type == "index"){
        
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
        if (current == nullptr){  // 此樹還未建立任何節點

            root = new leaf_page;
            current = root;

        } else if (current->page_type == "leaf"){ // 目前為資料節點
            
            leaf_page *leaf = dynamic_cast<leaf_page*>(current);

            leaf->push(value); // 對資料節點塞入 使用者輸入的值
            if (leaf->key.size() > page_size){ // 檢查是否超過資料節點的容量

                cout << "\noverflow ! Leaf page size: " << current->key.size() << "\n" <<endl;
                leaf->overflow(this);  // 處理該資料節點的overflow
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
        if (current == nullptr){  // 此樹還未建立任何節點

            cout << "Error! Key: " << value << " Not Exist" << endl; // 報錯: 該值不存在
            cout << "Delete Fail !" << endl;
            return -1;

        }else if (current->page_type == "leaf"){ // 目前為資料節點
            
            leaf_page *leaf = dynamic_cast<leaf_page*>(current);
            if (leaf->pop(value) == -1){   // 對資料節點移除目標值
                cout << "Error! Key: " << value << " Not Exist" << endl; // 報錯: 該值不存在
                cout << "Delete Fail !" << endl;
                return -1;
            }

            if (leaf == root && leaf->key.size() == 0){ // 檢查資料節點是否為root 且 已無key值

                root = nullptr;
                delete leaf;

            } else if (leaf != root && leaf->key.size() < page_size/2){ // 檢查資料節點是否剩餘過多空間

                cout << "\nunderflow ! Leaf page size: " << leaf->key.size() << "\n" <<endl;
                leaf->underflow(this);
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
        if (current == nullptr){  // 此樹還未建立任何節點

            cout << "false" << endl;

        } else if (current->page_type == "leaf"){ // 目前為資料節點
            
            leaf_page *leaf = dynamic_cast<leaf_page*>(current);
            itr = find(leaf->key.begin(), leaf->key.end(), value);
            if (itr != leaf->key.end())
                cout << "true" << endl;
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


// 在 B+ Tree 中建立使用者連續輸入的key值 //
Btree::BulkLoading()
{
    int input, count=0;
    list<int> array;
    leaf_page *current = nullptr, *temp = nullptr, *start = nullptr;
    index_page *parent;

    if (root != nullptr) {
        cout << "Error ! You can't not use bulkloading in a non empty tree!" << endl; 
        return -1;
    }

    cin >> input;
    while(input != -1){
        array.push_back(input);
        cin >> input;
    }

    array.sort();
    while(!array.empty()){

        if (current == nullptr){
            current = new leaf_page;
            count ++;
            start = current;
        } else if (current->key.size() == page_size){
            temp = new leaf_page;
            count ++;
            current->next_page = temp;
            temp->pre_page = current;
            current = temp;  
        }
        current->key.push_back(array.front());
        array.pop_front();
    }
    
    current = start;
    while(current != nullptr){
        
        for(int i=0 ; i<current->key.size() ; i++)
            cout << "Try to Insert value: " << current->key[i] << endl;

        if (root == nullptr){

            root = current;

        } else if (current->pre_page->parent == nullptr){

            cout << "Create new root page" << endl;
            index_page *temp_root = new index_page;
            temp_root->push(current->key[0]);
            temp_root->ptr.push_back(current->pre_page);
            temp_root->ptr.push_back(current);
            current->pre_page->parent = temp_root;
            current->parent = temp_root;
            root = temp_root;

        } else {

            current->parent = current->pre_page->parent;
            parent = dynamic_cast<index_page*>(current->parent);
            parent->push_up(current, this);
        }

        if (current != root && current->key.size() < page_size/2){
            cout << "underflow! leaf page key size: " <<  current->key.size() << endl;
            current->underflow(this);
        }
            
        current = dynamic_cast<leaf_page*>(current->next_page);
    }
    cout << "BulkLoading Success!" << endl;
}




Btree::page::page()
{
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

Btree::page::show()
{
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
        cout << "Delete a Index Page between [ ";
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

Btree::index_page::push_up(page* new_page, Btree* btree)
{
    int page_size = btree->page_size;
    index_page *myparent = dynamic_cast<index_page*>(parent);  // 將父節點轉換成路標節點型態

    int position = push(new_page->key[0]); // 將key放入此路標節點，並回傳放入的index
    vector<page*>::iterator itr = ptr.begin();
    itr = ptr.insert(itr + position + 1, new_page); // 將子節點的pointer 放入適當的位置

    if (key.size() > page_size){ // 檢查此節點是否超過容量

        cout << "\noverflow ! Index page size: " << key.size() << "\n" <<endl;

        if (pre_page != nullptr && pre_page->parent == parent && pre_page->key.size() < page_size){

            cout << "Left index page is sibling" <<  endl;
            cout << "throw a key and a pointer to left index page" << endl;
            index_page *left_page = dynamic_cast<index_page*>(pre_page);

            position = myparent->key_index_between_ptr(left_page, this);  // 取得父節點中 兩指標間key的index
            left_page->key.push_back(myparent->key[position]);            // 將父節點中 兩指標間的key 放入 左節點最後面的key位置
            left_page->ptr.push_back(ptr[0]);          // 將自己最前面的pointer 放入 左節點的最後面ptr位置
            myparent->key[position] = key[0];          // 將父節點中 兩指標間的key值 替換成 自己的第一個key值
            key.erase(key.begin(), key.begin()+1);     // 移除自己的第一個key
            ptr.erase(ptr.begin(), ptr.begin()+1);     // 移除自己的第一個pointer
            left_page->ptr.back()->parent = left_page; // 更新移動過後的pointer指向的節點 其父節點指標
        
        // 判斷右節點存在且為兄弟 並且有空間可以多塞一個key //
        } else if (next_page != nullptr && next_page->parent == parent && next_page->key.size() < page_size){

            cout << "Right leaf page is sibling" <<  endl;
            cout << "throw a key to right leaf page" << endl;
            index_page *right_page = dynamic_cast<index_page*>(next_page);

            position = myparent->key_index_between_ptr(this, right_page);             // 取得父節點中 兩指標間key的index
            right_page->key.insert(right_page->key.begin(), myparent->key[position]); // 將父節點中 兩指標間的key 插入 右節點最前面的key位置
            right_page->ptr.insert(right_page->ptr.begin(), ptr.back());              // 將自己最後面的pointer 插入 右節點最前面的ptr位置
            myparent->key[position] = key.back();                                     // 將父節點中 兩指標間的key值 替換成 自己的最後一個key值
            key.pop_back();                           // 移除自己的最後一個key
            ptr.pop_back();                           // 移除自己的最後一個pointer
            right_page->ptr[0]->parent = right_page;  // 更新移動過後的pointer指向的節點 其父節點指標
        
        } else {  // 左右都無兄弟可以多塞一個key  只能分裂這個路標節點 //

            index_page* temp = new index_page; // 新增路標節點
            temp->parent = parent;             // 設定新節點之父節點指標
            temp->pre_page = this;             // 設定新節點隻左節點指標
            temp->next_page = next_page;       // 設定新節點隻右節點指標
            if (next_page != nullptr)
                next_page->pre_page = temp;    // 設定舊節點之右邊節點的左節點指標
            next_page = temp;                  // 設定舊節點之右節點指標
            temp->key.assign(key.begin() + page_size/2, key.end());
            key.erase(key.begin() + page_size/2, key.end());
            temp->ptr.assign(ptr.begin() + page_size/2 + 1, ptr.end());
            ptr.erase(ptr.begin() + page_size/2 + 1, ptr.end());

            for(int i=0 ; i<temp->ptr.size() ; i++) // 新增的路標節點 其子節點的parent 為該路標節點
                temp->ptr[i]->parent = temp;
            
            if (myparent == nullptr){ // 本身為root層 需新增起始節點 //

                cout << "Create new root page" << endl;
                index_page *temp_root = new index_page;
                temp_root->push(temp->key[0]);
                temp_root->ptr.push_back(this);
                temp_root->ptr.push_back(temp);
                parent = temp_root;
                temp->parent = temp_root;
                btree->root = temp_root;

            } else { // 上層有父節點 //
                myparent->push_up(temp, btree);
            }
            temp->key.erase(temp->key.begin(), temp->key.begin()+1); // 移除新增節點的第一個key 因為被往上移了
        }
    }
}

Btree::index_page::pop_down(page* empty_page, page* merge_page, Btree* btree)
{
    int page_size = btree->page_size;
    vector<page*>::iterator itr;
    itr = find(ptr.begin(), ptr.end(), empty_page);                // 回傳空節點指標之位置
    int position = key_index_between_ptr(empty_page, merge_page);  // 回傳兩指標間key的index

    ptr.erase(itr, itr+1);                                         // 移除空節點之指標
    key.erase(key.begin() + position, key.begin() + position + 1); // 移除兩指標間的key值
    
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

    if (this == btree->root && key.size() == 0){ // 自己為根節點(Root) 且 已經沒有key了
        
        cout << "Root Page key size: 0, pointer size: " << ptr.size() << endl;
        cout << "Delete current Root Page!\n" << endl;
        if (ptr[0] != merge_page)
            cout << "Error! remain pointer in root not point to the merge page\n" << endl;
        btree->root = merge_page;      // 根節點為合併節點
        merge_page->parent = nullptr;  // 設定合併節點的父節點指標 為 NULL
        delete this;                   // 刪除舊的根節點空間

    } else if (key.size() < page_size/2){ // 自己剩餘太多空間

        cout << "\nunderflow ! Index page size: " << key.size() << "\n" <<endl;
        index_page *myparent = dynamic_cast<index_page*>(parent);

        // 判斷左節點是否為兄弟 且不為 NULL //
        if (pre_page != nullptr && pre_page->parent == parent){
            
            cout << "Left index page is sibling" <<  endl;
            index_page *left_page = dynamic_cast<index_page*>(pre_page);

            // 可以向左節點借一個key 和一個 pointer //
            if (left_page->key.size() > page_size/2){

                cout << "borrow a key and a pointer fom left index page" << endl;
                position = myparent->key_index_between_ptr(this, left_page); // 取得父節點中 兩指標間key的index
                key.insert(key.begin(), myparent->key[position]); // 將父節點中 兩指標間key值 插入自己的第一個key位置
                ptr.insert(ptr.begin(), left_page->ptr.back());   // 將左節點的最後一個pointer 插入自己的第一個ptr位置
                myparent->key[position] = left_page->key.back();  // 父節點中 兩指標間的key值 替換成 左節點的最後一個key
                left_page->key.pop_back();                        // 移除左節點最後一個key
                left_page->ptr.pop_back();                        // 移除左節點最後一個pointer
                ptr.front()->parent = this;                       // 設定移過來的pointer指向的節點的父節點指標 為 自己
                return 0; // 結束 Underflow 檢查與處理

            // 無法向左節點借一個key 且 右節點為NULL或不為兄弟  因此只能向左做合併 //
            } else if (next_page == nullptr || next_page->parent != parent){

                cout << "merge to left index page" << endl;

                position = myparent->key_index_between_ptr(this, left_page); // 取得父節點中 自己的指標與左節點指標間的key位置
                left_page->key.push_back(myparent->key[position]);           // 在左節點最後面 放入 父節點中自己的指標與左節點指標間的key
                
                // 將自己所有的key 與 pointer 都丟到左節點最後面 //
                for( ; key.size() > 0 ; ){                  
                    left_page->key.push_back(key.front());
                    left_page->ptr.push_back(ptr.front());
                    key.erase(key.begin(), key.begin()+1);
                    ptr.erase(ptr.begin(), ptr.begin()+1);
                    left_page->ptr.back()->parent = left_page;
                }
                left_page->ptr.push_back(ptr.front());
                ptr.erase(ptr.begin(), ptr.begin()+1);
                left_page->ptr.back()->parent = left_page;

                myparent->pop_down(this, left_page, btree);
                return 0; // 結束 Underflow 檢查與處理
            }
        }

        // 判斷右節點是否為兄弟 且 不為 null //
        if (next_page != nullptr && next_page->parent == parent){

            cout << "Right index page is sibling" << endl;
            index_page *right_page = dynamic_cast<index_page*>(next_page);

            // 可以向右節點借一個key 和一個 pointer //
            if (right_page->key.size() > page_size/2){    
                
                cout << "borrow a key and a pointer fom right index page" << endl;
                position = myparent->key_index_between_ptr(this, right_page); // 取得父節點中 兩指標間key的index
                key.push_back(myparent->key[position]);                       // 將父節點中 兩指標間key值 放入自己最後一個key位置
                ptr.push_back(right_page->ptr[0]);                            // 將右節點的第一個pointer  放入自己最後一個ptr位置
                myparent->key[position] = right_page->key[0];                 // 父節點中 兩指標間的key值 替換成 右節點的第一個key
                right_page->key.erase(right_page->key.begin(), right_page->key.begin()+1);  // 移除右節點第一個key
                right_page->ptr.erase(right_page->ptr.begin(), right_page->ptr.begin()+1);  // 移除右節點第一個pointer
                ptr.back()->parent = this;                                    // 設定移過來的pointer指向的節點的父節點指標 為 自己
            
            } else {  // 無法向右節點借一個key 因此只能向右做合併 //

                cout << "merge to right index page" << endl;
                position = myparent->key_index_between_ptr(this, right_page);             // 取得父節點中 自己的指標與右節點指標間的key位置
                right_page->key.insert(right_page->key.begin(), myparent->key[position]); // 在右節點最前面 插入 父節點中自己的指標與右節點指標間的key

                // 將這個自己所有的key 與 pointer 都依序插入右節點的最前面 //
                for( ; key.size() > 0 ; ){
                    right_page->key.insert(right_page->key.begin(), key.back());  // 將自己最後一個key 插入 右節點的第一個位置
                    right_page->ptr.insert(right_page->ptr.begin(), ptr.back());  // 將自己最後一個pointer 插入 右節點的第一個位置
                    key.pop_back();                                               // 移除自己最後一個key
                    ptr.pop_back();                                               // 移除自己最後一個pointer
                    right_page->ptr[0]->parent = right_page;
                }
                right_page->ptr.insert(right_page->ptr.begin(), ptr.back());
                ptr.pop_back();                                              
                right_page->ptr[0]->parent = right_page;

                myparent->pop_down(this, right_page, btree);
            }
        } else {cout << "Error! No sibling in this Index page\n" << endl;}
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
        cout << "Delete a Leaf Page between [ ";
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

Btree::leaf_page::overflow(Btree* btree)
{
    int position;
    int page_size = btree->page_size;
    index_page *myparent = dynamic_cast<index_page*>(parent); // 將父節點轉換成路標節點型態

    // 判斷左節點存在且為兄弟 並且有空間可以多塞一個key //
    if (pre_page != nullptr && pre_page->parent == parent && pre_page->key.size() < page_size){

        cout << "Left leaf page is sibling" <<  endl;
        cout << "throw a key to left leaf page" << endl;
        leaf_page *left_page = dynamic_cast<leaf_page*>(pre_page);

        left_page->key.push_back(key[0]);       // 將自己的第一個key 放入 左節點的最後面
        key.erase(key.begin(), key.begin()+1);  // 移除自己的第一個key
        position = myparent->key_index_between_ptr(left_page, this);
        myparent->key[position] = key[0];

    // 判斷右節點存在且為兄弟 並且有空間可以多塞一個key //
    } else if (next_page != nullptr && next_page->parent == parent && next_page->key.size() < page_size){

        cout << "Right leaf page is sibling" <<  endl;
        cout << "throw a key to right leaf page" << endl;
        leaf_page *right_page = dynamic_cast<leaf_page*>(next_page);

        right_page->key.insert(right_page->key.begin(), key.back()); // 將自己的最後面的key 放入 右節點的第一個位置
        key.pop_back();                                              // 移除自己最後面的key
        position = myparent->key_index_between_ptr(this, right_page);
        myparent->key[position] = right_page->key[0];

    } else {  // 左右都無兄弟可以多塞一個key  只能分裂這個資料節點 //

        leaf_page *temp = new leaf_page;// 新增一個資料節點
        temp->parent = parent;          // 設定新節點的父節點指標
        temp->pre_page = this;          // 設定新節點的左節點指標
        temp->next_page = next_page;    // 設定新節點的右節點指標
        if (next_page != nullptr)
            next_page->pre_page = temp; // 設定右邊節點的左節點指標
        next_page = temp;               // 設定右節點指標
        
        temp->key.assign(key.begin() + page_size/2, key.end());
        key.erase(key.begin() + page_size/2, key.end());

        // 無父節點 本身就是root層 需新增起始節點 //
        if (myparent == nullptr){        

            cout << "Create new root page" << endl;
            index_page *temp_root = new index_page;
            temp_root->push(temp->key[0]);
            temp_root->ptr.push_back(this);
            temp_root->ptr.push_back(temp);
            parent = temp_root;
            temp->parent = temp_root;
            btree->root = temp_root;

        } else {  // 具有父節點 //
            myparent->push_up(temp, btree);
        }
    }
}

Btree::leaf_page::underflow(Btree* btree)
{
    int position;
    int page_size = btree->page_size;
    vector<int>::iterator itr;
    index_page *myparent = dynamic_cast<index_page*>(parent);

    // 判斷左節點是否為兄弟 且不為 NULL //
    if (pre_page != nullptr && pre_page->parent == parent){

        cout << "Left leaf page is sibling" <<  endl;
        leaf_page *left_page = dynamic_cast<leaf_page*>(pre_page);
        
        // 可以向左節點借一個key //
        if (left_page->key.size() > page_size/2){
            
            cout << "borrow a key fom left leaf page" << endl;
            key.insert(key.begin(), left_page->key.back()); // 將左節點的最後一個key 放入此資料節點的第一個位置
            left_page->key.pop_back();                      // 移除左節點最後一個key
            position = myparent->key_index_between_ptr(left_page, this);
            myparent->key[position] = key[0];
            return 0; // 結束 Underflow 檢查與處理

        // 無法向左節點借一個key 且 右節點為NULL或不為兄弟  //
        } else if (next_page == nullptr || next_page->parent != parent){
            
            cout << "merge to left leaf page" << endl;

            // 將這個資料節點內所有的key都丟到左節點 //
            for( ; key.size() > 0 ; ){                  
                left_page->key.push_back(key.front());
                key.erase(key.begin(), key.begin()+1);
            }
            myparent->pop_down(this, left_page, btree);
            return 0; // 結束 Underflow 檢查與處理
        }
    }

    // 判斷右節點是否為兄弟 且 不為 null //
    if (next_page != nullptr && next_page->parent == parent){ 
        
        cout << "Right leaf page is sibling" << endl;
        leaf_page *right_page = dynamic_cast<leaf_page*>(next_page);

        // 可以向右節點借一個key //
        if (right_page->key.size() > page_size/2){    
            
            cout << "borrow a key fom right leaf page" << endl;
            key.push_back(right_page->key[0]);  // 將右節點的第一個key 放入此節點中
            right_page->key.erase(right_page->key.begin(), right_page->key.begin()+1); // 移除右節點第一個key
            position = myparent->key_index_between_ptr(this, right_page);
            myparent->key[position] = right_page->key[0];
        
        } else {  // 無法向右節點借一個key 因此只能向右做合併 //

            cout << "merge to right leaf page" << endl;

            // 將這個資料節點內所有的key都丟到右節點 //
            for( itr = right_page->key.begin() ; key.size() > 0 ; ){
                itr = right_page->key.insert(itr, key.back());
                key.pop_back();
            }
             myparent->pop_down(this, right_page, btree);
        }
    } else {cout << "Error! No sibling in this Leaf page\n" << endl;}
}