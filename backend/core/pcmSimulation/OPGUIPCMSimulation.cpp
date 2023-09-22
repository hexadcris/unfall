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

#include <QFile>
#include <QDir>
#include <QException>

#include <OPGUIPCMSimulation.h>
#include <OPGUIQtLogger.h>


namespace OPGUIPCMSimulation
{
    PCMSimulation::~PCMSimulation()
    {
        this->pcmDBPath = QStringLiteral("");
        this->agentsCar1.clear();
        this->agentsCar2.clear();
        this->agentsOther.clear();
        this->pathModulesFolder = QStringLiteral("");
        this->pathOpSimulationExe = QStringLiteral("");
        this->pathPCMResultFolder = QStringLiteral("");
        this->logLevel = QStringLiteral("");
    
        this->pcmCases.clear();
        this->pcmSelectedCasesForSimulation.clear();

        this->pathGeneratedOPSimulationManagerConfig = QStringLiteral("");

        if (this->configGeneratorShared) {
            delete this->configGeneratorShared;
            this->configGeneratorShared = nullptr;
        }
    }

    QString PCMSimulation::getPathGeneratedOPSimulationManagerConfig(){
        return this->pathGeneratedOPSimulationManagerConfig;
    }

    bool PCMSimulation::Init(const QString &pcmDBPath,const QString &pathModulesFolder,const QString &pathPCMResultFolder,const QString &pathOpSimulationExe, QString &errorMsg)
    {
        this->isInited = true;
        // Initialised Using APIS
        this->logLevel = QStringLiteral("");
        this->pathGeneratedOPSimulationManagerConfig = QStringLiteral("");

        LOG_INFO("Initialising PCM Simulation Plugin:");

        if(pcmDBPath.isEmpty() || !QFile::exists(pcmDBPath)){
            errorMsg="PCM DB file path empty / invalid: "+pcmDBPath;
            LOG_ERROR(errorMsg);
            this->isInited = false;
        }
        else{
            this->pcmDBPath = pcmDBPath;
            LOG_INFO("With PCM DB file path: " + pcmDBPath);
        }

        if(pathModulesFolder.isEmpty() || !QDir(pathModulesFolder).exists())
        {
            errorMsg="Modules folder path empty / invalid: "+pathModulesFolder;
            LOG_ERROR(errorMsg);
            this->isInited = false;    
        }
        else{
            this->pathModulesFolder = pathModulesFolder;
            LOG_INFO("With Modules folder path: " + pathModulesFolder);
        }

        if(pathPCMResultFolder.isEmpty() || !QDir(pathPCMResultFolder).exists())
        {
            errorMsg="Path to converted cases is empty/invalid:" + pathPCMResultFolder;
            LOG_ERROR(errorMsg);
            this->isInited = false;
        }
        else{
            this->pathPCMResultFolder = pathPCMResultFolder;
            LOG_INFO("With path to converted cases: " + pathPCMResultFolder);
        }

        if(pathOpSimulationExe.isEmpty() || !QFile::exists(pathOpSimulationExe))
        {
            errorMsg="opSimulation executable file path empty / invalid:" + pathOpSimulationExe;
            LOG_ERROR(errorMsg);
            this->isInited = false;    
        }
        else{
            this->pathOpSimulationExe = pathOpSimulationExe;
            LOG_INFO("With opSimulation executable file path: " + pathOpSimulationExe);
        }

        return this->isInited;
    }

    bool PCMSimulation::UIGenerateConfigOfPCMCases(const QStringList &pcmSelectedCasesForSimulation, QString &errorMsg)
    {
        LOG_INFO("Generating set of configs");

        this->pcmSelectedCasesForSimulation = pcmSelectedCasesForSimulation;
        LOG_INFO("Selected cases: " + pcmSelectedCasesForSimulation.join(", "));
        
        LOG_INFO("Agent Car 1 file path: " + this->agentsCar1.first());
        if(this->agentsCar1.empty() || !QFile::exists(this->agentsCar1.first())){
            errorMsg = "Agent Car 1 file path empty / invalid: "+this->agentsCar1.first();
            LOG_ERROR(errorMsg);
            return false;
        }

        LOG_INFO("Agent Car 2 file path: " + this->agentsCar2.first());
        if(this->agentsCar2.empty() || !QFile::exists(this->agentsCar2.first())){
            errorMsg = "Agent Car 2 file path empty / invalid: "+this->agentsCar2.first();
            LOG_ERROR(errorMsg);
            return false;
        }

        LOG_INFO("Agent Car Other file path: " + this->agentsOther.first());
        if(this->agentsOther.empty() || !QFile::exists(this->agentsOther.first())){
            errorMsg = "Agent Car Other file path empty / invalid: "+this->agentsOther.first();
            LOG_ERROR(errorMsg);
            return false;
        }

        if (this->configGeneratorShared) {
            delete this->configGeneratorShared;
        }

        this->configGeneratorShared = new ConfigGenerator(this->pathPCMResultFolder);
        
        bool configsCreated = CreateConfigs(*this->configGeneratorShared,
                  this->pcmSelectedCasesForSimulation,
                  this->agentsOther,
                  this->agentsCar1,
                  this->agentsCar2);

        return configsCreated;
    }
    
