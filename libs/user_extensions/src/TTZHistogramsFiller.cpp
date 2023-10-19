#include "TTZHistogramsFiller.hpp"

#include "ConfigManager.hpp"
#include "ExtensionsHelpers.hpp"

using namespace std;

TTZHistogramsFiller::TTZHistogramsFiller(shared_ptr<HistogramsHandler> histogramsHandler_) : histogramsHandler(histogramsHandler_) {
  // Create a config manager
  auto &config = ConfigManager::GetInstance();

  // Try to read weights branch
  try {
    config.GetValue("weightsBranchName", weightsBranchName);
  } catch (const Exception& e) {
    info() << "Weights branch not specified -- will assume weight is 1 for all events" << endl;
  }

  // Create an event processor
  eventProcessor = make_unique<EventProcessor>();
}

TTZHistogramsFiller::~TTZHistogramsFiller() {}

float TTZHistogramsFiller::GetWeight(const std::shared_ptr<Event> event) {
  // Try to get event weight, otherwise set to 1.0
  float weight = 1.0;
  try {
    weight = event->Get(weightsBranchName);
  } catch (...) {
  }
  return weight;
}

void TTZHistogramsFiller::Fill(const std::shared_ptr<Event> event) {

  // Extract a collection from the event                                                                                                                                                        
  auto muonObjects = event->GetCollection("Muon");
  auto electronObjects = event->GetCollection("Electron");
  
  // Loop over the collection                                                                                                                                                                   
  for (auto muonObject : *muonObjects) {
    for (auto electronObject : *electronObjects) {
      
      // Get a branch value using its name                                                                                                                                                        
      float muonPhi = muonObject->Get("phi");
      float electronPhi = electronObject->Get("phi");
      
      float deltaPhi = fabs(muonPhi - electronPhi);
      
      // Fill the histogram for given event (e.g. use EventProcessor to get some variables)
      histogramsHandler->Fill("deltaPhi", deltaPhi, GetWeight(event));
      
    }
  }


}
