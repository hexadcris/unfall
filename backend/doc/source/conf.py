################################################################################
# Copyright (c) 2021 in-tech GmbH
# Copyright (c) 2023 Hexad GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

# Configuration file for the Sphinx documentation builder. See also:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os
import sys
import datetime

# -- Path setup --------------------------------------------------------------
sys.path.append(os.path.abspath("_ext"))

# -- Project information -----------------------------------------------------
project = 'opGUI'
copyright = f'{datetime.datetime.now().year} OpenPASS Working Group'
author = 'Hexad GmbH'

# -- Version is generated via cmake
version_file = 'version.txt'
if os.path.exists(version_file):
    with open(version_file) as vf:
        version = vf.read().strip()
        release = version

# -- General configuration ---------------------------------------------------
def setup(app):
   app.add_css_file('css/custom.css')

extensions = []

extensions.append("sphinx_rtd_theme")
extensions.append('sphinx.ext.todo')
extensions.append('sphinx_tabs.tabs')
extensions.append('sphinx.ext.imgmath')
extensions.append('api_doc')

templates_path = ['_templates']

exclude_patterns = []

todo_include_todos = True

pdf_stylesheets = ['sphinx', 'kerning', 'a4']
pdf_style_path = ['.', '_styles']
pdf_fit_mode = "shrink" # literal blocks wider than frame
pdf_language = "en_US"
pdf_page_template = 'cutePage'

# -- Options for HTML output -------------------------------------------------

html_static_path = ['_static']
html_theme = 'sphinx_rtd_theme'
html_title = 'opGUI Documentation'
html_short_title = 'opGUI|Doc'
html_favicon = '_static/openPASS.ico'
html_logo = '_static/openPASS.png'

# -- Options for API DOC -----------------------------------------------------
api_doc_title = "Source Code Documentation"

# -- Define global replacements ----------------------------------------------
# See https://documentation.help/Sphinx/config.html
rst_epilog = """

.. |op| replace:: **openPASS**
.. |Op| replace:: **OpenPASS**
.. |opwg| replace:: **openPASS** Working Group
.. |op_oss| replace:: **openPASS** (Open Source)
.. |Op_oss| replace:: **OpenPASS** (Open Source)
.. |mingw_shell| replace:: ``MinGW 64-bit`` shell

"""
