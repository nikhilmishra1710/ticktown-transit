#include "train_state_machine.hpp"
#include "Train.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

void TrainFSM::idleToAlighting(Train& t) {
    std::cout << "Transitioning Train " << t.trainId << " from IDLE to ALIGHTING "
              << t.currentStationId << " " << t.nextStationId << std::endl;
    if (t.state != TrainState::IDLE) {
        throw std::logic_error("Invalid Train State in IDLE->ALIGHTING " +
                               std::to_string(t.trainId));
    }
    t.state = TrainState::ALIGHTING;
    t.progress = 0.0f;
}

void TrainFSM::movingToAlighting(Train& t) {
    std::cout << "Transitioning Train " << t.trainId << " from MOVING to ALIGHTING "
              << t.currentStationId << " " << t.nextStationId << std::endl;
    if (t.state != TrainState::MOVING || t.progress < 1.0f) {
        throw std::logic_error("Invalid Train State in MOVING->ALIGHTING " +
                               std::to_string(t.trainId));
    }
    t.state = TrainState::ALIGHTING;
    t.progress = 0.0f;
}

void TrainFSM::alightingToBoarding(Train& t) {
    if (t.state != TrainState::ALIGHTING || t.progress != 0.0f) {
        throw std::logic_error("Invalid Train State in ALIGHTING->BOARDING " +
                               std::to_string(t.trainId));
    }
    t.state = TrainState::BOARDING;
}

void TrainFSM::boardingToMoving(Train& t) {
    if (t.state != TrainState::BOARDING || t.progress != 0.0f) {
        throw std::logic_error("Invalid Train State in BOARDING->MOVING " +
                               std::to_string(t.trainId));
    }
    t.state = TrainState::MOVING;
}