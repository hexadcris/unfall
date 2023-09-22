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
 * OAISimulationConfig.h
 *
 * 
 */

#ifndef OAISimulationConfig_H
#define OAISimulationConfig_H

#include <QJsonObject>

#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAISimulationConfig : public OAIObject {
public:
    OAISimulationConfig();
    OAISimulationConfig(QString json);
    ~OAISimulationConfig() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getNumberOfExecutions() const;
    void setNumberOfExecutions(const qint32 &number_of_executions);
    bool is_number_of_executions_Set() const;
    bool is_number_of_executions_Valid() const;

    QString getLogFileSimulation() const;
    void setLogFileSimulation(const QString &log_file_simulation);
    bool is_log_file_simulation_Set() const;
    bool is_log_file_simulation_Valid() const;

    QString getConfigurations() const;
    void setConfigurations(const QString &configurations);
    bool is_configurations_Set() const;
    bool is_configurations_Valid() const;

    QString getResults() const;
    void setResults(const QString &results);
    bool is_results_Set() const;
    bool is_results_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 number_of_executions;
    bool m_number_of_executions_isSet;
    bool m_number_of_executions_isValid;

    QString log_file_simulation;
    bool m_log_file_simulation_isSet;
    bool m_log_file_simulation_isValid;

    QString configurations;
    bool m_configurations_isSet;
    bool m_configurations_isValid;

    QString results;
    bool m_results_isSet;
    bool m_results_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAISimulationConfig)

#endif // OAISimulationConfig_H
