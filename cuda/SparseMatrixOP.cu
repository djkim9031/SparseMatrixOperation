#include <stdio.h>
#include <iostream>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

using namespace std;

class SM{
public:
    
    //insertion and sort helper functions
    template<typename T>
    __host__ void insertElement(thrust::host_vector<T> &vect,int idx, T value){
        
        //First increase the size by 1
        vect.push_back(0);
        
        T temp = vect[idx];
        vect[idx] = value;
        for(int i=idx+1;i<vect.size();i++){
            T temp_ = vect[i];
            vect[i] = temp;
            temp = temp_;
        }
        return;
    }
    
    template<typename T>
    __host__ void sortElements(thrust::host_vector<T> &vect, int startIdx, int endIdx){
    
        for(int i=startIdx;i<endIdx-1;i++){
            for(int j=startIdx+1;j<endIdx;j++){
                if(vect[i]>vect[j]){
                    T temp = vect[i];
                    vect[i] = vect[j];
                    vect[j] = temp;
                }
            }
        }
        return;
    }
    
public:
    int row, col, size;
    
    thrust::host_vector<double> val;
    thrust::host_vector<int> column_index;
    thrust::host_vector<int> row_index;
    thrust::host_vector<int> rows;
    
    __host__ SM(){
        this->row = 0;
        this->col = 0;
        this->size = 0;
    }
    
    __host__ void insert(int r, int c, double v){
        if(v==0){
         printf("Input value 0 is ignored in the sparse matrix operation...\n");
         return;
        }
        
        /*initial case*/
        if(this->size==0){
            row_index.push_back(0);
            rows.push_back(r);
            column_index.push_back(c);
            val.push_back(v);
            this->row = r+1;
            this->col = c+1;
            this->size++;
            return;
        }
        
        /*rest*/
        bool updated = false;
        if(r>=this->row){this->row = r+1;}
        if(c>=this->col){this->col = c+1;}
        for(int i=0;i<rows.size();i++){
            //if updated in the previous loop, the remainder row_index needs to be shifted by 1, and continue looping till the end
            if(updated){
                row_index[i] += 1;
                continue;
            }
        
            //In case the row already exists
            if(r==rows[i]){
                int N;
                if(i==rows.size()-1){
                    N = this->size - row_index[i];
                }else{
                    N = row_index[i+1] - row_index[i];
                }
                for(int j=row_index[i];j<row_index[i]+N;j++){
                    if(c==column_index[j]){
                        printf("An element already exists at the row %d and column %d\n",r,c);
                        printf("Insertion of %lf at (%d, %d) failed...\n",v,r,c);
                        return;
                    }
                }
                //Inserting a new col value in the column_index vector at the 1st position of the corresponding row lump.
                insertElement(column_index,row_index[i], c);
                sortElements(column_index, row_index[i], row_index[i]+N+1);
                int idx = row_index[i];
                for(int j=row_index[i];j<row_index[i]+N+1;j++){
                    if(c==column_index[j]){
                        idx = j;
                        break;
                    }
                }
                insertElement(val,idx,v);
                updated = true;

            }else if(r<rows[i]){//In case the row doesn't exist
                int idx = row_index[i];
                insertElement(column_index,idx,c);
                insertElement(val,idx,v);
                insertElement(row_index,i,idx);
                insertElement(rows,i,r);
                updated = true;
           
            }
        }
        
        if(!updated){
            column_index.push_back(c);
            val.push_back(v);
            rows.push_back(r);
            row_index.push_back(this->size-1);
        }

        this->size++;
        return;
    }
  
    __host__ void printSparse(){
        cout<<"Sparse matrix: "<<endl;
        for(int i=0;i<rows.size();i++){
            int N;
            if(i==rows.size()-1){
                N = this->size - row_index[i];
            }else{
                N = row_index[i+1] - row_index[i];
            }
            for(int j=0;j<this->col;j++){
                bool mark_col = false;
                int idx = 0;
                for(int k=row_index[i];k<row_index[i]+N;k++){
                    if(j==column_index[k]){
                        idx = k;
                        mark_col = true;
                        break;
                    }
                }
                if(mark_col){
                    cout<<val[idx]<<"       ";
                }else{
                    cout<<"0.0       ";
                }
            }
            cout<<endl;
        }
    }
    
