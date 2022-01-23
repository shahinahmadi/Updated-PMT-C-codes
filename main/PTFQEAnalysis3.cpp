#include "PTFQEAnalysis.hpp"
#include "ScanPoint.hpp"
#include "WaveformFitResult.hpp"

using namespace std;

PTFQEAnalysis::PTFQEAnalysis( TFile *outfile, PTFAnalysis *ptfanalysis ){

  vector< double > xbins = ptfanalysis->get_bins( 'x' );
  vector< double > ybins = ptfanalysis->get_bins( 'y' );

  //Create detection efficiency histogram
   quantum_efficiency = new TH2D("quantum_efficiency", "Detection efficiency(20 in PMT)", xbins.size()-1, &xbins[0], ybins.size()-1, &ybins[0]);
  // const Int_t XBINS = 6; const Int_t YBINS = 6;
  // Double_t xEdges[XBINS + 1] ={0.05,0.15,0.25,0.35,0.45,0.55,0.65}; 
  // Double_t yEdges[YBINS + 1] ={0.05,0.15,0.25,0.35,0.45,0.55,0.65};
   
  //quantum_efficiency = new TH2D("eff", "Detection efficiency", XBINS, xEdges, YBINS, yEdges);
   
  //quantum_efficiency = new TH2D("eff","Detection efficiency", 6, 0.11, 0.71, 6, 0.03, 0.63);
  quantum_efficiency->GetXaxis()->SetTitle("X position on PMT(m)");
  quantum_efficiency->GetYaxis()->SetTitle("Y position on PMT(m)");
  quantum_efficiency->GetXaxis()->CenterTitle();
  quantum_efficiency->GetYaxis()->CenterTitle();
   
  //Loop through events and fill histogram
  //  double a[2]={1.112,1.06364};
  vector<ScanPoint>scanpoints=ptfanalysis->get_scanpoints();
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    ScanPoint scanpoint = scanpoints[iscan ];
    long long entry = scanpoint.get_entry();
    //Loop through fit results
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      int index=iscan/18;
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
     double efficiency= (double)fit_result.haswf/(double)scanpoint.nentries();
      quantum_efficiency->Fill(fit_result.x, fit_result.y,/*a[index]*/efficiency);
      
      // double maxi=quantum_efficiency->GetBinContent(quantum_efficiency->GetMaximumBin());
      // cout<<maxi<<endl;
    }
  }
  


}
