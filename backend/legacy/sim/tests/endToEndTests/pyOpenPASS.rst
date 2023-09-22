..
  *******************************************************************************
  Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

This tool acts as configurable executor for complete sets of configs for the openPASS simulation.
The test framework is located at ``sim/tests/endToEndTests/pyOpenPASS``.

Prerequisites
-------------

Please refer to :ref:`binary_packages` for instructions on installing the prerequisites.
Execution
---------

As pyOpenPASS is a pytest plugin (and is not yet a standalone-plugin) it will be automatically executed, when pytest finds its entry-point ``conftest.py`` (= local-pytest-plugin) next to files named ``test_*.json``.
So test files must be copied into the pyOpenPASS directory before execution.

.. code-block:: bash

   pytest
     --simulation=SIMULATION_EXE     # path to simulation executable, e.g. /openPASS/bin/opSimulation
     --mutual=MUTUAL_RESOURCES_PATH  # path to mutual config files for all runs, e.g. /openPASS/bin/examples/common
     --resources=RESOURCES_PATH      # path from where configs are retrieved - override common files if necessary
     --report-path=REPORT_PATH       # path to where the report shall be stored
     TEST_FILE                       # file under test, named `test_*.json`

.. note:: You can use additional pytest arguments, such as ``-v`` for verbose output, ``--collect-only`` for listing the available tests and so on (see https://docs.pytest.org).

In addition pyOpenPASS supports the following optional arguments:

.. code-block:: bash

    --configs-path=INPUT             # path for providing configs during testing
    --results-path=OUTPUT            # path for collecting test results during testing
    --artifacts-path=ARTIFACTS       # path for collecting test artifacts during testing

For each specified ``test_*.json`` a corresponding ``test_*.html`` will be generated.

Test Configuration
------------------

Test configuiration is by the test-json file, individually for each test.
Depending on the users choice, three different tests runners are executed:

1. Determinism: Check executability of configs + Determinism test (*1 x n* vs *n x 1* tests).
2. Parameterized: Check executability of configs using different parameters.
3. Query: Execute config and check for specific results in the output of the simulator, given one or more queries.

In general, the test-json splits into two sections:

1. Definition of ``Configuration Sets``
2. Definition of ``Tests`` using the ``Configuration Sets`` or a single ``Config`` directly

.. note:: Whenever possible, pyOpenPASS re-uses the results to speed up result analysis.

.. code-block:: js

    {
        "config_sets": {
            "Config_Set_1": [ // user defined name
                "Config_Folder_1",
                "Config_Folder_2"
            ],
            "Config_Set_2": [
                "Config_Folder_2",
                "Config_Folder_3"
            ],
            "Config_Set_3": [
                "Config_Folder_4"
            ]
        },
        "tests": {
            "Execution and Determinism": {
                "config_sets": [
                    "Config_Set_1",
                    "Config_Set_2",
                    "Config_Set_3"
                ],
                "determinism": true, // ACTIVATES DETERMINISM
                "duration": 30, // how long shall be simulated
                "invocations": 3 // compare 1x3 run with 3x1 runs
            },
            "Parameterization": {
                "config_sets": [
                    "Config_Set_2"
                ],
                "parameterization": { // ACTIVATES PARAMETERIZATION
                    "file": "systemConfigFmu.xml", // Name of config, which shall be parameterized
                    "xpath": "//value[../id='FmuPath']", // XPath, where values needs to be replaced
                    "values": [ // Values, which shall be set
                        "resources/FMU1_StaticFMU.fmu",
                        "resources/FMU2_StaticFMU.fmu"
                    ],
                    "duration": 10,
                    "invocations": 100
                },
                "Querying": {
                    "config": "Config_Folder_2" // single config specification
                    ],
                    "queries": [ // ACTIVATES QUERYING
                        "count(AgentId | AgentId == 0 and Timestep == 10000 and VelocityEgo >= 30) == 1",
                        "mean(VelocityEgo | AgentId != 0) > 30"
                    ],
                    "success_rate": 0.8, // 80% of 60 invocations needs to pass
                    "duration": 10,
                    "invocations": 60,
                    "description": "Optional description"
                }
            }
        }
    }

