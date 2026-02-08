#include "passenger_state_machine.hpp"
#include "Passenger.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

void PassengerFSM::waitingToOnTrain(Passenger& p, TrainId id) {
    if (p.state != PassengerState::WAITING || p.train.has_value()) {
        std::cerr << p << std::endl;
        throw std::logic_error("Passenger State Invalid Waiting->OnTrain: " +
                               std::to_string(p.passengerId));
    }

    p.state = PassengerState::ON_TRAIN;
    p.train = id;
    p.station.reset();
}

void PassengerFSM::onTrainToCompleted(Passenger& p) {
    if (p.state != PassengerState::ON_TRAIN || !p.train.has_value()) {
        std::cerr << p << std::endl;
        throw std::logic_error("Passenger State Invalid OnTrain->Completed: " +
                               std::to_string(p.passengerId));
    }

    p.state = PassengerState::COMPLETED;
    p.train.reset();
    p.station.reset();
}

void PassengerFSM::onTrainToTransferring(Passenger& p, StationId id) {
    if (p.state != PassengerState::ON_TRAIN || !p.train.has_value()) {
        std::cerr << p << std::endl;
        throw std::logic_error("Passenger State Invalid OnTrain->Transferring: " +
                               std::to_string(p.passengerId));
    }

    p.state = PassengerState::TRANSFERRING;
    p.train.reset();
    p.station = id;
}

void PassengerFSM::transferringToOnTrain(Passenger& p, TrainId id) {
    if (p.state != PassengerState::TRANSFERRING || p.train.has_value()) {
        std::cerr << p << std::endl;
        throw std::logic_error("Passenger State Invalid Transferring->OnTrain: " +
                               std::to_string(p.passengerId));
    }

    p.state = PassengerState::ON_TRAIN;
    p.train = id;
    p.station.reset();
}