    __host__ void printDense(){
        cout<<"Dense matrix: "<<endl;
        for(int i=0;i<this->row;i++){
            bool mark_row = false;
            int row_idx = 0;
            for(int r=0;r<rows.size();r++){
                if(i==rows[r]){
                    mark_row = true;
                    row_idx = r;
                    break;
                }
            }
            if(mark_row){
                int N;
                if(rows[row_idx]==this->row-1){
                    N = this->size - row_index[row_idx];
                }else{
                    N = row_index[row_idx+1] - row_index[row_idx];
                }
                for(int j=0;j<this->col;j++){
                    bool mark_col = false;
                    int idx =0;
                    for(int k=row_index[row_idx];k<row_index[row_idx]+N;k++){
                        if(j==column_index[k]){
                            idx = k;
                            mark_col = true;
                            break;
                        }
                    }
                    if(mark_col){
                        cout<<val[idx]<<"       ";
                    }else{
                        cout<<"0.0       ";
                    }
                }
                
            }else{
                for(int j=0;j<this->col;j++){
                    cout<<"0.0       ";
                }
            }
            cout<<endl;
        }
    }
 
    
};

template<class T>
void vect2arr(thrust::host_vector<T> vect,T *arr){
   for(int i=0;i<vect.size();i++){
      arr[i] = vect[i];
  }
}

__global__ void vec_product(int sparse_row_size_a, int sparse_row_size_b, double *val_a_d,double *val_b_d,
int *row_index_a_d,int *rows_a_d,int *row_index_b_d,int *rows_b_d,int *column_index_a_d,int *column_index_b_d,SM a,SM b,double *result){
    //operation validity check
    if(a.col!=b.row){
        printf("The first matrix's column dimension and the second one's row dimension doesn't match\n");
        printf("Failed to perform a product operation\n");
        return;
    }
    
    //result dim = a.row by b.col
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    if(i<a.row && j<b.col){
        int idx_st_a, N_a;
        for(int r=0;r<sparse_row_size_a;r++){
            if(rows_a_d[r] == i){
                idx_st_a = row_index_a_d[r];
                if(r==sparse_row_size_a-1){
                    N_a = a.size - idx_st_a;
                }else{
                    N_a = row_index_a_d[r+1] - idx_st_a;
                }
                for(int col_a_idx = idx_st_a;col_a_idx<idx_st_a+N_a;col_a_idx++){
                    for(int l=0;l<sparse_row_size_b;l++){
                        if(column_index_a_d[col_a_idx] == rows_b_d[l]){
                            int idx_st_b, N_b;
                            idx_st_b = row_index_b_d[l];
                            if(l==sparse_row_size_b-1){
                                N_b = b.size - idx_st_b;
                            }else{
                                N_b = row_index_b_d[l+1] - idx_st_b;
                            }
                            for(int col_b_idx = idx_st_b;col_b_idx<idx_st_b+N_b;col_b_idx++){
                                if(column_index_b_d[col_b_idx]==j){
                                    //printf("(%d, %d),%lf,%lf\n",i,j,val_a_d[col_a_idx],val_b_d[col_b_idx]);
                                    result[j+b.col*i] += val_a_d[col_a_idx]*val_b_d[col_b_idx];
                                }
                            }
                        }
                    }
                }
            }
        }
        


    }
    return;
}

