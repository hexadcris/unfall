//
// Created by goll on 8/17/21.
//

#include "fmuFileHelper.h"
#include "osi3/osi_sensordata.pb.h"
#include <QDir>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <optional>

std::string FmuFileHelper::createAgentIDString(int id) {
    std::stringstream ss;
    ss << std::setw(4) << std::setfill('0');
    ss << id;
    return ss.str();
}

QString FmuFileHelper::createOrOpenOutputFolder(const QString& outputDir, const QString& componentName, std::optional<const std::string> appendedFolder)
{
    QString outputDirForComponent = outputDir;
    if(!outputDir.contains(componentName))
    {
        outputDirForComponent += QDir::separator() + componentName; //only append component folder if the path is not describing it
    }

    if(appendedFolder.has_value() && !outputDirForComponent.contains(QString::fromStdString(appendedFolder.value())))
    {
        outputDirForComponent += QDir::separator() + QString::fromStdString(appendedFolder.value());
    }

    QDir directory{outputDirForComponent};
    if (!directory.exists())
    {
        directory.mkpath(outputDirForComponent);
    }
    return outputDirForComponent;
}
void FmuFileHelper::writeBinaryTrace(const std::string &message, const QString &fileName, const QString& componentName, int time,
                                     std::map<std::string, std::pair<std::string, int>> &targetOutputTracesMap)
{
    std::string outputTraceKey = (componentName + "_" + fileName).toStdString();
    targetOutputTracesMap[outputTraceKey].first = message;
    targetOutputTracesMap[outputTraceKey].second = time;
}

void FmuFileHelper::writeTracesToFile(const QString& outputDir, const std::map<std::string, std::pair<std::string, int>>& fileToOutputTracesMap) {

    for(const auto& file_trace_element : fileToOutputTracesMap)
    {
        size_t separator = file_trace_element.first.find("_");
        std::string outputType = file_trace_element.first.substr(separator+1,file_trace_element.first.length());
        std::string componentName = file_trace_element.first.substr(0,separator);
        const auto currentInterfaceVersion = osi3::InterfaceVersion::descriptor()->file()->options().GetExtension(osi3::current_interface_version);

        QString traceDir = FmuFileHelper::createOrOpenOutputFolder(outputDir, QString::fromStdString(componentName),
                                                                   "BinaryTraceFiles");
        traceDir += (QDir::separator() + QString::fromStdString(outputType) + "_" + (QString::number(file_trace_element.second.second / 10) + "_" + QString::number(GOOGLE_PROTOBUF_VERSION) + "_"
                + QString::number(currentInterfaceVersion.version_major()) + QString::number(currentInterfaceVersion.version_minor())
                + QString::number(currentInterfaceVersion.version_patch()) ));

        std::ofstream file {};
        std::string filename = traceDir.toStdString() + ".osi";
        if (!file.is_open())
            file.open(filename, std::ios::out | std::ios::trunc);
        file  << file_trace_element.second.first;
        file.close();
    }
}