- If the `success_rate` is specified, its values must be between 0 and 1.
- It is also possible to define a range of success (e.g. for excluding 100%) by using the following syntax:

  .. code-block:: js

     "success_rate": [0.8, 0.99] // 80% to 99% need to pass

Querying Results
----------------

Basic Syntax
~~~~~~~~~~~~

::

    [aggregate]([column] | [filter]) [operator] [value]

-  Aggregate:
   Everything pandas supports on dataframes, such as `pandas.DataFrame.count <https://pandas.pydata.org/pandas-docs/stable/reference/api/pandas.DataFrame.count.html?highlight=count#pandas.DataFrame.count>`__, min, max, mean
-  Column:
   A column on which the aggregate should operate.

   Columns are generally given by the simulation outputs cyclic columns, such as ``PositionRoute``.
   In addition the following columns are available:

   -  ``AgentId``
   -  From the tag ``Agents`` (see ``simulationOutput.xml``):

      - ``AgentTypeGroupName``
      - ``AgentTypeName``
      - ``VehicleModelType``
      - ``DriverProfileName``
      - ``AgentType``

   -  Everything from the tag ``RunStatistics`` (see ``simulationOutput.xml``), which is currently:

      - ``RandomSeed``
      - ``VisibilityDistance``
      - ``StopReason``
      - ``StopTime``
      - ``EgoAccident``
      - ``TotalDistanceTraveled``
      - ``EgoDistanceTraveled``

-  Filter:
   A filter based on `pandas.DataFrame.filter <https://pandas.pydata.org/pandas-docs/stable/reference/api/pandas.DataFrame.filter.html?highlight=filter#pandas.DataFrame.filter>`__ syntax using the available columns.
-  Operator:
   A comparison operator from the following list: ==, <=, >=, <, >, !=, ~= (approximate).
   The approximate operator allows ``1*e-6 x value`` as maximum deviation from value.
-  Value:
   A number

.. note:: In seldom cases, the filter can be skipped, e.g. when securing that no agent has been spawned: ``count(AgentId) == 0``.

**Example**

::

    count(AgentId | PositionRoute >= 800 and Lane != -3) == 0

Using Events in Filter
~~~~~~~~~~~~~~~~~~~~~~

In order to query for a specific event, use ``#(EVENT)`` within the filter syntax.

**Example**

