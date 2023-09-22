/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef OPENPASS_OSIDEFAULTVALUES_H
#define OPENPASS_OSIDEFAULTVALUES_H

#include "osi3/osi_common.pb.h"

namespace OWL
{
    //Provides Default Values to Fill OSI Messages
    class OsiDefaultValues {
    public:
        OsiDefaultValues()
        {
            d_dimension.set_height(std::numeric_limits<double>::signaling_NaN());
            d_dimension.set_width(std::numeric_limits<double>::signaling_NaN());
            d_dimension.set_length(std::numeric_limits<double>::signaling_NaN());
            d_orientation.set_pitch(std::numeric_limits<double>::signaling_NaN());
            d_orientation.set_roll(std::numeric_limits<double>::signaling_NaN());
            d_orientation.set_yaw(std::numeric_limits<double>::signaling_NaN());
            d_3d.set_x(std::numeric_limits<double>::signaling_NaN());
            d_3d.set_y(std::numeric_limits<double>::signaling_NaN());
            d_3d.set_z(std::numeric_limits<double>::signaling_NaN());
            d_identifier.set_value(std::numeric_limits<google::protobuf::uint64>::max());
            d_uint32 = std::numeric_limits<google::protobuf::uint32>::max();
            d_double = std::numeric_limits<double>::max();
        }

        osi3::Dimension3d d_dimension {};
        osi3::Orientation3d d_orientation{};
        osi3::Vector3d d_3d {};
        osi3::Identifier d_identifier {};
        google::protobuf::uint32 d_uint32;
        std::string model_reference;
        double d_double;
    };
}


#endif //OPENPASS_OSIDEFAULTVALUES_H
