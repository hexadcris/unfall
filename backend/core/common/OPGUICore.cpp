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

#include <QProcess>
#include <QException>
#include <QtConcurrent>
#include <QStack>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QAtomicInt> 
#include <QObject>
#include <QDateTime>

#include "OPGUICore.h"
#include "OPGUICoreGlobalConfig.h"
#include "OPGUIPCMSimulation.h"
#include "OPGUISystemEditor.h"
#include "OPGUIQtLogger.h"

namespace OPGUICore
{
    QAtomicInt isRunning = 0;  
    OPGUIPCMSimulation::PCMSimulation *sim = nullptr;

    bool isSimulationRunning()
    {
        return isRunning.loadRelaxed()==0?false:true;
    }

    void setSimulationRunning(bool value)
    {   
        int isRunningVal = value ? 1 : 0;
        isRunning.storeRelaxed(isRunningVal);
    }

    void setSimulation(OPGUIPCMSimulation::PCMSimulation *newSim) {
        sim = newSim;
    }

   OPGUIPCMSimulation::PCMSimulation* getSimulation() {
        return sim;
    }
    
    bool api_verify_path(OpenAPI::OAIPathRequest req, OpenAPI::OAIVerifyPath_200_response &resp)
    {
        auto& config = OPGUICoreGlobalConfig::getInstance();

        LOG_INFO("Initialising : api_verify_path");

        resp.setOk(false);
        resp.setEmpty(true);
        QString workspace_path = config.workspace();

        if(workspace_path.isEmpty())
        {
            LOG_ERROR("Error, Workspace path empty / invalid. Check config file.");
            return false;
        }
        else{
            LOG_INFO("workspace_path : " + workspace_path);
        }

        
        if(!req.isValid() || req.getPath().isEmpty())
        {
            LOG_ERROR("Internal Error, api_verify_path request path empty / invalid");
            return false;
        }   
        
        QString searched_path = req.getPath();
        LOG_INFO("Verify path requested : " + searched_path);

        int count = 0;
        bool isFile = false;
        bool isDir = false;
        bool isEmptyDir = false;
        bool found = false;

        QDir dir(workspace_path);
        QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

        QStack<QDir> dirsToCheck; // Using a stack to avoid recursion
        dirsToCheck.push(dir);
        while(!dirsToCheck.empty()){
            QDir currentDir = dirsToCheck.top();
            dirsToCheck.pop();

            QFileInfoList currentEntries = currentDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
            for(const QFileInfo &entry : currentEntries) {
                QString lastPart = entry.fileName();
                if(QDir::cleanPath(lastPart).remove(QChar('/')) == QDir::cleanPath(searched_path).remove(QChar('/'))) {
                    count++;
                    if(count > 1) {
                        LOG_INFO("Folder/File found more than once inside the workspace directory or subdirectories");
                        resp.setEmpty(true);
                        resp.setOk(false);
                        resp.setRealPath("Folder/File found more than once inside the workspace directory or subdirectories");
                        return true;
                        break;
                    }
                    LOG_INFO("Found searched item in the workspace with absoulte path:"+entry.absoluteFilePath());
                    isFile = entry.isFile();
                    isDir = entry.isDir();
                    isEmptyDir = isDir && QDir(entry.absoluteFilePath()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() == 0;

                    LOG_INFO(QString("Path is a %1").arg(isFile ? "file" : "folder"));
                    resp.setEmpty(isEmptyDir);
                    resp.setRealPath(entry.absoluteFilePath());
                    resp.setOk(true);
                    found = true;
                }
                if(entry.isDir()){
                    dirsToCheck.push(QDir(entry.absoluteFilePath()));
                }
            }
        }
        if(!found){
            LOG_INFO("Path was not found under the workspace:"+workspace_path);
            resp.setEmpty(true);
            resp.setRealPath("Path was not found in workspace");
            resp.setOk(false);
        }
        return true;
    }

    bool api_delete_information(OpenAPI::OAIPathRequest req, OpenAPI::OAIDefault200Response &res, QString &errorMsg)
    {
        LOG_INFO("Initialising : api_delete_information");

        if(!req.isValid() || req.getPath().isEmpty())
        {
            errorMsg="Internal Error, api_delete_information request path empty / invalid.";
            LOG_ERROR(errorMsg);
            return false;
        }

        LOG_INFO("Requested delete path : " + req.getPath());
        QString pathToDelete= req.getPath();

        if(!QDir(pathToDelete).exists())
        {
            errorMsg="Invalid Path, Path does not exist";
            LOG_ERROR(errorMsg);
            return false;
        }
        else
        {
            auto is_deleted = QDir(pathToDelete).removeRecursively() && QDir(pathToDelete).mkpath(".") && QDir(pathToDelete).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty();
            
            if(!is_deleted)
            {
                errorMsg="Error in deleting content. Check Permissions.";
                LOG_ERROR(errorMsg);
                return false;
            }
            else
            {
                LOG_INFO("Files deleted successfully. in path : " + req.getPath());
                res.setResponse("true");
                return true;
            } 
        }
    }

    bool api_run_opSimulationManager(OpenAPI::OAIDefault200Response &resp, QString &errorMsg)
    {   
        auto& config = OPGUICoreGlobalConfig::getInstance();
        LOG_INFO("Initialising : api_run_opSimulationManager");

        if(!isSimulationRunning())
        {
            setSimulationRunning(true); 
            
            if(config.pathOpenpassCore().isEmpty())
            {
                errorMsg="Error, PATH_OPENPASS_CORE path empty / invalid. Check config file.";
                LOG_ERROR(errorMsg);
                setSimulationRunning(false);
                return false;
            }

            if(config.opSimulationManagerExe().isEmpty())
            {
                errorMsg="Error, OPSIMULATION_MANAGER_EXE path empty / invalid. Check config file.";
                LOG_ERROR(errorMsg);
                setSimulationRunning(false);
                return false;
            }

            if(config.opSimulationManagerXml().isEmpty())
            {
                errorMsg="Error, OPSIMULATION_MANAGER_XML path empty / invalid. Check config file.";
                LOG_ERROR(errorMsg);
                setSimulationRunning(false);
                return false;
            }

            if(!QFile::exists(config.fullPathOpSimulationManagerExe())){
                errorMsg="Fatal Error During Initialisation, OpSimulation manager Executable  is not Found at : " + config.fullPathOpSimulationManagerExe();
                LOG_ERROR(errorMsg);
                setSimulationRunning(false);
                return false;
            }

            if(!QFile::exists(config.fullPathOpSimulationExe())){
                errorMsg="Fatal Error During Initialisation, OpSimulation Executable  is not Found at : " + config.fullPathOpSimulationExe();
                LOG_ERROR(errorMsg);
                setSimulationRunning(false);
                return false;
            }

            QtConcurrent::run(
                []()
                {
                    QString path_op_simulation_manager_exe = OPGUICoreGlobalConfig::getInstance().fullPathOpSimulationManagerExe();
                    LOG_INFO("Starting Simulation Manager with opSimulationManager at : " + path_op_simulation_manager_exe);

                    QStringList qArguments;
                    qArguments << "--config" << OPGUICoreGlobalConfig::getInstance().fullPathOpSimulationManagerXml();
                    LOG_INFO("Starting Simulation Manager as : " + qArguments[0] + " " + qArguments[1]);
        
                    QProcess* newProcess = new QProcess();

                    // Connect the finished signal of the process to its deleteLater slot. 
                    // This ensures that once the process is done, it schedules itself for deletion.
                    QObject::connect(newProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), newProcess, &QObject::deleteLater);

                    newProcess->start(path_op_simulation_manager_exe,qArguments);

                    if(newProcess->processId() == 0)
                    {
                        LOG_ERROR("Problem with opSimulationManager. Simulation process failed to start.");
                        setSimulationRunning(false);
                        return;
                    }

                    newProcess->waitForFinished(-1);
        
                    if(newProcess->exitCode() != 0)
                    {
                        LOG_ERROR("Simulation aborted. opSimulationManager returned with -1");
                        delete newProcess;
                        setSimulationRunning(false);
                        return;
                    }
                    else
                    {
                        LOG_INFO("Simulation Manager Finished, Results Generated.");
                        delete newProcess;
                        setSimulationRunning(false);
                        return;
                    }
            });
            //end of concurrent run
        
            resp.setResponse("Simulation started correctly");
            return true;
        }
        else
        {
            resp.setResponse("Simulation already running");
            LOG_INFO("Op Simulation Manager is still running. Please wait for last simulation to exit.");
            return false;
        }
    }

