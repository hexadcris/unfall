################################################################################
# Copyright (c) 2020-2021 in-tech GmbH
#               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from os import path
from dataclasses import dataclass

from lxml import etree as et
from pandas import DataFrame, concat
import pandas as pd
import event_parser


class Event:
    def __init__(self):
        self.meta = dict()
        self.triggering = []
        self.affected = []
        self.parameters = dict()

    def eval_triggering_and_affected(self):
        rows = []
        for t in self.triggering:
            row = {'AgentId': int(t),
                   'IsTriggering': True,
                   'IsAffected': True if t in self.affected else False}
            rows.append(row)

        for a in self.affected:
            if a not in self.triggering:
                row = {'AgentId': int(a),
                       'IsTriggering': False,
                       'IsAffected': True}
            rows.append(row)
        return rows

    def get(self):
        rows = []
        for ta in self.eval_triggering_and_affected():
            row = self.meta.copy()
            row.update(ta)
            for k, v in self.parameters.items():
                row.update({'Key': k, 'Value': v})
                rows.append(row.copy())
            if not len(self.parameters):
                row.update({'Key': '', 'Value': ''})
                rows.append(row)

        return rows


def load(file, invocation) -> DataFrame:
    """Loads event data from a simulation output for the specified invocation

    :param str file: absolute or relative file path
    :param int invocation: invocation to load data for
    :return: pandas DataFrames with agents and event data
    :rtype: DataFrame
    :raises FileNotFoundError: when file does not exist
    """
    if path.exists(file):
        return _load_trace_metadata(file, invocation)
    else:
        raise FileNotFoundError('Input file not found')


def _transform_flat_dict_to_dataframe(dictionary) -> DataFrame:
    """Transform a dictionary into a pandas dataframe

    :param list[dict] or dict dictionary: data as dictionary (row based)
    :return: pandas DataFrame
    :rtype: DataFrame
    """
    if isinstance(dictionary, list):
        return DataFrame(dictionary)
    else:
        return DataFrame(dictionary, index=[0])

# def _load_agents(simulation_output) -> DataFrame:
#     parser = et.XMLParser(ns_clean=True)
#     tree = et.parse(simulation_output, parser)
#     return _extract_agents_from_trace(tree.getroot())


def _load_trace_metadata(simulation_output, invocation):
    """Loads metadata (agents and events) from a simulation output,
       without checking file existence. RunStatistics are appended.

    :param str simulation_output: externally validated file
    :param int invocation: invocation to load data for
    :return: One DataFrames for agents and one for events
    :rtype: tuple(DataFrame)
    """
    try:
        run_statistics = pd.read_xml(simulation_output, xpath=f"//RunResults/RunResult[@RunId='{invocation}']/RunStatistics")
    except ValueError:
        run_statistics = pd.DataFrame()

    parser = et.XMLParser(ns_clean=True)
    tree_root = et.parse(simulation_output, parser).getroot()
    agent_trace = _extract_agents_from_trace(tree_root, invocation)

    return (
        agent_trace.merge(run_statistics, how='cross'),
        _extract_events_from_trace(tree_root, invocation)
    )

def _extract_agents_from_trace(root, invocation) -> DataFrame:
    rows = []

    for run_result in root.findall(f"RunResults/RunResult[@RunId='{invocation}']"):
        for a in _parse_agents(run_result):
            rows.append(_transform_flat_dict_to_dataframe(a.get()))

    df = concat(rows, ignore_index=True) if len(rows) else pd.DataFrame(columns=['AgentId'])
    df['AgentId'] = df['AgentId'].astype('int64')

    return df


def _extract_events_from_trace(root, invocation) -> DataFrame:
    rows = []

    for run_result in root.findall(f"RunResults/RunResult[@RunId='{invocation}']"):
        for e in _parse_events(run_result):
            rows.append(_transform_flat_dict_to_dataframe(e.get()))

    return concat(rows, ignore_index=True) if len(rows) else pd.DataFrame()


def _parse_events(run_result):
    events = []

    for event in run_result.find('Events'):
        e = Event()
        e.meta = {'RunId': int(run_result.attrib['RunId']),
                  'Timestep': int(event.attrib['Time']),
                  'Source': event.attrib['Source'],
                  'Name': event.attrib['Name']}

        for event_parameter in event.findall('Parameters/Parameter'):
            e.parameters.update(
                {event_parameter.attrib['Key']: event_parameter.attrib['Value']})

        e.triggering = _parse_entities('TriggeringEntities', event)
        e.affected = _parse_entities('AffectedEntities', event)

        events.append(e)

    return events


@dataclass(init=True)
class Agent:
    id: int
    AgentTypeGroupName: str
    AgentTypeName: str
    VehicleModelType: str
    DriverProfileName: str
    AgentType: str

    def get(self):
        return {
            'AgentId': self.id,
            'AgentTypeGroupName': self.AgentTypeGroupName,
            'AgentTypeName': self.AgentTypeName,
            'VehicleModelType': self.VehicleModelType,
            'DriverProfileName': self.DriverProfileName,
            'AgentType': self.AgentType
        }


def _parse_agents(run_result):
    return map(lambda agent:
               Agent(
                   str(agent.attrib['Id']),
                   agent.attrib['AgentTypeGroupName'],
                   agent.attrib['AgentTypeName'],
                   agent.attrib['VehicleModelType'],
                   agent.attrib['DriverProfileName'],
                   agent.attrib['AgentType']
                   ),
               run_result.find('Agents'))


def _parse_entities(tag, event):
    parsed_entities = []

    for entities in event.findall(tag):
        for entity in entities.findall('Entity'):
            parsed_entities.append(entity.attrib['Id'])
    return parsed_entities


def get_trace_metadata(file, events, invocation):

    df_agents, df_events = load(file, invocation)

    if len(events):
        return df_agents, list(event_parser.parse_regular(df_events, event) for event in events)
    else:
        return df_agents, []