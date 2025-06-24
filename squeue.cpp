
#include "squeue.h" 

// SQueue constructor: Initializes the queue with a priority function, heap type, and structure
SQueue::SQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
    m_heapType = heapType; // Stores whether it's a min-heap or max-heap
    m_structure = structure; // Stores whether it's a skew heap or leftist heap
    m_priorFunc = priFn; // Stores the function used to determine post priority
    m_heap = nullptr; // The root of the heap is initially null
    m_size = 0; // The queue is initially empty
}

// SQueue destructor: Cleans up all allocated memory when the object is destroyed
SQueue::~SQueue() {
    clear(); // Calls the clear function to deallocate nodes
}

// Clears all nodes from the queue and resets member variables to default states
void SQueue::clear() {
    m_heap = clearQueue(m_heap); // Recursively deletes all nodes in the heap
    m_size = 0; // Reset size to 0
    m_priorFunc = nullptr; // Clear priority function pointer
    m_heapType = MINHEAP; // Reset heap type to default
    m_structure = SKEW; // Reset structure to default
}

// Copy constructor: Performs a deep copy of another SQueue object
SQueue::SQueue(const SQueue& rhs) {
    m_size = rhs.m_size; // Copy the size
    m_heapType = rhs.m_heapType; // Copy the heap type
    m_structure = rhs.m_structure; // Copy the structure type
    m_priorFunc = rhs.m_priorFunc; // Copy the priority function pointer
    m_heap = copyTree( rhs.m_heap); // Recursively deep copy the heap tree structure
}

// Assignment operator: Allows assigning one SQueue object to another
SQueue& SQueue::operator=(const SQueue& rhs) {
    // Check for self-assignment to prevent issues
    if (this == &rhs)
        return *this;

    // Clear the current object's contents
    clear(); 
    
    // Copy member variables from the right-hand side object
    m_size = rhs.m_size;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
    m_priorFunc = rhs.m_priorFunc;
    // Perform a deep copy of the right-hand side's heap tree
    m_heap = copyTree(rhs.m_heap);

    return *this; // Return reference to the current object
}

// Merges another SQueue into the current SQueue
void SQueue::mergeWithQueue(SQueue& rhs) {
    // Prevent merging a queue with itself
    if (this == &rhs)
        throw domain_error("Self assignment is not allowed");
    
    // Check for consistency in queue properties before merging
    if (m_structure != rhs.m_structure || m_heapType != rhs.m_heapType || m_priorFunc != rhs.m_priorFunc)
        throw runtime_error("SQueues properties mismatch");
    
    // Transfer nodes from the right-hand side queue to the calling queue
    if (rhs.m_heap != nullptr){ // Only merge if the RHS heap is not empty
        if (m_heap == nullptr){ // If the current heap is empty, just take RHS's heap
            m_heap = rhs.m_heap;
            m_size = rhs.m_size;
        }
        else{
            // Merge the RHS heap into the current heap based on its structure
            if (m_structure == SKEW)
                m_heap = mergeSkew(m_heap, rhs.m_heap);
            else if(m_structure == LEFTIST)
                m_heap = mergeLeftist(m_heap, rhs.m_heap);

            // Increase the size of the current queue after merging
            m_size += rhs.m_size;
        }

        // Empty the right-hand side queue after successful merge
        rhs.m_heap = nullptr;
        rhs.m_size = 0;
    }
}

// Inserts a new Post into the queue
bool SQueue::insertPost(const Post& post) {
    // Return false if the post's priority is invalid (as determined by the priority function)
    if (m_priorFunc(post) == 0)
        return false;

    // Create a new Post object on the heap
    Post* newPost = new Post(post.m_postID, post.m_likes, post.m_connectLevel, post.m_postTime, post.m_interestLevel);
    
    // Merge the new post into the heap based on the current structure
    if(m_structure == SKEW)
        m_heap = mergeSkew( m_heap, newPost );
    else if (m_structure == LEFTIST)
        m_heap = mergeLeftist(m_heap, newPost);

    m_size++; // Increment the total number of posts in the queue
    
    return true; // Insertion successful
}

