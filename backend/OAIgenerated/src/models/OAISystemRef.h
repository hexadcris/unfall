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
 * OAISystemRef.h
 *
 * 
 */

#ifndef OAISystemRef_H
#define OAISystemRef_H

#include <QJsonObject>

#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAISystemRef : public OAIObject {
public:
    OAISystemRef();
    OAISystemRef(QString json);
    ~OAISystemRef() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    QString getName() const;
    void setName(const QString &name);
    bool is_name_Set() const;
    bool is_name_Valid() const;

    QString getFile() const;
    void setFile(const QString &file);
    bool is_file_Set() const;
    bool is_file_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    QString name;
    bool m_name_isSet;
    bool m_name_isValid;

    QString file;
    bool m_file_isSet;
    bool m_file_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAISystemRef)

#endif // OAISystemRef_H
