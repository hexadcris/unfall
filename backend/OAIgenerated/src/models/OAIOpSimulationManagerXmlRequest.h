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
 * OAIOpSimulationManagerXmlRequest.h
 *
 * 
 */

#ifndef OAIOpSimulationManagerXmlRequest_H
#define OAIOpSimulationManagerXmlRequest_H

#include <QJsonObject>

#include "OAISimulationConfig.h"
#include <QList>
#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIOpSimulationManagerXmlRequest : public OAIObject {
public:
    OAIOpSimulationManagerXmlRequest();
    OAIOpSimulationManagerXmlRequest(QString json);
    ~OAIOpSimulationManagerXmlRequest() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getLogLevel() const;
    void setLogLevel(const qint32 &log_level);
    bool is_log_level_Set() const;
    bool is_log_level_Valid() const;

    QString getLogFileSimulationManager() const;
    void setLogFileSimulationManager(const QString &log_file_simulation_manager);
    bool is_log_file_simulation_manager_Set() const;
    bool is_log_file_simulation_manager_Valid() const;

    QString getSimulation() const;
    void setSimulation(const QString &simulation);
    bool is_simulation_Set() const;
    bool is_simulation_Valid() const;

    QString getLibraries() const;
    void setLibraries(const QString &libraries);
    bool is_libraries_Set() const;
    bool is_libraries_Valid() const;

    QList<OAISimulationConfig> getSimulationConfigs() const;
    void setSimulationConfigs(const QList<OAISimulationConfig> &simulation_configs);
    bool is_simulation_configs_Set() const;
    bool is_simulation_configs_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 log_level;
    bool m_log_level_isSet;
    bool m_log_level_isValid;

    QString log_file_simulation_manager;
    bool m_log_file_simulation_manager_isSet;
    bool m_log_file_simulation_manager_isValid;

    QString simulation;
    bool m_simulation_isSet;
    bool m_simulation_isValid;

    QString libraries;
    bool m_libraries_isSet;
    bool m_libraries_isValid;

    QList<OAISimulationConfig> simulation_configs;
    bool m_simulation_configs_isSet;
    bool m_simulation_configs_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIOpSimulationManagerXmlRequest)

#endif // OAIOpSimulationManagerXmlRequest_H
