..
  *******************************************************************************
  Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
                2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

.. _opgui_prerequisites:

Installing Prerequisites
========================

Apart from openPASS dependencies is required to have correctly installed 

Node.js (Version from 18.13.2) link below

.. tabs::
   .. tab:: Windows 11
  
      Available on this link: https://nodejs.org/dist/v18.13.0/node-v18.13.0-x86.msi
  
   .. tab:: Debian

      .. code-block:: 

         curl https://raw.githubusercontent.com/creationix/nvm/master/install.sh | bash 

      .. code-block:: 

         source ~/.profile 

      .. code-block:: 

         nvm install 18.13.0  


NPM (Version 8.5.0)

.. tabs::
  .. tab:: Windows 11
    
      Included with nodeJS on the .msi file
  
  .. tab:: Debian

     Bundled with nodeJS

Yarn (Version 1.22.17)

.. tabs::
   .. tab:: Windows 11

         Available on this link: https://github.com/yarnpkg/yarn/releases/download/v1.22.19/yarn-1.22.19.msi
  
   .. tab:: Debian
      
      .. code-block:: 
         
         npm install --global yarn
    