    bool api_export_opSimulationManager_xml(OpenAPI::OAIOpSimulationManagerXmlRequest req,OpenAPI::OAIDefault200Response &resp, QString &errorMsg)
    {
        LOG_INFO("Initialising : api_export_opSimulationManager_xml");
        OPGUIPCMSimulation::PCMSimulation sim;

        if (!req.isValid()){
            LOG_ERROR("Internal Error, api_export_opSimulationManager_xml request was not correctly set");
            return false;
        }

        bool success = sim.GenerateSimulationXml(req.getSimulationConfigs(),req.getLogLevel(),req.getLogFileSimulationManager(),
            req.getLibraries(),req.getSimulation(),OPGUICoreGlobalConfig::getInstance().pathOpenpassCore(),errorMsg);

        if(success)
        {
            resp.setResponse("opsimulation manager data exported correctly!");
            LOG_INFO("Xml file exported.");
            return true;
        }
        else
        {
            resp.setResponse("Xml file export failed");
            LOG_INFO("Xml file export failed.");
            return true;
        }
    }

    bool api_send_PCM_file(OpenAPI::OAIPathRequest req, OpenAPI::OAISelectedExperimentsRequest &resp, QString &errorMsg)
    {   
        auto& config = OPGUICoreGlobalConfig::getInstance();
        LOG_INFO("Initialising : api_send_PCM_file");
        if(sim != nullptr)
        {
            LOG_INFO("Old Instance of PCMSimulation plugin found, deleting.");
            delete sim;
        }

        sim = new OPGUIPCMSimulation::PCMSimulation();
        if(!req.isValid() || req.getPath().isEmpty()){
            errorMsg="Path to pcm database in request is empty / invalid.";
            LOG_ERROR(errorMsg);
            return false;
        }

        auto pcm_db_path = req.getPath();
        LOG_INFO("Recieved path of PCM DB file : " + pcm_db_path);

        bool simInited = sim->Init(pcm_db_path,config.fullPathModulesFolder(),config.pathConvertedCases(),config.fullPathOpSimulationExe(),errorMsg);

        if(!simInited){
            LOG_ERROR(errorMsg);
            return false;
        }

        QStringList pcm_case_list;

        if( sim->LoadCasesFromPcmFile(pcm_db_path,pcm_case_list,errorMsg))
        {
            LOG_INFO("PCM Cases found : " + pcm_case_list.join(", "));
            resp.setSelectedExperiments(pcm_case_list);
            return true;
        }
        else
        {
            resp.setSelectedExperiments(QList<QString>());
            LOG_ERROR(errorMsg);
            return false;
        }
     
    }

