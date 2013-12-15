using namespace std;

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

