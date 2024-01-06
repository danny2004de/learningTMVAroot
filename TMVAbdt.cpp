#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TMVA/DataLoader.h>
#include <TMVA/Factory.h>

using namespace std;
void BDT(string signalFilepath, string backgroundFilepath) {
    TFile* signalFile = TFile::Open(signalFilepath.c_str());
    TFile* backgroundFile = TFile::Open(backgroundFilepath.c_str());
    TTree* signalTree = signalFile->Get<TTree>("nest");
    TTree* backgroundTree = backgroundFile->Get<TTree>("nest");
    signalTree->Print();
    backgroundTree->Print();
    TString outfile_name = "TMVABDT.root";
    TFile* outfile = TFile::Open(outfile_name, "RECREATE");
    TMVA::Factory* factory = new TMVA::Factory("TMVAClassification", outfile, "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
    TMVA::DataLoader* loader = new TMVA::DataLoader("nestdataloader");
    loader->AddVariable("x_cm", "x", "cm", 'F');
    loader->AddVariable("y_cm", "y", "cm", 'F');
    loader->AddVariable("driftTime_us", "Drift Time", "us", 'F');
    loader->AddVariable("s1Area_phd", "S1 Area", "phd", 'F');
    loader->AddVariable("s2Area_phd", "S2 Area", "phd", 'F');
    loader->AddSpectator("E_recon_keV", "Energy", "keV", 'F');
    loader->AddSpectator("z_cm", "z", "cm", 'F');
    loader->AddSignalTree(signalTree);
    loader->AddBackgroundTree(backgroundTree);
    TCut dataCut = "driftTime_us < 300 && s1Area_phd < 150";
    loader->PrepareTrainingAndTestTree(dataCut, "nTrain_Signal=7000:nTest_Signal=3000:nTrain_Background=7000:nTest_Background=3000:SplitMode=Random");
    factory->BookMethod(loader, TMVA::Types::kBDT, "BDT", "NTrees=100:MaxDepth=3");
    
    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();
    outfile->Close();
    cout << "==> Wrote root file: " << outfile->GetName() << endl;
    cout << "==> TMVAClassification is done!" << endl;
}