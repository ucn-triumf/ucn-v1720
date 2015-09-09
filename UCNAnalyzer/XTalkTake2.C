void XTalkTake2 (char*infilename = "output00000090.root", int refChan = 6) {

  gStyle->SetOptFit(1111);

  // set up file to read
  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");

  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tChargeL;
  UShort_t tChargeS;

  tin->SetBranchAddress("tChannel",&tChannel);
  tin->SetBranchAddress("tChargeL",&tChargeL);
  tin->SetBranchAddress("tChargeS",&tChargeS);
 
  Long64_t n = tin->GetEntries();

  // find events and tally
  const int NCH=9; // number of channels
  double eventTot[NCH]={0};
  for (unsigned long long i=1;i<n;i++) {
    // get event
    tin->GetEvent(i);
    // with cuts
    if (tChargeL-tChargeS<800 && tChargeL < 2500)
      continue;
    eventTot[tChannel]++;
  }

  // normalize and print output
  cout << "Crosstalk for reference channel " << refChan << ":" << endl;
  for (int j=0;j<NCH;j++) {
    if (j==refChan)
      continue;
    if(eventTot[j] != 0)
      eventTot[j] /=eventTot[refChan];

    cout << "Channel " << j << " has " << eventTot[j]*100 << "% crosstalk"<< endl;
  }

}
  
  
