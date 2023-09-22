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

/*
 * OAIBoolean200Response.h
 *
 * 
 */

#ifndef OAIBoolean200Response_H
#define OAIBoolean200Response_H

#include <QJsonObject>


#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIBoolean200Response : public OAIObject {
public:
    OAIBoolean200Response();
    OAIBoolean200Response(QString json);
    ~OAIBoolean200Response() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    bool isResponse() const;
    void setResponse(const bool &response);
    bool is_response_Set() const;
    bool is_response_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    bool response;
    bool m_response_isSet;
    bool m_response_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIBoolean200Response)

#endif // OAIBoolean200Response_H
