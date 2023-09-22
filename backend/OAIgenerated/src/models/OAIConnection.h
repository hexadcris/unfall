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
 * OAIConnection.h
 *
 * 
 */

#ifndef OAIConnection_H
#define OAIConnection_H

#include <QJsonObject>

#include "OAISource.h"
#include "OAITarget.h"

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIConnection : public OAIObject {
public:
    OAIConnection();
    OAIConnection(QString json);
    ~OAIConnection() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getId() const;
    void setId(const qint32 &id);
    bool is_id_Set() const;
    bool is_id_Valid() const;

    OAISource getSource() const;
    void setSource(const OAISource &source);
    bool is_source_Set() const;
    bool is_source_Valid() const;

    OAITarget getTarget() const;
    void setTarget(const OAITarget &target);
    bool is_target_Set() const;
    bool is_target_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 id;
    bool m_id_isSet;
    bool m_id_isValid;

    OAISource source;
    bool m_source_isSet;
    bool m_source_isValid;

    OAITarget target;
    bool m_target_isSet;
    bool m_target_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIConnection)

#endif // OAIConnection_H
