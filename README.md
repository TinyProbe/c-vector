# c-vector.
A vector in c.

### docs.
```c
//
//  Macro function list.
// 

void    vt_in()
    // The 'vt_in' function is called at the beginning of the scope of the
    // function where the vector will be created. Then, when 'vt_out' is
    // called, it automatically clears all vectors created during that time.
    // This does not clear any memory that the user dynamically allocated.

void    vt_out()
    // 'vt_out' clears all vectors created after 'vt_in' was called and
    // before 'vt_out' is called. It does not free memory that the user
    // may have allocated arbitrarily. (Even if you do not call 'vt_in'
    // and 'vt_out', all vectors will be automatically cleared before
    // the program terminates. This automatic clearing feature is safe
    // to use only in single-threaded mode. In multithreading, you should
    // declare the __VECTOR_MULTITHREAD macro and use manual clearing.)

void    vt_delete(vector)
    // The 'vt_delete' function is a vector manual deletion function that
    // is only available in multithreading. It recursively deletes all
    // vectors, even if a multidimensional vector is input.

    // vector: Vector to be deleted.
    // 
    // int main(void) {
    //     vector v = vt_new(int, 2, 10, 20); // same as [10][20].
    //     vt_delete(v); // All vectors within the vector are also deleted.
    // }

vector  vt_new(type, dimension, ...)
    // The vt_new() function creates and returns a vector object.

    // type: The type of elements to be contained in the vector.
    // dimension: Indicates the depth of the dimension of the vector.
    // ...: Represents the number of elements in the vector in each
    //      dimension.
    //
    // int main(void) {
    //     vt_in();
    //     vector v = vt_new(long, 3, 10, 20, 30); // same as [10][20][30].
    //     vt_out();
    // }

size_t  vt_count(vector)
    // The 'vt_count' function returns the number of elements in a vector.

    // vector: The vector for which you want to get the number of elements.

size_t  vt_size(vector)
    // Same as 'vt_count'.

size_t  vt_len(vector)
    // Same as 'vt_count'.

size_t  vt_empty(vector)
    // The 'vt_empty' function returns true if the vector is empty,
    // otherwise it returns false.

    // vector: A vector to check if it is empty or not.

size_t  vt_type_size(vector)
    // The 'vt_type_size' function returns the size of the element type of
    // a vector in bytes.

    // vector: A vector to get the type size.

size_t  vt_type_count(vector)
    // The 'vt_type_count' function returns the number of allocated
    // elements.

    // vector: A vector to get the number of types.

size_t  vt_capacity(vector)
    // The 'vt_capacity' function returns the size of the space allocated
    // to a vector in bytes.

    // vector: A vector to get the allocation size.

void    vt_resize(vector, count)
    // The 'vt_resize' function changes the number of elements in a vector.

    // vector: A vector whose number of elements will be changed.
    // count: Number of new elements.

void    vt_clear(vector)
    // The 'vt_clear' function removes and deallocates all elements.

    // vector: A vector from which all elements will be removed.

void    vt_add(vector, item_variable)
    // The 'vt_add' function adds an element to the end of a vector.
    // (Only variables are accepted.)

    // vector: A vector to add elements to.
    // item_variable: Elements to add.

void    vt_push(vector, item_literal)
    // The 'vt_push' function adds an element to the end of a vector.
    // (Only literal values are allowed.)

    // vector: A vector to add elements to.
    // item_literal: Elements to add.

void    vt_pop(vector)
    // The 'vt_pop' function removes the last element of a vector.

    // vector: A vector from which to remove the last element.

void    vt_insert(vector, index, item_variable)
    // The 'vt_insert' function adds an element to a vector at a specific
    // position. (Only variables are accepted.)

    // vector: A vector to add elements to.
    // index: Index at which to insert the element.
    // item_variable: Elements to add.

void    vt_input(vector, index, item_literal)
    // The 'vt_input' function adds an element to an arbitrary position
    // in a vector. (Only literal values are allowed.)

    // vector: A vector to add elements to.
    // index: Index at which to insert the element.
    // item_literal: Elements to add.

void    vt_erase(vector, index)
    // The 'vt_erase' function removes the last element of a vector.

    // vector: A vector from which to remove the last element.
    // index: The index of the element to remove.

type    vt_at(vector, index, type)
    // The 'vt_at' function dereferences a specific element of a vector.
    // (For performance reasons, vector index range checking is not
    // performed.)

    // vector: A vector to dereference elements.
    // index: Index of the element to dereference.
    // type: The type of element to dereference.

type    vt_front(vector, type)
    // The 'vt_front' function dereferences the first element of a vector.

    // vector: A vector that dereferences the first element.
    // type: The type of element to dereference.

type    vt_back(vector, type)
    // The 'vt_back' function dereferences the last element of the vector.

    // vector: A vector that dereferences the last element.
    // type: The type of element to dereference.

void    vt_swap(left, right)
    // The 'vt_swap' function swaps two vectors.

    // left: The left-hand vector to be exchanged.
    // right: The right-hand vector to be exchanged.

vector  vt_move(source)
    // The 'vt_move' function returns ownership of the vector.

    // source: The vector to return ownership to.

vector  vt_clone(source)
    // The 'vt_clone' function creates and returns a clone object of a
    // vector. (Deep replication is performed between vectors, but memory
    // arbitrarily allocated by the user is not replicated.)

    // source: The vector to be cloned.

void    vt_reverse(vector)
    // The 'vt_reverse' function reverses the order of elements in a vector.

    // vector: A vector whose elements are to be reversed in order.

vt_itr  vt_itr(vector, index)
    // The 'vt_itr' function returns an iterator pointing to a specific
    // index.

    // vector: A vector from which to create iterators.
    // index: The index to which the iterator will point.

vt_itr  vt_begin(vector)
    // The 'vt_begin' function returns an iterator pointing to the first
    // element.

    // vector: A vector from which to create iterators.

vt_itr  vt_end(vector)
    // The 'vt_end' function returns an iterator pointing to the position
    // after the last element.

    // vector: A vector from which to create iterators.

void    vt_next(iterator)
    // The 'vt_next' function moves the position pointed to by the
    // iterator to the next element.

    // iterator: An iterator that moves the specified element to the
    //           next one.

void    vt_prev(iterator)
    // The 'vt_prev' function moves the position pointed to by the
    // iterator to the previous element.

    // iterator: An iterator that moves the specified element to the
    //           previous element.

type    vt_ref(iterator, type)
    // The 'vt_ref' function returns a dereference to the element
    // specified by the iterator.

    // iterator: An iterator to get the dereference.
    // type: The type of element to dereference.

```
