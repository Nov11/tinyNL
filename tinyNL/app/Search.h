//
// Created by c6s on 17-5-31.
//

#ifndef TINYNL_SEARCH_H
#define TINYNL_SEARCH_H

#include <vector>
#include <string>

class Search{
    std::string pattern_;
    std::vector<int> next;

    int len_;
    void makeNext(){
        next[0] = -1;
        next[1] = 0;

        for(int i = 2; i <= len_; i++){
            int j = i - 1;
            while(j > 0 && pattern_[next[j]] != pattern_[i - 1]){j = next[j];}
            if(j == 0){next[i] = 0;}
            else{next[i] = next[j] + 1;}
        }
    }
public:
    Search(const std::string& pattern)
            :pattern_(pattern), next(pattern.size() + 1), len_(static_cast<int>(pattern_.size()))
    {makeNext();}

    template<class T>
    bool match(T& target, size_t& pos){
        int j = 0;

        for(size_t i = 0; i < target.size(); i++){
            if(pattern_[j] == target[i]){
                j++;
                if(j == len_){
                    pos =  i + 1 - len_;
                    return true;
                }
                continue;
            }
            while(j >= 0 && target[i] != pattern_[next[j]]){j = next[j];}
            if(j == -1){j = 0;}
            else{
                j++;
            }
        }
        return false;
    }

    template<class T>
    bool match(T tbeg, size_t tlen, size_t* pos ){
        int j = 0;

        for(size_t i = 0; i < tlen; i++){
            if(pattern_[j] == tbeg[i]){
                j++;
                if(j == len_){
                    if(pos){*pos =  i + 1 - len_;}
                    return true;
                }
                continue;
            }
            while(j >= 0 && tbeg[i] != pattern_[next[j]]){j = next[j];}
            if(j == -1){j = 0;}
            else{
                j++;
            }
        }
        return false;
    }
};


#endif //TINYNL_SEARCH_H
