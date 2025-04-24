#include "squeue.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>

using namespace std;

class Tester{
    public:
    bool testMinHeapInsertion();
    bool testMinHeapRemoval();
    bool testMaxHeapInsertion();
    bool testMaxHeapRemoval();
    bool testMergeWithQueueEdgeCase();

    

    //helper function to check properties
    bool testProperty(Post* root, prifn_t priorityFunc, HEAPTYPE type, STRUCTURE structure);
    
};

//random class for testing data
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(){}
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        m_generator = std::mt19937(seedNum);
    }
    void init(int min, int max){
        m_min = min;
        m_max = max;
        m_type = UNIFORMINT;
        m_generator = std::mt19937(10);// 10 is the fixed seed value
        m_unidist = std::uniform_int_distribution<>(min,max);
    }
    void getShuffle(vector<int> & array){
        // this function provides a list of all values between min and max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // this function provides a list of all values between min and max
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        result = std::floor(result*100.0)/100.0;
        return result;
    }

    string getRandString(int size){
        // the parameter size specifies the length of string we ask for
        string output = "";
        for (int i=0;i<size;i++){
            output = output + (char)getRandNum();
        }
        return output;
    }
    
    int getMin(){return m_min;}
    int getMax(){return m_max;}
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

//priority functions
int priorityFn1(const Post &post);// works with a MAXHEAP
int priorityFn2(const Post &post);// works with a MINHEAP


//function to check heap properties

bool Tester::testProperty(Post* root, prifn_t priorityFunc, HEAPTYPE type, STRUCTURE heapStr){
    if (!root) return true;
    switch(type){
        //checking that the minheap(leftist or skew) properties are conserved
        case MINHEAP:
        //case of a leftis heap
            if (heapStr == LEFTIST){
                if (!root->m_left && !root->m_right)
                    return true;
               if (!root->m_left && root->m_right)
                    return false;
                if (root->m_left){
                    int leftNpl = root->m_left->m_npl;
                    int rightNpl= root->m_right? root->m_right->m_npl: -1;

                    if (leftNpl < rightNpl)
                    return false;

                    if (priorityFunc(*root)> priorityFunc(*root->m_left))
                        return false;
                    if (root->m_right){
                        if (priorityFunc(*root)> priorityFunc(*root->m_right))
                        return false;
                    }

                }
                return testProperty(root->m_left, priorityFunc,  type,  heapStr) &&
                          testProperty(root->m_right, priorityFunc,  type,  heapStr);
            }

            //case of a Skew heap
            if (heapStr == SKEW){
                if (!root->m_left && !root->m_right)
                return true;
                else if (!root->m_left || !root->m_right){
                    Post* child= root->m_left? root->m_left:root->m_right;
                    if (child){
                        if (priorityFunc(*root)> priorityFunc(*child))
                        return false;
                    }
                    

                }
                else{
                    if (priorityFunc(*root)>priorityFunc(*root->m_left)|| priorityFunc(*root)>priorityFunc(*root->m_right))
                        return false;
                }

                
               return  testProperty(root->m_left, priorityFunc,  type,  heapStr) &&
                          testProperty(root->m_right, priorityFunc,  type,  heapStr);
            }
        
        //check that maxheap(leftist or skew) properties are conserved
        case MAXHEAP:
        if (heapStr == LEFTIST){
            if (!root->m_left && !root->m_right)
                return true;
           if (!root->m_left && root->m_right)
                return false;
            if (root->m_left){
                int leftNpl = root->m_left->m_npl;
                int rightNpl= root->m_right? root->m_right->m_npl: -1;

                if (leftNpl < rightNpl)
                return false;

                if (priorityFunc(*root)< priorityFunc(*root->m_left))
                    return false;
                if (root->m_right){
                    if (priorityFunc(*root)< priorityFunc(*root->m_right))
                    return false;
                }

            }
            return testProperty(root->m_left, priorityFunc,  type,  heapStr) &&
                      testProperty(root->m_right, priorityFunc,  type,  heapStr);
        }

        if (heapStr == SKEW){
            if (!root->m_left && !root->m_right)
            return true;
            else if (!root->m_left || !root->m_right){
                Post* child= root->m_left? root->m_left:root->m_right;
                if (child){
                    if (priorityFunc(*root)< priorityFunc(*child))
                    return false;
                }
                

            }
            else{
                if (priorityFunc(*root)<priorityFunc(*root->m_left)|| priorityFunc(*root)<priorityFunc(*root->m_right))
                    return false;
            }

            
           return  testProperty(root->m_left, priorityFunc,  type,  heapStr) &&
                      testProperty(root->m_right, priorityFunc,  type,  heapStr);
        }
    
    }

}


/* Priority functions */
int priorityFn1(const Post & post) {
    //needs MAXHEAP
    //the larger value means the higher priority
    int priority = post.getNumLikes() + post.getInterestLevel();
    int minValue = 1;
    int maxValue = 510;
    if (priority >= minValue && priority <= maxValue)
        return priority;
    else
        return 0; // this is an invalid order object
}

int priorityFn2(const Post & post) {
    //needs MINHEAP
    //the smaller value means the higher priority
    int minValue = 2;
    int maxValue = 55;
    int priority = post.getPostTime() + post.getConnectLevel();
    if (priority >= minValue && priority <= maxValue)
        return priority;
    else
        return 0; // this is an invalid order object
}

