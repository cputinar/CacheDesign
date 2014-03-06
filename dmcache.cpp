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
    
    short rw, offset, inCache[64];
    long long addr = 0, data = 0, tag = 0, lineNum = 0, DMCache[64][512][3];
    
    for (int i = 0; i < 64; i++) {
        inCache[i] = -1; //initialize cache check
        for (int j = 0; j < 512; j++) {
            for (int k = 0; k < 3; k++) {
                DMCache[i][j][k] = -1; //initialize data storage
            }
        }
    }
    
    while (infile >> addr >> rw >> data ) {
        
        tag = (unsigned)addr >> 7; //set tag, range is 0-511
        offset = addr % 2; // set offset, range 0-1
        lineNum = extractBits(addr >> 1);
        
        //cout << dec << "tag: " << tag << dec <<  " offset: " << offset << dec <<  " lineNum: " << lineNum << endl;
        
        if (rw == 255) { //write
            DMCache[lineNum][tag][offset] = data; //write data
            
            DMCache[lineNum][tag][2] = 1; //we have no writeback policy. always dirty on write no further action
            
            if (inCache[lineNum] != tag) { //if the block(line+tag) we are looking for is not in cache, put it in. else nothing
                inCache[lineNum] = tag;
            }
            else if(inCache[lineNum] == tag) {
                //nothing
            }
            
           
        }
        
        else if (rw == 0) { //read
            
            if (inCache[lineNum] != tag) { //read something that is not in cache. hit = 0, 
                DMCache[lineNum][tag][2] = 0; //since its from MM, dirty bit = 0
                if (DMCache[lineNum][tag][offset] != -1) {
                    cout << left << setw(36) << hex << DMCache[lineNum][tag][offset] << left << setw(5) << "0" << left << setw(5) << DMCache[lineNum][tag][2] << endl;
                }
                else if(DMCache[lineNum][tag][offset] == -1) {
                    cout << left << hex << addr << left << setw(5) << "0" << left << setw(5) << "0" << endl;
                }
            }
            
            else if (inCache[lineNum] == tag) { //read something that is in cache
                if (DMCache[lineNum][tag][offset] == -1) { //its in cache, but hasnt been written to => garbage, print read addr 0 0
                    cout << left << hex << data <<  left << setw(5) << "1" << left << setw(5) << "0" << endl;
                }
                else if(DMCache[lineNum][tag][offset] != -1) { //read something that was written to
                    cout << left << setw(36) << hex << DMCache[lineNum][tag][offset] << left << setw(5) << "1" << left << setw(5) << DMCache[lineNum][tag][2] << endl;
                }
            }
            
            inCache[lineNum] = tag;
        }
        
        
    }
}

unsigned long long extractBits(unsigned long long origin) {
    unsigned long long runningSum = 0;
    short i;
    
    for (i = 0; i < 6; i++) { //extract 6 bits
        runningSum += (origin % 2) * pow(2.0, i);
        origin /= 2;
    }
    return runningSum;
}