    bool PCMSimulation::LoadCasesFromPcmFile(QString path,QStringList &pcm_cases,QString &errorMsg)
    {
        
        LOG_INFO("Loading PCM DB.");
        
        DatabaseReader dbReader;
        
        if(path.isEmpty())
        {
            errorMsg="PCM DB File path is Empty / Invalid";
            LOG_ERROR(errorMsg);
            return false;
        }

        LOG_INFO("Reading DB located at : " + path);

        dbReader.SetDatabase(path);

        bool success = dbReader.OpenDataBase();

        if (success)
        {
            LOG_INFO("DB Reading Success.");
            success = dbReader.ReadCaseList(pcm_cases);
        }

        if (!success)
        {
            errorMsg="DB Reading failed.";
            LOG_ERROR(errorMsg);
            return false;
        }

        dbReader.CloseDataBase();

        return true;
    }

    bool PCMSimulation::CreateConfigs(ConfigGenerator &configGenerator, 
                                        QStringList &pcmCaseIndexList, 
                                        QStringList &otherSytemList, 
                                        QStringList &car1SystemList, 
                                        QStringList &car2SystemList)
    {
        for (auto &pcmCaseIndex : this->pcmSelectedCasesForSimulation)
        {

            // Prepare to read a simulation set
            QString pcmCase = QString("%1").arg(pcmCaseIndex.toInt());
            PCM_SimulationSet *simulationSet = ReadSimulationSetFromDb(pcmCase);
            if (simulationSet == nullptr)
            {
                LOG_ERROR("Failed to read data for case: " + pcmCase);
                return false;
            }

            int otherSystemCount = 0;
            for (QString otherSystem : otherSytemList)
            {
                if (otherSystem.isEmpty())
                {
                    continue;
                }
                int car1SystemCount = 0;
                for (QString car1System : car1SystemList)
                {
                    if (car1System.isEmpty())
                    {
                        continue;
                    }
                    int car2SystemCount = 0;
                    for (QString car2System : car2SystemList)
                    {
                        if (car2System.isEmpty())
                        {
                            continue;
                        }

                        QString systemName = QString::number(otherSystemCount) + "-" + QString::number(car1SystemCount) + "-" + QString::number(car2SystemCount);
                        QString caseSystemName = pcmCase + "/" + systemName;

                        for (int varIndex = 0; varIndex <= variationCount; varIndex++)
                        {
                            QString varName = QStringLiteral("");;
                            bool withVariation = false;

                            if (varIndex == 0)
                            {
                                varName = DIR_NO_VARIATION;
                                withVariation = false;
                            }
                            else
                            {
                                varName = QString("Var_%1").arg(varIndex, 5, 10, QChar('0')); // zero padding if var index less than 5 digits
                                withVariation = true;
                            }

                            QString caseSystemVarName = caseSystemName; 

                            // the random seed uses PCM case number if the inital seed is negative. Otherwise it uses the inital seed.
                            int randomSeed = (initRandomSeed < 0) ? (pcmCaseIndex.toInt() + varIndex)
                                                              : (initRandomSeed + varIndex);

                            if (!configGenerator.GenerateConfigs(pcmCase,
                                                             caseSystemVarName,
                                                             otherSystem, car1System, car2System,
                                                             randomSeed,
                                                             simulationSet))
                            {

                                LOG_ERROR("Failed to generate configuration file for case: " + pcmCase);
      
                                if (simulationSet != nullptr)
                                {
                                    delete simulationSet;
                                }

                                return false;
                            }
                        }

                        car2SystemCount++;
                    }
                    car1SystemCount++;
                }
                otherSystemCount++;
            }

            if (simulationSet != nullptr)
            {
                delete simulationSet;
            }
        }
        
        return true;
    }

    PCM_SimulationSet *PCMSimulation::ReadSimulationSetFromDb(QString pcmCase)
    {
        // Read from database
        DatabaseReader dbReader; 
        dbReader.SetDatabase(this->pcmDBPath);
        PCM_SimulationSet *simulationSet = dbReader.Read(pcmCase);

        if (simulationSet == nullptr)
        {
            LOG_ERROR("Failed to read database for case: " + pcmCase);
            return nullptr;
        }

        // Relocate trajectory because pcm cog is in the geometric middle of the car
        // but simulated cog is between the rear axle
        RelocateCog(simulationSet);

        return simulationSet;
    }