int main()
{
  SM a1,a2,a3;
  a1.insert(3,2,1.8);
  a1.insert(1,1,1);
  a1.insert(1,2,1.5);
  a1.insert(3,3,3.3);
  a1.insert(0,2,1.1);
  a1.insert(3,0,1);
  a1.insert(0,0,7);
  a1.insert(1,0,3);
  cout<<"----a1------"<<endl;
  a1.printSparse();
  a1.printDense();
  cout<<"__________________"<<endl;
  a2 = a1;
  cout<<"----a2------"<<endl;
  a2.printSparse();
  a2.printDense();
  cout<<"__________________"<<endl;
  
  
  //host_vector to array transformation. device_vector is only accessible on host, and on device operation is not viable.
  //Still need to figure out how best to perform operation without transforming it like this.
  //____________________________________________
  double val_a[a1.val.size()];
  int column_index_a[a1.column_index.size()];
  int row_index_a[a1.row_index.size()];
  int rows_a[a1.rows.size()];
  
  double val_b[a2.val.size()];
  int column_index_b[a2.column_index.size()];
  int row_index_b[a2.row_index.size()];
  int rows_b[a2.rows.size()];
  
  vect2arr(a1.val,val_a);
  vect2arr(a1.column_index,column_index_a);
  vect2arr(a1.row_index,row_index_a);
  vect2arr(a1.rows,rows_a);
  
  vect2arr(a2.val,val_b);
  vect2arr(a2.column_index,column_index_b);
  vect2arr(a2.row_index,row_index_b);
  vect2arr(a2.rows,rows_b);

  double *val_a_d, *val_b_d;
  int *row_index_a_d, *rows_a_d,*column_index_a_d, *column_index_b_d, *row_index_b_d, *rows_b_d;
  
  cudaMalloc((void**)&val_a_d,sizeof(double)*a1.val.size());
  cudaMalloc((void**)&val_b_d,sizeof(double)*a2.val.size());
  cudaMalloc((void**)&row_index_a_d,sizeof(int)*a1.row_index.size());
  cudaMalloc((void**)&rows_a_d,sizeof(int)*a1.rows.size());
  cudaMalloc((void**)&column_index_a_d,sizeof(int)*a1.column_index.size());
  cudaMalloc((void**)&column_index_b_d,sizeof(int)*a2.column_index.size());
  cudaMalloc((void**)&row_index_b_d,sizeof(int)*a2.row_index.size());
  cudaMalloc((void**)&rows_b_d,sizeof(int)*a2.rows.size());
  
  
  cudaMemcpy(val_a_d,val_a,sizeof(double)*a1.val.size(),cudaMemcpyHostToDevice);
  cudaMemcpy(val_b_d,val_b,sizeof(double)*a2.val.size(),cudaMemcpyHostToDevice);
  cudaMemcpy(column_index_a_d,column_index_a,sizeof(int)*a1.column_index.size(),cudaMemcpyHostToDevice);
  cudaMemcpy(column_index_b_d,column_index_b,sizeof(int)*a2.column_index.size(),cudaMemcpyHostToDevice);
  cudaMemcpy(row_index_a_d,row_index_a,sizeof(int)*a1.row_index.size(),cudaMemcpyHostToDevice);
  cudaMemcpy(rows_a_d,rows_a,sizeof(int)*a1.rows.size(),cudaMemcpyHostToDevice);
  cudaMemcpy(row_index_b_d,row_index_b,sizeof(int)*a2.row_index.size(),cudaMemcpyHostToDevice);
  cudaMemcpy(rows_b_d,rows_b,sizeof(int)*a2.rows.size(),cudaMemcpyHostToDevice);
  
  double result[a1.row*a2.col] = {0.0};
  double *result_d;
  cudaMalloc((void**)&result_d,sizeof(double)*a1.row*a2.col);
  cudaMemcpy(result_d,result,sizeof(double)*a1.row*a2.col,cudaMemcpyHostToDevice);

  //____________________________________________
  dim3 threadsPerBlock(a1.row, a1.col);
  dim3 numBlocks((a1.row + threadsPerBlock.x -1) / threadsPerBlock.x, (a2.col +threadsPerBlock.y -1) / threadsPerBlock.y);
  
  vec_product<<<numBlocks, threadsPerBlock>>>(a1.rows.size(), a2.rows.size(),val_a_d,val_b_d,row_index_a_d,rows_a_d,row_index_b_d,rows_b_d,column_index_a_d,column_index_b_d,a1,a2,result_d);
  
  cudaDeviceSynchronize();
  cudaMemcpy(result, result_d, sizeof(double)*a1.row*a2.col, cudaMemcpyDeviceToHost);
  printf("Product of a1 and a2 calculated!\n");
  
  for(int i=0;i<a1.row;i++){
      for(int j=0;j<a2.col;j++){
          cout<<result[j+a2.col*i]<<"       ";
      }
      cout<<endl;
  }
  
  cudaFree(val_a_d);
  cudaFree(val_b_d);
  cudaFree(row_index_a_d);
  cudaFree(rows_a_d);
  cudaFree(row_index_b_d);
  cudaFree(rows_b_d);
  cudaFree(column_index_a_d);
  cudaFree(column_index_b_d);
  cudaFree(result_d);

}
