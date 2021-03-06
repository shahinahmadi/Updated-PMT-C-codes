#include "PTFQEAnalysis.hpp"
#include "ScanPoint.hpp"
#include "WaveformFitResult.hpp"
#include <vector>

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
    vector<double> numbers;
    double number;

    ifstream myfile("example.txt"); //file opening constructor

    //Operation to check if the file opened
    if ( myfile.is_open() ){
    
        while(myfile >>number ){

            //putting data into the vector
            numbers.push_back(number);
          

        }
    }

    
    //   Writing a for loop which prints the data in the vectors to screen.
    for (int i; i < numbers.size(); ++i){
        cout<< numbers.at(i)<<endl; 
        }

    
    //   Conversion of vector to array 
    int n;
    double a[n];
    for (int i=0; i<numbers.size();i++)
    a[i]=numbers.at(i);
    cout<<a[i];
  //Loop through events and fill histogram
  vector<ScanPoint>scanpoints=ptfanalysis->get_scanpoints();
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    ScanPoint scanpoint = scanpoints[iscan ];
    long long entry = scanpoint.get_entry();
    //Loop through fit results
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
      
      quantum_efficiency->Fill(fit_result.x, fit_result.y, (double)fit_result.haswf/(double)scanpoint.nentries());
      
      // double maxi=quantum_efficiency->GetBinContent(quantum_efficiency->GetMaximumBin());
      // cout<<maxi<<endl;
    }
  }
  


}