    bool api_path_to_converted_cases(OpenAPI::OAIPathRequest req, OpenAPI::OAIDefault200Response &resp, QString &errorMsg)
    {
        auto& config = OPGUICoreGlobalConfig::getInstance();
        LOG_INFO("Initialising : api_path_to_converted_cases");
        if(!req.isValid() || req.getPath().isEmpty())
        {
            errorMsg = "Error in setting path of converted cases. Path is empty / invalid.";
            LOG_ERROR(errorMsg);
            return false;
        }

        auto path_to_converted_cases = req.getPath();
        LOG_INFO("Path to converted cases request:" + path_to_converted_cases);

        if(sim != nullptr)
        {
            LOG_INFO("PCM Plugin already Initialised, changing the path to converted cases for current instance.");
            sim->SetPathToConvertedCases(path_to_converted_cases);              
        }
        config.setPathConvertedCases(path_to_converted_cases);
        LOG_DEBUG("PATH_CONVERTED_CASES has been correctly set in memory as:"+path_to_converted_cases); 

        if(config.modifyOrAddValueToConfigFile("PATH_CONVERTED_CASES", path_to_converted_cases))
        {
            LOG_INFO("PATH_CONVERTED_CASES has been set in configuration file as:"+path_to_converted_cases);
            resp.setResponse("PATH_CONVERTED_CASES has been correctly set.");
            return true;
        }
        else
        {
            errorMsg = "Failed to set PATH_CONVERTED_CASES in configuration file as:"+path_to_converted_cases;
            LOG_ERROR(errorMsg);
            return false;
        }
    }