    void PCMSimulation::RelocateCog(PCM_SimulationSet *simulationSet)
    {
        for (uint i = 0; i < simulationSet->GetTrajectories().size(); i++)
        {
            if (simulationSet->GetParticipants().at(i)->GetType() == QStringLiteral("0"))
            {
                double wheelBase = simulationSet->GetParticipants().at(i)->GetWheelbase().toDouble();
                double distCgfa = simulationSet->GetParticipants().at(i)->GetDistcgfa().toDouble();
                double distanceRearAxleToCOG = wheelBase - distCgfa;

                simulationSet->GetTrajectories().at(i)->ShiftForward(distanceRearAxleToCOG);
            }
        }
    }

    bool PCMSimulation::GenerateSimulationXml(QList<OpenAPI::OAISimulationConfig> simConfigs,int logLevel,QString simulationManagerLogFile,QString libraries,QString simulationManagerXml,QString pathCore, QString &errorMsg)
    {
        if(pathCore.isEmpty())
        {
            errorMsg="Error, PATH_OPENPASS_CORE path empty / invalid. Check config file.";
            LOG_ERROR(errorMsg);
            return false;
        }

        if(simConfigs.empty())
        {
            errorMsg="Internal Error, simulation configs are empty / invalid.";
            LOG_ERROR(errorMsg);
            return false;
        }

        if(logLevel < 0 || logLevel > 5)
        {
            errorMsg="Internal Succes, log level is not invalid.";
            LOG_ERROR(errorMsg);
            return false;
        }

        if(simulationManagerLogFile.isEmpty())
        {
            errorMsg="Internal Error, log file simulation manager is empty / invalid.";
            LOG_ERROR(errorMsg);
            return false;
        }

        if(libraries.isEmpty())
        {
            errorMsg="Internal Error, libraries path is empty / invalid.";
            LOG_ERROR(errorMsg);
            return false;
        }

        if(simulationManagerXml.isEmpty())
        {
            errorMsg="Internal Error, simulationManager xml path is empty / invalid.";
            LOG_ERROR(errorMsg);
            return false;
        }

        ConfigGenerator configGenerator(pathCore);
        LOG_INFO("Config Generator Initialised");
        auto file_path = configGenerator.GenerateFrameworkConfigV2(pathCore,logLevel,simulationManagerLogFile,simulationManagerXml,libraries,simConfigs);

        if(file_path.isEmpty())
        {
            errorMsg="OPSIMULATIONMANAGER XML GENERATION FAILED";
            LOG_ERROR(errorMsg);
            return false;            
        }
        else
        {
            LOG_INFO("OPSIMULATION MANAGER GENERATED");
            return true;
        }
    }

    bool PCMSimulation::GenerateSimulationXmlUsingSharedConfig( QString &errorMsg)
    {
        if (!this->configGeneratorShared) {
            errorMsg="Config Generator was not initialised.";
            LOG_ERROR(errorMsg);
            return false;
        }

        int logLevelDefault = 3;

        LOG_INFO("opSimulationManager.xml Construction started.");
    
        this->pathGeneratedOPSimulationManagerConfig = this->configGeneratorShared->GenerateFrameworkConfig(logLevelDefault);
        
        if (this->pathGeneratedOPSimulationManagerConfig.isEmpty())
        {
            errorMsg="opSimulationManager.xml Construction failed.";
            LOG_ERROR(errorMsg);
            return false;
        }

        // Append timestamp to config file name
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString timestamp = currentDateTime.toString("yyMMddHHmm");
        
        QFileInfo fileInfo(this->pathGeneratedOPSimulationManagerConfig);
        QString newFilename = fileInfo.baseName() + "_" + timestamp + "." + fileInfo.completeSuffix();
        QString fullPath = fileInfo.path() + "/" + newFilename;

        if (QFile::rename(this->pathGeneratedOPSimulationManagerConfig, fullPath))
        {
            LOG_INFO("Config File renamed successfully to: "+newFilename);
            this->pathGeneratedOPSimulationManagerConfig = fullPath;
        }
        else
        {
            errorMsg="Failed to rename the config file and append date/time";
            LOG_ERROR(errorMsg);
            return false;
        }

        return true;
    }

    bool PCMSimulation::SetPathToConvertedCases(const QString &path)
    {
        this->pathPCMResultFolder = path;
        return true;
    }

    void PCMSimulation::setAgentsCar1(const QStringList &agentsCar1){
        this->agentsCar1 = agentsCar1;
    };
    void PCMSimulation::setAgentsCar2(const QStringList &agentsCar2){
        this->agentsCar2 = agentsCar2;
    };
    void PCMSimulation::setAgentsOther(const QStringList &agentsOther){
        this->agentsOther = agentsOther;
    };

}    
