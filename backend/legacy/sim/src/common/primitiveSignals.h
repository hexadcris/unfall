/********************************************************************************
 * Copyright (c) 2016-2021 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  primitiveSignals.h
//! @brief This file contains an interface to predefined primitive signals for
//!        usage by component models
//-----------------------------------------------------------------------------

#ifndef PRIMITIVESIGNALS_H
#define PRIMITIVESIGNALS_H

#include "include/modelInterface.h"

//-----------------------------------------------------------------------------
//! Primitive signal class for double values
//-----------------------------------------------------------------------------
class DoubleSignal : public ComponentStateSignalInterface
{
public:
    DoubleSignal(double inValue, ComponentState state = ComponentState::Undefined) : value(inValue)
    {
        componentState = state;
    }
    DoubleSignal(const DoubleSignal &) = default;
    DoubleSignal(DoubleSignal &&) = default;
    DoubleSignal &operator=(const DoubleSignal &) = default;
    DoubleSignal &operator=(DoubleSignal &&) = default;
    virtual ~DoubleSignal() = default;

    //-----------------------------------------------------------------------------
    //! Converts signal to string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        return std::to_string(value);
    }

    double value; //!< signal content
};

//-----------------------------------------------------------------------------
//! Primitive signal class for integer values
//-----------------------------------------------------------------------------
class IntSignal : public ComponentStateSignalInterface
{
public:
    IntSignal(int inValue, ComponentState state = ComponentState::Undefined) : value(inValue)
    {
        componentState = state;
    }
    IntSignal(const IntSignal &) = default;
    IntSignal(IntSignal &&) = default;
    IntSignal &operator=(const IntSignal &) = default;
    IntSignal &operator=(IntSignal &&) = default;
    virtual ~IntSignal() = default;

    //-----------------------------------------------------------------------------
    //! Converts signal to string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        return std::to_string(value);
    }

    int value; //!< signal content
};

//-----------------------------------------------------------------------------
//! Primitive signal class for boolean values
//-----------------------------------------------------------------------------
class BoolSignal : public ComponentStateSignalInterface
{
public:
    BoolSignal(bool inValue, ComponentState state = ComponentState::Undefined) : value(inValue)
    {
        componentState = state;
    }
    BoolSignal(const BoolSignal &) = default;
    BoolSignal(BoolSignal &&) = default;
    BoolSignal &operator=(const BoolSignal &) = default;
    BoolSignal &operator=(BoolSignal &&) = default;
    virtual ~BoolSignal() = default;

    //-----------------------------------------------------------------------------
    //! Converts signal to string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        return std::to_string(value);
    }

    bool value; //!< signal content
};

//-----------------------------------------------------------------------------
//! Primitive signal class for componentState values
//-----------------------------------------------------------------------------
class ComponentStateSignal : public ComponentStateSignalInterface
{
public:
    ComponentStateSignal(ComponentState state)
    {
        componentState = state;
    }
    ComponentStateSignal(const ComponentStateSignal&) = default;
    ComponentStateSignal(ComponentStateSignal&&) = default;
    ComponentStateSignal& operator=(const ComponentStateSignal&) = default;
    ComponentStateSignal& operator=(ComponentStateSignal&&) = default;
    virtual ~ComponentStateSignal() = default;

    //-----------------------------------------------------------------------------
    //! Converts signal to string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        return "";
    }
};

#endif // PRIMITIVESIGNALS_H
