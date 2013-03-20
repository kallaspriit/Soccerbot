#ifndef MATRIX_H
#define MATRIX_H
//
//Version 2.15 January 2012. 
//
//matrix.h contains a library of matrix creation and manipulation 
//routines for any shape matrix, and a suite of standard solvers
//based on a pseudo-inverse (a.k.a. truncated SVD) treatment when needed.
//matrix.h also contains slightly modified versions of decomposition routines provided
//by Mathworks, NIST, and the Template Numerical Toolkit web site.
//
//rejtrix.h contains a suite of manually- and automatically-regularized solvers
//plus code to support solution of three-way systems:
//least squares          (A*x=b); 
//equality constraints   (E*x==f); and
//inequality constraints (G*x>=h). 
//
//sparse.h contains the Node and Sparse classes that support creation and manipulation
//of sparse matrices, and the code for the GMRES solver.  Sparse.h requires matrix.h.
//Recently we added eigenvalue/eigenvector routines to sparse.h.
//
//All these packages are freely distributed from the author's web site at
//www.rejonesconsulting.com.
//See that site for usage instructions and tutorials.
//
//Class diagram...
//
//    [-----Matrix------]  [---Diagonal---]     (base classes)
//       ^      ^    ^          ^    ^
//       |      .    |          .    .           dashed lines mean "derives from"
//       |      .    |          .    .           dotted lines mean "references" 
//       |      .    |          .    .
//    [-Row-]   .  [---Vector-----]  .          (limited to one row or column)
//              .         ^          .
//              .         .          .
//    [-Node-]  .         .          .          (a Node is a row of a Sparse matrix)
//       ^      .         .          .
//       .      .         .          .
//    [-----------------Sparse-----------]      (sparse matrix class)
//                        ^ 
//                        .
//                        .
//         [---------SparseEig--------]         (for sparse eigenvalues & vectors)
//
//
//
//------Licensing Notice---------------------------------------
//Copyright (c) 2006, 2008, 2011 Rondall E. Jones, Albuquerque NM, USA.
//As of January 1, 2012, we grant completely free usage of all these
//software packages to any users and all purposes.
//(The included NIST decomposition routines are unlimited release.)
//We do ask that you not remove the author's identification and notes above,
//and when appropriate, reference the author's web site in publications.
//
//------Warning Notice-----------------------------------------
//Warning: This software is provided AS IS!  
//THERE IS NO WARRANTY OF ANY SORT WHATSOEVER!
//The algorithms contained herein are partly heuristic and/or experimental.
//Inevitably this package contains bugs/errors which are as yet unknown,
//as well as known deficiencies.
//The user must determine if the results of any calculation using this package
//are appropriate for the user's purposes.
//
//------Technical note on coding style------------------------
//In matrix.h and rejtrix.h we have emphasized code re-use over efficiency 
//in lower level utility routines.  
//This is an attempt to minimize errors,
//maximize the utility of code testing, 
//and improve reliability of the code.
//  
//Specifically, Row and Vector inherit many operations from Matrix,
//at less than optimum efficiency, in order to reuse that one implementation.
//Or, see how Matrix::operator+() uses Matrix::operator+=()
//or how Matrix::prepend_columns() leverages off Matrix::append_columns()
//to make a trivial implementation of prepend_columns at a cost in efficiency.
//
//In sparse.h, efficiency is paramount and key algorithms are 
//coded with that in mind.
//
//Efficiency of the higher level algorithm usually far
//exceeds in impact any such minor issue of efficiency in low level routines.

//amatrix.h-----------------------------------------------------------
#include <stdlib.h>
#include <math.h>

//Recent compilers may need the .h removed in the following three includes.
//The form with .h is deprecated, but needed by older compilers.

#include <iostream>
#include <fstream> 
#include <iomanip>
#include <string.h>

typedef int I;
typedef double T;
static const T BIG  =1.0E30;
static const T BIG2 =2.0E30;

inline T absf(T x) { if (x<0.0) return (-x); else return x; }
inline I absf(I x) { if (x<0  ) return (-x); else return x; }

