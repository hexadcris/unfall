/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2018-2019 in-tech GmbH
 *               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "common/opMath.h"
#include "common/hypot.h"

namespace OWL {
    namespace Primitive {

        struct Vector3 {
            double x;
            double y;
            double z;
        };

        struct Dimension {
            double length;
            double width;
            double height;
        };

        struct AbsCoordinate {
            double x;
            double y;
            double hdg;
        };

        struct AbsPosition {
            AbsPosition() = default;

            AbsPosition(const AbsPosition &) = default;

            AbsPosition operator+(const AbsPosition &ref) const {
                return {x + ref.x,
                        y + ref.y,
                        z + ref.z
                };
            }

            AbsPosition operator-(const AbsPosition &ref) const {
                return {x - ref.x,
                        y - ref.y,
                        z - ref.z
                };
            }

            double distance() const {
                return openpass::hypot(x, openpass::hypot(y, z));
            }

            void RotateYaw(double angle) {
                double cosValue = std::cos(angle);
                double sinValue = std::sin(angle);
                double newX = x * cosValue - y * sinValue;
                double newY = x * sinValue + y * cosValue;
                x = newX;
                y = newY;
            }

            double x;
            double y;
            double z;
        };

        struct AbsVelocity {
            double vx;
            double vy;
            double vz;
        };

        struct AbsAcceleration {
            double ax;
            double ay;
            double az;
        };

        struct AbsOrientation {
            double yaw;
            double pitch;
            double roll;
        };

        struct AbsOrientationRate {
            double yawRate;
            double pitchRate;
            double rollRate;
        };

        struct AbsOrientationAcceleration {
            double yawAcceleration;
            double pitchAcceleration;
            double rollAcceleration;
        };

        struct RoadCoordinate {
            double s;
            double t;
            double yaw;
        };

        struct RoadVelocity {
            double vs;
            double vt;
            double vz;
        };

        struct RoadAcceleration {
            double as;
            double at;
            double az;
        };

        struct RoadOrientationRate {
            double hdgRate;
        };

        struct LanePosition {
            double v;
            double w;
        };

        struct LaneVelocity {
            double vv;
            double vw;
            double vz;
        };

        struct LaneAcceleration {
            double av;
            double aw;
            double az;
        };

        struct LaneOrientation {
            double hdg;
        };

        struct LaneOrientationRate {
            double hdgRate;
        };

    } // namespace Primitive
} // namespace OWL
