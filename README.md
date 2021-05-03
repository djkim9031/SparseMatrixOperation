# SparseMatrixOperation
Operations of sparse matrices in embedded system.

Given the limited memory of an embedded system, how best to represent a sparse matrix, and operate them(addition, transpose, and multiplication with another sparse matrix)?
The key is to represent the matrix as following:

e.g.) [0 1 0 2 3]
      [1 4 0 0 0]
      [0 0 0 0 0]
      [0 0 0 0 0]
Given the sparse matrix above,
column_index = [1, 3, 4, 0, 1]
values = [1, 2, 3, 1, 4]
row_index = [(0,0), (3,1), 5]

That is, column_index and values arrays(or vectors) record all the corresponding elements.
However, the row_index records only a row's first (representative) element's index in the column_index/values array and its row.
By doing this, we don't need to record every element's row, column, and value metrics. This further saves memory.

However, as a tradeoff, the time complexity for multiplication operation could be at worst ~O(n^5)
