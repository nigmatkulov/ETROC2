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
#include "TStyle.h"

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
void set1DStyle(TH1 *h, Int_t type = 0) {
    // type: 0 - reco, 1 - gen, 2 - ref
    Int_t markerStyle = 20; // Full circle
    Double_t markerSize = 1.2;
    Int_t lineWidth{2}; 
    Int_t color{2};
    if (type == 0) {

    }
    else if (type == 1) { // gen
        color = 4;
        markerStyle = 24;
    }
    else if (type == 2) { // ref
        color = 8;
        markerStyle = 33;
        markerSize = 1.5;
    }
    else if (type == 3) {
        color = 6;
        markerStyle = 35;
        markerSize = 1.3;
    }
    else if ( type == 4 ) {
        color = 7;
        markerStyle = 37;
        markerSize = 1.3;  
    }
    else if ( type == 5 ) {
        color = 11;
        markerStyle = 40;
        markerSize = 1.3;  
    }
    else if ( type == 6 ) {
        color = 15;
        markerStyle = 41;
        markerSize = 1.3;  
    }
    else {
        color = 9;
        markerStyle = 38;
        markerSize = 1.3;
    }

    h->SetLineWidth( lineWidth );
    h->SetLineColor( color );
    
    h->SetMarkerStyle( markerStyle );
    h->SetMarkerColor( color );
    h->SetMarkerSize( markerSize );

    h->GetYaxis()->SetTitleSize(0.06);
    h->GetYaxis()->SetLabelSize(0.06);
    h->GetXaxis()->SetTitleSize(0.06);
    h->GetXaxis()->SetLabelSize(0.06);
    h->GetXaxis()->SetNdivisions(208);
    h->GetYaxis()->SetNdivisions(208);    
    h->GetYaxis()->SetTitleOffset(1.0);
}

//________________
void set2DStyle(TH2* h) {
    h->GetYaxis()->SetTitleSize(0.06);
    h->GetYaxis()->SetLabelSize(0.06);
    h->GetXaxis()->SetTitleSize(0.06);
    h->GetXaxis()->SetLabelSize(0.06);
    h->GetXaxis()->SetNdivisions(208);
    h->GetYaxis()->SetNdivisions(208);    
    h->GetYaxis()->SetTitleOffset(1.0);
}

//________________
int main(int argc, char const *argv[]) {

//int readEtroc2Boards() {

    gStyle->SetPalette(kBird);

    const Char_t *inFileName = "/Users/gnigmat/work/cms/soft/ETROC2/build/oEtroc2Trans.root";
    
    TFile *inFile = TFile::Open(inFileName);

    if ( !inFile->IsOpen() || inFile->IsZombie() ) {
        std::cout << "[ERROR] File is opened or zombie" << std::endl;
        return 1;
    }

    //
    // Histograms
    //

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
        set1DStyle(hNHits[i], i);
        hTOT[i] = new TH1D(Form("hTOT_%d", i), 
                           Form("TOT for the board #%d", i),
                           500, 0, 1500);
        set1DStyle(hTOT[i], i);
        hTOTvsTOA[i] = new TH2D(Form("hTOTvsTOA_%d",i), 
                                Form("TOT vs. TOA for board #%d;TOA;TOT",i), 
                                512, 0., 1024., 512, 0., 512.);
        set2DStyle(hTOTvsTOA[i]);
    }

    //
    // Create variables to fill
    //

    UInt_t runId;
    UInt_t loopId;
    UInt_t eventId;
    std::vector<Hit> *mHits{nullptr};

    //
    // Retrieve a tree
    //

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
    T->SetBranchAddress( "mHits", &mHits );

    //
    // Retrieve number of events to analyze and create variables
    //

    Long64_t nEntries = T->GetEntries();
    Int_t boardId{0};

    //
    // Loop over event in TTree and do analysis
    //

    for (Long64_t iEntry{0}; iEntry < nEntries; ++iEntry) {

        // Load the entry from the tree
        T->GetEntry( iEntry );

        h2->Fill( eventId );

        Int_t mNumberOfHitsPerBoard[4] = {0, 0, 0, 0};

        // Loop over hits in each board
        for (UInt_t iHit{0}; iHit<mHits->size(); iHit++) {

            // Retrieve the hit
            Hit hit = mHits->at( iHit );

            boardId = hit.boardNumber();

            // For the setups that do not contain more than 4 boards
            if ( TMath::Abs(boardId) > 4 ) continue;

            // Increment hit counter (per board)
            mNumberOfHitsPerBoard[ boardId ]++;
            hTOT[ boardId ]->Fill( hit.tot() );
            hTOTvsTOA[ boardId ]->Fill( hit.toa(), hit.tot() );
        }

        // Loop over boards to fill number of hits per board
        for (Int_t iBoard{0}; iBoard<4; iBoard++) {
            hNHits[iBoard]->Fill( mNumberOfHitsPerBoard[iBoard] );
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