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
 * OAIPosition.h
 *
 * 
 */

#ifndef OAIPosition_H
#define OAIPosition_H

#include <QJsonObject>


#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIPosition : public OAIObject {
public:
    OAIPosition();
    OAIPosition(QString json);
    ~OAIPosition() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getX() const;
    void setX(const qint32 &x);
    bool is_x_Set() const;
    bool is_x_Valid() const;

    qint32 getY() const;
    void setY(const qint32 &y);
    bool is_y_Set() const;
    bool is_y_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 x;
    bool m_x_isSet;
    bool m_x_isValid;

    qint32 y;
    bool m_y_isSet;
    bool m_y_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIPosition)

#endif // OAIPosition_H
