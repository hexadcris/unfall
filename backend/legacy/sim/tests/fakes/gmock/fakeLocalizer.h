/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "Localization.h"

namespace World {
namespace Localization {

class FakeLocalizer : public Localizer
{
public:

    explicit FakeLocalizer(const OWL::Interfaces::WorldData& worldData) :
        Localizer(worldData) {};

    MOCK_METHOD(void, Init, ());

    MOCK_CONST_METHOD2(Locate, Result(polygon_t &, OWL::Interfaces::WorldObject &w));
    //Result Locate(const polygon_t& boundingBox, OWL::Interfaces::WorldObject& object) const;

    //GlobalRoadPositions Locate(const Common::Vector2d& point, const double& hdg) const;
    //MOCK_CONST_METHOD2( Locate, GlobalRoadPositions(Common::Vector2d &point, double &hdg));

    //void Unlocate(OWL::Interfaces::WorldObject& object) const;
    MOCK_METHOD(void, Unlocate, (OWL::Interfaces::WorldObject & object), (const));

    //MOCK_METHOD4(AddConnection, ConnectionInterface *(std::string, std::string, std::string, ContactPointType));
};

}
}