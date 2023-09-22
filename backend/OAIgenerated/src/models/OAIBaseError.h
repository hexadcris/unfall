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
 * OAIBaseError.h
 *
 * 
 */

#ifndef OAIBaseError_H
#define OAIBaseError_H

#include <QJsonObject>


#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIBaseError : public OAIObject {
public:
    OAIBaseError();
    OAIBaseError(QString json);
    ~OAIBaseError() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getCode() const;
    void setCode(const qint32 &code);
    bool is_code_Set() const;
    bool is_code_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 code;
    bool m_code_isSet;
    bool m_code_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIBaseError)

#endif // OAIBaseError_H
