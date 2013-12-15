using namespace std;

#include <vector>
#include <iostream>
#include <fstream>

#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_randhist.h>
#include <gsl/gsl_sf_exp.h>
#include <gsl/gsl_sf_log.h>

// Type 2 functional response
// a: attack rate
// h: handling time
// N: population size
double TypeII(double a, double h, double N)
{
   double Numerator = a * N;
   double Denominator = 1.0 + a * h * N;
   return Numerator/Denominator;
}

// Class SPECIES
class SPECIES
{
   public:
      double N;
      double x;
      double dN;
      double nmut;
      double birth;
}

// Gaussian with constant area under curve
// x:  Position 1
// y:  Position 2
// xi: Breadth
double Gaussian(double x, double y, double xi)
{
   const double mismatch = ((x - y) * (x - y)) / xi;
   const double correct  = 1.0 / (xi * sqrt(M_PI * 2.0));
   return correct * gsl_sf_exp(-0.5*(mismatch * mismatch));
}