// Returns the current number of posts in the queue
int SQueue::numPosts() const {
    return m_size; // The size of the tree represents the number of posts
}

// Returns the priority function currently in use
prifn_t SQueue::getPriorityFn() const {
    return (m_priorFunc);
}

// Retrieves and removes the next highest (or lowest, depending on heap type) priority post
Post SQueue::getNextPost() {
    // Throw an error if the queue is empty
    if (m_heap == nullptr)
        throw out_of_range("Empty Queue");

    // Get the left and right subtrees of the current root
    Post* leftSubtree = m_heap->m_left;
    Post* rightSubtree = m_heap->m_right;

    // Extract the root's data and then delete the root node
    Post nextPost = *m_heap;
    delete m_heap;
    m_heap = nullptr; // Set the heap root to null after deletion
    m_size--; // Decrement the size

    // Merge the left and right subtrees to form the new heap root, based on structure
    if (m_structure == SKEW)
        m_heap = mergeSkew(leftSubtree, rightSubtree);
    else if (m_structure == LEFTIST)
        m_heap = mergeLeftist(leftSubtree, rightSubtree);
    
    return nextPost; // Return the extracted post
}

// Changes the priority function and heap type, then rebuilds the heap
void SQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    // If the heap type is already the same, do nothing
    if (m_heapType == heapType)
        return;
    m_priorFunc = priFn; // Set the new priority function
    m_heapType = heapType; // Set the new heap type
    m_heap = rebuildHeap(m_heap); // Rebuild the entire heap with the new priority logic
}

// Changes the underlying heap structure (Skew or Leftist)
void SQueue::setStructure(STRUCTURE structure){
    // Validate the requested structure type
    if (structure != SKEW && structure != LEFTIST)
        throw runtime_error("Invalid Heap structure");

    // If the tree is empty, just update the structure and return
    if(m_heap == nullptr)
    {
        m_structure = structure;
        return;
    }

    // Handle transition from SKEW to Leftist heap structure
    if (m_structure == SKEW && structure == LEFTIST)
    {
        m_heap = switchToLeftist(m_heap); // Convert the heap structure
        m_structure = LEFTIST;
    }

    // Handle transition from Leftist to SKEW heap structure
    if (m_structure == LEFTIST && structure == SKEW)
    {
        m_heap = switchToSkew(m_heap); // Convert the heap structure
        m_structure = SKEW;
    }
}

// Returns the current heap structure type
STRUCTURE SQueue::getStructure() const {
    return m_structure;
}

// Returns the current heap type (MINHEAP or MAXHEAP)
HEAPTYPE SQueue::getHeapType() const {
    return m_heapType;
}

// Prints the contents of the queue using a preorder traversal
void SQueue::printPostsQueue() const {
    if (m_size == 0)
    {
        cout << "There is no post yet" << endl;
        return;
    }
    cout << "Contents of the queue: " << endl;
    preorderPrint(m_heap); // Calls the helper function for recursive printing
}

// Dumps the internal structure of the heap for debugging
void SQueue::dump() const {
    if (m_size == 0) {
        cout << "Empty heap.\n" ;
    } else {
        dump(m_heap); // Calls the recursive dump helper
    }
    cout << endl;
}

// Recursive helper function to dump the heap's structure
void SQueue::dump(Post *pos) const {
    if ( pos != nullptr ) {
        cout << "(";
        dump(pos->m_left); // Recursively dump left child
        // Print node information based on heap structure (Leftist includes NPL)
        if (m_structure == SKEW)
            cout << m_priorFunc(*pos) << ":" << pos->m_postID;
        else
            cout << m_priorFunc(*pos) << ":" << pos->m_postID << ":" << pos->m_npl;
        dump(pos->m_right); // Recursively dump right child
        cout << ")";
    }
}

