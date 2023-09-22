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
 * OAIComponentUI.h
 *
 * 
 */

#ifndef OAIComponentUI_H
#define OAIComponentUI_H

#include <QJsonObject>

#include "OAIInput.h"
#include "OAIOutput.h"
#include "OAIParameter.h"
#include "OAIPosition.h"
#include "OAISchedule.h"
#include <QList>
#include <QString>

#include "OAIEnum.h"
#include "OAIObject.h"

namespace OpenAPI {

class OAIComponentUI : public OAIObject {
public:
    OAIComponentUI();
    OAIComponentUI(QString json);
    ~OAIComponentUI() override;

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

    QString getLibrary() const;
    void setLibrary(const QString &library);
    bool is_library_Set() const;
    bool is_library_Valid() const;

    QString getTitle() const;
    void setTitle(const QString &title);
    bool is_title_Set() const;
    bool is_title_Valid() const;

    OAISchedule getSchedule() const;
    void setSchedule(const OAISchedule &schedule);
    bool is_schedule_Set() const;
    bool is_schedule_Valid() const;

    QList<OAIParameter> getParameters() const;
    void setParameters(const QList<OAIParameter> &parameters);
    bool is_parameters_Set() const;
    bool is_parameters_Valid() const;

    QList<OAIInput> getInputs() const;
    void setInputs(const QList<OAIInput> &inputs);
    bool is_inputs_Set() const;
    bool is_inputs_Valid() const;

    QList<OAIOutput> getOutputs() const;
    void setOutputs(const QList<OAIOutput> &outputs);
    bool is_outputs_Set() const;
    bool is_outputs_Valid() const;

    OAIPosition getPosition() const;
    void setPosition(const OAIPosition &position);
    bool is_position_Set() const;
    bool is_position_Valid() const;

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

    QString library;
    bool m_library_isSet;
    bool m_library_isValid;

    QString title;
    bool m_title_isSet;
    bool m_title_isValid;

    OAISchedule schedule;
    bool m_schedule_isSet;
    bool m_schedule_isValid;

    QList<OAIParameter> parameters;
    bool m_parameters_isSet;
    bool m_parameters_isValid;

    QList<OAIInput> inputs;
    bool m_inputs_isSet;
    bool m_inputs_isValid;

    QList<OAIOutput> outputs;
    bool m_outputs_isSet;
    bool m_outputs_isValid;

    OAIPosition position;
    bool m_position_isSet;
    bool m_position_isValid;
};

} // namespace OpenAPI

Q_DECLARE_METATYPE(OpenAPI::OAIComponentUI)

#endif // OAIComponentUI_H
