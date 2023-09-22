/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2020 HLRS, University of Stuttgart
 *               2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  Stochastics.h
//! @brief This file contains the internal representation of the stochastics.
//!        For more information see stochastics.dll documentation.
//-----------------------------------------------------------------------------

#pragma once

#ifndef SIMULATION_STOCHASTICS_DEFINED

#include "common/log.h"
#include "include/stochasticsInterface.h"
#include "bindings/stochasticsBinding.h"
#include "bindings/stochasticsLibrary.h"

namespace core
{

class Stochastics : public StochasticsInterface
{
public:
    Stochastics(StochasticsBinding *stochasticsBinding) :
        stochasticsBinding(stochasticsBinding)
    {}

    Stochastics(const Stochastics&) = delete;
    Stochastics(Stochastics&&) = delete;
    Stochastics& operator=(const Stochastics&) = delete;
    Stochastics& operator=(Stochastics&&) = delete;

    virtual ~Stochastics() = default;

    int GetBinomialDistributed(int upperRangeNum, double probSuccess){
        return implementation->GetBinomialDistributed(upperRangeNum, probSuccess);
    }

    double GetUniformDistributed(double a, double b){
        return implementation->GetUniformDistributed(a, b);
    }

    double GetNormalDistributed(double mean, double stdDeviation){
        return implementation->GetNormalDistributed(mean, stdDeviation);
    }

    double GetExponentialDistributed(double lambda){
        return implementation->GetExponentialDistributed(lambda);
    }

    double GetGammaDistributed(double mean, double stdDeviation){
        return implementation->GetGammaDistributed(mean, stdDeviation);
    }

    double GetGammaDistributedShapeScale(double shape, double scale){
        return implementation->GetGammaDistributedShapeScale(shape, scale);
    }

    double GetLogNormalDistributed(double mean, double stdDeviation){
        return implementation->GetLogNormalDistributed(mean, stdDeviation);
    }

    double GetLogNormalDistributedMuSigma(double mu, double sigma){
        return implementation->GetLogNormalDistributedMuSigma(mu, sigma);
    }

    double GetSpecialDistributed(std::string distributionName, std::vector<double> args){
        return implementation->GetSpecialDistributed(distributionName, args);
    }

    double GetRandomCdfLogNormalDistributed(double mean, double stdDeviation){
        return implementation->GetRandomCdfLogNormalDistributed(mean, stdDeviation);
    }

    double GetPercentileLogNormalDistributed(double mean, double stdDeviation, double probability){
        return implementation->GetPercentileLogNormalDistributed(mean, stdDeviation, probability);
    }

    std::uint32_t GetRandomSeed() const{
        return implementation->GetRandomSeed();
    }

    void ReInit(){
        return implementation->ReInit();
    }

    void InitGenerator(std::uint32_t seed){
        return implementation->InitGenerator(seed);
    }

    bool Instantiate(std::string libraryPath)
    {
        if(!stochasticsBinding){
            return false;
        }else if(!implementation){
            implementation = stochasticsBinding->Instantiate(libraryPath);
            if(!implementation){
                return false;
            }
        }
        return true;
    }

private:
    StochasticsBinding *stochasticsBinding = nullptr;
    StochasticsInterface *implementation = nullptr;   
};

} // namespace core
#endif   // SIMULATION_STOCHASTICS_DEFINED
