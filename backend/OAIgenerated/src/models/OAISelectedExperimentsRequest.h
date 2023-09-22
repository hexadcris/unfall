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
 * OAISelectedExperimentsRequest.h
 *
 * 
 */

#ifndef OAISelectedExperimentsRequest_H
#define OAISelectedExperimentsRequest_H

#include <QJsonObject>

#include <QList>
#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAISelectedExperimentsRequest : public OAIObject {
public:
    OAISelectedExperimentsRequest();
    OAISelectedExperimentsRequest(QString json);
    ~OAISelectedExperimentsRequest() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    QList<QString> getSelectedExperiments() const;
    void setSelectedExperiments(const QList<QString> &selected_experiments);
    bool is_selected_experiments_Set() const;
    bool is_selected_experiments_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    QList<QString> selected_experiments;
    bool m_selected_experiments_isSet;
    bool m_selected_experiments_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAISelectedExperimentsRequest)

#endif // OAISelectedExperimentsRequest_H
