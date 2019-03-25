using namespace std;

class Node{
    /*
    * Create nodes which has
    * Character as data
    * how many is it repeated as rank
    * node to the left and right 
    */
    private:
        int rank;
        char data;
        Node * left;
        Node *right;
    public:
        Node(){}
        Node(int rank, char data);
        int getRank(){return this->rank;}
        char getData(){return this->data;}
        Node * getLeft(){return this->left;}
        Node * getRight(){return this->right;}
        void combine(Node* &leftNode, Node* &rightNode);
        
};

class Huffman{
    private:
        Node * root;
        map<char, string>generatedTable;
        map<string, char>decompressTable;
        int extraBit;

    public:
        Huffman(string fileName, string tag);
        Node * constructTree(vector<Node *> charNode);
        void constructTable(Node * currNode, string binCode);
        void compress(string origionalFile);
        void printGenTable();
        void decompress(string encodedFile);
};