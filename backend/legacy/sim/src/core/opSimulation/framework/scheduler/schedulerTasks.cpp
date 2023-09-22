/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "schedulerTasks.h"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <stdexcept>

//-----------------------------------------------------------------------------
/** \file  SchedulerTasks.cpp */
//-----------------------------------------------------------------------------

namespace core::scheduling {

SchedulerTasks::SchedulerTasks(std::vector<TaskItem> bootstrapTasks,
                               std::vector<TaskItem> spawningTasks,
                               std::vector<TaskItem> preAgentTasks,
                               std::vector<TaskItem> synchronizeTasks,
                               std::vector<TaskItem> finalizeTasks,
                               int scheduledTimestampsInterval)
{
    this->bootstrapTasks.tasks = std::multiset<TaskItem>(bootstrapTasks.begin(), bootstrapTasks.end());
    this->spawningTasks.tasks = std::multiset<TaskItem>(spawningTasks.begin(), spawningTasks.end());
    this->preAgentTasks.tasks = std::multiset<TaskItem>(preAgentTasks.begin(), preAgentTasks.end());
    this->synchronizeTasks.tasks = std::multiset<TaskItem>(synchronizeTasks.begin(), synchronizeTasks.end());
    this->finalizeTasks.tasks = std::multiset<TaskItem>(finalizeTasks.begin(), finalizeTasks.end());

    this->scheduledTimestampsInterval = scheduledTimestampsInterval;
    lowerBoundOfScheduledTimestamps = 0;
    upperBoundOfScheduledTimestamps = scheduledTimestampsInterval;
    CreateNewScheduledTimestamps();
}

void SchedulerTasks::ScheduleNewRecurringTasks(std::vector<TaskItem> newTasks)
{
    ScheduleNewTasks(recurringAgentTasks, std::move(newTasks));
}

void SchedulerTasks::ScheduleNewNonRecurringTasks(std::vector<TaskItem> newTasks)
{
    ScheduleNewTasks(nonRecurringAgentTasks, newTasks);
}

void SchedulerTasks::ScheduleNewTasks(Tasks &tasks, std::vector<TaskItem> newTasks)
{
    for (const auto &newTask : newTasks)
    {
        tasks.AddTask(newTask);
        UpdateScheduledTimestamps(newTask.cycletime, newTask.delay);
    }
}

void SchedulerTasks::UpdateScheduledTimestamps(int cycleTime, int delay)
{
    if (delay > lowerBoundOfScheduledTimestamps && delay < upperBoundOfScheduledTimestamps)
    {
        scheduledTimestamps.insert(delay);
    }

    if (cycleTime == 0)
    {
        // next timestamp is already in scheduledTimesteps for execution of init task
        return;
    }

    int currentScheduleTime = cycleTime + delay;
    int numberOfSkippedTimestamps = std::max(0,
                                             (lowerBoundOfScheduledTimestamps - currentScheduleTime + cycleTime - 1) /
                                                 cycleTime); //We add cycletime -1 because we want to round up
    currentScheduleTime += cycleTime * numberOfSkippedTimestamps;

    if (currentScheduleTime <= upperBoundOfScheduledTimestamps)
    {
        while (currentScheduleTime <= upperBoundOfScheduledTimestamps)
        {
            scheduledTimestamps.insert(currentScheduleTime);
            currentScheduleTime += cycleTime;
        }
    }
    // scheduledTimesteps higher than horizon will be considered on next timestamp
}

void SchedulerTasks::UpdateScheduledTimestamps(std::multiset<TaskItem> &tasks)
{
    for (const auto &task : tasks)
    {
        UpdateScheduledTimestamps(task.cycletime, task.delay);
    }
}

void SchedulerTasks::CreateNewScheduledTimestamps()
{
    scheduledTimestamps.clear();
    scheduledTimestamps.insert(lowerBoundOfScheduledTimestamps);
    scheduledTimestamps.insert(upperBoundOfScheduledTimestamps);

    UpdateScheduledTimestamps(spawningTasks.tasks);
    UpdateScheduledTimestamps(recurringAgentTasks.tasks);
    UpdateScheduledTimestamps(nonRecurringAgentTasks.tasks);
}

int SchedulerTasks::GetNextTimestamp(int timestamp)
{
    ExpandUpperBoundary(timestamp);

    for (const auto &scheduledTimestamp : scheduledTimestamps)
    {
        if (scheduledTimestamp > timestamp)
        {
            return scheduledTimestamp;
        }
    }

    throw std::runtime_error("Can not handle given timestamp. Please check validity.");
}

void SchedulerTasks::ExpandUpperBoundary(int timestamp)
{
    while (timestamp >= upperBoundOfScheduledTimestamps)
    {
        upperBoundOfScheduledTimestamps += scheduledTimestampsInterval;
        lowerBoundOfScheduledTimestamps += scheduledTimestampsInterval;
        CreateNewScheduledTimestamps();
    }
    // we are not time travelling.. backwards is illogical
}

void SchedulerTasks::GetTasks(int timestamp, std::multiset<TaskItem> &tasks, std::vector<TaskItem> &currentTasks)
{
    for (const auto &task : tasks)
    {
        if (task.cycletime == 0)
        {
            currentTasks.push_back(task);
            continue;
        }

        if ((timestamp - task.delay) % task.cycletime == 0)
        {
            currentTasks.push_back(task);
        }
    }
}

std::vector<TaskItem> SchedulerTasks::GetTasks(int timestamp)
{
    std::vector<TaskItem> currentTasks{};

    ExpandUpperBoundary(timestamp);
    if (scheduledTimestamps.find(timestamp) == scheduledTimestamps.end())
    {
        return currentTasks;
    }

    GetTasks(timestamp, spawningTasks.tasks, currentTasks);
    PullNonRecurringTasks(timestamp, currentTasks);
    GetTasks(timestamp, recurringAgentTasks.tasks, currentTasks);
    GetTasks(timestamp, synchronizeTasks.tasks, currentTasks);

    return currentTasks;
}

std::vector<TaskItem> SchedulerTasks::GetSpawningTasks(int timestamp)
{
    std::vector<TaskItem> currentTasks;
    GetTasks(timestamp, spawningTasks.tasks, currentTasks);
    return currentTasks;
}

std::vector<TaskItem> SchedulerTasks::GetPreAgentTasks(int timestamp)
{
    std::vector<TaskItem> currentTasks;
    GetTasks(timestamp, preAgentTasks.tasks, currentTasks);
    return currentTasks;
}

std::vector<TaskItem> SchedulerTasks::ConsumeNonRecurringAgentTasks(int timestamp)
{
    std::vector<TaskItem> currentTasks;
    PullNonRecurringTasks(timestamp, currentTasks);
    return currentTasks;
}

std::vector<TaskItem> SchedulerTasks::GetRecurringAgentTasks(int timestamp)
{
    std::vector<TaskItem> currentTasks;
    GetTasks(timestamp, recurringAgentTasks.tasks, currentTasks);
    return currentTasks;
}

std::vector<TaskItem> SchedulerTasks::GetSynchronizeTasks(int timestamp)
{
    std::vector<TaskItem> currentTasks;
    GetTasks(timestamp, synchronizeTasks.tasks, currentTasks);
    return currentTasks;
}

void SchedulerTasks::PullNonRecurringTasks(int timestamp, std::vector<TaskItem> &currentTasks)
{
    GetTasks(timestamp, nonRecurringAgentTasks.tasks, currentTasks);
    ClearNonrecurringTasks();
}

std::multiset<TaskItem> SchedulerTasks::GetBootstrapTasks()
{
    return bootstrapTasks.tasks;
}

std::multiset<TaskItem> SchedulerTasks::GetFinalizeTasks()
{
    return finalizeTasks.tasks;
}

void SchedulerTasks::ClearNonrecurringTasks()
{
    nonRecurringAgentTasks.tasks.clear();
}

void SchedulerTasks::DeleteAgentTasks(int agentId)
{
    std::vector<int> agentIds{agentId};
    DeleteAgentTasks(agentIds);
}

void SchedulerTasks::DeleteAgentTasks(std::vector<int> &agentIds)
{
    for (const auto &agentId : agentIds)
    {
        recurringAgentTasks.DeleteTasks(agentId);
        nonRecurringAgentTasks.DeleteTasks(agentId); //if agent immediately will be removed after spawning
    }

    if (!agentIds.empty())
    {
        CreateNewScheduledTimestamps();
    }
}

} // namespace openpass::scheduling