inline I minf(I x,I y) { return x<y ? x : y; }
inline T minf(T x,T y) { return x<y ? x : y; }

inline I maxf(I x,I y) { return x>y ? x : y; }
inline T maxf(T x,T y) { return x>y ? x : y; }

inline I minf(I x,I y,I z) { I a=x; if (y<a) a=y; if (z<a) a=z; return a; }
inline T minf(T x,T y,T z) { T a=x; if (y<a) a=y; if (z<a) a=z; return a; }

inline I maxf(I x,I y,I z) { I a=x; if (y>a) a=y; if (z>a) a=z; return a; }
inline T maxf(T x,T y,T z) { T a=x; if (y>a) a=y; if (z>a) a=z; return a; }

inline I minf(I x,I y,I z,I p) { I a=x; if (y<a) a=y; if (z<a) a=z; 
                                               if (p<a) a=p; return a; }

inline T limitabsf(T x, T limit)
  { T a=absf(limit); if (x>a) return a; if (x<-a) return -a; return x; }

inline T squaref(T x) { return x*x; }
inline T pif() { return 3.14159265358979; }
inline T signumf(T x) { if (x>=0.0) return 1.0; else return -1.0; }

inline void prompt() { std::cout << "Ready? "; char str[99]; std::cin.getline(str,99); }
inline void separate()  { for (int z=0; z<10; z++) std::cout << "-------"; std::cout << std::endl; }
inline void separateX() { for (int z=0; z<10; z++) std::cout << "---X---"; std::cout << std::endl; }
inline void skipline() { char str[99]; std::cin.getline(str,99); }

inline bool ask_the_user(const char * msg)
{ 
  std::cout << msg << std::endl;
  char answer; std::cin >> answer;
  skipline();
  if (answer=='y' || answer=='Y') return true; 
  else return false;
}

//******************************************************************
//The Matrix class.
//******************************************************************

class Matrix 
{ 
  private:
    I m_;
    I n_;
    T* data_;
    T** v_;  

    friend class Row;
    friend class Vector;
    
    //internal use only
    void checkdim(I m, I n) { if (m<0 || n<0) Matrix::xerror(4,"Matrix::checkdim"); return; }
    void setup2(I m, I n);
    void setup() { setup2(m_,n_); }

    void normalize_them(Matrix &B, Matrix &E, I i, T rownorm);

  public:

    //static utilities

    //error reporting routine
    static void xerror(I m, const char* who)
      {   
        std::cerr << "Error in routine " << who << std::endl;
             if (m==1)  std::cerr << "Reference Out-of-Bounds!" << std::endl; 
        else if (m==2)  std::cerr << "Dimensions do not match!" << std::endl;
        else if (m==3)  std::cerr << "Operation on an empty matrix!" << std::endl;
        else if (m==4)  std::cerr << "Invalid dimensions!" << std::endl;
        else if (m==5)  std::cerr << "Taking vector norm of non-vector! Use matrix norm?" << std::endl;
        else if (m==6)  std::cerr << "Divide by zero!" << std::endl;
        else if (m==7)  std::cerr << "Invalid input parameter" << std::endl;
        else if (m==8)  std::cerr << "Algorithm error" << std::endl;
        else if (m==9)  std::cerr << "Prohibited operation for Rows and Vectors!" << std::endl;
        else if (m==10) std::cerr << "Given row is too long for matrix!" << std::endl;
        else if (m==11) std::cerr << "Invalid argument vector!" << std::endl;
        else if (m==12) std::cerr << "Problem is too large for current limits!" << std::endl;
        else            std::cerr << "Miscellaneous error: " << m << std::endl;
        prompt();
        exit(1);
      }

    //find the value of roundoff versus 1.0
    static T roundoff()
      {
        static bool ok=false;
        static T round=1.0e-9;
        if (ok) return round;
        
        int j=0;
        T one=1.0;
        T two=1.0;
        T three;
        for (int i=0; i<=100; i++)
          { j=i; three=one+two; if (three==one) break; two/=2.0; }
        round = two*2.0; //went one too far
        if (j>=100) round = 1.0E-9;
        ok = true;
        return round;
      }

