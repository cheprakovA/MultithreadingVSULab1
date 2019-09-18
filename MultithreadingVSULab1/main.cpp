//
//  main.cpp
//  MultithreadingVSULab1
//
//  Created by Artemy Cheprakov on 19/09/2019.
//  Copyright © 2019 VSU. All rights reserved.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

const int VEC_SIZE = 10;
const short int THREADS_AMOUNT = 4;

template<typename T>
void fillVector(std::vector<T> &vec) throw (std::string) {
    
    switch( *typeid(T).name() ) {
        case 'i':
            for (int i = 0; i < VEC_SIZE; ++i)
                vec.push_back(rand() % 101);
            break;
        case 'd': case 'f':
            double val;
            for (int i = 0; i < VEC_SIZE; ++i) {
                val = rand() % 100;
                val += (rand() % 100) / 100.0;
                vec.push_back(val);
            }
            break;
        default:
            throw "Incorrect mass type";
    }
}

template <typename T>
short int getLastDigit(T var) {

    switch( *typeid(T).name() ) {
        case 'i':
            return static_cast<int>(var) % 10;
        case 'd': case 'f':
            while(var - static_cast<int>(var) != 0.0)
                var *= 10;
            return static_cast<int>(var) % 10;
        default:
            return -1;
    }
}

template <typename T>
void searchForLastDigitInMass(const std::vector<T> &vec, short int digit, bool &flag, unsigned int beginIndex, unsigned int endIndex) {
    
    short int var;
    
    for (unsigned int i = beginIndex; i < endIndex; ++i) {
        
        var = getLastDigit(vec[i]);
        if (var == digit) {
            flag = true;
            break;
        } else continue;
    }
}

template <typename T>
void showVec(const std::vector<T> &vec) {
    
    std::cout << std::endl;

    for (int i = 0; i < vec.size(); ++i)
        std::cout << vec[i] << " ";
    
    std::cout << std::endl;
}

int main() {
    
    srand(time(NULL));
    
    bool flag = false;
    const short int searchedValue = 3;
    
    // std::vector<int> vec1;
    std::vector<double> vec2;
    
    try {
        // fillVector(vec1);
        fillVector(vec2);
    } catch (std::string ex) {
        std::cout << ex << std::endl;
        return -1;
    }

    std::thread t1(searchForLastDigitInMass<double>, std::ref(vec2), searchedValue, std::ref(flag), 0, vec2.size() / 2);
    std::thread t2(searchForLastDigitInMass<double>, std::ref(vec2), searchedValue, std::ref(flag), vec2.size() / 2, vec2.size());
    
    
    t1.join();
    t2.join();
    
    showVec(vec2);
    
    std::cout << flag << std::endl;
    
    return 0;
}
