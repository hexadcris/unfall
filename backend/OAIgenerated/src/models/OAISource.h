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
 * OAISource.h
 *
 * 
 */

#ifndef OAISource_H
#define OAISource_H

#include <QJsonObject>


#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAISource : public OAIObject {
public:
    OAISource();
    OAISource(QString json);
    ~OAISource() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getComponent() const;
    void setComponent(const qint32 &component);
    bool is_component_Set() const;
    bool is_component_Valid() const;

    qint32 getOutput() const;
    void setOutput(const qint32 &output);
    bool is_output_Set() const;
    bool is_output_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 component;
    bool m_component_isSet;
    bool m_component_isValid;

    qint32 output;
    bool m_output_isSet;
    bool m_output_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAISource)

#endif // OAISource_H
