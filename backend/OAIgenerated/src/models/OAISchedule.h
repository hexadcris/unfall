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
 * OAISchedule.h
 *
 * 
 */

#ifndef OAISchedule_H
#define OAISchedule_H

#include <QJsonObject>


#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAISchedule : public OAIObject {
public:
    OAISchedule();
    OAISchedule(QString json);
    ~OAISchedule() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getOffset() const;
    void setOffset(const qint32 &offset);
    bool is_offset_Set() const;
    bool is_offset_Valid() const;

    qint32 getCycle() const;
    void setCycle(const qint32 &cycle);
    bool is_cycle_Set() const;
    bool is_cycle_Valid() const;

    qint32 getResponse() const;
    void setResponse(const qint32 &response);
    bool is_response_Set() const;
    bool is_response_Valid() const;

    qint32 getPriority() const;
    void setPriority(const qint32 &priority);
    bool is_priority_Set() const;
    bool is_priority_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 offset;
    bool m_offset_isSet;
    bool m_offset_isValid;

    qint32 cycle;
    bool m_cycle_isSet;
    bool m_cycle_isValid;

    qint32 response;
    bool m_response_isSet;
    bool m_response_isValid;

    qint32 priority;
    bool m_priority_isSet;
    bool m_priority_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAISchedule)

#endif // OAISchedule_H
