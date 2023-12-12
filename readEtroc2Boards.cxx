// ROOT headers
#include "TChain.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TString.h"
#include "TLatex.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TSystem.h"
#include "TCanvas.h"

// C++ headers
#include <iostream>
#include <vector>

Int_t status = gSystem->Load("/Users/gnigmat/work/cms/soft/etroc/build/libETROC2.dylib");

// ETROC2 headers
#include "Hit.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
  R__LOAD_LIBRARY(build/libETROC2.dylib);
#endif

//________________
int main(int argc, char const *argv[]) {

//int readEtroc2Boards() {

    const Char_t *inFileName = "/Users/gnigmat/work/cms/soft/etroc/build/oEtroc2Trans.root";
    
    TFile *inFile = TFile::Open(inFileName);

    if ( !inFile->IsOpen() || inFile->IsZombie() ) {
        std::cout << "[ERROR] File is opened or zombie" << std::endl;
        return 1;
    }

    //
    // Histograms
    //

    //TCanvas *canv = new TCanvas("canv","canv", 1200, 800);
    auto *h1 = new TH1D("h1", "h;TOT;Entries", 50, 0., 500.);
    h1->Sumw2();
    auto *h2 = new TH1D("h2", "Event", 10000, 0., 10000);

    const Int_t nBoards = 4;
    TH1D *hNHits[nBoards];
    TH1D *hTOT[nBoards];
    TH2D *hTOTvsTOA[nBoards];
    
    for (Int_t i{0}; i<nBoards; i++) {
        hNHits[i] = new TH1D(Form("hNHits_%d", i), 
                             Form("Number of hits in board #%d", i),
                             10, -0.5, 9.5);
        hTOT[i] = new TH1D(Form("hTOT_%d", i), 
                           Form("TOT for the board #%d", i),
                           500, 0, 1500);
        hTOTvsTOA[i] = new TH2D(Form("hTOTvsTOA_%d",i), 
                                Form("TOT vs. TOA for board #%d;TOA;TOT",i), 
                                512, 0., 1024., 512, 0., 512.);
    }

    //
    // Create variables to fill
    //

    UInt_t runId;
    UInt_t loopId;
    UInt_t eventId;

    std::vector<Hit> *mBoard0{nullptr};
    std::vector<Hit> *mBoard1{nullptr};
    std::vector<Hit> *mBoard2{nullptr};
    std::vector<Hit> *mBoard3{nullptr};

    auto T = dynamic_cast<TTree*>( inFile->Get("T") );
    if ( !T ) {
        std::cout << "[ERROR] No tree was found" << std::endl;
        inFile->Close();
        return 1;
    }

    //
    // Set branches to read (do not mess with naming)
    //

    T->SetBranchAddress( "mEventId", &eventId );

    T->SetBranchAddress( "mBoard0", &mBoard0 );
    T->SetBranchAddress( "mBoard1", &mBoard1 );
    T->SetBranchAddress( "mBoard2", &mBoard2 );
    T->SetBranchAddress( "mBoard3", &mBoard3 );

    // Retrieve number of events
    Long64_t nEntries = T->GetEntries();

    // Loop over event in TTree
    for (Long64_t iEntry{0}; iEntry < nEntries; ++iEntry) {

        // Load the entry from the tree
        T->GetEntry( iEntry );

        h2->Fill( eventId );

        if ( !mBoard0->empty() ) hNHits[0]->Fill( mBoard0->size() );
        if ( !mBoard1->empty() ) hNHits[1]->Fill( mBoard1->size() );
        if ( !mBoard2->empty() ) hNHits[2]->Fill( mBoard2->size() );
        if ( !mBoard3->empty() ) hNHits[3]->Fill( mBoard3->size() );

        // Loop over hits in each board
        for (UInt_t iHit{0}; iHit<mBoard0->size(); iHit++) {
            Hit hit = mBoard0->at(iHit);
            //if (!hit) continue;
            hTOT[0]->Fill( hit.tot() );
            hTOTvsTOA[0]->Fill( hit.toa(), hit.tot() );
        }
        for (UInt_t iHit{0}; iHit<mBoard1->size(); iHit++) {
            Hit hit = mBoard1->at(iHit);
            //if (!hit) continue;
            hTOT[1]->Fill( hit.tot() );
            hTOTvsTOA[1]->Fill( hit.toa(), hit.tot() );
        }
        for (UInt_t iHit{0}; iHit<mBoard2->size(); iHit++) {
            Hit hit = mBoard2->at(iHit);
            //if (!hit) continue;
            hTOT[2]->Fill( hit.tot() );
            hTOTvsTOA[2]->Fill( hit.toa(), hit.tot() );
        }
        for (UInt_t iHit{0}; iHit<mBoard3->size(); iHit++) {
            Hit hit = mBoard3->at(iHit);
            //if (!hit) continue;
            hTOT[3]->Fill( hit.tot() );
            hTOTvsTOA[3]->Fill( hit.toa(), hit.tot() );
        }

        
    }

    TFile *oFile = new TFile("oReadEtroc2.root", "recreate");
    h1->Write();
    h2->Write();

    for (Int_t i{0}; i<nBoards; i++) {
        hNHits[i]->Write();
        hTOT[i]->Write();
        hTOTvsTOA[i]->Write();
    }

    oFile->Close();

    return 0;
}