// Overloaded stream insertion operator to print a Post object
ostream& operator<<(ostream& sout, const Post& post) {
    sout << "Post#: " << post.getPostID()
         << ", likes#: " << post.getNumLikes() 
         << ", connect level: " << post.getConnectLevel();
    return sout;
}


// Helper functions implementation

// Recursively deletes all nodes in a heap tree
Post* SQueue::clearQueue(Post* node){
    if (!node) return nullptr; // Base case: if node is null, return

    // Recursively clear left and right subtrees (post-order traversal for deletion)
    if (node->m_left)
        node->m_left = clearQueue(node->m_left);

    if (node->m_right)
        node->m_right = clearQueue(node->m_right);

    delete node; // Delete the current node
    node = nullptr; // Set pointer to null after deletion
    return node;
}

// Deep copy function for a heap tree
Post* SQueue::copyTree(Post* node){
    if(!node) return nullptr; // Base case: if node is null, return null

    // Create a new Post node with copied data
    Post* newNode = new Post(node->m_postID, node->m_likes, node->m_connectLevel, node->m_postTime, node->m_interestLevel);
    newNode->m_npl = node->m_npl; // Copy NPL (Null Path Length)
    newNode->m_left = copyTree(node->m_left); // Recursively copy left subtree
    newNode->m_right = copyTree(node->m_right); // Recursively copy right subtree

    return newNode; // Return the new node (root of the copied subtree)
}

// Merges two skew heaps, maintaining the heap property
Post* SQueue::mergeSkew(Post * root, Post* node){
    // Handle null roots
    if (!root) return node;
    if(!node) return root;

    // Ensure root is the new root based on heap type (MINHEAP or MAXHEAP)
    if (m_heapType == MINHEAP){
        if ( m_priorFunc(*root) > m_priorFunc(*node)) 
            swap(root, node); // Swap if root's priority is greater than node's (for MINHEAP)
    }
    else if (m_heapType == MAXHEAP) // For MAXHEAP
    {
        if (m_priorFunc(*root) < m_priorFunc(*node))
            swap(root, node); // Swap if root's priority is smaller than node's (for MAXHEAP)
    }

    // Swap root's children (characteristic of skew heaps)
    swap(root->m_left, root->m_right);

    // Recursively merge the swapped left child with the other node
    root->m_left = mergeSkew(root->m_left, node);

    // Update NPL (Null Path Length) for future reference, though not strictly required for skew heap logic
    root->m_npl = (root->m_right ? root->m_right->m_npl : -1) + 1;
    return root; // Return the new root
}

// Swaps two Post pointers
void SQueue::swap(Post* &node1, Post* &node2){
    Post* temp = node1;
    node1 = node2;
    node2 = temp;
}

// Merges two leftist heaps, maintaining heap and leftist properties
Post* SQueue::mergeLeftist( Post* root, Post* node){
    // Handle null roots
    if (!root) return node;
    if(!node) return root;

    // Ensure 'root' is the new root based on heap type (MINHEAP or MAXHEAP)
    if (m_heapType == MINHEAP)
    {
        if (m_priorFunc(*root) > m_priorFunc(*node))
            swap (root, node); // Swap if root's priority is greater than node's (for MINHEAP)
    }
    else if (m_heapType == MAXHEAP) // For MAXHEAP
    {
        if (m_priorFunc(*root) < m_priorFunc(*node))
            swap(root, node); // Swap if root's priority is smaller than node's (for MAXHEAP)
    }

    // Recursively merge on the right subtree
    root->m_right = mergeLeftist(root->m_right, node);

    // Update NPL values and ensure leftist property (left child's NPL >= right child's NPL)
    if (!root->m_left){ // If left child is null, promote right child to left
        root->m_left = root->m_right;
        root->m_right = nullptr;
        root->m_npl = 0;
    }
    else // If both children exist, swap if leftist property is violated
    {
        if (root->m_right) // Only if right child is not null
        {
            if (root->m_left->m_npl < root->m_right->m_npl)
                swap(root->m_left, root->m_right); // Swap children to maintain leftist property
        }
        // Recalculate NPL based on the new right child's NPL
        root->m_npl = (root->m_right ? root->m_right->m_npl : -1) + 1;
    }

    return root; // Return the new root
}

