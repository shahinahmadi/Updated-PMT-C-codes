#include "PTFAnalysis.hpp"

#include "TH2D.h"

using namespace std;

bool PTFAnalysis::HasWaveform(){
  // check if fit is valid
  if ( fitresult->fitstat != 0 ) return false;
  // check if ringing is bigger than waveform
  if ( fitresult->amp - fitresult->sinamp < 10.0 ) return false; 
  // check if pulse width is reasonable
  if ( fitresult->sigma < 1.0 || fitresult->sigma > 5.0 ) return false;
  // check if mean pulse time is reasonable
  if ( fitresult->mean < 20.0 || fitresult->mean > 50.0 ) return false;
  // cut on chi2
  if ( fitresult->chi2 > 200.0 ) return false;
  return true;
}
  
// Returns constant reference to a particular fit result
// it is useable until next entry in TTree is added, or another get_fitresult
// is called again
const WaveformFitResult & PTFAnalysis::get_fitresult( int scanpt, unsigned long long wavenum ){
  ScanPoint scanpoint = scanpoints[ scanpt ];
  long long entry = scanpoint.get_entry();
  entry += wavenum;
  if ( entry > ptf_tree->GetEntries() ) throw std::runtime_error("WaveformFitResult::get_fitresult beyond end of TTree");
  ptf_tree->GetEvent( entry );
  return *fitresult;
}

//Gaussian fitting function
// x[0] is x value
// par[0] = amplitude
// par[1] = mean
// par[2] = sigma
// par[3] = offset
// par[4] = sin amplitude
// par[5] = sin frequency (rad/s)
// par[6] = sin phase (rad)
double PTFAnalysis::my_gaussian(double *x, double *par) {
  double arg=0;
  if(par[2]!=0) arg=(x[0]-par[1])/par[2];
  double gfunc=par[3] - par[0] * TMath::Exp( -0.5*arg*arg ) + par[4]*TMath::Sin( par[5]*x[0] + par[6] );
  return gfunc;
}

