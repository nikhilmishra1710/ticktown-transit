#include "train_state_machine.hpp"
#include "Train.hpp"
#include <stdexcept>
#include <string>

void TrainFSM::movingToAlighting(Train& t) {
    if (t.state != TrainState::MOVING || t.progress != 1.0f) {
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
        throw std::logic_error("Invalid Train State in ALIGHTING->BOARDING " +
                               std::to_string(t.trainId));
    }
    t.state = TrainState::MOVING;
}