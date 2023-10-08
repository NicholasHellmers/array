Notes from video: https://www.youtube.com/watch?v=Qx3P2wazwI0

# 1. Bounded-Buffer Problem: (AKA Producer-Consumer Problem)
* The Bounded Buffer Problem, also known as the Producer Consumer Problem, is one of the classic problems of synchronization.
* <u>**Problem characteristics**</u>:
    * There is a `buffer with n slots` and `each slot is capable of storing one unit of information`.
    * There are two processes running, `Producers` and `Consumers`, which interact with the buffer:
        * `Producers`: `Produce and store information` into the buffer. It attempts to store data into an empty slot within the buffer.
        * `Consumers`: `Remove information` from the buffer. It attempts to remove data from a non-empty slot within the buffer.
        [![](/img/Buffer-producer-consumer.png)](Buffer-producer-consumer.png) 
        
    * Problems to take into account:
        1. The Producer must not insert any data when the buffer is full.
        2. The Consumer must not attempt to remove data from an empty buffer.
        3. The Producer and Consumer must not insert and remove data at the same time.
    * We can use Semaphores to solve these problems.
* <u>**Solution**</u>:
    1. m (mutex), a binary semaphore, is used to provide mutual exclusion for the critical section.
    2. empty, a counting semaphore, is used to keep track of the number of empty buffers slots. It is initialized to the number of slots in the buffer as initially all slots within the buffer are empty.
    3. full, another counting semaphore, whose value is initially 0, is used to keep track of the number of full buffer slots. It is zero initially as the buffer is empty.
    4. The Producer and Consumer processes are implemented as separate threads. The Producer thread executes the following code:
        ``` c
        /* Producer */
        while (true) {
            /* produce an item in next_produced */
            
            P(empty);   // wait until empty > 0 and then decrement 'empty'
            P(m);       // acquire lock

            /* add next_produced to the buffer */

            V(m);       // release lock
            V(full);    // increment 'full'
        }
        ```
    5. The Consumer thread executes the following code:
        ``` c
        /* Consumer */
        while (true) {
            P(full);    // wait until full > 0 and then decrement 'full'
            P(m);       // acquire lock

            /* remove an item from buffer to next_consumed */

            V(m);       // release lock
            V(empty);   // increment 'empty'
            
            /* consume the item in next_consumed */
        }
        ```