// pointers are declared as function parameters, not local variables. 
// no assignment at all before the loop
void foo(double* e, double* p, double* dedt, double * dpdt,
     double ccon, double tx, double* rho, int n)
{
  for ( unsigned int i = 0 ; i < n ; i++ ) {

    e[i]      *= ccon * tx          ;
    p[i]      *= ccon * tx * rho[i] ;
    dedt[i]   *= ccon               ;
    dpdt[i]   *= ccon *      rho[i] ;
  }
}

// still a negative case: all things are declared locally, but no pattern like var = base + offset;
void foo()
{
  double* e; 
  double* p; 
  double* dedt; 
  double * dpdt;
  double ccon; 
  double tx; 
  double* rho; int n;
  for ( unsigned int i = 0 ; i < n ; i++ ) {

    e[i]      *= ccon * tx          ;
    p[i]      *= ccon * tx * rho[i] ;
    dedt[i]   *= ccon               ;
    dpdt[i]   *= ccon *      rho[i] ;
  }
}
