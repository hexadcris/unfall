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
 * OAIVerifyPath_200_response.h
 *
 * 
 */

#ifndef OAIVerifyPath_200_response_H
#define OAIVerifyPath_200_response_H

#include <QJsonObject>

#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIVerifyPath_200_response : public OAIObject {
public:
    OAIVerifyPath_200_response();
    OAIVerifyPath_200_response(QString json);
    ~OAIVerifyPath_200_response() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    bool isOk() const;
    void setOk(const bool &ok);
    bool is_ok_Set() const;
    bool is_ok_Valid() const;

    QString getRealPath() const;
    void setRealPath(const QString &real_path);
    bool is_real_path_Set() const;
    bool is_real_path_Valid() const;

    bool isEmpty() const;
    void setEmpty(const bool &empty);
    bool is_empty_Set() const;
    bool is_empty_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    bool ok;
    bool m_ok_isSet;
    bool m_ok_isValid;

    QString real_path;
    bool m_real_path_isSet;
    bool m_real_path_isValid;

    bool empty;
    bool m_empty_isSet;
    bool m_empty_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIVerifyPath_200_response)

#endif // OAIVerifyPath_200_response_H
