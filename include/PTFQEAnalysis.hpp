#ifndef __PTFQEANALYSIS__
#define __PTFQEANALYSIS__

#include "TFile.h"
#include "TH2D.h"

#include "PTFAnalysis.hpp"

using namespace std;

class PTFQEAnalysis {
public:
  PTFQEAnalysis(){}; // default constructor
  PTFQEAnalysis( TFile *outfile, PTFAnalysis *ptfanalysis);
  ~PTFQEAnalysis(){ delete d_efficiency; }

private:
  TH2D *d_efficiency{nullptr};

};

#endif // __PTFQEANALYSIS__
