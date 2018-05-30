void macro_LoopOnData(int first_run, int last_run=-1, int evt_amount=0)
{ 
  std::string data_path("/mnt/analysis/e15190/HiRAEVTMapper_rootfiles/");
  TChain *dataChain = (TChain*) new TChain("E15190");
  if(last_run==-1) {
    last_run=first_run;
  }
  // Adding all the files to the TChain ================================
  for(int i=first_run; i<=last_run; i++)
  {
    char file_evt[100];
    sprintf (file_evt,"run-%04d.root",i);
    std::string file_evt_string(file_evt);
    std::string path_to_evt_file(data_path+file_evt_string);
    int n_files = dataChain->Add((data_path+file_evt).c_str());
    printf("%d Event files added to chain for run %d\n", n_files, i);
  }

  HiRAReader HiRAAnalyzer(dataChain);
  HiRAAnalyzer.LoadGeometry("calibrations/PixelLocation_RomerArm.dat");
  HiRAAnalyzer.LoadSiCalibration("calibrations/HiRA_Si_Calibration_run4761.dat");
  //HiRAAnalyzer.LoadSiCalibration("calibrations/HiRA_Si_Calibration_run2873.dat");
  HiRAAnalyzer.LoadStripBad("calibrations/HiRA_StripBad.dat");
  //Loading pulser settings (mandatorly for CsI calibrations)
  HiRAAnalyzer.LoadCsIPulserInfo("calibrations/HiRA_CsI_Pulser_170setting.dat");
  //HiRAAnalyzer.LoadCsIPulserInfo("calibrations/HiRA_CsI_Pulser_200setting.dat");
  //Loading CsI calibrations for protons
  HiRAAnalyzer.LoadCsICalibration("calibrations/HiRA_CsI_LightVsEnergy_Z01_A01.dat", 1, 1);

  std::string FileOutName(Form("output/output_%04d_%04d.root", first_run, last_run));

  HiRAAnalyzer.Loop(FileOutName.c_str(), evt_amount);

  return 0;
}