    //generate a random value between zero and one
    static T myrandom(I reset=0) 
      {
        static int seed=13*13*13;
        if (reset!=0) seed=reset%16384;
        seed=seed*13;       //scramble
        seed=seed%16384;    //chop to 16 bits
        return T(seed)/16384.0;
      }

    //generate an approximately Gaussian random value, mean 0, sigma 1
    static T mygauss() 
      {
         T sum=0.0;
         //for (int i=0; i<9; i++)  sum += (myrandom()-0.5)*2.0*1.732;
         //rms is about 7% too large often... why???  so reduce it...
         for (int i=0; i<9; i++)  sum += (myrandom()-0.5)*2.0*1.62;
         return sum/3.0; 
      }

    //count the number of arguments less than BIG
    static I countargs(T t1, T t2, T t3, T t4, T t5, T t6, T t7, T t8, T t9, T t10)
      { 
        if  (t2>BIG) return 1;
        if  (t3>BIG) return 2;
        if  (t4>BIG) return 3;
        if  (t5>BIG) return 4;
        if  (t6>BIG) return 5;
        if  (t7>BIG) return 6;
        if  (t8>BIG) return 7;
        if  (t9>BIG) return 8;
        if (t10>BIG) return 9; 
        return 10;       
      }

    //constructors---------------------------------------------------

    //default constructor: 0 by 0 matrix
    Matrix () : m_(0), n_(0) {}
      
    //construct an m by n matrix (zero filled)
    explicit Matrix (int m, int n) { setup2(m,n); }
 
    //construct an m by n matrix filled with the value x
    explicit Matrix (int m, int n, T x); 

    //construct an m by n matrix; copy the contents from the array a[m][ndim] 
    Matrix (int m, int n, int ndim, const T *a);
    
    //copy constructor
    Matrix (const Matrix &A); 

    //destructors----------------------------------------------------

    //delete all data and set size to 0 by 0
    void clear()
      {
         if (m_>0 && n_>0) { delete [] data_; delete [] v_; }
         m_ = 0;
         n_ = 0; 
      }

     ~Matrix () { clear(); } 

    //assignment-----------------------------------------------------

    //supports for example B = 3.14;
    Matrix operator=(const T x);

    //supports for example B = A;
    Matrix operator=(const Matrix &A); 

    //accessors------------------------------------------------------

    //get the row dimension
    int dim1() const { return m_; }

    //get the column dimension
    int dim2() const { return n_; }

    //get the smaller dimension
    int dmin() const { return m_ < n_ ? m_ : n_ ; }

    //get the larger dimension
    int dmax() const { return m_ > n_ ? m_ : n_ ; }

    //get the 2-D size
    int dsize() const { return m_ * n_ ; }

    //get the 2-dimensional array representing the matrix
    void get_array(T* A) { int sz=m_*n_; for (int i=0; i<sz; i++) A[i]=data_[i]; }

    //see if the two matrices have matching dimensions ... A.matching(B)
    bool matches(const Matrix &B) const
      { return m_==B.m_ && n_==B.n_; }    

    //index----------------------------------------------------------

    inline T* operator[] (I i) 
      { 
        if (i < 0 || i >= m_) { Matrix::xerror(1,"operator[]"); }; //DELETE for no debug
        return v_[i]; 
      }

    inline const T* operator[] (I i) const
      { 
        if (i < 0 || i >= m_) { Matrix::xerror(1,"operator[]"); }; //DELETE for no debug
        return v_[i]; 
      }

    //Alternative index form... A(i,j) rather than A[i][j].
    //This checks both the indices for proper range.
    //(The A[i][j] form can only check the first index.)

    T& operator() (I i, I j)
      { 
        if (i < 0 || i >= m_ || j < 0 || j >= n_) Matrix::xerror(1,"operator(,)"); 
        return v_[i][j]; 
      }

    const T& operator() (I i, I j) const
      { 
        if (i < 0 || i >= m_ || j < 0 || j >= n_) Matrix::xerror(1,"operator(,)"); 
        return v_[i][j]; 
      }

