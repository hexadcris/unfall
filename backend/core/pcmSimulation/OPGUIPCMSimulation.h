/*
 * Copyright (c) 2023 Hexad GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 */

#pragma once

#ifndef OPGUI_PCM_SIMULATION_H
#define OPGUI_PCM_SIMULATION_H



#include <QString>
#include <QStringList>

#include "GUI_Definitions.h"
#include "ConfigurationGeneratorPcm/ConfigGeneratorPcm.h"

namespace OPGUIPCMSimulation
{

class PCMSimulation
{
    public:
        bool isInited = false;
        QString pathGeneratedOPSimulationManagerConfig;
        
        PCMSimulation() = default;
        ~PCMSimulation();

        bool Init(const QString &pcmDBPath,const QString &pathModulesFolder,const QString &pathPCMResultFolder,const QString &pathOpSimulationExe, QString &errorMsg);
        bool SetPathToConvertedCases(const QString &path);
        bool UIGenerateConfigOfPCMCases(const QStringList &pcmSelectedCasesForSimulation, QString &errorMsg);
        bool LoadCasesFromPcmFile(QString path, QStringList &pcm_cases,QString &errorMsg);
        bool GenerateSimulationXml(QList<OpenAPI::OAISimulationConfig> simConfigs,int logLevel,QString simulationManagerLogFile,QString libraries,
        QString simulationManagerXml,QString pathCore, QString &errorMsg);
        bool GenerateSimulationXmlUsingSharedConfig(QString &errorMsg);
        QString getPathGeneratedOPSimulationManagerConfig();
        void setAgentsCar1(const QStringList &agentsCar1 );
        void setAgentsCar2(const QStringList &agentsCar2  );
        void setAgentsOther(const QStringList &agentsOther);
  
    private:
        int variationCount = VARIATION_COUNT_DEFAULT;
        int initRandomSeed = INIT_RANDOM_SEED;
        bool CreateConfigs(ConfigGenerator &configGenerator, QStringList &pcmCaseIndexList, QStringList &otherSytemList, QStringList &car1SystemList, QStringList &car2SystemList);
        PCM_SimulationSet* ReadSimulationSetFromDb(QString pcmCase);
        void RelocateCog(PCM_SimulationSet *simulationSet);

        QString pcmDBPath;
        QStringList agentsCar1; 
        QStringList agentsCar2;
        QStringList agentsOther;
        QString pathModulesFolder;
        QString pathOpSimulationExe;
        QString pathPCMResultFolder;
        QString logLevel;
        QStringList pcmCases;
        QStringList pcmSelectedCasesForSimulation;
        ConfigGenerator* configGeneratorShared= nullptr;
};

}

#endif