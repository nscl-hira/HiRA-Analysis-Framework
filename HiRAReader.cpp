#include "include/HiRAReader.h"

// ASSUMPTIONS
const int NUM_TEL = 12;
const int NUM_STRIP_F =32;
const int NUM_STRIP_B =32;
const int NUM_CSI_TEL =4;
const int NUM_CSI_STRIP=2;
const int NUM_TOWERS = 3;
//===============================

//____________________________________________________
HiRAReader::HiRAReader(TChain *Chain) :
fChain(Chain),
DegToRad(TMath::Pi()/180.),
RadToDeg(180./TMath::Pi()),
fCsICalibrated(false),
fSiCalibrated(false),
fGeometryCalibrated(false),
fStripBadLoaded(false),
fSiHiLowMatched(false)
{
  if(fChain!=0) {
    fHiRAReader = new TTreeReader(fChain);
    for(int i=0; i<NUM_TEL; i++)
    {
      ftele[i]=new TTreeReaderValue<HTHiRAData>(*fHiRAReader, Form("HiRA.tele%d.",i));
    }
  }
  fGeometryTab=new HiRAGeometry[NUM_TEL*NUM_STRIP_F*NUM_STRIP_B];
  fSiCalibrationTab=new HiRASiCalibration[NUM_TEL*NUM_STRIP_F+NUM_TEL*NUM_STRIP_B];
  fSiHiLowMatching=new HiRASiHiLowMatching[NUM_TEL*NUM_STRIP_F+NUM_TEL*NUM_STRIP_B];
  fTelSiThickness=new double[NUM_TEL];
  fStripfBad = new bool*[NUM_TEL];
  fStripbBad = new bool*[NUM_TEL];
  for(int TelNum=0; TelNum<NUM_TEL; TelNum++) {
    fStripfBad[TelNum]= new bool[NUM_STRIP_F];
    fStripbBad[TelNum]= new bool[NUM_STRIP_B];
  }
  fTelSiThickness[0]=1537; fTelSiThickness[1]=1521; fTelSiThickness[2] =1500; fTelSiThickness [3]=1496;
  fTelSiThickness[4]=1517; fTelSiThickness[5]=1512; fTelSiThickness[6] =1491; fTelSiThickness [7]=1460;
  fTelSiThickness[8]=1537; fTelSiThickness[9]=1536; fTelSiThickness[10]=1471; fTelSiThickness[11]=1491;
  fCsIMylarThickness=2.076;
  fCsIlenght=100000;
}

//____________________________________________________
HiRAReader::~HiRAReader()
{
  delete fHiRAReader;
  for(int i=0; i<NUM_TEL; i++)
  {
    delete ftele[i];
    delete fStripfBad[i];
    delete fStripbBad[i];
  }
  delete [] fGeometryTab;
  delete [] fSiCalibrationTab;
  delete [] fSiHiLowMatching;
  delete [] fStripfBad;
  delete [] fStripbBad;
}

//____________________________________________________
int HiRAReader::LoadGeometry(const char * file_name)
{
  std::ifstream FileIn(file_name);
  if(!FileIn.is_open()) {
    printf("Error: error while opening geometry file\n");
    return -1;
  }
  int NRead=0;

  while (!FileIn.eof())
  {
    std::string LineRead;
    std::getline(FileIn, LineRead);

    if(LineRead.empty()) continue;
    if(LineRead.find('*')==0) continue;

    std::istringstream LineStream(LineRead);

    int numtel;
    int numstripf;
    int numstripb;
    double theta;
    double phi;

    LineStream >> numtel >> numstripf >> numstripb >> theta >> phi;

    // Phi Corretions // TEMPORARY
    if(phi<0) {
      phi=-phi-180;
    } else {
      phi=-phi+180;
    }

    // Strip order corrections // TEMPORARY
    numstripf=31-numstripf;

    HiRAGeometry newGeom;
    newGeom.numtel=numtel;
    newGeom.stripf=numstripf;

    newGeom.stripb=numstripb;
    newGeom.thetadeg=theta;
    newGeom.phideg=phi;
    newGeom.theta=theta*DegToRad;
    newGeom.phi=phi*DegToRad;

    fGeometryTab[numtel*NUM_STRIP_F*NUM_STRIP_B+numstripf*NUM_STRIP_B+numstripb]=newGeom;
    NRead++;
  }

  fGeometryCalibrated=true;
  return NRead;
}

