#pragma once
#include "core/graph/Train.hpp"

class TrainFSM {
  public:
    static void movingToAlighting(Train&);
    static void alightingToBoarding(Train&);
    static void boardingToMoving(Train&);
};
