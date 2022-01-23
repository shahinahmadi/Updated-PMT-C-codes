/// Analysis of PTF data
///
/// Definitions of terminology used, and associated structures:
///
/// Each input root file has a number of "ScanPoint" events, where each event has
/// several waveforms at a given location.
///
/// class  MeanRMSCalc           For calculating mean and rms
///
/// class  PTFErrorBarAnalysis   For calculating the error bar size to use on the waveforms
///
/// struct WaveformFitResult     Structure to hold one waveform fit result
///
/// class  ScanPoint             Holds location of scan point, first entry number in TTree of scan point, and number of waveforms
///
/// class  PTFAnalysis           For doing analysis of all of the waveforms, and keep track of scan points, store results in TTree
///
///
/// This program takes a PTF scan root file as input.
/// The analysis proceeds in these steps:
/// 1. Determine the uncertainties on the "collected charge" by studying the pedestal
///    of the waveforms in bins 1-20.  This part of the analysis produces two kinds of
///    pedestal histograms.
/// 2. Fit the waveforms to gaussians.  Each waveform is read in once into a Waveform object,
///    it is then fit to a gaussian using a fitter.  Fit results are stored in a TTee that
///    has one entry per scan-point
/// 3. The now filled TTree of fitted waveform parameters is analyzed to make histograms



#include "wrapper.hpp"
#include "MeanRMSCalc.hpp"
#include "ErrorBarAnalysis.hpp"
#include "WaveformFitResult.hpp"
#include "ScanPoint.hpp"
#include "PTFAnalysis.hpp"
#include "PTFQEAnalysis.hpp"
#include "PTFGainAnalysis.hpp"
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TF1.h"
#include <TVirtualFitter.h>
#include <TFitter.h>
#include <cmath>
#include "TMath.h"
#include <TStyle.h>

using namespace std;

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "give path to file to read" << endl;
    cerr << "usage: ptf_analysis filename.root" << endl;
    return 0;
  }

  // Opening the output root file
  TFile * outFile = new TFile("ptf_analysis_new1.root", "RECREATE");

  // Set up PTF Wrapper
  vector<int> phidgets = {0, 1, 3};
  PTF::PMTChannel Channel = {0,0}; // only looking at one channel at a time
  vector<PTF::PMTChannel> activeChannels = { Channel };
  PTF::Wrapper wrapper = PTF::Wrapper(200000, 70, activeChannels, phidgets);
  wrapper.openFile( string(argv[1]), "scan_tree");
  cerr << "Num entries: " << wrapper.getNumEntries() << endl << endl;

  // Determine error bars to use on waveforms
  ErrorBarAnalysis * errbars = new ErrorBarAnalysis( outFile, wrapper, Channel );

  std::cout<<"Using errorbar size "<<errbars->get_errorbar()<<std::endl;
  
  // Do analysis of waveforms for each scanpoint
  PTFAnalysis *analysis= new PTFAnalysis( outFile, wrapper, errbars->get_errorbar(), Channel, true );

  //First must modify code so that ttree has unique name
  //Change channel
  //Run ErrorBarAnalysis for new channel
  //Run PTFAnalysis for new channel

  // Do quantum efficiency analysis
  // PTFQEAnalysis *qeanalysis = new PTFQEAnalysis( outFile, analysis );
  PTFGainAnalysis *gainanalysis = new PTFGainAnalysis( outFile, analysis,wrapper );

  outFile->Write();
  outFile->Close();
  
  
  cout<<"Done"<<endl; 
}