    //equivalence operations-----------------------------------------

    //supports A==B
    bool operator==(const Matrix &B) const;

    //supports A!=B
    bool operator!=(const Matrix &B) const;

    //approximately equal test.
    //Two values are considered approx. equal if they differ by
    //less than tolerance.
    //So this is an absolute test, not relative.
    bool approximate(const Matrix &B, T tolerance) const;

    //element-wise Matrix operations---------------------------------

    //these operations support for example, A+=2.0, A-=2.0, A*=2.0, A/=2.0  
    Matrix operator+= (T x) 
      {int sz=m_*n_; for (int i=0; i<sz; i++) data_[i]+=x; return *this; }
    Matrix operator-= (T x) 
      {int sz=m_*n_; for (int i=0; i<sz; i++) data_[i]-=x; return *this; }
    Matrix operator*= (T x) 
      {int sz=m_*n_; for (int i=0; i<sz; i++) data_[i]*=x; return *this; }
    Matrix operator/= (T x) 
      {int sz=m_*n_; for (int i=0; i<sz; i++) data_[i]/=x; return *this; }

    //these operations support for example, A+2.0, A-2.0, A*2.0, A/2.0  
    Matrix operator+  (T x) const { Matrix C(*this); C+=x; return C; }
    Matrix operator-  (T x) const { Matrix C(*this); C-=x; return C; }
    Matrix operator*  (T x) const { Matrix C(*this); C*=x; return C; }
    Matrix operator/  (T x) const { Matrix C(*this); C/=x; return C; }
  
    //unary minus--- for B = -A; for example
    Matrix operator- ();

    //these operations support A+=B, A-=B, A*=B, A/=B, which are all element-wise.
    //A and B must have exactly the same shape.
    Matrix operator+= (const Matrix &B);
    Matrix operator-= (const Matrix &B);  
    Matrix operator*= (const Matrix &B);   

    //these operations support A+B  and A-B, which are all element-wise.
    //A and B must have exactly the same shape.
    Matrix operator+ (const Matrix &B) const;
    Matrix operator- (const Matrix &B) const;

    //the following provides the matrix product A*B, 
    //where A's second dimension must equal B's first dimension
    Matrix operator* (const Matrix &B) const;

    //--------

    //the following scales each row of the matrix to unit norm,
    //and carries along B and E. (B usually the RHS; E the error est.)
    void normalize_rows(Matrix &B, Matrix &E);

    //the following scales each row of the matrix to unit norm,
    //and carries along B
    void normalize_rows(Matrix &B) 
      { Matrix E(m_,1); normalize_rows(B,E); } 

    //the following scales each row of the matrix to unit norm
    void normalize_rows() 
      { Matrix B(m_,1); Matrix E(m_,1); normalize_rows(B,E); } 

    //--------

    //the following scales each row of the matrix to max element of 1.0
    //and carries along B and E. (B usually the RHS; E the error est.)
    void normalize_rows_max1(Matrix &B, Matrix &E);

    //the following scales each row of the matrix to max element of 1.0,
    //and carries along B
    void normalize_rows_max1(Matrix &B)
      { Matrix E(m_,1); normalize_rows_max1(B,E); } 

    //the following scales each row of the matrix to max element of 1.0
    void normalize_rows_max1() 
      { Matrix B(m_,1); Matrix E(m_,1); normalize_rows_max1(B,E); } 

    //element-wise operations----------------------------------------

    //replaces each element with its absolute value
    void mabs();

    //replaces each element with the square root of its absolute value
    void msqrt();

    //replaces each element with its square
    void msquare();

    //Replaces each element with the base 10 log of its absolute value
    //log(A.maxabs())-30.0 is used for zero elements.
    void mlog10();

    //Replaces each element a with 10^a.
    //That is, with the antilog10 of a.
    void mpow10();

    //makes each element at least x
    void at_least(T x);

    //truncates each number to n digits
    void keep_digits(I n);

    //In the following integer utilities, the values are computed as integers,
    //but the TYPE remains floating point.

    //truncates each element to integer                  -2.6 --> -2.0    2.6 --> 2.0
    void trunc();

