//  CutFlowManager.cpp
//
//  Created by Jeremi Niedziela on 16/08/2023.

#include "CutFlowManager.hpp"

#include "Helpers.hpp"
#include "Logger.hpp"

using namespace std;

CutFlowManager::CutFlowManager(shared_ptr<EventReader> eventReader_, shared_ptr<EventWriter> eventWriter_)
    : eventReader(eventReader_), eventWriter(eventWriter_), currentIndex(0) {
  
  auto &config = ConfigManager::GetInstance();

  try {
    config.GetValue("weightsBranchName", weightsBranchName);
  } catch (const Exception& e) {
    info() << "Weights branch not specified -- will assume weight is 1 for all events" << endl;
  }
  
  if (eventReader->inputFile->Get("CutFlow")) {
    info() << "Input file contains CutFlow directory - will store existing cutflow for output.\n";
    
    auto sourceDir = (TDirectory *)eventReader->inputFile->Get("CutFlow");

    TIter nextKey(sourceDir->GetListOfKeys());
    TKey *key;

    while ((key = dynamic_cast<TKey *>(nextKey()))) {
      TObject *obj = key->ReadObj();
      auto hist = (TH1D *)obj;
      weightsAfterCuts[key->GetName()] = hist->GetBinContent(1);
      existingCuts.push_back(key->GetName());
      delete obj;
      currentIndex++;
    }
  }
  if (!eventWriter_) info() << "No eventWriter given for CutFlowManager\n";
}

CutFlowManager::~CutFlowManager() {}

void CutFlowManager::UpdateCutFlow(string cutName) {
  
  string fullCutName;

  bool found = false;
  for (const auto &[key, value] : weightsAfterCuts) {
    if (key.find(cutName) != std::string::npos) {
      fullCutName = key;
      found = true;
      break;
    }
  }
  if (!found) {
    fullCutName = to_string(currentIndex) + "_" + cutName;
  }

  // If cutflow already exists, we don't want to overwrite it
  if (find(existingCuts.begin(), existingCuts.end(), fullCutName) != existingCuts.end()) {
    return;
  }

  float weight = 1.0;
  try {
    weight = eventReader->currentEvent->Get(weightsBranchName);
  } catch (...) {
    if(!weightsBranchWarningPrinted){
      error() << "Could not find weights branch " << weightsBranchName << endl;
      weightsBranchWarningPrinted = true;
    }
  }

  if (weightsAfterCuts.count(fullCutName)) {
    weightsAfterCuts[fullCutName] += weight;
  } else {
    weightsAfterCuts[fullCutName] = weight;
    currentIndex++;
  }
}

void CutFlowManager::SaveCutFlow() {
  if (!eventWriter) {
    error() << "No existing eventWriter for CutFlowManager - cannot save CutFlow" << endl;
  }
  eventWriter->outFile->mkdir("CutFlow");  
  eventWriter->outFile->cd("CutFlow");

  for (auto &[cutName, sumOfWeights] : weightsAfterCuts) {
    auto hist = new TH1D(cutName.c_str(), cutName.c_str(), 1, 0, 1);
    hist->SetBinContent(1, sumOfWeights);
    hist->Write();
  }
  eventWriter->outFile->cd();
}

std::map<std::string, float> CutFlowManager::GetCutFlow() { return weightsAfterCuts; }

void CutFlowManager::Print() {
  info() << "CutFlow:\n";
  for (auto &[cutName, sumOfWeights] : weightsAfterCuts) {
    info() << cutName << " " << sumOfWeights << "\n";
  }
}