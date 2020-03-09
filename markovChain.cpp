#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <unistd.h>

using namespace std;

//This is a simple class for each unique word that will record the word as well as create a list(vector) of all of 
//the words that follow it.
class Word{
    public:
        string tempWord;
        vector<string> nextWordList;
        
        Word(string word, string nextWord){
            tempWord = word;
            nextWordList.push_back(nextWord);
        }       
};

vector<Word> MarkovChain;
string tweet = "";

//This function is used to get the index of a specific word in the MarkovChain.
int getMarkovIndex(string word){
    for(int i = 0; i < MarkovChain.size(); i++){
        if(MarkovChain.at(i).tempWord.compare(word) == 0){
            return i;
        }
    }
    return -1;
}

//If a word has not already been added to the Markov chain than the word and the nextWord in the line will be added.
//If a word is already added to the chain than the following word will be added to the list of next words.
void addToChain(string word, string nextWord){
    int index = getMarkovIndex(word);
    
    if(index == -1){
        MarkovChain.push_back(Word(word, nextWord));
    }
    else{
        MarkovChain.at(index).nextWordList.push_back(nextWord);
    }
}

//This function is called when parsing the text file and will go line by line through the text file.
//This function will split up each line into individual words (marked by spaces) and after splitting in a single word
//and a next word, it will add them to the chain using the addToChain function.
void parseLine(string line){
    size_t wordStart = 0;
    size_t wordEnd;
    size_t wordLen;
    size_t nextStart;
    size_t nextEnd;
    size_t nextLen;
    
    string word = "";
    string nextWord = "";
  
    while(wordStart != string::npos){
        wordEnd = line.find(" ", wordStart);
        wordLen = wordEnd - wordStart;
        
        while(wordLen == 0){
            wordStart++;
            wordEnd = line.find(" ", wordStart);
            wordLen = wordEnd - wordStart;
            
            if(wordStart >= line.length()){
                wordStart = line.length();
                break;
            }
        }
        
        word = line.substr(wordStart, wordLen);
        
        if(wordEnd != string::npos){
            nextStart = wordEnd + 1;
            nextEnd = line.find(" ", nextStart);
            nextLen = nextEnd - nextStart;
            
            while(nextLen == 0){
                nextStart++;
                nextEnd = line.find(" ", nextStart);
                nextLen = nextEnd - nextStart;
                if(nextStart >= line.length()){
                    nextStart = line.length();
                    break;
                }
            }
            
            word = line.substr(wordStart, wordLen);
            nextWord = line.substr(nextStart, nextLen); 
            
            wordStart = nextStart;    
        }
        else{
            nextWord = "";
            wordStart = string::npos;
        }
        addToChain(word, nextWord);
    }
}

//This function will take in an input file and then proceed to parse it line by line until the entire text file is parsed.
int parseFile(string file){
    string line;
    
    ifstream myfile(file.c_str());
    if (myfile.is_open()){
        while(getline (myfile,line)){
            parseLine(line);
        }
        myfile.close();
        return 1;
    }
    else{
        return 0;
    }
}

//This function of the program that will take the words and list gathered by the parsing functions and create a tweeet
//based off of those entries. Tweet lengths will very will not exceed 280 characters (like a normal tweet).
void createTweet(){
    int wordIndex = rand() % MarkovChain.size(); 
    int nextWordIndex = 0;
    int nextWordCount = 0;
    
    bool tweetMax = false;
    string prevWord = "";
    
    tweet = "";
    tweet = MarkovChain.at(wordIndex).tempWord;
    prevWord = tweet;
    
    while(!tweetMax){
        wordIndex = getMarkovIndex(prevWord);
        Word temp = MarkovChain.at(wordIndex);
        
        nextWordCount = temp.nextWordList.size();
        nextWordIndex = rand() % nextWordCount;  

        prevWord = temp.nextWordList.at(nextWordIndex);
        
        if(prevWord.length() == 0){
            tweetMax = true;
        }
        else{
            if(tweet.length() + prevWord.length() > 279){
                tweetMax = true;
            }
            else{
                tweet = tweet + " " + prevWord;
            }
        }
    }
    cout << tweet << endl;
}

//The main function of this program simply asks the user to input the name of a text file and then generates tweets (5 at the moment)
//based on the contents of the parsed file.
int main(int argc, char* argv[]){
    if(argc <= 1){
        cout << "Please provide name of text file." << endl;
        return 0;
    }
    
    string arguement(argv[1]);
    srand(time(NULL));
    
    if(parseFile(arguement)){
        cout << "Parsing the entered text file " << arguement << "..." << endl;
        for(int tweetCount = 0; tweetCount < 5; tweetCount++) {
            cout << "////////////This is a line to separate the tweets///////////" << endl;
            createTweet();
        }
    }
    else{
        cout << "File "<< arguement << " was not found" << endl;
    }
    
    return 0;
}
