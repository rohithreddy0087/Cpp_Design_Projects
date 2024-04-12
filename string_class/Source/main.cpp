//
//  main.cpp
//  Assignment2
//
//

#include <iostream>
#include <sstream>
#include <string>
#include "autotest.hpp"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

// -----------------------------------

int main(int argc, const char * argv[]) {
    static const char* kMsgs[]={"FAIL","PASS"};
    if(argc>1) {
        std::string temp(argv[1]);
        std::stringstream theOutput;
        BufferManagerAutoTests bufferManagerTest;
        StringAutoTests stringTest;


        srand(static_cast<uint32_t>(time(NULL)));

        if("compile"==temp) {
            std::cout << temp << " test " << kMsgs[true] << "\n";
        }
        else if("BufferOCF" == temp) {
            auto result = bufferManagerTest("OCFTest",theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("OCF"==temp) {
            auto result = stringTest("OCFTest", theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("Expand"==temp) {
            auto result = bufferManagerTest("ExpandTest",theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("Compact"==temp) {
            auto result = bufferManagerTest("CompactTest",theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("Insert"==temp) {
            auto result = stringTest("InsertTest",theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("Append"==temp) {
            auto result = stringTest("AppendTest",theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("Replace"==temp) {
            auto result = stringTest("ReplaceTest",theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("Erase"==temp) {
            auto result = stringTest("EraseTest",theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("Search"==temp) {
            auto result = stringTest("SearchTest",theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("Compare"==temp) {
            auto result = stringTest("CompareTest",theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("Speed"==temp) {
            auto result = stringTest("SpeedTest",theOutput);
            std::cout<< temp << " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";
            return !result;
        }
        else if("All"==temp) {
            return stringTest.runTests() && bufferManagerTest.runTests();
        }
        else{
            std::cout<<"Unknown test " << temp << "\n";
            return 1;
        }
    }
    return 0;
}


// int main() {
//     static const char* kMsgs[]={"FAIL","PASS"};
    
//     std::stringstream theOutput;
//     std::stringstream theOutput2;
//     // BufferManagerAutoTests bufferManagerTest;
//     StringAutoTests stringTest;

//     srand(static_cast<uint32_t>(time(NULL)));
//     // auto result1 = bufferManagerTest("OCFTest",theOutput);
//     // std::cout<< " OCF test " << kMsgs[result1] << "\n" << theOutput.str() <<"\n";
        
//     // auto result2 = bufferManagerTest("ExpandTest",theOutput);
//     // std::cout<< " Expand test " << kMsgs[result2] << "\n" << theOutput.str() <<"\n";
        
//     // auto result3 = bufferManagerTest("CompactTest",theOutput);
//     // std::cout<< " Compact test " << kMsgs[result3] << "\n" << theOutput.str() <<"\n";

   
//     auto result = stringTest("OCFTest", theOutput);
//     std::cout<< " test " << kMsgs[result] << "\n" << theOutput.str() <<"\n";

//     auto result2 = stringTest("AppendTest",theOutput2);
//     std::cout<< " test " << kMsgs[result2] << "\n" << theOutput2.str() <<"\n";

//     std::stringstream theOutput3;
//     auto result3 = stringTest("InsertTest",theOutput3);
//     std::cout<< " test " << kMsgs[result3] << "\n" << theOutput3.str() <<"\n";

//     std::stringstream theOutput4;
//     auto result4 = stringTest("EraseTest",theOutput4);
//     std::cout<< " test " << kMsgs[result4] << "\n" << theOutput4.str() <<"\n";

//     std::stringstream theOutput5;
//     auto result5 = stringTest("ReplaceTest",theOutput5);
//     std::cout<< " test " << kMsgs[result5] << "\n" << theOutput5.str() <<"\n";

//     std::stringstream theOutput6;
//     auto result6 = stringTest("SearchTest",theOutput6);
//     std::cout<< " test " << kMsgs[result6] << "\n" << theOutput6.str() <<"\n";

//     std::stringstream theOutput7;
//     auto result7 = stringTest("CompareTest",theOutput7);
//     std::cout<< " test " << kMsgs[result7] << "\n" << theOutput7.str() <<"\n";
            
//     return 0;
//     }