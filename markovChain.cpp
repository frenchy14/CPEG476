#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <unistd.h>

using namespace std;

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

int getMarkovIndex(string word){
    for(int i = 0; i < MarkovChain.size(); i++){
        if(MarkovChain.at(i).word.compare(word) == 0){
            return i;
        }
    }
    return -1;
}

void addToChain(string word, string nextWord){
    int index = getMarkovIndex(word);
    
    if(index == -1){
        MarkovChain.push_back(Word(word, nextWord));
    }
    else{
        MarkovChain.at(index).nextWordList.push_back(nextWord);
    }
}

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


void createTweet(){
    int wordIndex = rand() % MarkovChain.size(); 
    int nextWordIndex = 0;
    int nextWordCount = 0;
    
    bool tweetMax = false;
    string prevWord = "";
    
    tweet = "";
    tweet = MarkovChain.at(wordIndex).word;
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
    cout << "[" << tweet.length() << " characters]" << endl;
}

int main(int argc, char* argv[]){
    if(argc <= 1){
        cout << "Please provide name of text file." << endl;
        return 0;
    }
    
    string arguement(argv[1]);
    srand(time(NULL));
    
    if(parseFile(arguement)){
        cout << "Parsing text file " << arguement << "..." << endl;
        for(int tweetCount = 0; tweetCount < 5; tweetCount++) {
        createTweet();
    }
    else{
        cout << "File "<< arguement << " was not found" << endl;
    }
    
    return 0;
}