void PTFAnalysis::FitWaveform( int wavenum, int nwaves  ) {
  // assumes hwaveform already defined and filled
  // assumes fit result structure already setup
  // check if we need to build the function to fit
  if( fmygauss == nullptr ) fmygauss = new TF1("mygauss",my_gaussian,0,70,7);
  fmygauss->SetParameters( 1.0, 35.0, 3.6, 8135.0, 10.0, 0.5, 0.0 );
  fmygauss->SetParNames( "Amplitude", "Mean", "Sigma", "Offset",
			 "Sine-Amp",  "Sin-Freq", "Sin-Phase" );

  fmygauss->SetParLimits(0, 0.0, 8500.0);
  fmygauss->SetParLimits(1, 25.0, 45.0 );
  fmygauss->SetParLimits(2, 1.0, 5.0 );
  fmygauss->SetParLimits(3, 7500.0, 8500.0 );
  fmygauss->SetParLimits(4, 0.0, 8500.0);
  fmygauss->SetParLimits(5, 0.4, 0.7);
  fmygauss->SetParLimits(6, -TMath::Pi(), TMath::Pi() );
 
  // first fit for sine wave:
  fmygauss->FixParameter(0,1.0);
  fmygauss->FixParameter(1,35.0);
  fmygauss->FixParameter(2,3.6);
  hwaveform->Fit( fmygauss, "Q", "", 0,30.0);

  fmygauss->ReleaseParameter(0);
  fmygauss->ReleaseParameter(1);
  fmygauss->ReleaseParameter(2);
  fmygauss->SetParLimits(0, 0.0, 8500.0);
  fmygauss->SetParLimits(1, 25.0, 45.0 );
  fmygauss->SetParLimits(2, 1.0, 5.0 );
  fmygauss->FixParameter(3, fmygauss->GetParameter(3) );
  fmygauss->FixParameter(4, fmygauss->GetParameter(4));
  fmygauss->FixParameter(5, fmygauss->GetParameter(5));
  fmygauss->FixParameter(6, fmygauss->GetParameter(6));
  hwaveform->Fit( fmygauss, "Q", "", 20.0, 50.0);

  fmygauss->ReleaseParameter(3);
  fmygauss->ReleaseParameter(4);
  fmygauss->ReleaseParameter(5);
  fmygauss->ReleaseParameter(6);
  fmygauss->SetParLimits(0, 0.0, 8500.0);
  fmygauss->SetParLimits(1, 25.0, 45.0 );
  fmygauss->SetParLimits(2, 1.0, 5.0 );
  fmygauss->SetParLimits(3, 7500.0, 8500.0 );
  fmygauss->SetParLimits(4, 0.0, 8500.0);
  fmygauss->SetParLimits(5, 0.4, 0.7);
  fmygauss->SetParLimits(6, -TMath::Pi(), TMath::Pi() );
  int fitstat = hwaveform->Fit( fmygauss, "Q", "", 0, 70);
  // collect fit results
  ScanPoint & scanpoint = scanpoints[ scanpoints.size()-1 ];
  fitresult->scanpt    = scanpoints.size()-1;
  fitresult->wavenum   = wavenum;
  fitresult->nwaves    = nwaves;
  fitresult->x         = scanpoint.x();
  fitresult->y         = scanpoint.y();
  fitresult->z         = scanpoint.z();
  fitresult->ped       = fmygauss->GetParameter(3);
  fitresult->mean      = fmygauss->GetParameter(1);
  fitresult->sigma     = fmygauss->GetParameter(2);
  fitresult->amp       = fmygauss->GetParameter(0);
  fitresult->sinamp    = fmygauss->GetParameter(4);
  fitresult->sinw      = fmygauss->GetParameter(5);
  fitresult->sinphi    = fmygauss->GetParameter(6);
  fitresult->ped_err   = fmygauss->GetParError(3);
  fitresult->mean_err  = fmygauss->GetParError(1);
  fitresult->sigma_err = fmygauss->GetParError(2);
  fitresult->amp_err   = fmygauss->GetParError(0);
  fitresult->sinamp_err= fmygauss->GetParError(4);
  fitresult->sinw_err  = fmygauss->GetParError(5);
  fitresult->sinphi_err= fmygauss->GetParError(6);
  fitresult->chi2      = fmygauss->GetChisquare();
  fitresult->ndof      = 30-4;
  fitresult->prob      = TMath::Prob( fmygauss->GetChisquare(), 30-4 );
  fitresult->fitstat   = fitstat;
}

