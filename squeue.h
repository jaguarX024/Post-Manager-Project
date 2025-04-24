#ifndef SQUEUE_H
#define SQUEUE_H
#include <stdexcept>
#include <iostream>
#include <string>
using namespace std;
class Tester;   // forward declaration (for testing purposes)
class SQueue;   // forward declaration
class Post;     // forward declaration
#define DEFAULTPOSTID 100000
const int MINPOSTID = 100001;//minimum post ID
const int MAXPOSTID = 999999;//maximum post ID
const int MINLIKES = 0;//lowest priority
const int MAXLIKES = 500;//highest priority
const int MININTERESTLEVEL = 1;//lowest priority
const int MAXINTERESTLEVEL = 10;//highest priority
const int MINCONLEVEL = 1;//highest priority
const int MAXCONLEVEL = 5;//lowest priority
const int MINTIME = 1;//highest priority
const int MAXTIME = 50;//lowest priority
enum HEAPTYPE {MINHEAP, MAXHEAP};
enum STRUCTURE {SKEW, LEFTIST};

// Priority function pointer type
typedef int (*prifn_t)(const Post&);

class Post{
    public:
    friend class Tester; // for testing purposes
    friend class SQueue;
    Post(){
        m_postID = DEFAULTPOSTID;m_likes = MINLIKES;
        m_connectLevel = MAXCONLEVEL;m_postTime = MAXTIME;
        m_interestLevel = MININTERESTLEVEL;
        m_right = nullptr;
        m_left = nullptr;
        m_npl = 0;
    }
    Post(int ID, int likes, int connectLevel, int postTime, int interestLevel){
        if (ID < MINPOSTID || ID > MAXPOSTID) m_postID = DEFAULTPOSTID;
        else m_postID = ID;
        if (likes < MINLIKES || likes > MAXLIKES) m_likes = MINLIKES;
        else m_likes = likes;
        if (connectLevel < MINCONLEVEL || connectLevel > MAXCONLEVEL) m_connectLevel = MAXCONLEVEL;
        else m_connectLevel = connectLevel;
        if (postTime < MINTIME || postTime > MAXTIME) m_postTime = MAXTIME;
        else m_postTime = postTime;
        if (interestLevel < MININTERESTLEVEL || interestLevel > MAXINTERESTLEVEL) m_interestLevel = MININTERESTLEVEL;
        else m_interestLevel = interestLevel;
        m_right = nullptr;
        m_left = nullptr;
        m_npl = 0;
    }
    int getPostID() const {return m_postID;}
    int getNumLikes() const {return m_likes;}
    int getConnectLevel() const {return m_connectLevel;}
    int getPostTime() const {return m_postTime;}
    int getInterestLevel() const {return m_interestLevel;}
    void setNPL(int npl) {m_npl = npl;}
    int getNPL() const {return m_npl;}
    // Overloaded insertion operator for Post
    friend ostream& operator<<(ostream& sout, const Post& post);

    private:
    int m_postID;           // every post is identified by a unique ID
    int m_likes;            // 0-500, constant variables defined for this
    int m_connectLevel;     // 1-5, constant variables defined for this
    int m_postTime;         // 1-50, constant variables defined for this
    int m_interestLevel;    // 1-10, constant variables defined for this

    Post * m_right;   // right child
    Post * m_left;    // left child
    int m_npl;        // null path length for leftist heap
};

class SQueue{
    public:
    friend class Tester; // for testing purposes
    
    SQueue(){}
    SQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure);
    ~SQueue();
    SQueue(const SQueue& rhs);
    SQueue& operator=(const SQueue& rhs);
    bool insertPost(const Post& post);
    Post getNextPost(); // Returns the highest priority post
    void mergeWithQueue(SQueue& rhs);
    void clear();
    int numPosts() const; // Returns number of posts in queue
    void printPostsQueue() const; // Print the queue using preorder traversal
    prifn_t getPriorityFn() const;
    void setPriorityFn(prifn_t priFn, HEAPTYPE heapType);
    HEAPTYPE getHeapType() const;
    STRUCTURE getStructure() const;
    void setStructure(STRUCTURE structure);
    void dump() const; // For debugging purposes

    private:
    Post * m_heap;          // Pointer to root of the heap
    int m_size;             // Current size of the heap
    prifn_t m_priorFunc;    // Function to compute priority
    HEAPTYPE m_heapType;    // either a MINHEAP or a MAXHEAP
    STRUCTURE m_structure;  // skew heap or leftist heap

    void dump(Post *pos) const; // helper function for dump

    Post* clearQueue(Post* node);
    //function to make a deep copy
    Post* copyTree( Post* node);

    //merge function for a leftist Heap
    Post* mergeLeftist (Post* root, Post* node);
    //merge function for a skew Heap
    Post* mergeSkew(Post* root, Post* node);

    //swapping function
    void swap(Post* &node1, Post* &node2);

    //preorder print of the tree
    void preorderPrint(Post* root)const;

    //structure change functions
    Post* switchToLeftist(Post* root);
    Post* switchToSkew(Post* root);

    //funtion to restructure heap based on type
    Post* rebuildHeap(Post* root);
   

};
#endif