//____________________________________________________
int HiRAReader::LoadStripBad(const char * file_name)
{
  std::ifstream FileIn(file_name);
  if(!FileIn.is_open()) {
    printf("Error: error while opening strip bad file\n");
    return -1;
  }
  int NRead=0;

  while (!FileIn.eof())
  {
    std::string LineRead;
    std::getline(FileIn, LineRead);

    if(LineRead.empty()) continue;
    if(LineRead.find('*')==0) continue;

    std::istringstream LineStream(LineRead);

    int numtel;
    int striptype;
    int numstrip;
    int isbad;

    LineStream >> striptype >> numtel >> numstrip >> isbad;

    if(striptype==0) {
      fStripbBad[numtel][numstrip]=bool(isbad);
    } else if(striptype==1) {
      fStripfBad[numtel][numstrip]=bool(isbad);
    }

    NRead++;
  }

  fStripBadLoaded=true;
  return NRead;
}

//____________________________________________________
int HiRAReader::LoadSiCalibration(const char * file_name)
{
  std::ifstream FileIn(file_name);
  if(!FileIn.is_open()) {
    printf("Error: error while opening Si calibration file\n");
    return -1;
  }
  int NRead=0;

  while (!FileIn.eof())
  {
    std::string LineRead;
    std::getline(FileIn, LineRead);

    if(LineRead.empty()) continue;
    if(LineRead.find('*')==0) continue;

    std::istringstream LineStream(LineRead);

    int numtel;
    int numstrip;
    bool FB;
    double intercept;
    double error_intercept;
    double slope;
    double error_slope;

    LineStream >> FB >> numtel >> numstrip >> intercept >> slope;

    HiRASiCalibration newCalib;
    newCalib.numtel=numtel;
    newCalib.FB=FB;
    newCalib.numstrip=numstrip;
    newCalib.intercept=intercept;
    newCalib.slope=slope;

    if(FB==0) {
      fSiCalibrationTab[numtel*NUM_STRIP_F+numstrip]=newCalib;
    } else if (FB==1) {
      fSiCalibrationTab[NUM_TEL*NUM_STRIP_F+numtel*NUM_STRIP_B+numstrip]=newCalib;
    }
    NRead++;
  }

  fSiCalibrated=true;
  return NRead;
}

//____________________________________________________
int HiRAReader::LoadCsICalibration(const char * file_name, int Z, int A)
{
  int NRead=fCsICalibrationModule.LoadEnergyCalibration(file_name, Z, A);
  if(NRead>0) {
    fCsICalibrated=true;
  }
  return NRead;
}

//____________________________________________________
int HiRAReader::LoadSiHiLowMatching(const char * file_name)
{
  std::ifstream FileIn(file_name);
  if(!FileIn.is_open()) {
    printf("Error: error while opening Si matching file\n");
    return -1;
  }
  int NRead=0;

  while (!FileIn.eof())
  {
    std::string LineRead;
    std::getline(FileIn, LineRead);

    if(LineRead.empty()) continue;
    if(LineRead.find('*')==0) continue;

    std::istringstream LineStream(LineRead);

    int numtel;
    int numstrip;
    bool FB;
    double intercept;
    double slope;
    double saturationpoint;

    LineStream >> numtel >> numstrip >> FB >> intercept >> slope >> saturationpoint;

    HiRASiHiLowMatching newMatching;
    newMatching.numtel=numtel;
    newMatching.FB=FB;
    newMatching.numstrip=numstrip;
    newMatching.intercept=intercept;
    newMatching.slope=slope;
    newMatching.saturationpoint=saturationpoint;

    if(FB==0) {
      fSiHiLowMatching[numtel*NUM_STRIP_B+numstrip]=newMatching;
    } else if (FB==1) {
      fSiHiLowMatching[NUM_TEL*NUM_STRIP_B+numtel*NUM_STRIP_F+numstrip]=newMatching;
    }
    NRead++;
  }

  fSiHiLowMatched=true;
  return NRead;
}


//____________________________________________________
int HiRAReader::LoadCsIPulserInfo(const char * file_name)
{
  return fCsICalibrationModule.LoadPulserInfo(file_name);
}

//____________________________________________________
bool HiRAReader::IsStripfBad(int telescope, int strip_front)
{
  return fStripfBad[telescope][strip_front];
}

//____________________________________________________
bool HiRAReader::IsStripbBad(int telescope, int strip_back)
{
  return fStripbBad[telescope][strip_back];
}

//____________________________________________________
double HiRAReader::GetThetaPixel(int telescope, int strip_front, int strip_back)
{
  return fGeometryTab[telescope*NUM_STRIP_F*NUM_STRIP_B+strip_front*NUM_STRIP_B+strip_back].theta;
}

