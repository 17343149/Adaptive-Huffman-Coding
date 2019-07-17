#include<stdio.h>
#include<cstring>
#include<iostream>
#include<fstream>
#include<queue>
#include<stack>
#include<map>

#define DEBUG

struct Node{
    int count;
    char str;
    Node *left_child, *right_child, *parent;

    Node(){
        count = 0;
        str = -1;
        left_child = right_child = parent = nullptr;
    }

    Node(int parm, char s){
        count = parm;
        str = s;
        left_child = right_child = parent = nullptr;
    }

    Node(Node *left, Node *right, Node *par, char s, int parm = 0){
        count = parm;
        str = s;
        left_child = left, right_child = right, parent = par;
    }
};

Node root;

Node* getBrother(Node *node){ // 得到此节点的兄弟节点
    if(node->parent == nullptr)
        return nullptr;
    if(node->parent->left_child == node)
        return node->parent->right_child;
    return node->parent->left_child;
}

/**
 * 查到特定count的所有节点
 */
std::queue<Node*> find(Node *node, int parm){
    std::queue<Node*> BFS_queue, mqueue;
    if(parm == 0)
        return mqueue;
    BFS_queue.push(node);
    while(!BFS_queue.empty()){
        Node *temp = BFS_queue.front();
        if(temp->count == parm)
            mqueue.push(temp);
        if(temp->left_child != nullptr)
            BFS_queue.push(temp->left_child);
        if(temp->right_child != nullptr)
            BFS_queue.push(temp->right_child);
        BFS_queue.pop();
    }
    return mqueue;
}

Node* find(Node *root, char s){ // DFS找到char
    if(root->str == s)
        return root;
    if(root->left_child == nullptr && root->right_child == nullptr)
        return nullptr;
    Node *res = nullptr;
    if((res = find(root->left_child, s)) != nullptr)
        return res;
    return find(root->right_child, s);
}

Node* bfs_find(Node *root, char s){
    std::queue<Node*> mqueue;
    mqueue.push(root);
    Node *temp;
    while(!mqueue.empty()){
        temp = mqueue.front();
        mqueue.pop();
        if(temp->str == s)
            return temp;
        if(temp->left_child != nullptr)
            mqueue.push(temp->left_child);
        if(temp->right_child != nullptr)
            mqueue.push(temp->right_child);
    }
    return nullptr;
}

void update(Node*);

void addOnce(Node *node){ // 所有parent->count + 1
    while(node->parent != nullptr){
        ++(node->parent->count);
        node = node->parent;
    }
}

int findStep(Node*);
void printHuffman();

void update(Node *node){ // 此节点次数+1, 并且更新整个树(可能交换)
    if(node == nullptr)
        return;
    if(node == &root){
        ++(node->count);
        return;
    }
    ++(node->count);
    std::queue<Node*> mqueue = find(&root, node->count - 1); // 找到所有与此节点count相等的节点
    if(mqueue.empty()){ // 如果没有与此节点相等的节点则直接++count, 然后update parent
        update(node->parent);
        return;
    }
    /**
     * 从队列中选出编号最大的节点, 进行交换
     */
    Node *temp = nullptr; // 即将被交换的节点
    Node *mfront;
    int front_path, node_path = findStep(node);
    while(!mqueue.empty()){
        mfront = mqueue.front();
        front_path = findStep(mfront);
        if(front_path > node_path) // 队列中的节点在此节点的下方, 则不交换
            break;
        else if(front_path == node_path){ // 只能与同一层右边的交换, 不能与左边的交换
            Node *front_par = mfront, *node_par = node;
            int base = 1, front_sum = 0, node_sum = 0;
            while(front_par != node_par){
                if(front_par == front_par->parent->right_child)
                    front_sum += base;
                if(node_par == node_par->parent->right_child)
                    node_sum += base;
                ++base;
                front_par = front_par->parent;
                node_par = node_par->parent;
            }
            if(front_sum > node_sum)
                temp = mfront;
        }else{  // 队列中的节点在此节点的上方
            if(mfront->left_child != node && mfront->right_child != node) // 不与父节点交换
                temp = mfront;
        }
        mqueue.pop();
    }

    if(temp != nullptr){
        if(temp->parent == node->parent){
            if(temp->parent->left_child == temp){
                temp->parent->left_child = node;
                temp->parent->right_child = temp;
            }else{
                temp->parent->left_child = temp;
                temp->parent->right_child = node;
            }
        }else{
            Node *node_par = node->parent, *temp_par = temp->parent;
            if(node_par->left_child == node)    node_par->left_child = temp;
            else    node_par->right_child = temp;
            if(temp_par->left_child == temp)    temp_par->left_child = node;
            else    temp_par->right_child = node;
            node->parent = temp_par, temp->parent = node_par;
        }
    }
    update(node->parent);
}

int findStep(Node *node){ // 从此节点到根节点的距离
    int step = 0;
    while(node != &root){
        node = node->parent;
        ++step;
    }
    return step;
}

