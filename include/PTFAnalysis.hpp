#ifndef __PTFANALYSIS__
#define __PTFANALYSIS__

#include "TFile.h"
#include "TF1.h"
#include "TH1D.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TMath.h"
#include <vector>


#include "wrapper.hpp"
#include "ScanPoint.hpp"
#include "WaveformFitResult.hpp"

/// This class takes a PTFWrapper reference, charge errorbar, and PMT channel as input
/// It then does main analysis to fill a TTree of WaveformFitResults
/// Has methods to later read back entries of the TTree
/// Keeps track of number of Scan Points, and locations used find entries in TTree
class PTFAnalysis {
public:
  PTFAnalysis( TFile * outfile, PTF::Wrapper & ptf, double errorbar, PTF::PMTChannel & channel, bool savewf=false );
  ~PTFAnalysis(){
    if ( fitresult ) delete fitresult;
  }

  // Access fit results
  const std::vector< ScanPoint > & get_scanpoints()             const { return scanpoints; };
  const unsigned                   get_nscanpoints()            const { return scanpoints.size(); }
  const unsigned long long         get_firstentry( int scanpt ) const { return scanpoints[ scanpt ].get_entry(); }
  const unsigned long long         get_nentries( int scanpt )   const { return scanpoints[ scanpt ].nentries(); }
  
  const WaveformFitResult &        get_fitresult( int scanpt, unsigned long long wavenum );

  // Post-fitresult analysis
  const std::vector< double > get_bins( char dim );
  
private:
  void  FitWaveform( int wavenum, int nwaves );
  bool  HasWaveform(); 
  static double my_gaussian(double *x, double *par);
  static bool comparison (double i, double j){ return (fabs( i-j ) < 1e-5); }

  std::vector< ScanPoint > scanpoints;
  TF1*  fmygauss{nullptr};  // gaussian function used to fit waveform
  TH1D* hwaveform{nullptr}; // current waveform
  WaveformFitResult * fitresult{nullptr};
  TTree* ptf_tree{nullptr};
  bool  save_waveforms{false};
  TDirectory* wfdir{nullptr};
  TDirectory* nowfdir{nullptr};

};

#endif // __PTFANALYSIS__