//test whether the heap properties are conserved after several insertion in a minheap
bool Tester::testMinHeapInsertion(){
    Random idGen(MINPOSTID,MAXPOSTID);
    Random likesGen(MINLIKES,MAXLIKES);
    Random timeGen(MINTIME,MAXTIME);
    Random conLevelGen(MINCONLEVEL,MAXCONLEVEL);
    Random interestGen(MININTERESTLEVEL,MAXINTERESTLEVEL);
    SQueue tree(priorityFn2, MINHEAP, LEFTIST);
    for (int i=0;i<300;i++){
        Post myPost(idGen.getRandNum(),
                    likesGen.getRandNum(),
                    conLevelGen.getRandNum(),
                    timeGen.getRandNum(),
                    interestGen.getRandNum());
        tree.insertPost(myPost);
    }

    return testProperty(tree.m_heap, tree.m_priorFunc, tree.m_heapType, tree.m_structure);

}

//test whether the heap properties are conserved after several removals from a minheap
bool Tester::testMinHeapRemoval(){
    Random idGen(MINPOSTID,MAXPOSTID);
    Random likesGen(MINLIKES,MAXLIKES);
    Random timeGen(MINTIME,MAXTIME);
    Random conLevelGen(MINCONLEVEL,MAXCONLEVEL);
    Random interestGen(MININTERESTLEVEL,MAXINTERESTLEVEL);
    SQueue tree(priorityFn2, MINHEAP, LEFTIST);
    for (int i=0;i<10;i++){
        Post myPost(idGen.getRandNum(),
                    likesGen.getRandNum(),
                    conLevelGen.getRandNum(),
                    timeGen.getRandNum(),
                    interestGen.getRandNum());
        tree.insertPost(myPost);
    }
    tree.getNextPost();
    tree.getNextPost();
    tree.getNextPost();
    return testProperty(tree.m_heap, tree.m_priorFunc, tree.m_heapType, tree.m_structure);

}

//test 300 insertions in a maxheap
bool Tester::testMaxHeapInsertion(){
    Random idGen(MINPOSTID,MAXPOSTID);
    Random likesGen(MINLIKES,MAXLIKES);
    Random timeGen(MINTIME,MAXTIME);
    Random conLevelGen(MINCONLEVEL,MAXCONLEVEL);
    Random interestGen(MININTERESTLEVEL,MAXINTERESTLEVEL);
    SQueue tree(priorityFn1, MAXHEAP, LEFTIST);
    for (int i=0;i<300;i++){
        Post myPost(idGen.getRandNum(),
                    likesGen.getRandNum(),
                    conLevelGen.getRandNum(),
                    timeGen.getRandNum(),
                    interestGen.getRandNum());
        tree.insertPost(myPost);
    }

    return testProperty(tree.m_heap, tree.m_priorFunc, tree.m_heapType, tree.m_structure);
}

//test of removals from a Leftist Maxheap
bool Tester::testMaxHeapRemoval(){
    Random idGen(MINPOSTID,MAXPOSTID);
    Random likesGen(MINLIKES,MAXLIKES);
    Random timeGen(MINTIME,MAXTIME);
    Random conLevelGen(MINCONLEVEL,MAXCONLEVEL);
    Random interestGen(MININTERESTLEVEL,MAXINTERESTLEVEL);
    SQueue tree(priorityFn1, MAXHEAP, LEFTIST);
    for (int i=0;i<10;i++){
        Post myPost(idGen.getRandNum(),
                    likesGen.getRandNum(),
                    conLevelGen.getRandNum(),
                    timeGen.getRandNum(),
                    interestGen.getRandNum());
        tree.insertPost(myPost);
    }
    tree.getNextPost();
    tree.getNextPost();
    tree.getNextPost();
    return testProperty(tree.m_heap, tree.m_priorFunc, tree.m_heapType, tree.m_structure);
}

//test merge of an empty queue with a normal queue 
bool Tester::testMergeWithQueueEdgeCase(){
    Random idGen(MINPOSTID,MAXPOSTID);
    Random likesGen(MINLIKES,MAXLIKES);
    Random timeGen(MINTIME,MAXTIME);
    Random conLevelGen(MINCONLEVEL,MAXCONLEVEL);
    Random interestGen(MININTERESTLEVEL,MAXINTERESTLEVEL);
    SQueue tree(priorityFn1, MAXHEAP, LEFTIST);
    for (int i=0;i<10;i++){
        Post myPost(idGen.getRandNum(),
                    likesGen.getRandNum(),
                    conLevelGen.getRandNum(),
                    timeGen.getRandNum(),
                    interestGen.getRandNum());
        tree.insertPost(myPost);
    }

    SQueue emptyQueue(priorityFn1,MAXHEAP, LEFTIST);
    tree.mergeWithQueue(emptyQueue);

    return testProperty(tree.m_heap, tree.m_priorFunc, tree.m_heapType, tree.m_structure);


}


int main(){
    Tester tester;
    
    //test of minheap insertion for both LEFTIST and SKEW
    cout<<"Test of 300 insertions in MINHEAP(LEFTIST/SKEW): "<<(tester.testMinHeapInsertion()? "Passed":"Failed")<<endl;
    cout<<"Test whether the Minheap(LEFTIST/SKEW) conserves its properties after removals: "<<(tester.testMinHeapRemoval()? "Passed":"Failed")<<endl;
    cout<<"Test of 300 insertions in a LEFTIST Maxheap: "<<(tester.testMaxHeapInsertion()? "Passed":"Failed")<<endl;
    cout<<"Test whether the Leftist Maxheap conserves its properties after removals: "<<(tester.testMaxHeapRemoval()? "Passed":"Failed")<<endl;
    cout<<"Test of merging a normal and empty queue: "<<(tester.testMergeWithQueueEdgeCase()?"Passed":"Failed")<<endl;

    
    
    return 0;
}
