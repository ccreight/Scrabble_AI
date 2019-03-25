#include "Trie.h"
#include <iostream>

TrieSet::TrieSet(){ //initializes null root
	root = new TrieNode;
	root->inSet = false;
	root->parent = nullptr;
	for(int x = 0; x < 26; x++){
		root->children[x] = nullptr;
	}
}

//recursive deletion
void TrieSet::deleteHelper(TrieNode* root){

	if(root == nullptr){
		return;
	}

	for(int x = 0; x < 26; x++){
		deleteHelper(root->children[x]);
	}

	delete root;
}

TrieSet::~TrieSet(){
	deleteHelper(root);
}

void TrieSet::insert(std::string input){

	if(root == nullptr){ //nothing to do
		return;
	}

	//just in case
	for(size_t x = 0; x < input.size(); x++){
		input[x] = tolower(input[x]);
	}

	TrieNode* temp = root; //for traversing

	//inserts each letter at a time
	for(size_t x = 0; x < input.size(); x++){
		if(temp->children[input[x] - 97] == nullptr){
			TrieNode* newNode = new TrieNode;
			newNode->inSet = false;
			newNode->parent = temp;
			//have to also initialize newNode's children to nullptr?
			for(int x = 0; x < 26; x++){
				newNode->children[x] = nullptr;
			}
			temp->children[input[x] - 97] = newNode;
		}
		temp = temp->children[input[x] - 97];
	}

	temp->inSet = true; //it's a word!

}

void TrieSet::remove(std::string input){

	TrieNode* temp = root;

	//nothing to do
	if(root == nullptr || prefix(input) == nullptr){
		return;
	}

	//moves to last letter
	for(size_t x = 0; x < input.size(); x++){
		input[x] = tolower(input[x]);
		if(temp->children[input[x] - 97] == nullptr){
			return;
		}
		temp = temp->children[input[x] - 97];
	}

	int count = 0, indexCheck = input.size() - 1;

	while(temp != nullptr){ //while applicable, deletes

		bool isLeaf = true;

		//check if leaf node
		for(size_t x = 0; x < 26; x++){
			if(temp->children[x] != nullptr){
				isLeaf = false;
			}
		}

		if(!isLeaf && count == 0){ //has children
			temp->inSet = false;
			return;
		}
		else if(!isLeaf){ //not a leaf
			temp = root;
			return;
		}
		else if(count != 0 && temp->inSet){ //end of a word
			temp = root;
			return;
		}
		else{ //leaf
			if(temp->parent != nullptr){
				TrieNode* toDelete = temp;
				temp = temp->parent;
				delete toDelete;
				toDelete = nullptr;
				temp->children[input[indexCheck]-97] = nullptr;
				indexCheck--;
			}
			else{
				return;
			}
		}
		count++;
	}

}

TrieNode* TrieSet::prefix(std::string px){

	TrieNode* temp = root;

	if(root == nullptr){ //just do nothing
		return nullptr;
	}

	//traverses trie
	for(size_t x = 0; x < px.size(); x++){
		px[x] = tolower(px[x]);
		if(px[x] > 122 || px[x] < 97){
			continue;
		}
		if(temp->children[px[x] - 97] == nullptr){ //letter not there
			return nullptr;
		}
		temp = temp->children[px[x] - 97];
	}
	return temp;
}