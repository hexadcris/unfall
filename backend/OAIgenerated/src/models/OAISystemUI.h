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
 * OAISystemUI.h
 *
 * 
 */

#ifndef OAISystemUI_H
#define OAISystemUI_H

#include <QJsonObject>

#include "OAIComponentUI.h"
#include "OAIConnection.h"
#include <QList>
#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAISystemUI : public OAIObject {
public:
    OAISystemUI();
    OAISystemUI(QString json);
    ~OAISystemUI() override;

    QString asJson() const override;
    QJsonObject asJsonObject() const override;
    void fromJsonObject(QJsonObject json) override;
    void fromJson(QString jsonString) override;

    qint32 getId() const;
    void setId(const qint32 &id);
    bool is_id_Set() const;
    bool is_id_Valid() const;

    QString getTitle() const;
    void setTitle(const QString &title);
    bool is_title_Set() const;
    bool is_title_Valid() const;

    qint32 getPriority() const;
    void setPriority(const qint32 &priority);
    bool is_priority_Set() const;
    bool is_priority_Valid() const;

    QList<OAIComponentUI> getComponents() const;
    void setComponents(const QList<OAIComponentUI> &components);
    bool is_components_Set() const;
    bool is_components_Valid() const;

    QList<OAIConnection> getConnections() const;
    void setConnections(const QList<OAIConnection> &connections);
    bool is_connections_Set() const;
    bool is_connections_Valid() const;

    QString getFile() const;
    void setFile(const QString &file);
    bool is_file_Set() const;
    bool is_file_Valid() const;

    bool isDeleted() const;
    void setDeleted(const bool &deleted);
    bool is_deleted_Set() const;
    bool is_deleted_Valid() const;

    virtual bool isSet() const override;
    virtual bool isValid() const override;

private:
    void initializeModel();

    qint32 id;
    bool m_id_isSet;
    bool m_id_isValid;

    QString title;
    bool m_title_isSet;
    bool m_title_isValid;

    qint32 priority;
    bool m_priority_isSet;
    bool m_priority_isValid;

    QList<OAIComponentUI> components;
    bool m_components_isSet;
    bool m_components_isValid;

    QList<OAIConnection> connections;
    bool m_connections_isSet;
    bool m_connections_isValid;

    QString file;
    bool m_file_isSet;
    bool m_file_isValid;

    bool deleted;
    bool m_deleted_isSet;
    bool m_deleted_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAISystemUI)

#endif // OAISystemUI_H
