/**
 * opGUI - OpenAPI 3.0
 * Dummy description about the available endpoints Some useful links: - [The opGUI repository](https://gitlab.eclipse.org/eclipse/openpass/opgui) - [The documentation page](https://www.eclipse.org/openpass/content/html/index.html)
 *
 * The version of the OpenAPI document: 3.0.1
 *
 * NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
 * https://openapi-generator.tech
 * Do not edit the class manually.
 */

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QDebug>
#include <QNetworkReply>

#include "OAIError500.h"
#include "OAIError400.h"
#include "OAICustomHelpers.h"
#include "OAIExportOpsimulationManagerXmlApiHandlerOP.h"
#include "OAIExportOpsimulationManagerXmlApiRequest.h"
#include "OAIDefault200Response.h"
#include "OPGUICore.h"


namespace OpenAPI {

    OAIExportOpsimulationManagerXmlApiHandlerOP::OAIExportOpsimulationManagerXmlApiHandlerOP() {
    }

    OAIExportOpsimulationManagerXmlApiHandlerOP::~OAIExportOpsimulationManagerXmlApiHandlerOP() {
    }

    void OAIExportOpsimulationManagerXmlApiHandlerOP::apiExportOpsimulationManagerXmlPost(OAIOpSimulationManagerXmlRequest oaiop_simulation_manager_xml_request) {
        auto reqObj = qobject_cast<OAIExportOpsimulationManagerXmlApiRequest*>(sender());
        QString errorMsg;
        OAIDefault200Response res;
        
        if( reqObj != nullptr )
        {
            bool success = OPGUICore::api_export_opSimulationManager_xml(oaiop_simulation_manager_xml_request,res,errorMsg);
            if (success){
                reqObj->apiExportOpsimulationManagerXmlPostResponse(res);
            }
            else{
                int code=500;
                OAIError500 error;
                error.setMessage("Server error");
                if (!errorMsg.isEmpty()) {
                    error.setMessage(error.getMessage() + " - Details: " + errorMsg);
                }
                handleSocketResponse(reqObj, error.asJsonObject(), code);
            }
        }
        else{
            int code=400;
            OAIError400 error;
            error.setMessage("Bad client request");
            handleSocketResponse(reqObj, error.asJsonObject(), code);
        }
    }

}

