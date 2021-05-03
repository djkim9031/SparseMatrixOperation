//
//  main.cpp
//  SparseMatrix
//
//  Created by Dongjin Kim on 4/29/21.
//  Copyright © 2021 Dongjin Kim. All rights reserved.
//
/*
 [1 0 0 0 0]
 [8 3 0 0 0]
 [0 0 0 0 0]
 [0 4 9 0 0]
 [6 0 7 8 0]
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Node{
public:
    string data;
    Node* next;
};

void insertLast(Node* n, string d){
    if(n->next==NULL){
        Node* temp = new Node();
        temp->data = d;
        temp->next = NULL;
        n->next = temp;
        temp=NULL;
        delete temp;
        return;
    }
    insertLast(n->next,d);
}
void deleteLast(Node* n){
    if(n->next->next==NULL){
        n->next = NULL;
        return;
    }
    return deleteLast(n->next);
}


int* rowStringParser(string str){
    int* rowInfo = new int[2];
    string x,y;
    bool preParse = true;
    
    for(int i=0;i<str.length();i++){
        if(preParse){
            if(str[i]==','){
                preParse=false;
            }else{
                x+=str[i];}
        }else{
            y+=str[i];
        }
    }
    
    rowInfo[0] = stoi(x);
    if(y.length()>0){
    rowInfo[1] = stoi(y);
    }
    return rowInfo;
}

class SM{
    vector<double> val;
    vector<int> column_index;
    Node* row_index = new Node();
    int size;
    int row,col;
    
public:
    //constructor:
    SM(int r, int c){
        row = r;
        col = c;
        row_index = NULL;
        size=0;
    }
    
    //insert each row
    void insert(double *r, int row_num){
        bool firstElem = 1;
        bool anyInserted = false;
        int firstVal;
        
        for(int i=0;i<col;i++){
            if((int)r[i]!=0){
                
                val.push_back(r[i]);
                column_index.push_back(i);
                if(firstElem){
                    firstVal = size;

                    firstElem=0;
                }
                size++;
                anyInserted = true;
            }
        }
        if(row_index==NULL and anyInserted){//First insert
            Node* elem = new Node();
            Node* _size = new Node();
            elem->data = to_string(firstVal)+','+to_string(row_num);
            _size->data = to_string(size);
            elem->next = _size;
            _size->next = NULL;
            
            row_index = elem;
            
            elem = NULL;
            _size = NULL;
            delete elem;
            delete _size;
            
        }else{
            if(anyInserted){
                deleteLast(row_index);
                insertLast(row_index, to_string(firstVal)+','+to_string(row_num));
                insertLast(row_index, to_string(size));
            }
            
        }
       /*
        Node* nn = row_index;
        int* y = rowStringParser(nn->data);
        while(nn!=NULL){
            y = rowStringParser(nn->data);
            cout<<y[0]<<","<<y[1]<<"   ";
            nn = nn->next;
        }
        cout<<endl;*/
        /*
        for(int i=0;i<size;i++){
            cout<<val[i]<<"...";
        }
        cout<<endl;
         */
    }
    
    //Addition
    SM add(SM other){
        SM result(row,col);
        if(row!=other.row or col!=other.col){
            std::cout<<"Please check the matrix dimensions"<<std::endl;
            return result;
        }
        
        int *rowInfo1_1 = new int[2];
        int *rowInfo1_2 = new int[2];
        int *rowInfo2_1 = new int[2];
        int *rowInfo2_2 = new int[2];
       
        Node *rowIndexer1 = row_index;
        Node *rowIndexer2 = other.row_index;

        
        for(int i=0;i<row;i++){
            if(rowIndexer1->next==NULL and rowIndexer2->next==NULL){
                break;
            }else if(rowIndexer1->next==NULL){
                rowInfo2_1 = rowStringParser(rowIndexer2->data);
                rowInfo2_2 = rowStringParser(rowIndexer2->next->data);
                rowInfo1_1[0]=-1; rowInfo1_1[1]=-1;
                rowInfo1_2[0]=-1; rowInfo1_2[1]=-1;
            }else if(rowIndexer2->next==NULL){
                rowInfo1_1 = rowStringParser(rowIndexer1->data);
                rowInfo1_2 = rowStringParser(rowIndexer1->next->data);
                rowInfo2_1[0]=-1; rowInfo2_1[1]=-1;
                rowInfo2_2[0]=-1; rowInfo2_2[1]=-1;
            }else{
                rowInfo1_1 = rowStringParser(rowIndexer1->data);
                rowInfo2_1 = rowStringParser(rowIndexer2->data);
                rowInfo1_2 = rowStringParser(rowIndexer1->next->data);
                rowInfo2_2 = rowStringParser(rowIndexer2->next->data);
            }
            double *r = new double[col];
            
            if(i==rowInfo1_1[1] and i==rowInfo2_1[1]){
                int start1 = rowInfo1_1[0]; int start2 = rowInfo2_1[0];
                int num_vals1 = rowInfo1_2[0]-rowInfo1_1[0]; int num_vals2 = rowInfo2_2[0] - rowInfo2_1[0];
                for(int j=0;j<col;j++){
                    if(j==column_index[start1] and j==other.column_index[start2] and num_vals1>0 and num_vals2>0){
                        r[j] = val[start1] + other.val[start2];
                        start1++; start2++;
                        num_vals1--; num_vals2--;
                    }else if(j==column_index[start1] and num_vals1>0){
                        r[j] = val[start1];
                        start1++; num_vals1--;
                    }else if(j==other.column_index[start2] and num_vals2>0){
                        r[j] = other.val[start2];
                        start2++; num_vals2--;
                    }else{
                        r[j]=0.0;
                    }
                }
                rowIndexer1 = rowIndexer1->next;
                rowIndexer2 = rowIndexer2->next;
                result.insert(r, i);
            }else if(i==rowInfo1_1[1]){
                int start1 = rowInfo1_1[0];
                int num_vals1 = rowInfo1_2[0]-rowInfo1_1[0];
                for(int j=0;j<col;j++){
                    if(j==column_index[start1] and num_vals1>0){
                        r[j] = val[start1];
                        start1++;
                        num_vals1--;
                    }else{
                        r[j]=0.0;
                    }
                }
                rowIndexer1 = rowIndexer1->next;
                result.insert(r, i);
            }else if(i==rowInfo2_1[1]){
                int start2 = rowInfo2_1[0];
                int num_vals2 = rowInfo2_2[0]-rowInfo2_1[0];
                for(int j=0;j<col;j++){
                    if(j==other.column_index[start2] and num_vals2>0){
                        r[j] = other.val[start2];
                        start2++;
                        num_vals2--;
                    }else{
                        r[j]=0.0;
                    }
                }
                rowIndexer2 = rowIndexer2->next;
                result.insert(r, i);
            }else{
                continue;
            }
            delete[] r;
        }

        delete[] rowInfo1_1;
        delete[] rowInfo1_2;
        delete[] rowInfo2_1;
        delete[] rowInfo2_2;
        rowIndexer1 = NULL;
        rowIndexer2 = NULL;
        delete rowIndexer1;
        delete rowIndexer2;
        
        return result;
    }
    
    //Transpose
    SM transpose(){
        SM transposed(col,row);
        //column_index (0 0 1 1 2 0 3 ...)
        //row_index (0,0 1,1 4,3 ... 10)
        int* rowInfo1 = new int[2];
        int* rowInfo2 = new int[2];
        
        for(int i=0;i<col;i++){ //iteration of transposed's row
            double *r = new double[row];
            bool elemAdded = false;
            for(int j=0;j<row;j++){//Initialize
                r[j]=0.0;
            }
            for(int j=0;j<size;j++){
                if(i==column_index[j]){
                    Node* rowIndexer = row_index;
                    while(rowIndexer->next!=NULL){
                        rowInfo1 = rowStringParser(rowIndexer->data);
                        rowInfo2 = rowStringParser(rowIndexer->next->data);

                        if(j>=rowInfo1[0] and j<rowInfo2[0]){
                            r[rowInfo1[1]] = val[j];
                            elemAdded = true;
                        }
                        
                        
                        rowIndexer = rowIndexer->next;
                    }
                    rowIndexer = NULL;
                    delete rowIndexer;
                }
            }
            if(elemAdded){
                transposed.insert(r, i);
            }
            delete[] r;
        }

        delete[] rowInfo1;
        delete[] rowInfo2;
        
        return transposed;
    }
    
    //Multiplication
    SM multiply(SM other){
        //other = other.transpose();
        SM result(row,other.col);
        if(col!=other.row){
            std::cout<<"Please check the matrix dimensions -- MxN * NxA is possible operation"<<std::endl;
            return result;
        }
        Node* rowIndexer1 = row_index;
        while(rowIndexer1->next!=NULL){
            double *r = new double[other.col];
            int *rowInfo1_1 = new int[2];
            int *rowInfo1_2 = new int[2];
            rowInfo1_1 = rowStringParser(rowIndexer1->data);
            rowInfo1_2 = rowStringParser(rowIndexer1->next->data);
            int currRow = rowInfo1_1[1];int start1 = rowInfo1_1[0];int numVals1 = rowInfo1_2[0] - rowInfo1_1[0];
            for(int i=0;i<other.col;i++){
                double sum=0.0;
                for(int j=start1;j<start1+numVals1;j++){
                    Node* rowIndexer2 = other.row_index;
                    while(rowIndexer2->next!=NULL){
                        auto rowInfo2_1 = rowStringParser(rowIndexer2->data);
                        auto rowInfo2_2 = rowStringParser(rowIndexer2->next->data);
                        int start2 = rowInfo2_1[0];
                        int numVals2 = rowInfo2_2[0] - rowInfo2_1[0];
                        if(rowInfo2_1[1]==column_index[j]){
                            for(int k=start2;k<start2+numVals2;k++){
                                if(other.column_index[k]==i){
                                    sum+=val[j]*other.val[k];
                                }
                            }
                        }
                        rowIndexer2 = rowIndexer2->next;
                    }
                    rowIndexer2=NULL;
                    delete rowIndexer2;
                }
                r[i]=sum;
            }
            result.insert(r,currRow);
            rowIndexer1 = rowIndexer1->next;
            delete[] rowInfo1_1;
            delete[] rowInfo1_2;
            delete[] r;
        }
        rowIndexer1=NULL;
        delete rowIndexer1;
        return result;
    }
    
    
    //print Dense Matrix
    void printDense(){
        Node *indexer = row_index;
        if(indexer==NULL){
            for(int i=0;i<row;i++){
                for(int j=0;j<col;j++){
                    cout<<0<<"     ";
                }
                cout<<endl;
            }
            return;
        }

        bool firstRow = true;
        bool lastRow = false;
        int currRow=0;
        while(indexer->next!=NULL){
            if(indexer->next->next==NULL){
                lastRow = true;
            }
            int* rowInfo1 = rowStringParser(indexer->data);
            int* rowInfo2 = rowStringParser(indexer->next->data);
            currRow = rowInfo1[1]; int start = rowInfo1[0]; int numVals = rowInfo2[0] - rowInfo1[0];
            if(firstRow){
                for(int i=0;i<currRow;i++){
                    for(int j=0;j<col;j++){
                        cout<<0<<"     ";
                    }
                    cout<<endl;
                }
                firstRow=false;
            }
            if(lastRow){
                for(int j=0;j<col;j++){
                    if(j==column_index[start] and numVals>0){
                        cout<<val[start]<<"     ";
                        start++; numVals--;
                    }else{
                        cout<<0<<"     ";
                    }
                }
                cout<<endl;
            }else{
                for(int i=currRow;i<rowInfo2[1];i++){
                    for(int j=0;j<col;j++){
                        if(j==column_index[start] and numVals>0){
                            cout<<val[start]<<"     ";
                            start++; numVals--;
                        }else{
                            cout<<0<<"     ";
                        }
                    }
                    cout<<endl;
                }
            }
            
            indexer = indexer->next;
        }
        if(currRow<row-1){
            for(int i=currRow;i<row-1;i++){
                for(int j=0;j<col;j++){
                    cout<<0<<"     ";
                }
                cout<<endl;
            }
        }
        indexer=NULL;
        delete indexer;
        

    }
    
    //print Sparse Matrix
    void printSparse(){
        Node* indexer = row_index;
        if(indexer==NULL){
            return;
        }
        
        while(indexer->next!=NULL){
            int* rowInfo1 = rowStringParser(indexer->data);
            int* rowInfo2 = rowStringParser(indexer->next->data);
            int start = rowInfo1[0]; int numVals = rowInfo2[0] - rowInfo1[0];
            for(int j=0;j<col;j++){
                if(j==column_index[start] and numVals>0){
                    cout<<val[start]<<"     ";
                    start++; numVals--;
                }else{
                    cout<<0<<"     ";
                }
            }
            cout<<endl;
            indexer = indexer->next;
        }
        indexer=NULL;
        delete indexer;
    }
    
};