//____________________________________________________
double HiRAReader::GetPhiPixel(int telescope, int strip_front, int strip_back)
{
  return fGeometryTab[telescope*NUM_STRIP_F*NUM_STRIP_B+strip_front*NUM_STRIP_B+strip_back].phi;
}

//____________________________________________________
double HiRAReader::GetThetaPixelDeg(int telescope, int strip_front, int strip_back)
{
  return fGeometryTab[telescope*NUM_STRIP_F*NUM_STRIP_B+strip_front*NUM_STRIP_B+strip_back].thetadeg;
}

//____________________________________________________
double HiRAReader::GetPhiPixelDeg(int telescope, int strip_front, int strip_back)
{
  return fGeometryTab[telescope*NUM_STRIP_F*NUM_STRIP_B+strip_front*NUM_STRIP_B+strip_back].phideg;
}

//____________________________________________________
double HiRAReader::GetSifIntercept(int telescope, int numstrip)
{
  return fSiCalibrationTab[telescope*NUM_STRIP_F+numstrip].intercept;
}

//____________________________________________________
double HiRAReader::GetSibIntercept(int telescope, int numstrip)
{
  return fSiCalibrationTab[NUM_TEL*NUM_STRIP_F+telescope*NUM_STRIP_B+numstrip].intercept;
}

//____________________________________________________
double HiRAReader::GetSifSlope(int telescope, int numstrip)
{
  return fSiCalibrationTab[telescope*NUM_STRIP_F+numstrip].slope;
}

//____________________________________________________
double HiRAReader::GetSibSlope(int telescope, int numstrip)
{
  return fSiCalibrationTab[NUM_TEL*NUM_STRIP_F+telescope*NUM_STRIP_B+numstrip].slope;
}

//____________________________________________________
double HiRAReader::GetSifHiLowMatched(int chHi, int chLow, int telescope, int numstrip)
{
  if(chLow<fSiHiLowMatching[NUM_TEL*NUM_STRIP_B+telescope*NUM_STRIP_F+numstrip].saturationpoint) {
    return chHi;
  }
  else {
    return chLow*fSiHiLowMatching[NUM_TEL*NUM_STRIP_B+telescope*NUM_STRIP_F+numstrip].slope+fSiHiLowMatching[NUM_TEL*NUM_STRIP_B+telescope*NUM_STRIP_F+numstrip].intercept;
  }
}

//____________________________________________________
double HiRAReader::GetSibHiLowMatched(int chHi, int chLow, int telescope, int numstrip)
{
  if(chLow<fSiHiLowMatching[telescope*NUM_STRIP_B+numstrip].saturationpoint) {
    return chHi;
  }
  else {
    return chLow*fSiHiLowMatching[telescope*NUM_STRIP_B+numstrip].slope+fSiHiLowMatching[telescope*NUM_STRIP_B+numstrip].intercept;
  }
}

//____________________________________________________
double HiRAReader::GetCsIEMeV(int ch, int telescope, int numcsi, int Z, int A)
{
  return fCsICalibrationModule.GetEnergyValue(ch, telescope, numcsi, Z, A);
}

//____________________________________________________
double HiRAReader::GetSifEMeV(int ch, int telescope, int numstripf)
{
  return ch*GetSifSlope(telescope, numstripf)+GetSifIntercept(telescope, numstripf);
}

//____________________________________________________
double HiRAReader::GetSibEMeV(int ch, int telescope, int numstripb)
{
  return ch*GetSibSlope(telescope, numstripb)+GetSibIntercept(telescope, numstripb);
}

//____________________________________________________
void HiRAReader::Loop(const char * file_name, Long64_t evt_amount)
{
  // this method loops on the first "evt_amount" data entries
  // and creates a ROOT otput file named "file_name"
  // where the user can write some ROOT output
  TFile * FileOut = new TFile(file_name, "RECREATE");

  Long64_t nentries=fChain->GetEntries();
  if(evt_amount!=0) {
    nentries=evt_amount;
  }
  Long64_t jentry=0;
  std::cout << "found " << nentries << " entries\n";
  for(;fHiRAReader->Next() && jentry<nentries; jentry++)
  {
    if(jentry%100000==0) {
      printf("Percentage = %.2f %%\n", 100*double(jentry)/nentries);
    }
    //Loop on HiRA telescopes
    for(int i=0; i<12; i++)
    {
      HTHiRAData * HiRATel = ftele[i]->Get();
    }
    jentry++;
  }

  // closing output file
  FileOut->Close();
}