// Performs a preorder traversal and prints each node's information
void SQueue::preorderPrint(Post* root)const{
    if (!root) return; // Base case: if node is null, return
    // Print current node's priority, Post ID, likes, and connect level
    cout << "[" << m_priorFunc(*root) << "] Post#: " << root->m_postID << ", likes#: " << root->m_likes << ", connect level: " << root->m_connectLevel << endl;
        
    preorderPrint(root->m_left); // Recursively print left child
    preorderPrint(root->m_right); // Recursively print right child
}

// Functions to change heap structure

// Converts a heap from Skew to Leftist structure (updates NPLs and ensures leftist property)
Post* SQueue::switchToLeftist(Post* root){
    if (!root) return nullptr; // Base case

    // Recursively convert left and right subtrees
    root->m_left = switchToLeftist(root->m_left);
    root->m_right = switchToLeftist(root->m_right);

    // Get NPL of left and right subtrees (or -1 if null)
    int rightNpl = (root->m_right ? root->m_right->m_npl : -1) + 1;
    int leftNpl = (root->m_left ? root->m_left->m_npl : -1) + 1;
    
    // Swap children if the left child's NPL is smaller than the right child's NPL
    if (leftNpl < rightNpl)
        swap(root->m_left, root->m_right);

    // Update the NPL of the current root based on its (possibly new) right child
    root->m_npl = (root->m_right ? root->m_right->m_npl : -1) + 1;

    return root; // Return the restructured root
}

// Converts a heap from Leftist to Skew structure (swaps children for skew property)
Post* SQueue::switchToSkew(Post* root){
    if (!root) return nullptr; // Base case
    swap(root->m_left, root->m_right); // Swap children as per skew heap property
    // Update NPL for future switches, though it's not strictly used for skew heap logic
    root->m_npl = (root->m_right ? root->m_right->m_npl : -1) + 1;

    return root; // Return the restructured root
}

// Rebuilds the heap structure after a priority function or heap type change
// This function essentially re-heaps the tree to satisfy the new heap property
Post* SQueue::rebuildHeap(Post* root){
    if (!root) return nullptr; // Base case

    // Recursively rebuild left and right subtrees
    root->m_left = rebuildHeap(root->m_left);
    root->m_right = rebuildHeap(root->m_right);

    Post* leader = root; // Start by assuming current root is the leader (min/max)
    // Compare current root with left child to find the true leader
    if (root->m_left){
        if ((m_heapType == MINHEAP && m_priorFunc(*root->m_left) < m_priorFunc(*leader))
            || (m_heapType == MAXHEAP && m_priorFunc(*root->m_left) > m_priorFunc(*leader))){
                leader = root->m_left;
            }
    }

    // Compare current leader with right child to find the overall leader
    if (root->m_right) {
        if ((m_heapType == MINHEAP && m_priorFunc(*root->m_right) < m_priorFunc(*leader)) ||
            (m_heapType == MAXHEAP && m_priorFunc(*root->m_right) > m_priorFunc(*leader))) {
            leader = root->m_right;
        }   
    }

    // If a child is the leader, swap with the current root and recursively rebuild that subtree
    if(leader != root){
        Post temp = *root; // Temporarily store root's data
        *root = *leader; // Copy leader's data to root
        *leader = temp; // Copy root's original data to leader's position
        rebuildHeap(leader); // Recursively rebuild the subtree where the swap occurred
    }

    return root; // Return the (potentially new) root of the subtree
}