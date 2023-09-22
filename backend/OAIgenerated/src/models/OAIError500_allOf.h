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
 * OAIError500_allOf.h
 *
 * 
 */

#ifndef OAIError500_allOf_H
#define OAIError500_allOf_H

#include <QJsonObject>

#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIError500_allOf : public OAIObject {
public:
    OAIError500_allOf();
    OAIError500_allOf(QString json);
    ~OAIError500_allOf() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    QString getMessage() const;
    void setMessage(const QString &message);
    bool is_message_Set() const;
    bool is_message_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    QString message;
    bool m_message_isSet;
    bool m_message_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIError500_allOf)

#endif // OAIError500_allOf_H
