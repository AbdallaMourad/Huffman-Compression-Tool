#include <bits/stdc++.h>
#include "huffman.h"

#define START_POINT 32
#define END_POINT 127

using namespace std;

Node::Node(int rank, char data){
    this->rank = rank;
    this->data = data;
    this->left = NULL;
    this->right = NULL;
}

void Node::combine(Node * &leftNode, Node * &rightNode){
    /*
    * Connect two nodes with one parent
    * Assign the summation of their rank to parent
    */
    this->left = leftNode;
    this->right = rightNode;
    this->rank = leftNode->getRank()+rightNode->getRank();
}

Huffman::Huffman(string fileName){
    /*
    * read data from a file
    * count how many repetition letters are there
    * createNode for every character
    * construct Tree assign it to root attribute
    * construct table which contains the letter with binCode
    * compress the file 
    */

    freopen(fileName.c_str(), "r", stdin);
    char letter;
    vector<int>rank(127, 0); //printable character in asscii code;
    vector<Node * > charNode;

    while(scanf("%c", &letter) != EOF){
        ++rank[int(letter)];
    }
    fclose(stdin);

    for(int i=START_POINT; i<END_POINT; ++i){
        if(rank[i] > 0){
            Node * newNode = new Node(rank[i], char(i));
            charNode.push_back(newNode);
        }
    }

    this->root = constructTree(charNode);
    this->constructTable(this->root, "");
    this->compress(fileName);
}

bool compareRank(Node * firstNode, Node * secondNode){
    if(firstNode->getRank() > secondNode->getRank())
        return true;
    return false;
}

Node * Huffman::constructTree(vector<Node *> charNode){
    /*
    * make minimum heap using priority queue
    * connect all nodes with parents
    * at the end min_heap will be of size 1 which is the root 
    */

   priority_queue<Node *, vector<Node *>, decltype(&compareRank)> min_heap(&compareRank);

   for(int i=0; i<(int)charNode.size(); ++i){
       min_heap.push(charNode[i]);
   }

   while((int)min_heap.size() != 1){
        //push 2 nodes at a time
        //make parent then push into min_heap
        Node * leftNode = min_heap.top();
        min_heap.pop();

        Node * rightNode = min_heap.top();
        min_heap.pop();

        Node * newParent = new Node();
        newParent->combine(leftNode, rightNode);
 
        min_heap.push(newParent);
   }
   return min_heap.top();
}

void Huffman::constructTable(Node * currNode, string binCode){
    /*
    * CHECK IF THE RIGHT AND LEFT NODES ARE NULL
    * IF SO INIZIALIZE THE MAP TO CHARACTER AND BINCODE
    * ELSE CALL THE LEFTNODE
    * CALL THE RIGHT NODE
    */

    if(currNode->getLeft() == NULL && currNode->getRight() == NULL){
        this->generatedTable[currNode->getData()] = binCode;
        return;
    }
    this->constructTable(currNode->getLeft(), binCode + "0");
    this->constructTable(currNode->getRight(), binCode + "1");
    return;
}


void Huffman::compress(string fileName){
    /*
    * open the file
    * get the codefor correspond caracter
    * if the code is not 8 bit append the next character
    * check if the last byte is complete or not 
    * if not append 0
    */

    freopen(fileName.c_str(), "r", stdin);
    ofstream encode("encode.bin", ios::out | ios::binary);
    if(!encode){
        cout << "Cannot open file" << endl;
        exit(1);
    }

    char let;
    string bits = "";
    while(scanf("%c", &let) != EOF){
        for(int i=0; i<(int)generatedTable[let].size(); ++i){
            bits += generatedTable[let][i];
            if((int)bits.size() == 8){
                encode.write((char*) &bits, sizeof(char));
                bits = "";
            }
        }        
    }

    if((int)bits.size() < 8 && (int)bits.size() != 0){
        while((int)bits.size() != 8){
            bits += "0";
        }
        encode.write((char*) &bits, sizeof(char));
        bits = "";
    }  
    encode.close();
    fclose(stdin);
    cout << "COMPRESSED" << endl;
    //decompress();
}

void Huffman::decompress(){
    //TODO
}

void Huffman::printGenTable(){
    for (auto itr:this->generatedTable){
        cout << itr.first << " " << itr.second << endl;
    }
}

int main(int argc, char *argv[]){
    Huffman obj("testdata.txt"); 
    return 0;
}