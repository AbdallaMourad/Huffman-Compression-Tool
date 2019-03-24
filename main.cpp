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

    this->extraBit = 0;
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

int binaryToDecimal(string bin){
    int number = 0;
    for(int i=0, j=(int)bin.size()-1; i<(int)bin.size(); ++i, j--){
        if(bin[i] == '1')
            number += (1<<j);
    }
    return number;
}

string decimalToBinary(int number){
    string bin = "";
    for(int i=0, j=7; i<8; i++, j--){
        if((1<<j) <= number){
            number -= (1<<j);
            bin += '1';
        } else {
            bin += '0';
        }
        return bin;
    }
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
    * get the code for correspond caracter
    * if the code is not 8 bit change it to character
    * check if the last byte is complete or not 
    * if not append 0 then add to buffer
    * add buffer to file encode.bin
    */

    freopen(fileName.c_str(), "r", stdin);
    ofstream encode("encode.bin", ios::out | ios::binary);
    if(!encode){
        cout << "Cannot open file" << endl;
        exit(1);
    }

    char let;
    string bits = "";
    string buffer = "";

    while(scanf("%c", &let) != EOF){
        for(int i=0; i<(int)generatedTable[let].size(); ++i){
            bits += generatedTable[let][i];
            if((int)bits.size() % 8 == 0){
                buffer += char(binaryToDecimal(bits));
                bits = "";
            }
        }        
    }

    if((int)bits.size() < 8 && (int)bits.size() != 0){
        while((int)bits.size() != 8){
            bits += "0";
            ++this->extraBit;
        }        
        buffer += char(binaryToDecimal(bits));
        bits = "";
    }  
    encode.write(buffer.c_str(), buffer.size());
    encode.close();
    fclose(stdin);
    cout << "COMPRESSED" << endl;
    decompress();
}

void Huffman::decompress(){
    //TODO
    /*
    * read from encoded file
    * change the binary representation to character
    * write in the output file.
    */


    char let;
    string binCode = "";
    string bufStr = "";

    freopen("encode.bin", "r", stdin);
    ofstream decode("decode.txt");
    if(!decode){
        cout << "ERROR WHILE OPENING The FILE" << endl;
        exit(1);
    }

    while(scanf("%c", &let) != EOF){
        bitset<8> bt = let;
		binCode += bt.to_string();    
    }
    fclose(stdin);

    for(int i=0; i<(int)binCode.size()-this->extraBit   ; i++){
        bufStr += binCode[i];

        for(auto itr : this->generatedTable){
            if(bufStr == itr.second){
                decode << itr.first;
                bufStr = "";
            }
        }
    }

    decode.close();
    cout << "DECOMPRESSED" << endl;
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