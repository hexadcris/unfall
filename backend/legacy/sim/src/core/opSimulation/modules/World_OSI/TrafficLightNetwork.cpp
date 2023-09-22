/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrafficLightNetwork.h"
#include "include/dataBufferInterface.h"
#include "commonTools.h"

TrafficLightNetwork::TrafficLightNetwork()
{

}

void TrafficLightNetwork::UpdateStates(int time)
{
    for (auto& controller : controllers)
    {
        controller.UpdateStates(time);
    }
}

void TrafficLightNetwork::AddController(TrafficLightController &&controller)
{
    controllers.push_back(std::move(controller));
}

void TrafficLightNetwork::PublishGlobalData(AcyclicPublisher publish) {

    for (auto& controller : controllers) {


        if(controller.getTimeToPublish())
        {
            TrafficLightController::Phase phase = controller.getCurrentPhase();
            for (auto& [trafficLight, state] : phase.states)
            {
                const auto signalId = trafficLight->GetId();
                publish(phase.osiId, "TrafficLightController", Acyclic{"TrafficLight", {{phase.osiId}}, {{phase.osiId}}, {{"traffic_light_state", phase.stateDescriptor},{"opendrive_id", signalId}}});
                controller.setTimeToPublishFalse();
            }
        }
    }
}


void TrafficLightController::UpdateStates(int time)
{
    double deltaTime = time - lastTime;
    lastTime = time;
    timeRemainingInCurrentPhase -= deltaTime;
    while (timeRemainingInCurrentPhase <= 0)
    {
        currentPhase++;
        if (currentPhase == phases.cend())
        {
            currentPhase = phases.begin();
        }
        timeRemainingInCurrentPhase += currentPhase->duration;
        for (auto [trafficLight, newState] : currentPhase->states)
        {
            trafficLight->SetState(newState);
        }
        timeToPublish = true;
    }
}

TrafficLightController::TrafficLightController( std::vector<TrafficLightController::Phase> &&phases, double delay) :
    phases(phases),
    currentPhase(this->phases.begin()),
    timeRemainingInCurrentPhase(currentPhase->duration + delay),
    lastTime(0)
{
    for (auto [trafficLight, newState] : currentPhase->states)
    {
        trafficLight->SetState(newState);
    }
    UpdateStates(0);
}
