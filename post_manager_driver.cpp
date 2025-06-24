#include "squeue.h" 
#include <math.h> 
#include <algorithm> 
#include <random> 
#include <vector> 
using namespace std;

// Enum to define different types of random number distributions
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};

// Random class for generating various types of random numbers and sequences
class Random {
public:
    Random(){} // Default constructor

    // Constructor to initialize the random number generator with specific parameters
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            // Initialize for normal distribution to generate integer numbers
            m_generator = std::mt19937(m_device()); // Uses a non-deterministic seed from hardware
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            // Initialize for uniform integer distribution
            m_generator = std::mt19937(10); // 10 is the fixed seed value for reproducibility
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { // For generating real numbers with uniform distribution
            m_generator = std::mt19937(10); // 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { // For shuffling, generates every number only once
            m_generator = std::mt19937(m_device()); // Uses a non-deterministic seed
        }
    }

    // Allows setting a custom seed for the random number generator
    void setSeed(int seedNum){
        m_generator = std::mt19937(seedNum);
    }

    // Initializes the random generator for uniform integer distribution with new min/max
    void init(int min, int max){
        m_min = min;
        m_max = max;
        m_type = UNIFORMINT;
        m_generator = std::mt19937(10); // 10 is the fixed seed value
        m_unidist = std::uniform_int_distribution<>(min,max);
    }

    // Populates a vector with numbers from min to max and shuffles them
    void getShuffle(vector<int> & array){
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator); // Randomly shuffles the elements
    }

    // Populates an array with numbers from min to max and shuffles them
    void getShuffle(int array[]){
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator); // Randomly shuffles the elements
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    // Generates a random integer based on the selected distribution type
    int getRandNum(){
        int result = 0;
        if(m_type == NORMAL){
            // Returns a random number from a normal distribution, constrained by min and max
            result = m_min - 1; // Initialize to an invalid value to ensure loop runs
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            // Generates a random integer between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    // Generates a random real number, rounded to two decimal places
    double getRealRandNum(){
        double result = m_uniReal(m_generator);
        result = std::floor(result*100.0)/100.0; // Rounds down to two decimal places
        return result;
    }

    // Generates a random string of a specified length
    string getRandString(int size){
        string output = "";
        for (int i=0;i<size;i++){
            output = output + (char)getRandNum(); // Appends random ASCII characters
        }
        return output;
    }
    
    // Getter for minimum value
    int getMin(){return m_min;}
    // Getter for maximum value
    int getMax(){return m_max;}
private:
    int m_min; // Minimum value for random generation
    int m_max; // Maximum value for random generation
    RANDOM m_type; // Type of random distribution
    std::random_device m_device; // Non-deterministic random number generator source
    std::mt19937 m_generator; // Mersenne Twister pseudo-random number generator
    std::normal_distribution<> m_normdist; // Normal distribution object
    std::uniform_int_distribution<> m_unidist; // Integer uniform distribution object
    std::uniform_real_distribution<double> m_uniReal; // Real uniform distribution object

};

// Declaration of priority functions for sorting Posts in the SQueue
int priorityFn1(const Post &post); // Designed to work with a MAXHEAP
int priorityFn2(const Post &post); // Designed to work with a MINHEAP

// Placeholder Tester class 
class Tester{};

// Main function to demonstrate the SQueue functionality with different configurations
int main() {
    // Initialize random number generators for various Post attributes
    Random idGen(MINPOSTID,MAXPOSTID);
    Random likesGen(MINLIKES,MAXLIKES);
    Random timeGen(MINTIME,MAXTIME);
    Random conLevelGen(MINCONLEVEL,MAXCONLEVEL);
    Random interestGen(MININTERESTLEVEL,MAXINTERESTLEVEL);

    // Create an SQueue instance with priorityFn2 (MINHEAP) and LEFTIST heap structure
    SQueue queue1(priorityFn2, MINHEAP, LEFTIST);

    // Insert 10 randomly generated Post objects into the queue
    for (int i=0;i<10;i++){
        Post aPost(idGen.getRandNum(),
                    likesGen.getRandNum(),
                    conLevelGen.getRandNum(),
                    timeGen.getRandNum(),
                    interestGen.getRandNum());
        queue1.insertPost(aPost);
    }
    cout << "\nDump of the leftist heap queue with priorityFn2 (MINHEAP):\n";
    queue1.dump(); // Display the contents of the queue

    queue1.setStructure(SKEW); // Change the underlying heap structure to SKEW heap
    cout << "\nDump of the skew heap queue with priorityFn2 (MINHEAP):\n";
    queue1.dump(); // Display the contents of the queue again with the new structure

    cout << "\nPreorder traversal of the nodes in the queue with priorityFn2 (MINHEAP):\n";
    queue1.printPostsQueue(); // Print posts using preorder traversal

    queue1.setPriorityFn(priorityFn1, MAXHEAP); // Change the priority function to priorityFn1 and heap type to MAXHEAP
    cout << "\nDump of the skew queue with priorityFn1 (MAXHEAP):\n";
    queue1.dump(); // Display the contents with the new priority and heap type

    return 0; // Indicate successful execution
}

/* Priority functions */
// Priority function 1: Calculates priority based on likes and interest level
int priorityFn1(const Post & post) {
    // This function needs to work with a MAXHEAP (higher priority means higher value)
    
    int priority = post.getNumLikes() + post.getInterestLevel();
    int minValue = 1; // Minimum expected priority value
    int maxValue = 510; // Maximum expected priority value
    // Return priority if within valid range, otherwise return 0 (invalid)
    if (priority >= minValue && priority <= maxValue)
        return priority;
    else
        return 0; // This indicates an invalid Post object for ordering
}

// Priority function 2: Calculates priority based on post time and connection level
int priorityFn2(const Post & post) {
    // This function needs to work with a MINHEAP (lower priority means lower value)
    int minValue = 2; // Minimum expected priority value
    int maxValue = 55; // Maximum expected priority value
    int priority = post.getPostTime() + post.getConnectLevel();
    // Return priority if within valid range, otherwise return 0 (invalid)
    if (priority >= minValue && priority <= maxValue)
        return priority;
    else
        return 0; // This indicates an invalid Post object for ordering
}