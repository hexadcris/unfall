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
 * OAIError500.h
 *
 * 
 */

#ifndef OAIError500_H
#define OAIError500_H

#include <QJsonObject>

#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIError500 : public OAIObject {
public:
    OAIError500();
    OAIError500(QString json);
    ~OAIError500() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getCode() const;
    void setCode(const qint32 &code);
    bool is_code_Set() const;
    bool is_code_Valid() const;

    QString getMessage() const;
    void setMessage(const QString &message);
    bool is_message_Set() const;
    bool is_message_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 code;
    bool m_code_isSet;
    bool m_code_isValid;

    QString message;
    bool m_message_isSet;
    bool m_message_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIError500)

#endif // OAIError500_H
