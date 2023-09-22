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

#ifndef OPGUI_CORE_H
#define OPGUI_CORE_H

#include "OAIPathRequest.h"
#include "OAIExportOpsimulationManagerXmlApiRequest.h"
#include "OAIOpSimulationManagerXmlRequest.h"
#include "OAISelectedExperimentsRequest.h"
#include "OAIVerifyPath_200_response.h"
#include "OAIDefault200Response.h"
#include "OAIBoolean200Response.h"
#include "OPGUIPCMSimulation.h"
#include "OAIComponentUI.h"
#include "OAISystemUI.h"
#include "OAIConfigsRequest.h"


namespace OPGUICore
{

    extern QAtomicInt isRunning;  
    extern OPGUIPCMSimulation::PCMSimulation *sim;
    /*
        Checks if a simulation is in progress.
    */
    bool isSimulationRunning();  
    void setSimulationRunning(bool value); 
    OPGUIPCMSimulation::PCMSimulation* getSimulation();
    void setSimulation(OPGUIPCMSimulation::PCMSimulation *newSim);
    

    /*
        POST
        /verify-path
        Verify if a path exists and has data
        
        Request :
        {
            "path": "/test"
        }

        Response :
        {
            "ok": true,
            "realPath": "os/workspace/path/to/folder/test1",
            "empty": false
        }       
    */
    bool api_verify_path(OpenAPI::OAIPathRequest req, OpenAPI::OAIVerifyPath_200_response &resp);
    
    /*
        POST
        /delete-information
        Detele information of given folder
        
        Request :
        {
            "path": "/os/workspace/path/to/file"
        }

        Response :
        {
            "ok": true
        }        
    */
    bool api_delete_information(OpenAPI::OAIPathRequest req, OpenAPI::OAIDefault200Response &resp, QString &errorMsg);
    
    /*
        GET
        /runOpSimulationManager
        execute opSimulationManager
        
        Request :
            NULL

        Response :
        {
            "key": "value"
        }      
    */
    bool api_run_opSimulationManager(OpenAPI::OAIDefault200Response &resp, QString &errorMsg);

    /*
        POST
        /exportOpsimulationManagerXml
        export valid data into the opSimulationManagerXml file
        
        Request :
        {
            "logLevel": 0,
            "logFileSimulationManager": "opSimulationManager.log",
            "simulation": "opSimulation",
            "libraries": "modules",
            "simulationConfig": {
                                    "numberOfExecutions": 1,
                                    "logFileSimulation": "opSimulation.log",
                                    "configurations": "configs",
                                    "results": "results"
                                }
        }

        Response :
        {
            "response": "opsimulation manager data exported correctly!"
        }     
    */
    bool api_export_opSimulationManager_xml(OpenAPI::OAIOpSimulationManagerXmlRequest req, OpenAPI::OAIDefault200Response &resp, QString &errorMsg);

    /*
        POST
        /sendPCMFile
        send a PCM formatted file to the backend to be processed
        
        Request :
        {
            "path": "/pcmFile.db"
        }

        Response :
        {
            "response": "pcm file sent correctly"
        }    
    */
    bool api_send_PCM_file(OpenAPI::OAIPathRequest req, OpenAPI::OAISelectedExperimentsRequest &resp, QString &errorMsg);

    /*
        POST
        /pathToConvertedCases
        path to save the selected PCM converted cases
        
        Request :
        {
            "path": "/converted/PCM/selected/path"
        }

        Response :
        {
            "selectedExperiments":  [
                                        "1",
                                        "2",
                                        "3",
                                        "4"
                                    ]
        }   
    */
    bool api_path_to_converted_cases(OpenAPI::OAIPathRequest req, OpenAPI::OAIDefault200Response &resp, QString &errorMsg);

    /*
        POST
        /converToConfigs
        convert the selected PCM cases into configuration folders
        
        Request :
        {
            "selectedExperiments":  [
                                        "1",
                                        "2",
                                        "3",
                                        "4"
                                    ]
        }

        Response :
        {
            "response": "location setted correctly"
        }   
    */
    bool api_convert_to_configs(OpenAPI::OAIConfigsRequest req, OpenAPI::OAIDefault200Response &resp, QString &errorMsg);

    /*
        POST
        /exportToSimulations
        execute pcm simulation manager and store the results
        
        Request :
        {
            "selectedExperiments":  [
                                        "1",
                                        "2",
                                        "3",
                                        "4"
                                    ]
        }

        Response :
        {
            "response": "location setted correctly"
        }   
    */
    bool api_export_to_simulations(OpenAPI::OAISelectedExperimentsRequest req, OpenAPI::OAIDefault200Response &resp, QString &errorMsg);

    bool api_get_components(QList<OpenAPI::OAIComponentUI> &resp, QString &errorMsg);

    bool api_export_systems_config(QList<OpenAPI::OAISystemUI> &req, OpenAPI::OAIDefault200Response &resp, QString &errorMsg);

    bool api_get_systems(QList<OpenAPI::OAISystemUI> &resp, QString &errorMsg);
}

#endif
