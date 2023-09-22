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
 * OAIDefault200Response.h
 *
 * 
 */

#ifndef OAIDefault200Response_H
#define OAIDefault200Response_H

#include <QJsonObject>

#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIDefault200Response : public OAIObject {
public:
    OAIDefault200Response();
    OAIDefault200Response(QString json);
    ~OAIDefault200Response() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    QString getResponse() const;
    void setResponse(const QString &response);
    bool is_response_Set() const;
    bool is_response_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    QString response;
    bool m_response_isSet;
    bool m_response_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIDefault200Response)

#endif // OAIDefault200Response_H
