// ROOT headers
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TString.h>

// C++ headers
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

// ETROC2 headers
#include "Hit.h"

//________________
void readInputFile(const Char_t* inFileName, TTree *T = nullptr);

//________________
int main(int argc, char const *argv[]) {

    const Char_t *inFileName="/Users/gnigmat/work/cms/data/etroc/Run_2_loop_1/TDC_Data_translated_0.nem";
    const Char_t *oFileName = "oEtroc2Trans.root";

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <inputFileName>" << std::endl;
        return 1;
    }

    // Data structure
    UInt_t runId{123};
    UInt_t loopId{1};
    UInt_t eventId{0};
    std::vector<Hit> Hits;

    Hit::Class()->IgnoreTObjectStreamer();

    // Create a TTree for events
    TTree *T = new TTree("T", "ETROC2 tree");

    T->Branch("mRunId",   &runId);
    T->Branch("mLoopId",  &loopId);
    T->Branch("mEventId", &eventId);
    T->Branch("mHits", &Hits);

    // Read input argument list 
    if (argc > 1) inFileName = argv[1];
    if (argc > 2) oFileName = argv[2];

    // Check if the filename ends with ".list"
    if (inFileName.length() >= 5 && inFileName.substr(filename.length() - 5) == ".list") {
        std::cout << "The filename ends with \".list\"" << std::endl;
    } 
    else if (inFileName.length() >= 4 && inFileName.substr(filename.length() - 4) == ".nem") {
        // Check if the filename ends with ".nem"
        std::cout << "The filename ends with \".nem\"" << std::endl;
        readInputFile(inFileName, T);
    } 
    else {
        std::cout << "The filename does not end with \".list\" or \".nem\"" << std::endl;
    }

    // Open the input file
    std::ifstream inputFile("inputFiles.list");

    if (!inputFile.is_open()) {
        std::cerr << "Error opening the input file." << std::endl;
        return 1;
    }

    // Regular expression to match the pattern "Run_<number>_loop_<number>"
    std::regex pattern("Run_(\\d+)_loop_(\\d+)");

    inputFile.close();



    return 0;
}

//________________
void readInputFile(const Char_t* inFileName, TTree *T = nullptr) {

    std::cout << Form("Read input file: %s\n", inFileName);
    std::ifstream inputFile(inFileName); 

    if (!inputFile.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        return 1;
    }

    // Variables to fill
    Int_t boardNumber{0}, errorCode{0}, column{0};
    Int_t row{0}, tot{0}, toa{0}, cal{0};    

    // Read events from text file line by line and create events

    std::cout << "Start data processing...";
    while (inputFile) {

        std::string line;
        // Read the line
        std::getline(inputFile, line);

        if (line.empty()) {
            // Skip empty lines
            continue;
        }

        if (line.substr(0, 2) == "EH") {
            // Beginning of event
            // std::cout << "New event" << std::endl;
        } 
        else if (line[0] == 'D') {

            //std::cout << "Data: " << line << std::endl;
            // Data entry (Hit)
            std::istringstream iss(line);
            char type;

            iss >> type >> boardNumber >> errorCode >> column >> row >> toa >> tot >> cal;

            // std::cout << Form("-- type: %c bn: %d ec: %d col: %d row: %d tot: %d toa: %d cal: %d\n",
            //                   type, boardNumber, errorCode, column, row, tot, toa, cal);

            //Hit hit(boardNumber, errorCode, column, row, tot, toa, cal);
            Hit hit;
            hit.setBoardNumber(boardNumber);
            hit.setErrorCode(errorCode);
            hit.setColumn(column);
            hit.setRow(row);
            hit.setTot(tot);
            hit.setToa(toa);
            hit.setCal(cal);

            // Add hit to a proper board
            Hits.push_back( hit );
        }
        else if (line.substr(0, 2) == "ET") {

            //std::cout << "Event trailor" << std::endl;
            // std::cout << Form("B1: %d B2: %d B3: %d B4: %d\n", board1.size(), board2.size(), board3.size(), board4.size());
            // for(UInt_t i{0}; i<board1.size(); i++) {
            //     std::cout << Form("Row: %d Column: %d\n", board1.at(i).row(), board1.at(i).column());
            // }

            T->Fill();
            Hits.clear();
            // Increment event counter
            eventId++;
        } 
        else if (line[0] == 'T') {
            // Handle trailer information if needed
        }
    } // while (inputFile)
    std::cout << "\t[DONE]" << std::endl;

    // Create a ROOT file to store the TTree
    TFile *oFile = new TFile(oFileName, "RECREATE");

    // Write the TTree to the file
    T->Write();

    // Write the TTree to the ROOT file
    oFile->Write();

    // Close the ROOT file
    oFile->Close();

    return 0;
}