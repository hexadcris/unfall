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
 * OAIPathRequest.h
 *
 * 
 */

#ifndef OAIPathRequest_H
#define OAIPathRequest_H

#include <QJsonObject>

#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIPathRequest : public OAIObject {
public:
    OAIPathRequest();
    OAIPathRequest(QString json);
    ~OAIPathRequest() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    QString getPath() const;
    void setPath(const QString &path);
    bool is_path_Set() const;
    bool is_path_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    QString path;
    bool m_path_isSet;
    bool m_path_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIPathRequest)

#endif // OAIPathRequest_H
