#ifndef TTZHistogramsFiller_hpp
#define TTZHistogramsFiller_hpp

#include "Event.hpp"
#include "EventProcessor.hpp"
#include "Helpers.hpp"
#include "HistogramsHandler.hpp"

class TTZHistogramsFiller {
 public:
  TTZHistogramsFiller(std::shared_ptr<HistogramsHandler> histogramsHandler_);
  ~TTZHistogramsFiller();

  void Fill(const std::shared_ptr<Event> event);

 private:
  std::shared_ptr<HistogramsHandler> histogramsHandler;
  std::unique_ptr<EventProcessor> eventProcessor;
  std::string weightsBranchName;

  float GetWeight(const std::shared_ptr<Event> event);
};

#endif /* TTZHistogramsFiller_hpp */
