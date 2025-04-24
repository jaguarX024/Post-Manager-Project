
#include "squeue.h"
SQueue::SQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
  m_heapType= heapType;
  m_structure= structure;
  m_priorFunc= priFn;
  m_heap= nullptr;
  m_size=0;
}
SQueue::~SQueue() {
  clear();
}
void SQueue::clear() {
    m_heap= clearQueue(m_heap);
    m_size=0;
    m_priorFunc= nullptr;
    m_heapType= MINHEAP;
    m_structure= SKEW;
}
SQueue::SQueue(const SQueue& rhs) {
  m_size= rhs.m_size;
  m_heapType= rhs.m_heapType;
  m_structure= rhs.m_structure;
  m_priorFunc= rhs.m_priorFunc;
  m_heap= copyTree( rhs.m_heap);
  
}

SQueue& SQueue::operator=(const SQueue& rhs) {
  //check for self assignment
  if (this == &rhs)
  return *this;

  //clear the object
  clear();
  
  m_size=rhs.m_size;
  m_heapType= rhs.m_heapType;
  m_structure= rhs.m_structure;
  m_priorFunc=rhs.m_priorFunc;
  //deepcopy of rhs
  m_heap= copyTree(rhs.m_heap);

  return *this;

  
}
void SQueue::mergeWithQueue(SQueue& rhs) {
  //self assignment is not allowed
  if (this == &rhs)
    throw domain_error("Self assignment is not allowed");
  
    //check that the SQueues have the same properties
  if (m_structure != rhs.m_structure || m_heapType != rhs.m_heapType || m_priorFunc != rhs.m_priorFunc)
  throw runtime_error("SQueues properties mismatch");
  
  //transfer nodes from rhs to the calling SQueue
    if (rhs.m_heap != nullptr){
      if (m_heap == nullptr){
        m_heap= rhs.m_heap;
        m_size=rhs.m_size;
      }
      else{
        //merge the rhs heap into the current heap
        if (m_structure == SKEW)
          m_heap= mergeSkew(m_heap, rhs.m_heap);
        else if(m_structure == LEFTIST)
          m_heap= mergeLeftist(m_heap, rhs.m_heap);

        //increase size
        m_size+= rhs.m_size;
      }

      //empty rhs
      rhs.m_heap= nullptr;
      rhs.m_size=0;
    }

}

bool SQueue::insertPost(const Post& post) {
    if (m_priorFunc(post)==0)
      return false;

    Post* newPost= new Post(post.m_postID, post.m_likes, post.m_connectLevel, post.m_postTime, post.m_interestLevel);
    
    //check for heap type before insertion
    if(m_structure == SKEW)
      m_heap= mergeSkew( m_heap, newPost );
    else if (m_structure == LEFTIST)
      m_heap= mergeLeftist(m_heap, newPost);

      //increment size after insertion
      m_size++;
    
      return true;

}

int SQueue::numPosts() const {
  return m_size; //the size of the tree represents the number of posts
}
prifn_t SQueue::getPriorityFn() const {
  return (m_priorFunc);
}
Post SQueue::getNextPost() {
    if (m_heap== nullptr)
    throw out_of_range("Empty Queue");

    // get left and and right subtrees
    Post* leftSubtree= m_heap->m_left;
    Post* rightSubtree= m_heap->m_right;

    //extract root then delete it 
    Post nexPost= *m_heap;
    delete m_heap;
    m_heap= nullptr;
    m_size--; //Decrement the size
    //merge according to the structure
    if (m_structure == SKEW)
      m_heap= mergeSkew(leftSubtree, rightSubtree);
    else if (m_structure == LEFTIST)
      m_heap= mergeLeftist(leftSubtree, rightSubtree);
    
    return nexPost;
}
void SQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
      if (m_heapType= heapType)
      return;
      m_priorFunc= priFn;
      m_heapType= heapType; 
      m_heap= rebuildHeap(m_heap);
}
void SQueue::setStructure(STRUCTURE structure){
  if (structure != SKEW && structure != LEFTIST)
  throw runtime_error("Invalid Heap structure");

  //case when tree is empty
  if(m_heap== nullptr)
  {
    m_structure= structure;
    return;
  }

  //change heap structure from SKEW to Leftist
  if (m_structure == SKEW && structure == LEFTIST)
  {
    m_heap= switchToLeftist(m_heap);
    m_structure= LEFTIST;
  }

  //change heap structure from Leftist to Skew
  if (m_structure == LEFTIST && structure == SKEW)
  {
    m_heap= switchToSkew(m_heap);
    m_structure= SKEW;
  }

}
STRUCTURE SQueue::getStructure() const {
  return m_structure;
}
HEAPTYPE SQueue::getHeapType() const {
  return m_heapType;
}
void SQueue::printPostsQueue() const {
  if (m_size ==0)
  {
    cout<<"There is no post yet"<<endl;
    return;
  }
  cout<<"Contents of the queue: "<<endl;
  preorderPrint(m_heap);
}

void SQueue::dump() const {
  if (m_size == 0) {
    cout << "Empty heap.\n" ;
  } else {
      dump(m_heap);
  }
  cout << endl;
}
void SQueue::dump(Post *pos) const {
  if ( pos != nullptr ) {
    cout << "(";
    dump(pos->m_left);
    if (m_structure == SKEW)
        cout << m_priorFunc(*pos) << ":" << pos->m_postID;
    else
        cout << m_priorFunc(*pos) << ":" << pos->m_postID << ":" << pos->m_npl;
    dump(pos->m_right);
    cout << ")";
  }
}