int main(int argc, const char * argv[]) {
    
    double row0[5] = {1.1,0.0,0.0,0.0,0.0};
    double row1[5] = {0.0,8.0,3.0,0.0,0.0};
    double row2[5] = {0.0,0.0,0.0,0.0,0.0};
    double row3[5] = {0.0,4.2,9.7,0.0,0.0};
    double row4[5] = {6.0,0.0,7.2,8.1,0.0};
    SM s(5,5);
    s.insert(row0,0);
    s.insert(row1,1);
    s.insert(row2,2);
    s.insert(row3,3);
    s.insert(row4,4);
    cout<<"s1 (Dense): "<<endl;
    s.printDense();
    cout<<endl;
    cout<<"s1 (Sparse): "<<endl;
    s.printSparse();
    cout<<endl;
    cout<<endl;
    double r0[5] = {0.0,0.0,2.1,0.0,0.0};
    double r1[5] = {0.0,8.0,0.0,2.5,0.0};
    //double r2[5] = {0.0,4.2,0.0,1.1,0.0};
    double r4[5] = {6.0,0.0,7.2,8.1,0.0};
    SM s2(5,5);
    s2.insert(r0,0);
    s2.insert(r1,1);
    //s2.insert(r2,2);
    s2.insert(r4,4);
    cout<<"s2 (Dense): "<<endl;
    s2.printDense();
    cout<<endl;
    cout<<"s2 (Sparse): "<<endl;
    s2.printSparse();
      
    cout<<endl;
    cout<<endl;
   
    SM a(5,5);
    a = s.add(s2);
    cout<<"Addition(Dense): "<<endl;
    a.printDense();
    cout<<endl;
    cout<<"Addition(Sparse): "<<endl;
    a.printSparse();
   
  SM t(5,5);
    t = s.transpose();
    cout<<"Transposed s1(Dense): "<<endl;
    t.printDense();
    cout<<endl;
    cout<<"Transposed s1(Sparse): "<<endl;
    t.printSparse();
    cout<<endl;
    ///
   
    cout<<"s1 (Dense): "<<endl;
    s.printDense();
    cout<<endl;
    ///
   
    t = s2.transpose();
    cout<<"Transposed s2(Dense): "<<endl;
    t.printDense();
    cout<<endl;
    cout<<"Transposed s2(Sparse): "<<endl;
    t.printSparse();
    cout<<endl;
    
    SM m(5,5);
    m = s.multiply(s2);
    cout<<"Multiplication (Dense): "<<endl;
    m.printDense();
    cout<<endl;
    cout<<"Multiplication (Sparse): "<<endl;
    m.printSparse();
    
 
    
    
    
    SM w(7,7);
    SM x(7,7);
    SM y(7,7);
    SM z(7,7);
    double _r0[7] = {0,5.2,0,0,0,0,0};
    double _r1[7] = {0,0,0,0,12.1,0,0};
    double _r2[7] = {0,0,5.7,0,0,0,0};
    double _r5[7] = {0,0,0,7.2,0,0,0};
    double _r6[7] = {0,0,0,0,0,0,2.5};
    w.insert(_r0,0);
    w.insert(_r1,1);
    w.insert(_r2,2);
    w.insert(_r5,5);
    w.insert(_r6,6);
    
    double r_1[7] = {2.1,0,0,0,0,0,0};
    double r_2[7] = {0,0,9.4,0,0,0,0};
    double r_3[7] = {2.2,0,0,0,0,8.3,0};
    double r_6[7] = {0,0,0,0,0,0,7.0};
    x.insert(r_1,1);
    x.insert(r_2,2);
    x.insert(r_3,3);
    x.insert(r_6,6);
    
    cout<<"a (Dense): "<<endl;
    w.printDense();
    cout<<"a (Sparse): "<<endl;
    w.printSparse();
    cout<<"b (Dense): "<<endl;
    x.printDense();
    cout<<"b.T (Dense):"<<endl;
    y = x.transpose();
    y.printDense();
    cout<<"a.T (Dense):"<<endl;
    y=w.transpose();
    y.printDense();
    cout<<"a.T (Sparse):"<<endl;
    y.printSparse();
    
    z = w.multiply(x);
    cout<<"axb (Dense):"<<endl;
    z.printDense();
    cout<<"axb (Sparse):"<<endl;
    z.printSparse();

    return 0;
}

