#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
using namespace std;

unsigned long long extractBits(unsigned long long origin);

int main(int argc, char **argv) {
    ifstream infile(argv[1]);
    
    
    infile.setf(ios:: hex, ios::basefield);
    infile.setf(ios::showbase);
    
    short rw, offset, hit, foundPos;
    long long LRU[8][10], temp;
    long long addr = 0, data = 0, tag = 0, setNum = 0, SACache[8][4096][3];
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4096; j++) {
            for (int k = 0; k < 10; k++) {
                LRU[i][k] = -1; //initialize LRU checker
            }
            for (int m = 0; m < 3; m++) {
                SACache[i][j][m] = -1; //initialize data storage
            }
        }
    }
    
    while (infile >> addr >> rw >> data ) {
        tag = addr >> 4;
        offset = addr % 2;
        setNum = extractBits(addr >> 1);
        
        hit = 0;
        foundPos = -1;
        
        if (rw == 0) { //read. call LRU 

            for (int i = 0; i < 3; i++) {
                if (LRU[setNum][i] == tag) {
                    hit = 1;
                    foundPos = i;
                }
            }
            
            if (hit == 0) {
                for (int i = 3; i < 10; i++) {
                    if (LRU[setNum][i] == tag) {
                        hit = 0;
                        foundPos = i;
                    }
                    SACache[setNum][LRU[setNum][i]][2] = 0; //reset dirty bits for those outside the cache
                }
            }
            
            if (hit == 1) { //found in positions 0 1 or 2 of LRU structure
                temp = LRU[setNum][foundPos]; //store found tag into temp
                
                if (foundPos == 0) { //what we found is already at the top
                    
                }
                
                else if (foundPos == 1) { //find(tag) = second. second = first, first = temp
                    LRU[setNum][1] = LRU[setNum][0];
                    LRU[setNum][0] = temp;
                }
                
                else if (foundPos == 2) { //find(tag) = third. third = second, second = first, first = temp
                    LRU[setNum][2] = LRU[setNum][1]; //2 -> 3
                    LRU[setNum][1] = LRU[setNum][0]; //1 -> 2
                    LRU[setNum][0] = temp;
                }
                
                if (SACache[setNum][LRU[setNum][0]][offset] == -1) { //if hit but not valid data
                    cout << left << hex << addr << left << setw(5) << hit << left << setw(5) << "0" << endl;
                }
                
                else if ( SACache[setNum][LRU[setNum][0]][offset] != -1) { //hit and good data
                    cout << left << setw(40) << hex << SACache[setNum][LRU[setNum][0]][offset] << left << setw(5) << hit << left << setw(5) << SACache[setNum][LRU[setNum][0]][2] << endl;
                }
            }
            
            else if (hit == 0) {
                if (foundPos == -1) { //nowhere in LRU 
                    temp = tag; 
                    
                    for (int k = 9; k > 0; k--) {
                        LRU[setNum][k] = LRU[setNum][k-1];
                    }
                    LRU[setNum][0] = temp;
                }
                
                else if (foundPos != -1) { //in MM but not cache
                    temp = LRU[setNum][foundPos];
                    
                    for (int k = foundPos; k > 0; k--) {
                        LRU[setNum][k] = LRU[setNum][k-1];
                    }
                    LRU[setNum][0] = temp;
                }
                
                if (SACache[setNum][LRU[setNum][0]][offset] == -1) { //no hit, bad data
                    cout << hex << addr << left << setw(5) << hit << left << setw(5) << "0" << endl;
                }
                
                else if (SACache[setNum][LRU[setNum][0]][offset] != -1) { //no hit, good data
                    cout << left << setw(40) << hex << SACache[setNum][LRU[setNum][0]][offset] << left << setw(5) << hit << left << setw(5) << "0" << endl;
                }
            }
            
        }
        
        if (rw == 255) { //write, call LRU then write to SACache[setNum][LRU[setNum][0]][offset]
            for (int i = 0; i < 3; i++) {
                if (LRU[setNum][i] == tag) {
                    hit = 1;
                    foundPos = i;
                }
            }
            
            if (hit == 0) {
                for (int i = 3; i < 10; i++) {
                    if (LRU[setNum][i] == tag) {
                        hit = 0;
                        foundPos = i;
                    }
                    
                    if (LRU[setNum][i] != -1) {
                        SACache[setNum][LRU[setNum][i]][2] = 0; //reset dirty bits for those outside the cache
                    }
                }
            }
            
            if (hit == 1) { //found in positions 0 1 or 2 of LRU structure
                temp = LRU[setNum][foundPos]; //store found tag into temp
                
                if (foundPos == 0) { //what we found is already at the top
                    
                }
                
                else if (foundPos == 1) { //find(tag) = second. second = first, first = temp
                    LRU[setNum][1] = LRU[setNum][0];
                    LRU[setNum][0] = temp;
                }
                
                else if (foundPos == 2) { //find(tag) = third. third = second, second = first, first = temp
                    LRU[setNum][2] = LRU[setNum][1]; //2 -> 3
                    LRU[setNum][1] = LRU[setNum][0]; //1 -> 2
                    LRU[setNum][0] = temp;
                }
                
                SACache[setNum][LRU[setNum][0]][offset] = data;
                SACache[setNum][LRU[setNum][0]][2] = 1; //set dirty
                
            }
            
            else if (hit == 0) {
                if (foundPos == -1) { //nowhere in LRU 
                    temp = tag; 
                    
                    for (int k = 9; k > 0; k--) {
                        LRU[setNum][k] = LRU[setNum][k-1];
                    }
                    LRU[setNum][0] = temp;
                }
                
                else if (foundPos != -1) { //in MM but not cache
                    temp = LRU[setNum][foundPos];
                    
                    for (int k = foundPos; k > 0; k--) {
                        LRU[setNum][k] = LRU[setNum][k-1];
                    }
                    LRU[setNum][0] = temp;
                }
                
                
                SACache[setNum][LRU[setNum][0]][offset] = data;
                SACache[setNum][LRU[setNum][0]][2] = 1; //set dirty
               
            }
            
        }
        
        
    }
    return 0;
}

unsigned long long extractBits(unsigned long long origin) { 
    unsigned long long runningSum = 0;
    short i;
    
    for (i = 0; i < 3; i++) { //extract 3 bits
        runningSum += (origin % 2) * pow(2.0, i);
        origin /= 2;
    }
    return runningSum;
}