    bool api_convert_to_configs(OpenAPI::OAIConfigsRequest req, OpenAPI::OAIDefault200Response &resp, QString &errorMsg) 
    {
        auto& config = OPGUICoreGlobalConfig::getInstance();
        LOG_INFO("Initialising : api_convert_to_configs");
        if(!req.isValid()){
            errorMsg = QString("Invalid JSON request");
            LOG_ERROR(errorMsg);
            return false;
        }

        if(req.getSelectedExperiments().empty())
        {
            errorMsg = QString("Config generation failed as no experiments where selected, please select at least one experiment.");
            LOG_ERROR(errorMsg);
            return false;
        }

        if(req.getSelectedSystems().empty() || req.getSelectedSystems().size()<2)
        {
            errorMsg = QString("Config generation failed since list of agents not properly set, please set at least two agents for the experiments.");
            LOG_ERROR(errorMsg);
            return false;
        }

        if(sim != nullptr)
        {
            QStringList listExperiments = req.getSelectedExperiments();

            //add the systems using the request... 
            sim->setAgentsCar1(QList<QString>{req.getSelectedSystems()[0].getFile()});
            sim->setAgentsCar2(QList<QString>{req.getSelectedSystems()[1].getFile()});
            if(req.getSelectedSystems().size()==2)
                sim->setAgentsOther(QList<QString>{req.getSelectedSystems()[1].getFile()});
            else
                sim->setAgentsOther(QList<QString>{req.getSelectedSystems()[2].getFile()});

            if(!sim->UIGenerateConfigOfPCMCases(listExperiments,errorMsg))
            {
                errorMsg = QString("Config generation failed with error:"+errorMsg);
                LOG_ERROR(errorMsg);
                return false;
            }

            resp.setResponse("Succesfully created config files under:" + config.pathConvertedCases());
            
            LOG_INFO("Results of config generation stored in : " + config.pathConvertedCases());
            return true;
        }
        else
        {
            errorMsg = QString("Config generation failed since PCM plugin was not initialised");
            LOG_ERROR(errorMsg);
            
            return false;
        }
    }

    bool api_export_to_simulations(OpenAPI::OAISelectedExperimentsRequest req, OpenAPI::OAIDefault200Response &resp, QString &errorMsg)
    {
        //selected experiments is unused
        LOG_INFO("Initialising : api_export_to_simulations");
        if(!req.isValid() || req.getSelectedExperiments().isEmpty())
        {
            errorMsg = "List of experiments for config conversion is invalid / empty."; 
            LOG_ERROR(errorMsg);
            return false;
        }

        if(sim != nullptr)
        {
            if(sim->GenerateSimulationXmlUsingSharedConfig(errorMsg)){
                LOG_INFO("Simulation Manager Config File created as:"+sim->pathGeneratedOPSimulationManagerConfig);
                resp.setResponse("Simulation Manager Config File created as:"+sim->pathGeneratedOPSimulationManagerConfig);
                return true;
            }
            else{
                LOG_ERROR(errorMsg);
                return false;
            }
        }
        else
        {
            errorMsg = "PCM Plugin is not initialised."; 
            LOG_ERROR(errorMsg);
            return false;
        }
    }

    bool api_get_components(QList<OpenAPI::OAIComponentUI> &resp, QString &errorMsg){
        auto& config = OPGUICoreGlobalConfig::getInstance();
        LOG_INFO("Initialising : api_get_components");

        OPGUISystemEditor::SystemEditor editor;

        bool success = editor.loadComponentsFromDirectory(config.fullPathComponentsFolder(), resp, errorMsg);

        if(success) {
            return true;
        } else {
            LOG_ERROR("Failed to load components under path: " + config.fullPathComponentsFolder());
            if (!errorMsg.isEmpty()) {
                LOG_ERROR("Details: " + errorMsg); 
            }
            return false;
        }
    };

    bool api_get_systems(QList<OpenAPI::OAISystemUI> &resp, QString &errorMsg){
        auto& config = OPGUICoreGlobalConfig::getInstance();
        LOG_INFO("Initialising : api_get_systems");

        OPGUISystemEditor::SystemEditor editor;

        bool success = editor.loadSystemsFromDirectory(config.fullPathUserSystemsFolder(),resp,errorMsg);
        if(success) {
            return true;
        } else {
            LOG_ERROR("Failed to load systems under path: " + config.fullPathUserSystemsFolder());
            if (!errorMsg.isEmpty()) {
                LOG_ERROR("Details: " + errorMsg); 
            }
            return false;
        }
    };

    bool api_export_systems_config(QList<OpenAPI::OAISystemUI> &req, OpenAPI::OAIDefault200Response &resp, QString &errorMsg){
        auto& config = OPGUICoreGlobalConfig::getInstance();
        LOG_INFO("Initialising : api_get_components");
        if(req.isEmpty())
        {
            LOG_ERROR("Received JSON list of systems is empty");
            errorMsg = QString("Received JSON list of systems is empty");
            return false;
        }
        
        OPGUISystemEditor::SystemEditor editor;

        bool success = editor.exportSystemsToXmlFiles(config.fullPathUserSystemsFolder(),req,errorMsg);
        if(success) {
            resp.setResponse("Systems correctly exported.");
            return true;
        } else {
            LOG_ERROR("Failed to export system XML files");
            if (!errorMsg.isEmpty()) {
                LOG_ERROR("Details: " + errorMsg); 
            }
            return false;
        }
    };
}


    
