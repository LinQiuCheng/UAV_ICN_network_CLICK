#include <iostream>
#include <unordered_map>
using namespace std;

struct Value{
    uint64_t create_time; //创建时间
};
// 双向链表的节点结构
struct PITTableNode {
    uint32_t key;
    Value *value;
    PITTableNode* prev;
    PITTableNode* next;
    PITTableNode():key(0),value(NULL),prev(NULL),next(NULL){}
};



class PITTable
{
private:
    unordered_map<uint32_t, PITTableNode*> m;  // 代替hash_map
    PITTableNode* head;     // 指向双链表的头结点
    PITTableNode* tail;     // 指向双链表的尾结点
    int capacity;           // Cache的容量
    int count;              // 计数
public:
    PITTable(int capacity);       // 构造函数
    ~PITTable();                  // 析构函数
    Value* get(uint32_t key);             // 查询数据项
    void set(uint32_t key, Value *value); // 未满时插入,已满时替换
    void DeleteNode(uint32_t key);   //删除某数据项
private:
    void removeLRUNode();                 // 删除尾结点（最久未使用）
    void detachNode(PITTableNode* node);    // 分离当前结点
    void insertToFront(PITTableNode* node); // 节点插入到头部
};


PITTable::PITTable(int capacity)
{
    this->capacity = capacity;
    this->count = 0;
    head = new PITTableNode;
    tail = new PITTableNode;
    head->prev = NULL;
    head->next = tail;
    tail->prev = head;
    tail->next = NULL;
}

PITTable::~PITTable()
{
    delete head;
    delete tail;
}

Value * PITTable::get(uint32_t key)
{
    if(m.find(key) == m.end())  // 没找到
        return NULL;
    else
    {
        PITTableNode* node = m[key];
        detachNode(node);      // 命中，移至头部
        insertToFront(node);
        return node->value;
    }
}

void PITTable::set(uint32_t key, Value *value)
{
    if(m.find(key) == m.end())  // 没找到
    {
        PITTableNode* node = new PITTableNode;
        if(count == capacity)   // Cache已满
            removeLRUNode();
        
        node->key = key;
        node->value = value;
        m[key] = node;          // 插入哈希表
        insertToFront(node);    // 插入链表头部
        ++count;
    }
    else
    {
        PITTableNode* node = m[key];
        detachNode(node);
        node->value = value;
        insertToFront(node);
    }
}

void PITTable::DeleteNode(uint32_t key)
{
    PITTableNode* node=m[key];
    detachNode(node);
    m.erase(node->key);
    this->count--;
}

void PITTable::removeLRUNode()
{
    PITTableNode* node = tail->prev;
    detachNode(node);
    m.erase(node->key);
    --count;
}

void PITTable::detachNode(PITTableNode* node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}


void PITTable::insertToFront(PITTableNode* node)
{
    node->next = head->next;
    node->prev = head;
    head->next = node;
    node->next->prev = node;
}