ostream& operator<<(ostream& sout, const Post& post) {
  sout  << "Post#: " << post.getPostID()
        << ", likes#: " << post.getNumLikes() 
        << ", connect level: " << post.getConnectLevel();
  return sout;
}


//Helper functions implementation
Post* SQueue:: clearQueue(Post* node){
  if (!node) return nullptr;

  //Use postorderTraversal and recursion to delete nodes
  if (node->m_left)
   node->m_left= clearQueue(node->m_left);

   if (node->m_right)
   node->m_right= clearQueue(node->m_right);

   delete node;
   node=nullptr;
   return node;
}

//deep copy function
Post* SQueue::copyTree(Post* node){
  if(!node) return nullptr;
  Post* newNode= new Post(node->m_postID, node->m_likes, node->m_connectLevel, node->m_postTime, node->m_interestLevel);
  newNode->m_npl= node->m_npl;
  newNode->m_left=copyTree(node->m_left);
  newNode->m_right= copyTree(node->m_right);

  return node;
}

//merge function for a skew Heap
Post* SQueue::mergeSkew(Post * root, Post* node){
    if (!root)
    return node;
    if(!node)
    return root;

    if (m_heapType == MINHEAP){
      if ( m_priorFunc(*root) > m_priorFunc(*node)) 
          swap(root, node);
    }
    else if (m_heapType == MAXHEAP)
    {
      if (m_priorFunc(*root)< m_priorFunc(*node))
        swap(root, node);
    }

    //swap root subtrees before merging
    swap(root->m_left, root->m_right);

    //merge in the left subtree
    root->m_left= mergeSkew(root->m_left, node);

    //keep track of npl and use it when switching from skew to leftist
    root->m_npl= (root->m_right? root->m_right->m_npl : -1) +1;
    return root;
   
}

//swapping function
void SQueue::swap(Post* &node1, Post* &node2){
  Post* temp = node1;
  node1= node2;
  node2=temp;
}

//merge function for leftis Heap
Post* SQueue::mergeLeftist( Post* root, Post* node){
  if (!root) return node;
  if(!node) return root;

  //check for the right root for MINHEAP and MAXHEAP cases
  if (m_heapType == MINHEAP)
  {
    if (m_priorFunc(*root) > m_priorFunc(*node))
      swap (root, node);
  }
  else if (m_heapType == MAXHEAP)
  {
    if (m_priorFunc(*root) < m_priorFunc(*node))
      swap(root, node);
  }

  //merge on the right subtree
  root->m_right= mergeLeftist(root->m_right, node);

  //update npl values
  if (!root-> m_left){
    root->m_left= root->m_right;
    root->m_right= nullptr;
    root->m_npl=0;
  }
  else
  {
    if (root->m_right)
      {
        if (root->m_left->m_npl < root->m_right->m_npl)
          swap(root->m_left, root->m_right);
      }
      root->m_npl= (root->m_right? root->m_right->m_npl : -1) +1;
  }


return root;

}

//preorder print of the tree
void SQueue::preorderPrint(Post* root)const{
  if (!root) return;
  //print root first
  cout<<"["<<m_priorFunc(*root)<<"] Post#: "<<root->m_postID<<", likes#: "<<root->m_likes<<", connect level: "<<root->m_connectLevel<<endl;
    
  //print left child
  preorderPrint(root->m_left);
  //print right child
  preorderPrint(root->m_right);
}

//change structure functions
Post* SQueue::switchToLeftist(Post* root){
  if (!root) return nullptr;

  //visit left subtree
  root->m_left= switchToLeftist(root->m_left);
  //visit right subtree
  root->m_right= switchToLeftist(root->m_right);

  int rightNpl= (root->m_right? root->m_right->m_npl : -1) +1;
  int leftNpl= (root->m_left? root->m_left->m_npl : -1) +1;
  
  //swap if npl of left subtree is smaller than npl of right subtree
  if (leftNpl < rightNpl)
  swap(root->m_left, root->m_right);

  //update npl of the root, then return the root
  root->m_npl= (root->m_right? root->m_right->m_npl : -1) +1;

  return root;
}

Post* SQueue::switchToSkew(Post* root){
  if (!root) return nullptr;
  swap(root->m_left, root->m_right);
  //update npl for future switches
  root->m_npl= (root->m_right? root->m_right->m_npl : -1) +1;

  return root;
}

//restructuring functions based on type
Post* SQueue::rebuildHeap(Post* root){
  if (!root) return nullptr;

  root->m_left= rebuildHeap(root->m_left);
  root->m_right= rebuildHeap(root->m_right);

  Post* leader= root;
  if (root->m_left){
    if ((m_heapType == MINHEAP && m_priorFunc(*root->m_left)< m_priorFunc(*leader))
         || (m_heapType == MAXHEAP && m_priorFunc(*root->m_left) > m_priorFunc(*leader))){
           leader= root->m_left;
         }
  }

  if (root->m_right) {
    if ((m_heapType == MINHEAP && m_priorFunc(*root->m_right) < m_priorFunc(*leader)) ||
        (m_heapType == MAXHEAP && m_priorFunc(*root->m_right) > m_priorFunc(*leader))) {
        leader = root->m_right;
    }   
}

if(leader != root){
  Post temp=*root;
  *root= *leader;
  *leader= temp;
  rebuildHeap(leader);
}

return root;

}