#include "TFile.h"
#include "TDirectory.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TChain.h"

#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TProfile2D.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

void GetFiles(char const* input, vector<string>& files) {
    TSystemDirectory dir(input, input);
    TList *list = dir.GetListOfFiles();

    if (list) {
        TSystemFile *file;
        string fname;
        TIter next(list);
        while ((file = (TSystemFile*) next())) {
            fname = file->GetName();

            if (file->IsDirectory() && (fname.find(".") == string::npos)) {
                string newDir = string(input) + fname + "/";
                GetFiles(newDir.c_str(), files);
            }
            else if ((fname.find(".root") != string::npos)) {
                files.push_back(string(input) + fname);
            }
        }
    }

    return;
}

int CheckZombies(char const* input) {
    /* read in information */
    vector<string> files;
    GetFiles(input, files);

    for (auto const& file : files) {
        TFile* infile = new TFile(file.data(), "read");

        if(infile->IsZombie()){
            std::cout << file << std::endl;
        }

        infile->Close()
    }
    
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        return CheckZombies(argv[1]);

    else {
        cout << "ERROR: Please pass one path." << endl;
        return -1;
    }
}
