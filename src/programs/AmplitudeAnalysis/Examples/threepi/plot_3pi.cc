#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib>

#include "AMPTOOLS_DATAIO/GlueXPlotGenerator.h"
#include "AMPTOOLS_DATAIO/ROOTDataReader.h"
#include "AMPTOOLS_AMPS/ThreePiAngles.h"
#include "AMPTOOLS_AMPS/BreitWigner.h"
#include "IUAmpTools/ConfigFileParser.h"
#include "IUAmpTools/AmpToolsInterface.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "TH1F.h"
using namespace std;
using namespace CLHEP;

int main( int argc, char* argv[] ){
  
  // these params should probably come in on the command line
  double lowMass = 0.7;
	double highMass = 2.0;
	enum{ kNumBins = 65 };
  string fitDir( "threepi_fit" );
  
  // set default parameters
  
  string outfileName("");
  
  // parse command line
  
  for (int i = 1; i < argc; i++){
    
    string arg(argv[i]);
    
    if (arg == "-o"){  
      if ((i+1 == argc) || (argv[i+1][0] == '-')) arg = "-h";
      else  outfileName = argv[++i]; }
    if (arg == "-h"){
      cout << endl << " Usage for: " << argv[0] << endl << endl;
      cout << "\t -o <file>\t Ouput text file" << endl;
      exit(1);}
  }
  
  if (outfileName.size() == 0){
    cout << "No config file specified" << endl;
    exit(1);
  }
  
	double step = ( highMass - lowMass ) / kNumBins;
	
  ofstream outfile;
  outfile.open( outfileName.c_str() );
  
  // descend into the directory that contains the bins
  chdir( fitDir.c_str() );
  
  // To create an AmpToolsInterface all of the relevant amplitudes need
  // to be registered.  Look for this to go away in future version of
  // AmpTools where it will be possible to create a PlotGenerator or
  // get information about the fit directly from the FitResults object.
  
  AmpToolsInterface::registerAmplitude( ThreePiAngles() );
  AmpToolsInterface::registerAmplitude( BreitWigner() );
  AmpToolsInterface::registerDataReader( ROOTDataReader() );
  
  AmpToolsInterface ati;

	for( int i = 0; i < kNumBins; ++i ){
		
    ostringstream dir;
    dir << "bin_" << i;
    chdir( dir.str().c_str() );

    ostringstream parFile;
		parFile << "bin_" << i << ".fit";
    
    // check to be sure the parameter file exists
    // in cases where the fit fails, there won't be a parameter file
    ifstream parFileTest;
    parFileTest.open( parFile.str().c_str() );
    if( parFileTest.fail() ){
      
      chdir( ".." );
      continue; 
    }
    else{
      
      parFileTest.close();
    }
    
    ostringstream configFile;
		configFile << "bin_" << i << ".cfg";
    
    ConfigFileParser parser(configFile.str());
    ConfigurationInfo* cfgInfo = parser.getConfigurationInfo();
    
    ati.resetConfigurationInfo( cfgInfo );
    
  	// print out the bin center
		outfile << lowMass + step * i + step / 2. << "\t";
		
    GlueXPlotGenerator plotGen( ati );
    
    bool yPol = false;
    
    vector< string > rhoPiS;
		rhoPiS.push_back( "Pi+Pi-Pi+::xpol::a1_rhopi_S" );
    if( yPol ) rhoPiS.push_back( "Pi+Pi-Pi+::ypol::a1_rhopi_S" );
		pair< double, double > rhoPiSInt = plotGen.intensity( rhoPiS );
		outfile << rhoPiSInt.first << "\t" << rhoPiSInt.second << "\t";
    
    vector< string > rhoPiD;
    rhoPiD.push_back( "Pi+Pi-Pi+::xpol::a2_rhopi_D" );
    if( yPol ) rhoPiD.push_back( "Pi+Pi-Pi+::ypol::a2_rhopi_D" );
    pair< double, double > rhoPiDInt = plotGen.intensity( rhoPiD );
    outfile << rhoPiDInt.first << "\t" << rhoPiDInt.second << "\t";
    
		vector< string > rhoPiPX;
		rhoPiPX.push_back( "Pi+Pi-Pi+::xpol::pi1_rhopi_P" );
    if( yPol ) rhoPiPX.push_back( "Pi+Pi-Pi+::ypol::pi1_rhopi_P" );
		pair< double, double > rhoPiPXInt = plotGen.intensity( rhoPiPX );
		outfile << rhoPiPXInt.first << "\t" << rhoPiPXInt.second << "\t";
    
    vector< string > f2PiS;
		f2PiS.push_back( "Pi+Pi-Pi+::xpol::pi2_f2pi_S" );
    if( yPol ) f2PiS.push_back( "Pi+Pi-Pi+::ypol::pi2_f2pi_S" );
		pair< double, double > f2PiSInt = plotGen.intensity( f2PiS );
		outfile << f2PiSInt.first << "\t" << f2PiSInt.second << "\t";
    
    vector< string > rhoPiP;
		rhoPiP.push_back( "Pi+Pi-Pi+::xpol::pi2_rhopi_P" );
    if( yPol ) rhoPiP.push_back( "Pi+Pi-Pi+::ypol::pi2_rhopi_P" );
		pair< double, double > rhoPiPInt = plotGen.intensity( rhoPiP );
		outfile << rhoPiPInt.first << "\t" << rhoPiPInt.second << "\t";
    
		vector< string > all;
		all.push_back( "Pi+Pi-Pi+::xpol::a1_rhopi_S" );
    all.push_back( "Pi+Pi-Pi+::xpol::a2_rhopi_D" );
    all.push_back( "Pi+Pi-Pi+::xpol::pi1_rhopi_P" );
    all.push_back( "Pi+Pi-Pi+::xpol::pi2_f2pi_S" );
    all.push_back( "Pi+Pi-Pi+::xpol::pi2_rhopi_P" );
    if( yPol ) all.push_back( "Pi+Pi-Pi+::ypol::a1_rhopi_S" );
    if( yPol ) all.push_back( "Pi+Pi-Pi+::ypol::a2_rhopi_D" );
    if( yPol ) all.push_back( "Pi+Pi-Pi+::ypol::pi1_rhopi_P" );
    if( yPol ) all.push_back( "Pi+Pi-Pi+::ypol::pi2_f2pi_S" );
    if( yPol ) all.push_back( "Pi+Pi-Pi+::ypol::pi2_rhopi_P" );
    pair< double, double > allInt = plotGen.intensity( all );
		outfile << allInt.first << "\t" << allInt.second << "\t";
		
    //		outfile << plotGen.phaseDiff( dWave, sWave ) << endl;
  
    outfile << endl;
    
    chdir( ".." );
	}
  
	return 0;
}
