#ifndef HIRAREADER_H
#define HIRAREADER_H

#define GRAPHICAL_DEBUG

#include <string.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <TChain.h>
#include <TMath.h>
#include <TFile.h>
#include <TString.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraphErrors.h>
#include <TCutG.h>
#include <TMultiGraph.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <HTHiRARootEvent.h>
#include <HiRAGeometry.h>
#include <HiRASiCalibration.h>
#include <HiRACsICalibration.h>
#include <HiRASiHiLowMatching.h>
#include <EnergyLossModule.h>

class HiRAReader
{
public :
  HiRAReader(TChain * Chain=0);
  ~HiRAReader();

  int LoadGeometry(const char *);
  int LoadCsICalibration(const char *, int Z=1, int A=1);
  int LoadSiCalibration(const char *);
  int LoadStripBad(const char *);
  int LoadSiHiLowMatching(const char *);
  int LoadCsIPulserInfo(const char *);

  bool IsStripfBad(int telescope, int strip_front);
  bool IsStripbBad(int telescope, int strip_back);

  double GetThetaPixel(int telescope, int strip_front, int strip_back);
  double GetPhiPixel(int telescope, int strip_front, int strip_back);
  double GetThetaPixelDeg(int telescope, int strip_front, int strip_back);
  double GetPhiPixelDeg(int telescope, int strip_front, int strip_back);

  double GetSifIntercept(int telescope, int numstrip);
  double GetSibIntercept(int telescope, int numstrip);
  double GetSifSlope(int telescope, int numstrip);
  double GetSibSlope(int telescope, int numstrip);
  double GetCsIIntercept(int telescope, int numcsi);
  double GetCsISlope(int telescope, int numcsi);
  double GetSifHiLowMatched(int chHi, int chLow, int telescope, int numstrip);
  double GetSibHiLowMatched(int chHi, int chLow, int telescope, int numstrip);

  double GetCsIEMeV(int ch, int telescope, int numcsi, int Z=1, int A=1);
  double GetSifEMeV(int ch, int telescope, int numstripf);
  double GetSibEMeV(int ch, int telescope, int numstripb);

  void   Loop(const char *, Long64_t evt_amount=0);

  void   DrawGeometry();

private :
  TTreeReader * fHiRAReader;
  TTreeReaderValue<HTHiRAData> *ftele[12];
  TChain      * fChain;

  EnergyLossModule fLISEModule;

  HiRAGeometry *fGeometryTab;
  HiRASiCalibration *fSiCalibrationTab;
  HiRASiHiLowMatching *fSiHiLowMatching;
  HiRACsICalibrationManager fCsICalibrationModule;

  double DegToRad;
  double RadToDeg;

  bool fCsICalibrated;
  bool fSiCalibrated;
  bool fGeometryCalibrated;
  bool fStripBadLoaded;
  bool fSiHiLowMatched;

  double * fTelSiThickness;
  double   fCsIMylarThickness;
  double   fCsIlenght;
  bool   **fStripfBad;
  bool   **fStripbBad;
};

#endif
