################################################################################
# Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import os
import shutil
from textwrap import dedent

from sphinx.directives.other import TocTree

class ARGS:
    """Default argument values"""
    API_DOC_BUILD_DEACTIVATED = "off"
    DEFAULT_API_DOC_ROOT = "../../../sim"
    DEFAULT_API_DOC_BUILD = API_DOC_BUILD_DEACTIVATED
    DEFAULT_API_DOC_PATH = "api"
    DEFAULT_API_DOC_TITLE = "Source Code Documentation"


def api_doc_build_activated(config):
    """True if api_doc_build is NOT set to deactivated"""
    return config.api_doc_build.lower() != ARGS.API_DOC_BUILD_DEACTIVATED


def purge_api_doc_path(app, config):
    """Purges the rst files from the build folder"""
    api_doc_path = os.path.join(app.srcdir, config.api_doc_path)
    if os.path.exists(api_doc_path):
        shutil.rmtree(api_doc_path, ignore_errors=True)


def on_config_inited(app, config):
    """
    Sphinx Event Hook:

    Activates and configures breath and exhale if api_doc_build is active
    """
    if api_doc_build_activated(config):
        app.setup_extension('breathe')
        app.setup_extension('exhale')

        config.exhale_args = {
            # These arguments are required
            "containmentFolder":     f"./{config.api_doc_path}",
            "rootFileName":          "index.rst",
            "rootFileTitle":         config.api_doc_title,
            "doxygenStripFromPath":  config.api_doc_root,
            # Suggested optional arguments
            "createTreeView":        True,
            # TIP: if using the sphinx-bootstrap-theme, you need
            # "treeViewIsBootstrap": True,
            "exhaleExecutesDoxygen": True,
            "exhaleDoxygenStdin":    dedent(f'''
                INPUT                 = {config.api_doc_root}/src
                WARN_IF_UNDOCUMENTED  = NO
                WARN_AS_ERROR         = NO
                PREDEFINED           += DOXYGEN_SHOULD_SKIP_THIS
                GENERATE_HTML         = NO
                RECURSIVE             = YES
                FULL_PATH_NAMES       = YES
                QUIET                 = YES
                FILE_PATTERNS         = *.cpp *.h
                ENABLE_PREPROCESSING  = YES
                MACRO_EXPANSION       = YES
                SKIP_FUNCTION_MACROS  = NO
                EXPAND_ONLY_PREDEF    = NO
            ''')}

        config.breathe_projects = {"opGUI": "doxyoutput/xml"}
        config.breathe_default_project = "opGUI"
        config.cpp_id_attributes = ['Q_DECL_IMPORT']
    else:
        purge_api_doc_path(app, config)


class ApiDoc(TocTree):
    """
    Customized TocTree Directive `.. toctree-api::`

    Enhances the regular toctree by a conditional :api-doc: entry.
    If api_doc_build is active :api-doc: will be replaced by the
    entry point to the api documentation such as api/index.rst

    Use config values `api_doc_path` and `api_doc_title` to adjust
    subdirectory and title shown in the toc, respectively.

    Example:

    .. toctree-api::
       :caption: Developer Information
       :glob:

       developer_information/*
       :api-doc:

    """
    API_DOC_DIRECTIVE = "toctree-api"
    API_DOC_TOKEN = ':api-doc:'

    @property
    def api_doc_active(self):
        return api_doc_build_activated(self.state.document.settings.env.config)

    @property
    def api_doc_entry_point(self):
        config = self.state.document.settings.env.config
        return f'/{config.api_doc_path}/index.rst'

    def _parse_conditional_api_doc(self):
        """
        Scans for the API_DOC_TOKEN and transforms it if active

        Returns a new list of toc entries (= self.content)
        """
        toc_entries = list()
        for toc_entry in self.content:
            if toc_entry.strip() == self.API_DOC_TOKEN:
                if self.api_doc_active:
                    toc_entries.append(self.api_doc_entry_point)
            else:
                toc_entries.append(toc_entry)
        return toc_entries

    def run(self):
        self.content = self._parse_conditional_api_doc()
        return super().run()


def setup(app):
    app.connect('config-inited', on_config_inited)

    app.add_directive(ApiDoc.API_DOC_DIRECTIVE, ApiDoc)

    app.add_config_value(name='api_doc_root',
                         default=ARGS.DEFAULT_API_DOC_ROOT, rebuild='env')
    app.add_config_value(name='api_doc_build',
                         default=ARGS.DEFAULT_API_DOC_BUILD, rebuild='env')
    app.add_config_value(name='api_doc_path',
                         default=ARGS.DEFAULT_API_DOC_PATH, rebuild='env')
    app.add_config_value(name='api_doc_title',
                         default=ARGS.DEFAULT_API_DOC_TITLE, rebuild='env')

    return {'version': '0.1'}
