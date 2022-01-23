#ifndef __PTFGAINANALYSIS__
#define __PTFGAINANALYSIS__

#include "PTFAnalysis.hpp"
#include "wrapper.hpp"
#include "ScanPoint.hpp"
#include "WaveformFitResult.hpp"
#include "TH2D.h"

using namespace std;

class PTFGainAnalysis {
  // private:
  // void  FitWaveform( int wavenum, int nwaves );
  // bool  HasWaveform();
  // WaveformFitResult * fitresult{nullptr};
public:
   PTFGainAnalysis(){}; // default constructor
  PTFGainAnalysis( TFile *outfile, PTFAnalysis *ptfanalysis, PTF::Wrapper & ptf);
  //  double Function(double x);
  // double numerical_integrate(double min, double max);
private:
  TH1D *Gain_hist{nullptr};
  TH1D *Sum{nullptr};
  TH2D *Gain_2Dhist{nullptr};


};

#endif // __PTFGAINANALYSIS__
