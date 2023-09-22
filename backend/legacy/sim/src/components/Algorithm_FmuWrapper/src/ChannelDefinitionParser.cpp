/********************************************************************************
 * Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <regex>
#include <string>
#include <tuple>
#include <typeindex>
#include <unordered_map>

#include "ChannelDefinitionParser.h"

ChannelDefinitionParser::ChannelDefinitionParser(const FmuVariables &fmuVariables, fmi_version_enu_t fmiVersion) :
    fmiVersion(fmiVersion),
    unmappedFmuVariables(fmuVariables)
{
    if (fmiVersion == fmi_version_enu_t::fmi_version_1_enu)
    {
        fmuOutputs.emplace<FMI1>();
        fmuRealInputs.emplace<FMI1>();
        fmuIntegerInputs.emplace<FMI1>();
        fmuBooleanInputs.emplace<FMI1>();
        fmuStringInputs.emplace<FMI1>();
        fmuIntegerParameters.emplace<FMI1>();
        fmuDoubleParameters.emplace<FMI1>();
        fmuBoolParameters.emplace<FMI1>();
        fmuStringParameters.emplace<FMI1>();
    }
    if (fmiVersion == fmi_version_enu_t::fmi_version_2_0_enu)
    {
        fmuOutputs.emplace<FMI2>();
        fmuRealInputs.emplace<FMI2>();
        fmuIntegerInputs.emplace<FMI2>();
        fmuBooleanInputs.emplace<FMI2>();
        fmuStringInputs.emplace<FMI2>();
        fmuIntegerParameters.emplace<FMI2>();
        fmuDoubleParameters.emplace<FMI2>();
        fmuBoolParameters.emplace<FMI2>();
        fmuStringParameters.emplace<FMI2>();
    }
}

bool ChannelDefinitionParser::AddOutputChannel(const std::string& outputType, const std::string& variableName)
{
    if (fmiVersion == fmi_version_enu_t::fmi_version_1_enu)
    {
        auto unmappedVariableItem = std::get<FMI1>(unmappedFmuVariables).find(variableName);
        auto fmuOutput = stringToSignalValueMap.find(outputType);

        if (unmappedVariableItem == std::get<FMI1>(unmappedFmuVariables).end())
        {
            return false;
        }

        if (fmuOutput == stringToSignalValueMap.end())
        {
            return false;
        }

        const auto& [valueReference, variableTypeInFmu] = unmappedVariableItem->second;
                const auto& [fmuOutputType, variableTypeInWrapper] = fmuOutput->second;

                if (variableTypeInFmu != variableTypeInWrapper)
        {
            return false;
        }

        std::get<FMI1>(fmuOutputs)[fmuOutputType] = valueReference;

        std::get<FMI1>(unmappedFmuVariables).erase(unmappedVariableItem);
    }
    else if (fmiVersion == fmi_version_enu_t::fmi_version_2_0_enu)
    {
        auto unmappedVariableItem = std::get<FMI2>(unmappedFmuVariables).find(variableName);
        auto fmuOutput = stringToSignalValueMap.find(outputType);

        if (unmappedVariableItem == std::get<FMI2>(unmappedFmuVariables).end())
        {
            return false;
        }

        if (fmuOutput == stringToSignalValueMap.end())
        {
            return false;
        }

        const auto& [valueReference, variableTypeInFmu] = unmappedVariableItem->second;
                const auto& [fmuOutputType, variableTypeInWrapper] = fmuOutput->second;

                if (variableTypeInFmu != variableTypeInWrapper)
        {
            return false;
        }

        std::get<FMI2>(fmuOutputs)[fmuOutputType] = valueReference;

        std::get<FMI2>(unmappedFmuVariables).erase(unmappedVariableItem);
    }

    return true;
}

bool ChannelDefinitionParser::AddInputChannel(const std::string& inputType, const std::string& variableName)
{

    const auto pos = inputType.find('_');
    const auto variableType = inputType.substr(0, pos);
    std::variant<double, size_t> additionalParameter;
    if (variableType == "SpeedLimit" || variableType == "RoadCurvature")
    {
        const auto rangeString = inputType.substr(pos + 1);
        additionalParameter = std::stod(rangeString);
    }
    else if (variableType.substr(0,12)  == "SensorFusion")
    {
        const auto indexString = inputType.substr(pos + 1);
        additionalParameter = static_cast<size_t>(std::stoul(indexString));
    }

    if (fmiVersion == fmi_version_enu_t::fmi_version_1_enu)
    {
        auto unmappedVariableItem = std::get<FMI1>(unmappedFmuVariables).find(variableName);
        auto fmuInput = stringToFmuTypeMap.find(variableType);

        if (unmappedVariableItem == std::get<FMI1>(unmappedFmuVariables).end())
        {
            return false;
        }

        if (fmuInput == stringToFmuTypeMap.end())
        {
            return false;
        }

        const auto& [valueReference, variableTypeInFmu] = unmappedVariableItem->second;
                const auto& [fmuInputType, variableTypeInWrapper] = fmuInput->second;

                if (variableTypeInFmu != variableTypeInWrapper)
        {
            return false;
        }

        if (variableTypeInFmu == VariableType::Double)
        {
            std::get<FMI1>(fmuRealInputs).emplace_back(fmuInputType, additionalParameter, valueReference);
        }
        else if (variableTypeInFmu == VariableType::Int)
        {
            std::get<FMI1>(fmuIntegerInputs).emplace_back(fmuInputType, additionalParameter, valueReference);
        }
        else if (variableTypeInFmu == VariableType::Bool)
        {
            std::get<FMI1>(fmuBooleanInputs).emplace_back(fmuInputType, additionalParameter, valueReference);
        }
        else if (variableTypeInFmu == VariableType::String)
        {
            std::get<FMI1>(fmuStringInputs).emplace_back(fmuInputType, additionalParameter, valueReference);
        }

        std::get<FMI1>(unmappedFmuVariables).erase(unmappedVariableItem);
    }
    else if (fmiVersion == fmi_version_enu_t::fmi_version_2_0_enu)
    {
        auto unmappedVariableItem = std::get<FMI2>(unmappedFmuVariables).find(variableName);
        auto fmuInput = stringToFmuTypeMap.find(variableType);

        if (unmappedVariableItem == std::get<FMI2>(unmappedFmuVariables).end())
        {
            return false;
        }

        if (fmuInput == stringToFmuTypeMap.end())
        {
            return false;
        }

        const auto& [valueReference, variableTypeInFmu] = unmappedVariableItem->second;
                const auto& [fmuInputType, variableTypeInWrapper] = fmuInput->second;

                if (variableTypeInFmu != variableTypeInWrapper)
        {
            return false;
        }

        if (variableTypeInFmu == VariableType::Double)
        {
            std::get<FMI2>(fmuRealInputs).emplace_back(fmuInputType, additionalParameter, valueReference);
        }
        else if (variableTypeInFmu == VariableType::Int)
        {
            std::get<FMI2>(fmuIntegerInputs).emplace_back(fmuInputType, additionalParameter, valueReference);
        }
        else if (variableTypeInFmu == VariableType::Bool)
        {
            std::get<FMI2>(fmuBooleanInputs).emplace_back(fmuInputType, additionalParameter, valueReference);
        }
        else if (variableTypeInFmu == VariableType::String)
        {
            std::get<FMI2>(fmuStringInputs).emplace_back(fmuInputType, additionalParameter, valueReference);
        }

        std::get<FMI2>(unmappedFmuVariables).erase(unmappedVariableItem);
    }
    return true;
}

FmuOutputs ChannelDefinitionParser::GetFmuOutputs()
{
    return fmuOutputs;
}

std::set<SignalType> ChannelDefinitionParser::GetOutputSignals()
{
    return outputSignals;
}

bool ChannelDefinitionParser::ParseOutputSignalTypes()
{
    std::vector<SignalType> nonExistingOutputSignals;
    for (const auto& [signalType, values] : valuesOfSignalType)
    {
        for (const auto signalValue : values)
        {
            if (fmiVersion == fmi_version_enu_t::fmi_version_1_enu)
            {
                auto valueFound = std::find_if(std::get<FMI1>(fmuOutputs).cbegin(), std::get<FMI1>(fmuOutputs).cend(),
                                               [&](const auto& pair) {return pair.first == signalValue;}) != std::get<FMI1>(fmuOutputs).cend();
                if (valueFound)
                {
                    outputSignals.emplace(signalType);
                    if (std::find(nonExistingOutputSignals.cbegin(), nonExistingOutputSignals.cend(), signalType) != nonExistingOutputSignals.cend())
                    {
                        return false;
                    }
                }
                else
                {
                    nonExistingOutputSignals.emplace_back(signalType);
                    if (std::find(outputSignals.cbegin(), outputSignals.cend(), signalType) != outputSignals.cend())
                    {
                        return false;
                    }
                }
            }
            else if (fmiVersion == fmi_version_enu_t::fmi_version_2_0_enu)
            {
                auto valueFound = std::find_if(std::get<FMI2>(fmuOutputs).cbegin(), std::get<FMI2>(fmuOutputs).cend(),
                                               [&](const auto& pair) {return pair.first == signalValue;}) != std::get<FMI2>(fmuOutputs).cend();
                if (valueFound)
                {
                    outputSignals.emplace(signalType);
                    if (std::find(nonExistingOutputSignals.cbegin(), nonExistingOutputSignals.cend(), signalType) != nonExistingOutputSignals.cend())
                    {
                        return false;
                    }
                }
                else
                {
                    nonExistingOutputSignals.emplace_back(signalType);
                    if (std::find(outputSignals.cbegin(), outputSignals.cend(), signalType) != outputSignals.cend())
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

FmuInputs ChannelDefinitionParser::GetFmuRealInputs()
{
    return fmuRealInputs;
}

FmuInputs ChannelDefinitionParser::GetFmuIntegerInputs()
{
    return fmuIntegerInputs;
}

FmuInputs ChannelDefinitionParser::GetFmuBooleanInputs()
{
    return fmuBooleanInputs;
}

FmuInputs ChannelDefinitionParser::GetFmuStringInputs()
{
    return fmuStringInputs;
}

FmuParameters<std::string> ChannelDefinitionParser::GetFmuStringParameters()
{
    return fmuStringParameters;
}

FmuParameters<int> ChannelDefinitionParser::GetFmuIntegerParameters()
{
    return fmuIntegerParameters;
}

FmuParameters<double> ChannelDefinitionParser::GetFmuDoubleParameters()
{
    return fmuDoubleParameters;
}

FmuParameters<bool> ChannelDefinitionParser::GetFmuBoolParameters()
{
    return fmuBoolParameters;
}