    //rounds each element to the nearest integer         -2.6 --> -3.0    2.6 --> 3.0
    void round(); 

    //rounds each element toward +infinity               -2.6 --> -2.0    2.6 --> 3.0
    void ceil();    

    //rounds each element to +1 or -1; zero goes to +1   -2.6 --> -1.0    2.6 --> 1.0
    void signum();      

    //rounds each element to +1 or -1; zero stays as 0   -2.6 --> -1.0    2.6 --> 1.0
    void trinity();

    //convert each column to percentages based on the sum of the
    //(absolute values of the) elements of the column
    void to_percentages();

    //min/max/sum functions------------------------------------------------
      
    //returns the element which is algebraically largest
    T maxval() const; 

    //returns the element which is algebraically smallest
    T minval() const;

    //returns the (absolute value of the) element which is largest in absolute value
    T maxabs() const; 

    //returns the (absolute value of the) element which is smallest in absolute value
    T minabs() const;

    //returns the smallest element greater than zero
    T minpos() const;

    //returns (imax,jmax) position of element with largest abs value
    void ijmaxabs(I &imax, I &jmax) const;

    //returns the sum of the elements
    T sum() const;

    //returns the sum of the absolute values of the elements
    T sumabs() const;

    //returns the average of the values of the elements
    T average() const { return sum()/(m_*n_); }

    //returns the average of the absolute values of the elements
    T averageabs() const { return sumabs()/(m_*n_); }

    //find a neglible value for *this
    T epsilon() const { return maxabs()*8.0*Matrix::roundoff(); }

    //count the number of non-zero elements
    I num_non_zero() const ;

    //cout the number of non-negative elements
    I num_non_negative() const;

    //1-D norms------------------------------------------------------
    //These methods require that the object be 1-dimensional.
    //That is, a Row, a Vector, or a Matrix of size 1 by n, or m by 1.
    //For a row v, norm(v) = sqrt(v * v').

    //returns square(norm(*this)) 
    T norm2() const; 
 
    //returns norm(*this)
    T norm () const { return sqrt(norm2()); }

    //returns root-mean-square(*this)
    T rms  () const { return sqrt(norm2()/T(maxf(m_,n_,1))); }

    //returns the population standard deviation
    T popstddev() const { T a=average(); Matrix d=*this - a; return d.rms(); }

    //returns the sample standard deviation
    T samstddev() const 
      { if (m_<2) return 0.0; return popstddev()*sqrt(T(n_)/T(n_-1)); }

    //norms of the elements of the matrix as if it were 1-D ---------
    //These methods NO NOT require that the object be 1-dimensional.

    //returns the sum of the squares of all the elements
    T norm2_as_vector() const;   

    //returns the square root of the sum of the squares of the elements
    T norm_as_vector() const { return sqrt(norm2_as_vector()); }

    //Frobenius norm is another name for our norm_as_vector 
    T Frobenius() const { return sqrt(norm2_as_vector()); }

    //returns root-mean-square of the matrix elements
    T rms_as_vector() const { return sqrt(norm2_as_vector()/T(m_*n_)); } 
    
    //row/column operations-------------------------------------------

    //returns the dot product of two rows of *this
    T rowdot(I i,I k) const;

    //dot product of two equal-length 1-dimensional matrices.
    //dot tolerates any two equal length 1-D matrices: row.row, row.col, col.col
    T dot (Matrix &B) const;

    //returns a row of *this
    Matrix get_row (I i) const;

    //returns a column of *this
    Matrix get_column (I j) const;

    //sets all values in row i to val
    void set_row (I i, T val);    

    //sets all values in column j to val
    void set_column (I j, T val);

    //sets row i from a row matrix.  The row sizes must match.
    void set_row (I i, Matrix A);    

    //sets column i from a column matrix.  The columns sizes must match.
    void set_column (I j, Matrix A);

    //sets all values in row i to zero
    void set_row_zero(I i);

    //sets all values in column j to zero
    void set_column_zero(I j);

    //matrix shape operations----------------------------------------

    //transposes *this
    Matrix t();              