PTFAnalysis::PTFAnalysis( TFile* outfile, PTF::Wrapper & wrapper, double errorbar, PTF::PMTChannel & channel, bool savewf ){
  static int instance_count =0;
  int l=0,m=0;
  channel={l,m};
  static int savewf_count =0;
  static int savenowf_count = 0;
  ++instance_count;
  save_waveforms = savewf;

  // get length of waveforms
  wrapper.setCurrentEntry(0);
  int  numTimeBins= wrapper.getSampleLength();
  
  // build the waveform histogram
  std::string hname = "hwaveform" + std::to_string(instance_count);
  outfile->cd();
  hwaveform = new TH1D( hname.c_str(), "Pulse waveform; Time bin (tdc clock ticks); Charge (adc units)", numTimeBins, 0., float(numTimeBins) );
  
  // set up the output TTree
    
  std:: string tname = "ptfanalysis" + std::to_string(m); // change so you have unique file name
  ptf_tree = new TTree(tname.c_str(),"ptfanalysis");
  fitresult = new WaveformFitResult();
  fitresult->MakeTTreeBranches( ptf_tree );

  if ( save_waveforms && wfdir==nullptr ) wfdir = outfile->mkdir("Waveforms");
  if ( save_waveforms && nowfdir==nullptr ) nowfdir = outfile->mkdir("NoWaveforms");
  outfile->cd();
  
  // Loop over scan points (index i)
  unsigned long long nfilled = 0;// number of TTree entries so far
  for (unsigned i = 0; i < wrapper.getNumEntries(); i++) {
    std::cout << "PTFAnalysis scan point " << i << " / " << wrapper.getNumEntries() << std::endl;
    wrapper.setCurrentEntry(i);
    auto location = wrapper.getDataForCurrentEntry(PTF::Gantry1);
    
    scanpoints.push_back( ScanPoint( location.x, location.y, location.z, nfilled ) );
    ScanPoint& curscanpoint = scanpoints[ scanpoints.size()-1 ];
    
    // loop over the number of waveforms at this ScanPoint (index j)
    int numWaveforms = wrapper.getNumSamples();
    for ( int j=0; j<numWaveforms; j++) {
      //if( j>50 ) continue;
      double* pmtsample=wrapper.getPmtSample( channel.pmt, j );
      // set the contents of the histogram
      for ( int ibin=1; ibin <= numTimeBins; ++ibin ){
        hwaveform->SetBinContent( ibin, pmtsample[ibin-1] );
	    hwaveform->SetBinError( ibin, errorbar );
      }
      FitWaveform( j, numWaveforms ); // Fit waveform and copy fit results into TTree
      fitresult->haswf = HasWaveform();
      ptf_tree->Fill();
      
      
      // check if we should clone waveform histograms
      if ( save_waveforms && savewf_count<500 && savenowf_count<500 ){
	    if  ( fabs( curscanpoint.x() - 0.46 ) < 0.0005 && 
	      fabs( curscanpoint.y() - 0.38 ) < 0.0005 ) {
          if ( savewf_count < 10 ){
            cout<<"Save wf at "<< curscanpoint.x()<<", "<< curscanpoint.y() << endl;
	      }

          std::string hwfname = "hwf_" + std::to_string( nfilled );
          if ( fitresult->haswf && savewf_count<500 ) {
            wfdir->cd();
            TH1D* hwf = (TH1D*) hwaveform->Clone( hwfname.c_str() );
            hwf->SetName( hwfname.c_str() );
            hwf->SetTitle("HAS a Pulse; Time bin (tdc clock ticks); Charge (adc units)");
            hwf->SetDirectory( wfdir );
            ++savewf_count;	  
          } else if ( !fitresult->haswf && savenowf_count<500 ){
            nowfdir->cd();
            TH1D* hwf = (TH1D*) hwaveform->Clone( hwfname.c_str() );
            hwf->SetName( hwfname.c_str() );
            hwf->SetTitle("Noise pulse; Time bin (tdc clock ticks); Charge (adc units)");
            hwf->SetDirectory( nowfdir );
            ++savenowf_count;	  
          }

	    outfile->cd();


        }

      }
      ++curscanpoint;  // increment counters
      ++nfilled;
    }
  }
  // Done.

}

const std::vector< double > PTFAnalysis::get_bins( char dim ){

  vector< double > positions;
  for(unsigned int iscan=0; iscan<get_nscanpoints(); iscan++){
    ScanPoint scanpoint = scanpoints[ iscan ];
    if( scanpoint.x() < 1e-5 ) continue; // Ignore position (0,0,0)
    if( dim == 'x' ){
      positions.push_back( scanpoint.x() );
    }
    else if( dim == 'y' ){
      positions.push_back( scanpoint.y() ); 
    }
    else if( dim == 'z' ){
      positions.push_back( scanpoint.z() );
    }
    else{
      cout << "PTFAnalysis::get_bins Error: input must be x, y or z!" << endl;
      exit( EXIT_FAILURE );
    }
  }

  sort( positions.begin(), positions.end() );
  positions.erase( unique( positions.begin(), positions.end(), comparison ), positions.end() ); // comparison function may not be necessary but was concerned about the comparison of doubles

  //std::cout << "positions contains:";
  //vector<double>::iterator it;
  //for (it=positions.begin(); it!=positions.end(); ++it)
  //  std::cout << ' ' << *it;
  //std::cout << '\n';

  /* if( positions.size() < 3 ){
    cout << "PTFAnalysis::get_bins Error: fewer than 3 positions a problem for automatic binning!" << endl;
    exit( EXIT_FAILURE );
    }*/

  vector< double > bins;
  for(unsigned int i=0; i<positions.size()-1; i++){
    bins.push_back( (positions[i]+positions[i+1])/2. );
  }
  bins.push_back( 2.*bins[ bins.size()-1 ] - bins[ bins.size()-2 ] );
  bins.insert( bins.begin(), 2.*bins[0] - bins[1] );

  //std::cout << "bins contains:";
  //vector<double>::iterator it;
  //for (it=bins.begin(); it!=bins.end(); ++it)
  //  std::cout << ' ' << *it;
  //std::cout << '\n';

  return bins;
}
