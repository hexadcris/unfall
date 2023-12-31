/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  sceneryImporterPCM.h
//! @brief This file contains the importer of the scenery configuration.
//-----------------------------------------------------------------------------

#ifndef SCENERYIMPORTERPCM_H
#define SCENERYIMPORTERPCM_H

#include <sstream>

#include <QDomDocument>
#include <QFile>

#include "../PCM_Data/pcm_data.h"
#include "../PCM_Data/pcm_definitions.h"
#include "../PCM_Data/pcm_helper.h"
#include "../PCM_Data/pcm_trajectory.h"

/*!
 * \brief The SceneryImporterPCM class
 * This class is responsible to import pcm data from a given scnenery configuration file.
 */
class SceneryImporterPCM
{
public:
    SceneryImporterPCM() = default;
    SceneryImporterPCM(const SceneryImporterPCM &) = delete;
    SceneryImporterPCM(SceneryImporterPCM &&) = delete;
    SceneryImporterPCM &operator=(const SceneryImporterPCM &) = delete;
    SceneryImporterPCM &operator=(SceneryImporterPCM &&) = delete;
    virtual ~SceneryImporterPCM() = default;

    //-----------------------------------------------------------------------------
    //! Imports data structures from the scenery configuration PCM file
    //!
    //! @param[in]  filename       path to PCM file
    //! @param[in]  pcmData        pcmData object to fill with data from pcm file
    //! @return                    true on success
    //-----------------------------------------------------------------------------
    bool Import(const std::string &filename,
                PCM_Data &pcmData,
                std::map<int, PCM_Trajectory *> &trajectories);

private:
    //! Parse all global data.
    //!
    //! \param[in] rootNode     rootNode
    //! \param[in,out] pcmData  pcmData object to be filled
    //! \return true for success
    bool ParseGlobalData(QDomNode rootNode, PCM_Data &pcmData);

    //! Parse all marks.
    //!
    //! \param[in] rootNode     rootNode
    //! \param[in,out] pcmData  pcmData object to be filled
    //! \return true for success
    bool ParseMarks(QDomNode rootNode, PCM_Data &pcmData);

    //! Parse all objects.
    //!
    //! \param[in] rootNode     rootNode
    //! \param[in,out] pcmData  pcmData object to be filled
    //! \return true for success
    bool ParseObjects(QDomNode rootNode, PCM_Data &pcmData);

    //! Parse all viewObjects.
    //!
    //! \param[in] rootNode     rootNode
    //! \param[in,out] pcmData  pcmData object to be filled
    //! \return true for success
    bool ParseViewObjects(QDomNode rootNode, PCM_Data &pcmData);

    //! Parse all intendedCourses.
    //!
    //! \param[in] rootNode     rootNode
    //! \param[in,out] pcmData  pcmData object to be filled
    //! \return true for success
    bool ParseIntendedCourses(QDomNode rootNode, PCM_Data &pcmData);

    //! Parse all lines.
    //!
    //! \param[in] lineContainerNode  node with lineContainer
    //! \param[in,out] lineContainer  lineContainer object to be filled
    //! \return true for success
    bool ParseLines(QDomNode lineContainerNode, PCM_LineContainer *lineContainer);

    //! Parse all points.
    //!
    //! \param[in] pointContainerNode  node with pointContainer
    //! \param[in,out] pointContainer  pointContainer object to be filled
    //! \return true for success
    bool ParsePoints(QDomNode pointContainerNode, PCM_PointContainer *pointContainer);

    //! Parse double vector.
    //!
    //! \param[in] rootNode          rootNode
    //! \param[in,out] doubleVector  double vector to be filled
    //! \return true for success
    bool ParseDoubleVector(QDomNode rootNode, std::vector<double> *doubleVector);
};

#endif // SCENERYIMPORTERPCM_H
