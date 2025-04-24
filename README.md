# Post Manager Project

This project manages social media posts, prioritizing them based on factors like posting time, connection level, and other criteria. It uses skew-heap or Leftist-heap data structures to efficiently handle post ordering.

**Classes:**

* **SQueue**: This class implements the post queue, ordering posts according to their calculated priority. It employs a skew-heap or Leftist-heap for efficient management and allows switching between these heap types.
* **Post**: This class represents a social media post with attributes such as its ID, number of likes, connection level between users, posting time, and user interest level.

**Relationship:**

The SQueue class organizes and prioritizes Post objects, effectively managing the order in which posts are displayed or processed. Post priority is determined by a user-defined function that considers the post's attributes.

**Key Features:**

* Prioritizes social media posts based on attributes like posting time, connection level, likes, and user interest.
* Uses skew-heap or Leftist-heap for efficient post queue management.
* Supports dynamic switching between skew-heap and Leftist-heap implementations.
* Allows flexible customization of post prioritization through user-defined priority functions.
* Handles social media posts with varying attributes relevant to social media platforms.


