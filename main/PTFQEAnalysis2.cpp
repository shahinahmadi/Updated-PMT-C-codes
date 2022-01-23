#include "PTFQEAnalysis.hpp"
#include "ScanPoint.hpp"
#include "WaveformFitResult.hpp"

using namespace std;

PTFQEAnalysis::PTFQEAnalysis( TFile *outfile, PTFAnalysis *ptfanalysis ){

  vector< double > xbins = ptfanalysis->get_bins( 'x' );
  vector< double > ybins = ptfanalysis->get_bins( 'y' );

  //Create detection efficiency histogram
   quantum_efficiency = new TH2D("quantum_efficiency", "Detection efficiency(Monitor of  g1)", xbins.size()-1, &xbins[0], ybins.size()-1, &ybins[0]);
  // const Int_t XBINS = 6; const Int_t YBINS = 6;
  // Double_t xEdges[XBINS + 1] ={0.05,0.15,0.25,0.35,0.45,0.55,0.65}; 
  // Double_t yEdges[YBINS + 1] ={0.05,0.15,0.25,0.35,0.45,0.55,0.65};
   
  //quantum_efficiency = new TH2D("eff", "Detection efficiency", XBINS, xEdges, YBINS, yEdges);
   
  //quantum_efficiency = new TH2D("eff","Detection efficiency", 6, 0.11, 0.71, 6, 0.03, 0.63);
  quantum_efficiency->GetXaxis()->SetTitle("X position on PMT(m)");
  quantum_efficiency->GetYaxis()->SetTitle("Y position on PMT(m)");
  quantum_efficiency->GetXaxis()->CenterTitle();
  quantum_efficiency->GetYaxis()->CenterTitle();
  //double a[80]={1,2,4,5,6,7,8,9,10,1,2,4,5,6,7,8,9,10,1,2,4,5,6,7,8,9,10,1,2,4,5,6,7,8,9,10,1,2,4,5,6,7,8,9,10,1,2,4,5,6,7,8,9,10,1,2,4,5,6,7,8,9,10,1,2,4,5,6,7,8,9,10};
   vector<ScanPoint>scanpoints=ptfanalysis->get_scanpoints();
   for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
     ScanPoint scanpoint = scanpoints[iscan ];
    long long entry = scanpoint.get_entry();
    //
    //Loop through fit results
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
    
      double efficiency=(double)fit_result.haswf/(double)scanpoint.nentries();
      int index = iscan/20;
      
      quantum_efficiency->Fill(fit_result.x, fit_result.y,/*a[index]*/efficiency);
     
      //a[index]=max/mean 
     
    }
    
   }
    double maxi=quantum_efficiency->GetBinContent(quantum_efficiency->GetMaximumBin());
 
  //Loop through events and fill histogram
    // vector<ScanPoint>scanpoints=ptfanalysis->get_scanpoints();
  double arr1[scanpoints.size()/20];
  for (int i=0;i<scanpoints.size()/20;i++){arr1[i]=0;}
  // double mean;
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    ScanPoint scanpoint = scanpoints[iscan ];
    long long entry = scanpoint.get_entry();
    //
    //Loop through fit results
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
    
      double efficiency=(double)fit_result.haswf/(double)scanpoint.nentries();
      int index = iscan/20;
      arr1[index]+=efficiency/20;
      
      //a[index]=max/mean 
     
    }
  }
 
  for (int i=0;i<scanpoints.size()/20;i++)
    
    cout<<maxi/ arr1[i]<<endl;
 }

  //
  /// for (int index=0;index<scanpoints.size()/18;index++)
  //  mean=arr1[index]/18;
 


