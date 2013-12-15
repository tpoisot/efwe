using namespace std;

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
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
      int nmut;
      int birth;
};

// Class PARAMS
class PARAMS
{
   public:
      double breadth_comp;
      double breadth_pred;
      double breadth_grow;
      double r0;
      double q0;
      double a0;
      double h0;
      double d0;
      int MAX_PRED;
      int MAX_PREY;
      int SIM_STEPS;
      int SUB_STEPS;
      int REC_EACH;
};

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

int main(int argc, char *argv[])
{
   // Initialize GSL random number generators
   time_t begin_time = time(0);
   gsl_rng *rng = gsl_rng_alloc(gsl_rng_taus2);
   gsl_rng_set(rng, begin_time);
   // TODO get options
   // TODO write options as JSON
   // Initialize list of species
   vector<SPECIES> PREYS;
   PREYS.push_back({10.0, 0.0, 0.0, 0, 0});
   vector<SPECIES> PREDS;
   PREDS.push_back({3.0, 0.0, 0.0, 0, 0});
   // TODO population dynamics loop
   // TODO mutations
   // TODO extinctions
   // TODO record
}
