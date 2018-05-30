{
  gROOT->ProcessLine(".L Nuclear_Masses/nuclear_masses.cc"); 
  gROOT->ProcessLine(".L EnergyLossModule.cpp"); 
  gROOT->ProcessLine(".L HiRACsICalibration.cpp"); 
  gROOT->ProcessLine(".L lib/HTHiRADict.so"); 
}
