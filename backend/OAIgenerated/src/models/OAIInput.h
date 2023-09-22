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
 * OAIInput.h
 *
 * 
 */

#ifndef OAIInput_H
#define OAIInput_H

#include <QJsonObject>

#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIInput : public OAIObject {
public:
    OAIInput();
    OAIInput(QString json);
    ~OAIInput() override;

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

    qint32 getCardinality() const;
    void setCardinality(const qint32 &cardinality);
    bool is_cardinality_Set() const;
    bool is_cardinality_Valid() const;

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

    qint32 cardinality;
    bool m_cardinality_isSet;
    bool m_cardinality_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIInput)

#endif // OAIInput_H