    //resize to smaller or larger
    //keeps upper left content as far as possible; fills with zero
    void resize(I m,I n);   

    //deletes row r; decreases matrix size!                     
    void del_row(I r);

    //deletes column c; decreases matrix size!      
    void del_column(I c);
    
    //add m new rows at the bottom of *this; zero filled
    void add_rows(I m) { resize(m_ + m, n_); }       

    //append Matrix B to bottom of *this
    void append_rows(const Matrix &B );

    //prepend the Matrix B to the top of *this
    void prepend_rows(const Matrix &B);
 
    //add n new columns at the right side of *this
    void add_columns(I n) { resize(m_, n_ + n); };

    //append Matrix B to right side of *this
    void append_columns(const Matrix &B ); 

    //prepend the Matrix B to the left of *this
    void prepend_columns(const Matrix &B);

    //common matrices------------------------------------------------

    void zeros();   //set *this to all zeros     
    void ones();    //set *this to all ones   
    void identity();//set *this to identity matrix     
    void iota();    //set *this[i][j] = i + j + 1.  In a row that's 1, 2, 3, ...
    void iotazero();//set *this[i][j] = i + j.  In a row that's 0, 1, 2, ...
    void random();  //set *this to random values in (0,1)
    void gauss();   //set *this to random Gaussian, mean 0, standard deviation 1  
    void hilbert(); //set *this[i][j] = 1/(i+j+1)
    void heat();    //set to an example heat equation style kernel
    void laplace(); //set to an example inverse Laplace style kernel
    void cusp();    //set to one positive cusp of a sine function

    //following static methods create a Matrix of the given size and
    //call the appropriate routine above to define the elements of the Matrix.

    static Matrix zeros   (I m, I n) { Matrix A(m,n); A.zeros();    return A; }
    static Matrix ones    (I m, I n) { Matrix A(m,n); A.ones();     return A; }
    static Matrix identity(I m, I n) { Matrix A(m,n); A.identity(); return A; }
    static Matrix iota    (I m, I n) { Matrix A(m,n); A.iota();     return A; }
    static Matrix iotazero(I m, I n) { Matrix A(m,n); A.iotazero(); return A; }
    static Matrix random  (I m, I n) { Matrix A(m,n); A.random();   return A; }
    static Matrix gauss   (I m, I n) { Matrix A(m,n); A.gauss();    return A; }
    static Matrix hilbert (I m, I n) { Matrix A(m,n); A.hilbert();  return A; }
    static Matrix heat    (I m, I n) { Matrix A(m,n); A.heat();     return A; }
    static Matrix laplace (I m, I n) { Matrix A(m,n); A.laplace();  return A; }

    //displays-------------------------------------------------------

    //print a rectangular layout with default width
    void print() const;       
 
    //print by row in narrow (less than 80 column) format
    void print_by_row() const;      

    //print by column in narrow format
    void print_by_column() const;

    //print a glimpse of the matrix, in an 80-column wide format        
    void printA() const;

    //print a glimpse of the matrix and the right hand side vector, b, 
    //in an 80-column wide format
    void printAb(const Matrix &b) const;

    //print a glimpse of the matrix and the right hand side vector, b, 
    //and an error estimate vector e, in an 80-column wide format
    void printAbe(const Matrix &b, const Matrix &e) const;

    //print a glimpse of the matrix, the solution, x, and the right hand side vector, b,
    //in an 80-column wide format.
    //x and b must be single column or single row matrices, or a Row or Vector.
    //By default, up to 25 rows will be printed.
    void printAxb(const Matrix &x, const Matrix &b, I maxrows=25) const;
 
    //Compute for each element an Order of Magnitude of 1 to 16 or so.
    //This arrangement follows the way stars are classified: 
    //magnitude 1 is from largest to about 1/10th that;
    //magnitude 2 is from 1/10th of the largest to about 1/100th of the largest; 
    //etc
    Matrix compute_star_magnitudes() const;

    //show each element as Order of Magnitude 1 to 9 or blank for smaller than 9th magnitude
    void print_star_magnitudes() const;

}; //end class Matrix

#endif
