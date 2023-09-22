..
  *******************************************************************************
    Copyright (c) 2023 Hexad GmbH


    This program and the accompanying materials are made available under the
    terms of the Eclipse Public License 2.0 which is available at
    http://www.eclipse.org/legal/epl-2.0.

    SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

.. _installation:

Installation
============

You can either execute opGUI installation with the following command:

.. tabs::
   .. tab:: Windows 11
  
      Go to the project diretory which shall be placed on your home
      
      .. code-block:: 

         cd %USERPROFILE%\opgui

      Execute yarn 
  
      .. code-block:: 

         yarn install

      Execute the installation procedure
  
      .. code-block:: 

         yarn win-install
  
   .. tab:: Debian
  
      Go to the project diretory which shall be placed on your home
  
      .. code-block:: 

         cd $HOME/opgui

      Execute yarn 
  
      .. code-block:: 

         yarn install

      Execute the installation procedure
  
      .. code-block:: 

         yarn deb-install

After the installation process finishes there is one more thing remaining, to configure a JSON file on the backend folder. the file is backendConfig.json and it contains the following content:

- **"PATH_OPENPASS_CORE"**: has to be setted into the correct core folder, inside the compiled opSimulation folder, which must be inside the home folder aswell (Users for Windows OS).

- **"OPSIMULATION_EXE"**: has to be setted into the name of the opSimulation executable (opSimulation on Linux or opSimulation.exe on Windows).

- **"OPSIMULATION_MANAGER_XML"**: do not modify this value at this point, in the future this will allow any user to select the default opSimulationManager.xml file to use (by default now is setted to opSimulationManager.xml).

- **"OPSIMULATION_MANAGER_EXE"**: has to be setted into the name of the opSimulationManager executable (opSimulation on Linux or opSimulationManager.exe on Windows).

- **"WORKSPACE"**: has to be setted into an existing folder inside your Documents folder (by default will be /Documents/workspaces/opGUI/workspace1 on any OS, will be improved in the future).

- **"BACKEND_PORT"**: do not modify this value at this point, it allows a user to modify the port used by the application (default setted to 8848).

- **"BACKEND_IP"**: do not modify this value at this point, it allows a user to modify ip address where the application is located (default is 127.0.0.1).

   ..  "PATH_OPENPASS_CORE":"<ABSOLUTE PATH TO HOME>/opSimulation/bin/core",
   ..  "OPSIMULATION_EXE": "opSimulation",
   ..  "OPSIMULATION_MANAGER_XML": "opSimulationManager.xml",
   ..  "OPSIMULATION_MANAGER_EXE": "opSimulationManager",
   ..  "WORKSPACE": "<ABSOLUTE PATH TO HOME>/Documents/workspaces/opGUI/workspace1",
   ..  "BACKEND_PORT":8848,
   ..  "BACKEND_IP":"127.0.0.1" 

.. or follow the next steps (TBD)

.. 1 Set the project on the correct destination

.. Go to your user home and place the opgui repository inside, this can be done executing

.. cd $home

.. git clone https://gitlab.eclipse.org/eclipse/openpass/opgui.git

.. NOTE (If you placed opSimulation nor opGUI on another folders, please copy or move them into your home for security reasons)


.. 2 Build the backend

.. Go into the backend folder
.. > cd $HOME\opgui\backend
.. > mkdir build
.. > cd build
.. > cmake -G "MinGW Makefiles" -DWITH_TESTS=OFF -DWITH_DOC=ON ..
.. > cmake --build .

.. 3 Build the frontend

.. Go into thee frontend folder

.. > cd $HOME\opgui\frontend
.. > yarn install
.. > yarn build
.. > cp -r dist ..\backend\build

.. 4 Execute the opGUI

.. open a terminal and execute
.. > $home\opgui\backend\build\OPGUICore.exe

.. Then open google chrome browser and type the following address: http;//localhost:8848

.. Note: this GUI is not connected to the internet, visible urls are for user guidance only