::

    count(AgentId | PositionRoute >= 800 and #(Collision) == True) == 0

Event Payload
^^^^^^^^^^^^^

Each event is associated with a set of triggering entity ids, affected entity ids, and arbitrary key/value pairs (please refer to the openPASS documentation for details).
This information is transformed into a "per agent" scope.

In the following the ``Collision`` event is taken as example.

**TriggeringEntity**

All agents, flagged as triggering become ``IsTriggering``

Query: ``#(Collision):IsTriggering == True``

**AffectedEntity**

All agents, flagged as affected become ``IsAffected``

Query: ``#(Collision):IsAffected == True``

**Key/Value Pairs**

If an event publishes additional payload with the key ``XYZ``, it will can be queried by ``#(EVENT):XYZ``.

Query: ``#(Collision):WithAgent``

.. warning::

    Keys carrying the event name as prefix, such as in ``#(Collision):CollisionWithAgent``, will be stripped to ``Collision:WithAgent``

Query Example
^^^^^^^^^^^^^

| *No agent should collide with agent 0:*
| ``count(AgentId | AgentId == 0 and #(Collision):WithAgent == 1) == 0``


Using openSCENARIO Events
~~~~~~~~~~~~~~~~~~~~~~~~~

OpenScenario events are processed in the same manner as regular events (see above).

This allows to query for occurrences of openSCENARIO events with a name specified within the following xpath:
``OpenSCENARIO/Story/Act/Sequence/Maneuver/Event/@name``

**openSCENARIO Event Definition**

.. code:: xml

    <Story name="TheStory">
      <Act name="TheAct">
        <Sequence name="TheSequence" numberOfExecutions="1">
          ...
          <Maneuver name="TheManeuver">
            ...
            <!-- example name "ttc_event"-->
            <Event name="ttc_event" priority="overwrite">
            ...
              <StartConditions>
                <ConditionGroup>
                  <Condition name="Conditional">
                    <ByEntity>
                      ...
                      <EntityCondition>
                         <TimeToCollision>
                           ...
                         </TimeToCollision>
                      </EntityCondition>
                    </ByEntity>
                  </Condition>
                </ConditionGroup>
              </StartConditions>
            </Event>
            ...
          </Maneuver>
        </Sequence>
      </Act>
    </Story>

**Example openPASS Output**

.. code:: xml

    <Event Time="0" Source="OpenSCENARIO" Name="TheStory/TheAct/TheSequence/TheManeuver/ttc_event">
        <TriggeringEntities/>
        <AffectedEntities>
            <Entity Id="1"/>
        </AffectedEntities>
        <Parameters/>
    </Event>

**Query**

``count(AgentId | #(TheStory/TheAct/TheSequence/TheManeuver/ttc_event) == True ) > 0``


Querying Transitions
~~~~~~~~~~~~~~~~~~~~

Sometimes it is necessary to check, whether a transition happened, such as counting agents, passing a certain position.

This can be achieved by shifting individual columns by ``N`` time steps.

**Time Shift Syntax**

``Column-Shift`` =>  ``PositionRoute-1`` means PositionRoute at one time step earlier

**Example Use Case**

Counting agents passing ``PositionRoute == 350`` on ``LaneId == -1``

**Query**

``count(AgentId | LaneId == -1 and PositionRoute-1 < 350 and PositionRoute >= 350 ) > 0``

.. warning::

    In seldom cases,  a result column happens to have a name like ``Name-N`` where ``N`` is an integer.
    Querying this column would automatically apply time shifting (default behavior) leading to a parsing error.
    In such cases, escape the column name with single quotes (e.g. ``'Name-1'``).

Querying Spawning Time
~~~~~~~~~~~~~~~~~~~~~~

Queries can be restricted to the spawning time:

**Query**

``count(AgentId | Timestep == {first} and Velocity < 30) == 0``

.. warning::

    ``Timestep == {first}`` must be the first parameter in the filter and can only succeeded by ``and``.


Explicit Datatypes
~~~~~~~~~~~~~~~~~~

pyOpenPASS uses Pandas DataFrames internally.
Pandas will try to detect the datatype of the individual cyclic columns automatically.
This won't fit the user's intention in some cases, such as when the column holds a semicolon separated list of integers but every list contains just one element.
In such cases it is impossible to distinguish between integers and strings based on the data.

For this reason, datatypes can be specified explicitly along with a query:

.. code:: js

   "queries": [ ... ],
   "datatypes": {
       "Sensor0_DetectedAgents": "str" // string with "missing value" support
   }

Dev Notes
---------

If you want to execute/debug pyOpenPASS in VS-Code, you can add a configuration, similar to the one shown below, to the ``launch.json`` after opening pyOpenPASS as VS-Code project:

.. code-block:: json

   "configurations": [
   {
       "name": "pytest-openpass",
       "type": "python",
       "module": "pytest",
       "args": [
           "--simulation=/OpenPASS/bin/core/opSimulation",
           "--mutual=/OpenPASS/bin/core/examples/OSS/Common/",
           "--resources=/OpenPASS/bin/core/examples/OSS/Configurations/",
           "--report-path=/OpenPASS/reports",
           "test_end_to_end.json",
           "-v"],
       "request": "launch",
       "console": "integratedTerminal"
   }]
