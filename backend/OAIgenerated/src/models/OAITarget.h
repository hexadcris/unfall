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
 * OAITarget.h
 *
 * 
 */

#ifndef OAITarget_H
#define OAITarget_H

#include <QJsonObject>


#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAITarget : public OAIObject {
public:
    OAITarget();
    OAITarget(QString json);
    ~OAITarget() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getComponent() const;
    void setComponent(const qint32 &component);
    bool is_component_Set() const;
    bool is_component_Valid() const;

    qint32 getInput() const;
    void setInput(const qint32 &input);
    bool is_input_Set() const;
    bool is_input_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 component;
    bool m_component_isSet;
    bool m_component_isValid;

    qint32 input;
    bool m_input_isSet;
    bool m_input_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAITarget)

#endif // OAITarget_H
