//
//  main.cpp
//  SparseMatrixOP
//
//  Created by Dongjin Kim on 7/14/21.
//  Copyright © 2021 Dongjin Kim. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;

class SM{

    vector<double> val;
    vector<int> column_index;
    vector<vector<int>> row_index;
    //=vector<vector<int>>(num_of_unique_rows, vector<int>(2,vals))
    //where vals = col_index of the row's first elem, row_num
    int row, col, size;
    //biggest row val, col val, and size = total # of non-zero elements
    
    
public:
    SM(){
        this->row = 0;
        this->col = 0;
        this->size = 0;
    }
    
    int insert(int r, int c, double v, string op){
        if(v==0){
            cout<<"Input value 0 is ignored in the sparse matrix operation..."<<endl;
            return 0;
        }
        
        //initial case
        if(row_index.size()==0){
            vector<int> row_info = vector<int>(2,0);
            column_index.push_back(c);
            val.push_back(v);
            
            row_info.at(1) = r;
            row_index.push_back(row_info);
            
            this->row = r+1;
            this->col = c+1;
            this->size++;
            return 1;
        }
        
        //rest
        bool updated = false;
        if(r>=this->row){this->row = r+1;}
        if(c>=this->col){this->col = c+1;}
        for(int i=0;i<this->row_index.size();i++){
            vector<int> row_info = this->row_index.at(i);
            //if updated in the previous loop, the remainder row_index[0] needs to be shifted by 1, and continue looping till the end
            if(updated){
                row_index.at(i).at(0) +=1;
                continue;
            }
            
            if(r==row_info[1]){//this row already exists, now working on col with insert and sort
                int N;
                if(i==(int)this->row_index.size()-1){
                    N = (this->size) - row_info[0];
                }else{
                    N = (this->row_index.at(i+1).at(0)) - row_info[0];
                }
                for(int j=row_info[0];j<row_info[0]+N;j++){
                    if(c==this->column_index[j]){
                        
                        if(op=="add"){
                            //in case op = "add", it's a part of an addition operation
                            this->val[j]+=v;
                            return 1;
                        }
                        
                        cout<<"An element already exists at the row "<<r<<" and column "<<c<<endl;
                        cout<<"Insertion of "<<v<<" at ("<<r<<", "<<c<<") failed..."<<endl;
                        return 0;
                    }
                }
                //Inserting a new col value in the column_index vector at the 1st position of the corresponding row lump.
                column_index.insert(column_index.begin()+row_info[0], c);
                //sorting only the relevent row_lump within the column_index vector in ascending order
                sort(column_index.begin()+row_info[0], column_index.begin()+row_info[0]+N+1);
                
                int idx = row_info[0];
                for(int j=row_info[0];j<row_info[0]+N+1;j++){ //N+1 since a new element is added
                    if(c == column_index[j]){
                        idx = j;
                    }
                }

                val.insert(val.begin()+idx, v);
                updated = true;
                
            }else if(r<row_info[1]){//insert the new r
                vector<int> ri = vector<int>(2,0);
                int idx = row_info[0];

                column_index.insert(column_index.begin()+idx,c);
                val.insert(val.begin()+idx,v);
                
                ri[0] = idx;
                ri[1] = r;
                row_index.insert(row_index.begin()+i, ri);
                updated = true;
            }
        }
        if(!updated){
            vector<int> ri = vector<int>(2,0);
            column_index.push_back(c);
            val.push_back(v);
            
            ri[0] = (int) column_index.size() - 1;
            ri[1] = r;
            row_index.push_back(ri);
        }
        this->size++;
        return 1;
    }
    
    SM add(SM other){
        SM result;
        int nRows_a, nRows_b;
        nRows_a = (int)this->row_index.size();
        nRows_b = (int)other.row_index.size();
        
        for(int i=0;i<nRows_a;i++){
            int N;
            if(i==nRows_a-1){
                N = this->size - this->row_index.at(i)[0];
                
            }else{
                N = this->row_index.at(i+1)[0] - this->row_index.at(i)[0];
            }
            for(int j=this->row_index.at(i)[0];j<this->row_index.at(i)[0]+N;j++){
                int c = this->column_index[j];
                double v = this->val[j];
                result.insert(this->row_index.at(i)[1], c, v, "");
            }
        }
        
        for(int i=0;i<nRows_b;i++){
            int N;
            if(i==nRows_b-1){
                N = other.size - other.row_index.at(i)[0];
                
            }else{
                N = other.row_index.at(i+1)[0] - other.row_index.at(i)[0];
            }
            for(int j=other.row_index.at(i)[0];j<other.row_index.at(i)[0]+N;j++){
                int c = other.column_index[j];
                int v = other.val[j];
                result.insert(other.row_index.at(i)[1], c, v, "add");
            }
        }

        return result;
    }
    
