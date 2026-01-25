#pragma once
#include "Passenger.hpp"

class PassengerFSM {
  public:
    static void waitingToOnTrain(Passenger&, TrainId);
    static void onTrainToTransferring(Passenger&, StationId);
    static void transferringToOnTrain(Passenger&, TrainId);
    static void onTrainToCompleted(Passenger&);
};
