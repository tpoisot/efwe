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
      unsigned MAX_PRED;
      unsigned MAX_PREY;
      unsigned SIM_STEPS;
      unsigned SUB_STEPS;
      unsigned REC_EACH;
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
   // Default options
   PARAMS P = {0.5, 0.1, 0.1, 1.01, 1.1, 0.15, 0.90, 0.07, 100, 100, 10, 50, 10};
   // TODO get options
   // TODO write options as JSON
   // Initialize list of species
   vector<SPECIES> PREYS;
   PREYS.push_back({10.0, 0.0, 0.0, 0, 0});
   vector<SPECIES> PREDS;
   PREDS.push_back({3.0, 0.0, 0.0, 0, 0});
   // TODO population dynamics loop
   unsigned mainStep = 0;
   for(mainStep = 0 ; mainStep < P.SIM_STEPS ; ++mainStep)
   {
      // Enter sub-loop
      unsigned subStep = 0;
      for(subStep = 0 ; subStep < P.SUB_STEPS ; ++subStep)
      {
         // Loop through the PREDS
         unsigned pred_it;
         for(pred_it = 0 ; pred_it < PREDS.size() ; ++pred_it)
         {
            PREDS[pred_it].dN -= P.d0 * PREDS[pred_it].N;
         }
         // Loop through the PREYS
         unsigned prey_it;
         for(prey_it = 0 ; prey_it < PREYS.size() ; ++prey_it)
         {
            // Growth rate
            PREYS[prey_it].dN += P.r0 * Gaussian(0.0, PREYS[prey_it].x, P.breadth_grow);
            // Competition
            unsigned second_prey_it;
            for(second_prey_it = 0 ; second_prey_it < PREYS.size() ; ++second_prey_it)
            {
               PREYS[prey_it].dN -= (PREYS[second_prey_it].N * Gaussian(PREYS[prey_it].x, PREYS[second_prey_it].x, P.breadth_comp) * P.q0);
            }
            // Predation
            for(pred_it = 0 ; pred_it < PREDS.size() ; ++pred_it)
            {
               double PredStr = TypeII(P.a0*Gaussian(PREYS[prey_it].x, PREDS[pred_it].x, P.breadth_pred), P.h0, PREYS[prey_it].N);
               PREDS[pred_it].dN += PredStr*PREYS[prey_it].N;
               PREYS[prey_it].dN -= PredStr*PREDS[pred_it].N;
            }
         }
         // Update population sizes for preys
         for(prey_it = 0 ; prey_it < PREYS.size() ; ++prey_it)
         {
            PREYS[prey_it].N += ((PREYS[prey_it].N * PREYS[prey_it].dN) * (1.0 / P.SUB_STEPS));
            PREYS[prey_it].dN = 0.0;
         }
         // Updated population sizes for predators
         for(pred_it = 0 ; pred_it < PREDS.size() ; ++pred_it)
         {
            PREDS[pred_it].N += ((PREDS[pred_it].N * PREDS[pred_it].dN) * (1.0 / P.SUB_STEPS));
            PREDS[pred_it].dN = 0.0;
         }
      }
      // TODO mutations
      // TODO extinctions
      // TODO record
   }
}
