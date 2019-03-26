#include <bits/stdc++.h>
#include "huffman.h"

#define START_POINT 0
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

Huffman::Huffman(string fileName, string tag){
    /*
    * read data from a file
    * count how many repetition letters are there
    * createNode for every character
    * construct Tree assign it to root attribute
    * construct table which contains the letter with binCode
    * compress the file 
    */
    if(tag == "-c"){
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
    } else if(tag == "-d"){
        decompress(fileName);
    }
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
    }
    return bin;
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

string checkbits(string bits){
    while((int)bits.size() % 8 != 0){
        bits+="0";
    }
    return bits;
}

void Huffman::compress(string origionalFile){
    /*
    * open the file
    * get the code for correspond caracter
    * if the code is not 8 bit change it to character
    * check if the last byte is complete or not 
    * if not append 0 then add to buffer
    * add buffer to file encode.bin
    */
    string key_size_bin="";
    string header = "";

    for(int i=START_POINT; i<END_POINT; ++i){
        key_size_bin += decimalToBinary(i);

        if(generatedTable.find(char(i)) != generatedTable.end()){
            string binCode = generatedTable[char(i)];
            key_size_bin += decimalToBinary((int)binCode.size())+checkbits(binCode);
        }
        else
            key_size_bin += decimalToBinary(0);

        //change all binary bits to bytes
        for(int j=0; j<(int)key_size_bin.size(); j+=8){
            header += char(binaryToDecimal(key_size_bin.substr(j, 8)));
        }
        key_size_bin = ""; 
    }

    freopen(origionalFile.c_str(), "r", stdin);
    ofstream encode("encode.bin", ios::out | ios::binary);
    if(!encode){
        cout << "Cannot open file" << endl;
        exit(1);
    }

    char let;
    string bits = "";
    string buffer = header;
    string bitBuff = ""; //save the content of file

    while(scanf("%c", &let) != EOF){
        for(int i=0; i<(int)generatedTable[let].size(); ++i){
            bits += generatedTable[let][i];
            if((int)bits.size() % 8 == 0){
                bitBuff += char(binaryToDecimal(bits));
                bits = "";
            }
        }        
    }
    if((int)bits.size() < 8 && (int)bits.size() != 0){
        //add the size of extra bit after the header.
        buffer += char(8 - int(bits.size()));
        while((int)bits.size() != 8){
            bits += "0";
        }        
        bitBuff += char(binaryToDecimal(bits));
        bits = "";
    } 
    //append the content of file.
    buffer += bitBuff;

    encode.write(buffer.c_str(), buffer.size());
    encode.close();
    fclose(stdin);
    cout << "COMPRESSED" << endl;
}

void Huffman::decompress(string encodedFile){
    /*
    * read from encoded file
    * change the binary representation to character
    * write in the output file.
    */

    char let;
    string binCode = "";
    string bufStr = "";

    freopen(encodedFile.c_str(), "r", stdin);
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


    /*
    * Read the header
    * Make map based on this header
    */
    int counter = 0;
    for(int i=START_POINT; i<END_POINT; i++){
        char key = char(binaryToDecimal(binCode.substr(counter, 8)));//first 8 bits
        int size = binaryToDecimal(binCode.substr(counter+8, 8)); //second 8 bits
        string binRep = binCode.substr(counter+16, size); //store binary representation for a character
        counter += 16+size;
        while(counter % 8 != 0) ++counter;
        if(size != 0)
            decompressTable[binRep] = key;
    }

    int extraBit = binaryToDecimal(binCode.substr(counter, 8));
    counter += 8;

    binCode = binCode.substr(counter, int(binCode.size())-counter);

    //add extraBits to the end of header.   
    for(int i=0; i<(int)binCode.size()-extraBit; i++){
        bufStr += binCode[i];

        if(decompressTable.find(bufStr) != decompressTable.end()){
            decode << decompressTable[bufStr];
            bufStr = "";
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
    if(argc != 3 || (argc == 3 && (string(argv[1]) != "-c" && string(argv[1]) != "-d"))){
        printf("[-] HELP:\n");
        printf("To Compress: %s -c <FILE-NAME>\n", string(argv[0]).c_str());
        printf("To Decompress: %s -d <FILE-NAME>\n", string(argv[0]).c_str());
    } else{
        Huffman huf(argv[2], argv[1]);
    }
    return 0;
}