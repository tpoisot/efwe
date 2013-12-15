using namespace std;

#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf_exp.h>
#include <gsl/gsl_sf_log.h>

#include <unistd.h>
#include <getopt.h>

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
      bool extinct;
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
      double RARITY;
      double MUTSPR;
};

// Functions for extinctions
bool toDelete(const SPECIES & S)
{
   return S.extinct;
}

void extinct(vector<SPECIES>* S)
{
   (*S).erase(
         remove_if((*S).begin(), (*S).end(), toDelete),
         (*S).end());
}

void speciate(vector<SPECIES>* S, PARAMS* P, gsl_rng* rng, int stp)
{
   int idx = gsl_rng_uniform_int(rng, (*S).size());
   double ntrait = gsl_ran_gaussian(rng, P->MUTSPR) + (*S)[idx].x;
   const SPECIES mutant = {P->RARITY, ntrait, 0.0, 0, stp, false};
   (*S).push_back(mutant);
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

int main(int argc, char *argv[])
{
   time_t begin_time = time(0);
   gsl_rng *rng = gsl_rng_alloc(gsl_rng_taus2);
   gsl_rng_set(rng, begin_time);
   // Default options
   string ID = "0";
   PARAMS P = {0.5, 0.3, 0.5, 1.21, 1.1, 0.15, 0.90, 0.05, 90, 90, 100, 50, 10, 0.001, 0.001};
   // Get options
   int opt = 0;
   static struct option long_options[] = {
      {"id",  optional_argument, 0, 'i'},
      {"bC",  optional_argument, 0, 'c'},
      {"bP",  optional_argument, 0, 'p'},
      {"bG",  optional_argument, 0, 'g'},
      {"a0",  optional_argument, 0, 'a'},
      {"r0",  optional_argument, 0, 'r'},
      {"d0",  optional_argument, 0, 'd'},
      {"q0",  optional_argument, 0, 'q'},
      {"time",  optional_argument, 0, 't'},
      {0, 0, 0, 0}
   };
   int long_index = 0;
   while ((opt = getopt_long(argc, argv, "", long_options, &long_index)) != -1)
   {
      switch(opt)
      {
         case 'i':
            ID = optarg;
            break;
         case 'c':
            P.breadth_comp = atof(optarg);
            break;
         case 'p':
            P.breadth_pred = atof(optarg);
            break;
         case 'g':
            P.breadth_grow = atof(optarg);
            break;
         case 'a':
            P.a0 = atof(optarg);
            break;
         case 'r':
            P.r0 = atof(optarg);
            break;
         case 'd':
            P.d0 = atof(optarg);
            break;
         case 'q':
            P.q0 = atof(optarg);
            break;
         case 't':
            P.SIM_STEPS = atoi(optarg);
            break;
      }
   }
   // write options as JSON
   string jsfile = "ef" + ID + ".json";
   ofstream json;
   json.open(jsfile);
   json << "{\"params\": {"
      << "\"id\":  1,"
      << "\"breadth_comp\":  " << P.breadth_comp  << ","
      << "\"breadth_grow\":  " << P.breadth_grow  << ","
      << "\"breadth_pred\":  " << P.breadth_pred  << ","
      << "\"r0\":  " << P.r0  << ","
      << "\"a0\":  " << P.a0  << ","
      << "\"q0\":  " << P.q0  << ","
      << "\"d0\":  " << P.d0  << ","
      << "\"mut\":  " << P.MUTSPR
      << "},"
      << "\"steps\" : [";
   // Initialize list of species
   vector<SPECIES> PREYS;
   PREYS.push_back({5.0, 0.0, 0.0, 0, 0, false});
   vector<SPECIES> PREDS;
   PREDS.push_back({3.0, 0.0, 0.0, 0, 0, false});
   // Population dynamics loop
   unsigned mainStep = 0;
   for(mainStep = 0 ; mainStep < P.SIM_STEPS ; ++mainStep)
   {
      unsigned pred_it, prey_it, second_prey_it;
      // Enter sub-loop
      unsigned subStep = 0;
      for(subStep = 0 ; subStep < P.SUB_STEPS ; ++subStep)
      {
         // Loop through the PREDS
         for(pred_it = 0 ; pred_it < PREDS.size() ; ++pred_it)
         {
            PREDS[pred_it].dN -= P.d0 * PREDS[pred_it].N;
         }
         // Loop through the PREYS
         for(prey_it = 0 ; prey_it < PREYS.size() ; ++prey_it)
         {
            // Growth rate
            PREYS[prey_it].dN += P.r0 * Gaussian(0.0, PREYS[prey_it].x, P.breadth_grow);
            // Competition
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
            if(PREYS[prey_it].N < P.RARITY)
            {
               PREYS[prey_it].extinct = true;
            }
         }
         // Updated population sizes for predators
         for(pred_it = 0 ; pred_it < PREDS.size() ; ++pred_it)
         {
            PREDS[pred_it].N += ((PREDS[pred_it].N * PREDS[pred_it].dN) * (1.0 / P.SUB_STEPS));
            if(PREDS[pred_it].N < P.RARITY)
            {
               PREDS[pred_it].extinct = true;
            }
         }
      }
      double nPreys = 0;
      for(prey_it = 0 ; prey_it < PREYS.size() ; ++prey_it)
      {
         nPreys += PREYS[prey_it].N;
      }
      // Speciations
      if(PREYS.size() < P.MAX_PREY)
      {
         speciate(&PREYS, &P, rng, mainStep);
      }
      if(PREDS.size() < P.MAX_PRED)
      {
         speciate(&PREDS, &P, rng, mainStep);
      }
      // Extinctions
      extinct(&PREYS);
      extinct(&PREDS);
      // Print
      // Set derivatives to 0
      for(prey_it = 0 ; prey_it < PREYS.size() ; ++prey_it)
      {
         PREYS[prey_it].dN = 0.0;
      }
      for(pred_it = 0 ; pred_it < PREDS.size() ; ++pred_it)
      {
         PREDS[pred_it].dN = 0.0;
      }
      // record
      if(mainStep % P.REC_EACH == 0)
      {
         json << "{\"t\": " << mainStep + 1 << ",";
         json << "\"preys\": [";
         for(prey_it = 0 ; prey_it < PREYS.size() ; ++prey_it)
         {
            json << "{"
               << "\"x\": " << PREYS[prey_it].x << ","
               << "\"n\": " << PREYS[prey_it].N << ","
               << "\"b\": " << PREYS[prey_it].birth
               << "}";
            if(prey_it < (PREYS.size()-1))
            {
               json << ",";
            }
         }
         json << "], ";
         json << "\"preds\": [";
         for(pred_it = 0 ; pred_it < PREDS.size() ; ++pred_it)
         {
            json << "{"
               << "\"x\": " << PREDS[pred_it].x << ","
               << "\"n\": " << PREDS[pred_it].N << ","
               << "\"b\": " << PREDS[pred_it].birth
               << "}";
            if(pred_it < (PREDS.size()-1))
            {
               json << ",";
            }
         }
         json << "]";
         json << "}";
         if(mainStep < (P.SIM_STEPS - P.REC_EACH))
         {
            json << ",";
         }
      }
   }
   json << "]}";
   json.close();
}
