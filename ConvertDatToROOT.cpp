// credit to Dr. Scott Kravitz for his 2020 ML tutorial, which has a python version of this script (https://github.com/swkravitz/dance-ml-2020-tutorial/tree/master)
// a ROOT macro to convert data generated via NEST into a ROOT TTree 
// Using this script:
// 1. Launch a ROOT session
// 2. Load this script (.L ConvertDatToROOT.cpp)
// 3. Call the function below (ConvertDatToROOT("<filepath>"))

#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

void ConvertDatToROOT(string filename) {
    int end = (int) filename.rfind(".dat");
    if (end == -1) {
        cerr << "input file should be a .dat file" << endl;
        return;
    }
    ifstream fp;
    fp.open(filename);
    if (!fp.is_open()) {
        cerr << "file could not be opened" << endl;
        return;
    }
    cout << "file opened" << endl;
    // construct TTree and .root file, initialize branches
    string outfile_name = filename.substr(0, end) + ".root";
    TFile* outfile = outfile = TFile::Open(outfile_name.c_str(), "RECREATE");
    TTree* nest = new TTree("nest", "NEST events");
    Float_t x_cm = 0;
    Float_t y_cm = 0;
    Float_t z_cm = 0;
    Float_t driftTime_us = 0;
    Float_t s1Area_phd = 0;
    Float_t s2Area_phd = 0;
    Float_t E_recon_keV = 0;
    nest->Branch("x_cm",&x_cm,"x_cm/F");
    nest->Branch("y_cm",&y_cm,"y_cm/F");
    nest->Branch("z_cm",&z_cm,"z_cm/F");
    nest->Branch("driftTime_us",&driftTime_us,"driftTime_us/F");
    nest->Branch("s1Area_phd",&s1Area_phd,"s1Area_phd/F");
    nest->Branch("s2Area_phd",&s2Area_phd,"s2Area_phd/F");
    nest->Branch("E_recon_keV",&E_recon_keV,"E_recon_keV/F");

    int i = 0;
    char line[200];
    while (fp.getline(line, 200)){
        i += 1;
        if (i <= 4) { // ignore the first four lines i.e. the header
            continue;
        }
        // parse each row and store the values in our variables
        sscanf(&line[0], "%f %*f %f %f%*c %f%*c %f %*f %*f %f %*f %*f %*f %f %*f", &E_recon_keV, &driftTime_us, &x_cm, &y_cm, &z_cm, &s1Area_phd, &s2Area_phd);
        if (s1Area_phd <= 0 | s2Area_phd <= 0) { // ignore events below thresholds
            continue;
        }
        // convert mm to cm
        x_cm /= 10;
        y_cm /= 10;
        z_cm /= 10;
        nest->Fill();
    }
    nest->Print();
    nest->Write();

    fp.close();
    delete outfile;
    return;
}

int main () {
    return 0;
}