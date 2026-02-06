#pragma once
#include "Train.hpp"

class TrainFSM {
  public:
    static void idleToAlighting(Train&);
    static void movingToAlighting(Train&);
    static void alightingToBoarding(Train&);
    static void boardingToMoving(Train&);
};