void insert(Node *root, Node *node){ // 插入一个新节点, 此字符之前未出现过
    if(root->left_child == nullptr && root->right_child == nullptr){ // 只有一个根节点
        root->count = 0;
        root->str = (char)-2;
        root->left_child = new Node(); // count = 0, str = -1;
        root->left_child->parent = root;
        root->right_child = node;
        node->parent = root;
        return;
    }
    Node *blank = find(root, (char)-1);
    blank->count = 0;
    blank->str = (char)-2;
    blank->left_child = new Node(); // count = 0, str = -1
    blank->left_child->parent = blank;
    blank->right_child = node;
    node->parent = blank;
}

void freeNode(){ // 自底而上删除节点
    std::stack<Node*> mstack;
    std::queue<Node*> mqueue;
    mqueue.push(&root);
    while(!mqueue.empty()){
        Node *temp = mqueue.front();
        mstack.push(temp);
        if(temp->left_child != nullptr)
            mqueue.push(temp->left_child);
        if(temp->right_child != nullptr)
            mqueue.push(temp->right_child);
        mqueue.pop();
    }
    while(mstack.size() > 1){
        delete mstack.top();
        mstack.pop();
    }
}

void printHuffman(){ // 广度优先遍历huff树并printf
    std::queue<Node*> mqueue;
    mqueue.push(&root);
    int path = -1;
    Node *mfront;
    while(!mqueue.empty()){
        mfront = mqueue.front();
        if(path < findStep(mfront))
            printf("\n\n%d\n", ++path);
        printf("Node: %d", mfront->count);
        if(mfront->str == (char)-1)
            printf("    -1  ");
        else if(mfront->str == (char)-2)
            printf("    -2  ");
        else
            printf("    %c  ", mfront->str);
        if(mfront->left_child != nullptr)
            mqueue.push(mfront->left_child);
        if(mfront->right_child != nullptr)
            mqueue.push(mfront->right_child);
        mqueue.pop();
    }
}

/**
 * 深度优先搜索各个char的编码, 然后存储到map中
 */
void dfs(Node *ptr, std::string str, std::map<char, std::string> &mmap){
    if(ptr == nullptr)
        return;
    if(ptr->str != (char)-1 && ptr->str != (char)-2){
        mmap.insert(std::make_pair(ptr->str, str));
        return;
    }else{
        dfs(ptr->left_child, str + '0', mmap);
        dfs(ptr->right_child, str + '1', mmap);
    }
}

void write2File(const std::map<char, std::string> &mmap){ // 编码到encode中
    std::ifstream hoff_file("../txt/huffman.txt", std::ios::in);
    std::ofstream encode_file("encode", std::ios::out | std::ios::binary);
    char value, bin_content;
    int base = 0, len;
    std::string str;
    memset(&bin_content, 0, sizeof(bin_content));
    while(hoff_file.peek() != EOF){
        hoff_file.read((char*)&value, sizeof(value));
        str = mmap.find(value)->second;
        len = str.length();
        for(int i = 0; i < len; ++i){
            if(str[i] == '1')
                bin_content = bin_content | (1 << (7 - base));
            if(++base >= 8){
                encode_file.write((char*)&bin_content, sizeof(bin_content));
                base -= 8;
                memset(&bin_content, 0, sizeof(bin_content));
            }
        }
    }
    hoff_file.close();
    encode_file.close();
}

void readFromFile(){ // 解码到txt文件中
    std::ifstream encode_file("encode", std::ios::in | std::ios::binary);
    std::ofstream decode_file("decode.txt", std::ios::out);
    char value, bin_content;
    Node *ptr = &root;
    while(encode_file.peek() != EOF){
        encode_file.read((char*)&bin_content, sizeof(char));
        for(int i = 0; i < 8; ++i){
            if((bin_content >> (7 - i)) & 1) // 此位为1
                ptr = ptr->right_child;
            else
                ptr = ptr->left_child;
            if(ptr->str != -1 && ptr->str != -2){
                decode_file.write((char*)&(ptr->str), sizeof(char));
                ptr = &root;
            }
        }
    }
    encode_file.close();
    decode_file.close();
}

int main(){
    char value;
    std::ifstream mfile("../txt/huffman.txt", std::ios::in);
    while(mfile.peek() != EOF){
        mfile.read((char*)&value, sizeof(value));
        Node *temp = find(&root, value);
        if(temp != nullptr)
            update(temp);
        else{
            Node *ptr = new Node(1, value);
            insert(&root, ptr);
            update(ptr->parent);
        }
    }
    mfile.close();
    std::map<char, std::string> mmap;
    std::string str;
    dfs(&root, str, mmap);
    for(std::map<char, std::string>::iterator it = mmap.begin(); it != mmap.end(); ++it)
        printf("char: %c  ascii: %d  encode: %s\n", it->first, (int)it->first, it->second.c_str());
    write2File(mmap);
    printf("[main] encode to this file-->bin/encode\n");
    readFromFile();
    printf("[main] decode to this file-->bin/decode.txt\n");
    freeNode();
    return 0;
}