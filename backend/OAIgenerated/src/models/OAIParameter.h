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
 * OAIParameter.h
 *
 * 
 */

#ifndef OAIParameter_H
#define OAIParameter_H

#include <QJsonObject>

#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIParameter : public OAIObject {
public:
    OAIParameter();
    OAIParameter(QString json);
    ~OAIParameter() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getId() const;
    void setId(const qint32 &id);
    bool is_id_Set() const;
    bool is_id_Valid() const;

    QString getType() const;
    void setType(const QString &type);
    bool is_type_Set() const;
    bool is_type_Valid() const;

    QString getTitle() const;
    void setTitle(const QString &title);
    bool is_title_Set() const;
    bool is_title_Valid() const;

    QString getUnit() const;
    void setUnit(const QString &unit);
    bool is_unit_Set() const;
    bool is_unit_Valid() const;

    QString getValue() const;
    void setValue(const QString &value);
    bool is_value_Set() const;
    bool is_value_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 id;
    bool m_id_isSet;
    bool m_id_isValid;

    QString type;
    bool m_type_isSet;
    bool m_type_isValid;

    QString title;
    bool m_title_isSet;
    bool m_title_isValid;

    QString unit;
    bool m_unit_isSet;
    bool m_unit_isValid;

    QString value;
    bool m_value_isSet;
    bool m_value_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIParameter)

#endif // OAIParameter_H
