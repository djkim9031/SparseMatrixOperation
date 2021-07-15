# SparseMatrixOperation
Personal project on the operations of sparse matrices using C++

Given the limited memory of an embedded system, how best to represent a sparse matrix, and operate them(addition, transpose, and product with another sparse matrix)?
The key is to represent the matrix as following:

e.g.) 

      [0 1 0 2 3]

      [1 4 0 0 0]
      
      [0 0 0 0 0]
      
      [0 0 0 0 0]
      
      
Given the sparse matrix above,
column_index = [1, 3, 4, 0, 1]
values = [1, 2, 3, 1, 4]
row_index (vector of vectors)= [[0,0], [3,1]]

That is, column_index and values arrays(or vectors) record all the corresponding elements.
However, the row_index records only a row's first (representative) element's index in the column_index/values array and its row.

It is of the form 
```
vector<vector<int>>(vector<int>(2,corresponding vals),number_of_unique_rows)
```
By doing this, we don't need to record every element's row, column, and value metrics. This further saves memory.

Major update has been made since the last upload