    SM transpose(){
        SM result;
        for(int i=0;i<(int)this->row_index.size();i++){
            vector<int> row_info = this->row_index.at(i);
            int N;
            if(i==(int)this->row_index.size()-1){
                N = this->size - row_info[0];
            }else{
                N = this->row_index.at(i+1)[0] - row_info[0];
            }
            for(int j=row_info[0];j<row_info[0]+N;j++){
                int c = this->column_index[j];
                double v = this->val[j];
                result.insert(c, row_info[1], v, "");
            }
        }
        
        return result;
    }
    
    SM product(SM other){
        SM result;
        other = other.transpose();

        //check for validity of dimensions for further operation
        if(this->col!=other.row){
            cout<<"The first matrix's column dimension and the second's row dimension have to be identical for a product operation"<<endl;
            cout<<"Product operation failed"<<endl;
            return result;
        }
        
        for(int i_a=0;i_a<(int)this->row_index.size();i_a++){
            vector<int> row_info_a = this->row_index.at(i_a);
            int N_a;
            if(i_a == (int)this->row_index.size()-1){
                N_a = this->size - row_info_a[0];
            }else{
                N_a = this->row_index.at(i_a+1)[0] - row_info_a[0];
            }
            for(int j_a=row_info_a[0];j_a<row_info_a[0]+N_a;j_a++){
                int col_a = this->column_index[j_a];
                double val_a = this->val[j_a];
                
                for(int i_b=0;i_b<(int)other.row_index.size();i_b++){
                    vector<int> row_info_b = other.row_index.at(i_b);
                    if(col_a==row_info_b[1]){
                        int N_b;
                        if(i_b == (int)other.row_index.size()-1){
                            N_b = other.size - row_info_b[0];
                        }else{
                            N_b = other.row_index.at(i_b+1)[0] - row_info_b[0];
                        }
                        for(int j_b=row_info_b[0];j_b<row_info_b[0]+N_b;j_b++){
                            int col_b = other.column_index[j_b];
                            double val_b = other.val[j_b];
                            result.insert(row_info_a[1], col_b, val_a*val_b, "add");
                        }
                    }else{
                        continue;
                    }
                }
            }
        }
        
        return result;
    }
    
    
    void printSparse(){
        cout<<"Sparse matrix: "<<endl;
        for(int i=0;i<this->row_index.size();i++){
            int N;
            vector<int> row_info = row_index.at(i);
            if(i==(int)this->row_index.size()-1){
                N = (this->size) - row_info[0];
            }else{
                N = this->row_index.at(i+1)[0] - row_info[0];
            }
            std::cout << std::fixed;
            std::cout << std::setprecision(1);
            std::cout<<setw(7);
            for(int j=0;j<this->col;j++){
                bool mark_col = false;
                int idx = 0;
                for(int k=row_info[0];k<row_info[0]+N;k++){
                    if(j==this->column_index[k]){
                        idx = k;
                        mark_col = true;
                        break;
                    }
                }
                
                if(mark_col){
                    cout<<this->val[idx]<<setw(7);;
                }else{
                    cout<<"0.0"<<setw(7);
                }
            }

            cout<<endl;
        }
    }
    
    void printDense(){
        cout<<"Dense matrix: "<<endl;
        std::cout << std::fixed;
        std::cout << std::setprecision(1);
        std::cout<<setw(7);
        for(int i=0;i<this->row;i++){
            bool mark_row = false;
            int row_idx = 0;
            for(int r=0;r<this->row_index.size();r++){
                if(i==this->row_index.at(r).at(1)){
                    mark_row = true;
                    row_idx = r;
                    break;
                }
            }
            
            if(mark_row){
                int N;
                vector<int> row_info = row_index.at(row_idx);
                if(row_info==row_index.back()){
                    N = (this->size) - row_info[0];
                }else{
                    N = this->row_index.at(row_idx+1)[0] - row_info[0];
                }
                for(int j=0;j<this->col;j++){
                    bool mark_col = false;
                    int idx = 0;
                    for(int k=row_info[0];k<row_info[0]+N;k++){
                        if(j==this->column_index[k]){
                            idx = k;
                            mark_col = true;
                            break;
                        }
                    }
                    
                    if(mark_col){
                        cout<<this->val[idx]<<setw(7);;
                    }else{
                        cout<<"0.0"<<setw(7);
                    }
                }
                
            }else{
                for(int j=0;j<this->col;j++){
                    cout<<"0.0"<<setw(7);
                }
            }
            cout<<endl;
        }
    }
        
      
    
};

int main(int argc, const char * argv[]) {
    
    SM a1,a2,a3;
    a1.insert(1,1,1,"");
    a1.insert(1,3,2.5,"");
    a1.insert(2,3,3.5,"");
    cout<<"----a1------"<<endl;
    a1.printSparse();
    a1.printDense();

    a2.insert(2,3,1.7,"");
    a2.insert(1,3,1.1,"");
    a2.insert(1,1,8,"");
    a2.insert(0,1,3.1,"");
    cout<<"----a2 transposed------"<<endl;
    a2.transpose().printSparse();
    a2.transpose().printDense();
    
    cout<<"the product of a1 and a2 transposed"<<endl;
    a3 = a1.product(a2);
    a3.printSparse();
    a3.printDense();

    return 0;
}
