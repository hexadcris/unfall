################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

# The rst files of the sphinx documentation contain PLACEHOLDERS for referencing
# files outside the source folder of the documentation. This makes references
# into the repository "as a whole" easier, but also introduces a step for
# resolving these references before building. To prevent changes in the original
# documentation source, the this file copies the original source folder ${SRC}
# into a new ${DST} folder. After that, the PLACEHOLDERS are updated w.r.t
# to the the new origin.
#
# Currently supported PLACEHOLDERS
# - @OP_REL_ROOT@ => relative path to root of the openpass repository
# - @OP_REL_GUI@  => resolves to @OP_REL_ROOT@/gui
# - @OP_REL_SIM@  => resolves to @OP_REL_ROOT@/sim

macro(copy_documentation source destination)
    message(VERBOSE "Copy ${source} to ${destination}")
    file(COPY ${source} DESTINATION ${destination})
endmacro()

macro(update_placeholder source destination)
    message(VERBOSE "Updating ${source} to ${destination}")
    file(RELATIVE_PATH target ${destination} ${source}/../..)

    # Remove potential trailing "/"
    string(REGEX REPLACE "(.*)/$" "\\1" target ${target})

    # Placeholder for conf.py: no initial '/' => real relative paths
    set(OP_REL_ROOT ../${target})         # relative path to repository root
    set(OP_REL_GUI ${OP_REL_ROOT}/gui)    # relative path to gui root
    set(OP_REL_SIM ${OP_REL_ROOT}/sim)    # relative path to simulation root

    configure_file(${destination}/source/conf.py
                   ${destination}/source/conf.py @ONLY)

    # Placeholder for RST files: use initial '/' => sphinx style for "from source"
    # Override old one, because we want to use the same placeholder in both contexts
    set(OP_REL_ROOT /${OP_REL_ROOT})
    set(OP_REL_GUI /${OP_REL_GUI})
    set(OP_REL_SIM /${OP_REL_SIM})

    file(GLOB_RECURSE rstFiles LIST_DIRECTORIES false ${destination}/*.rst)

    foreach(rstFile IN LISTS rstFiles)
        message(DEBUG "Replacing placeholders in ${rstFile}")
        configure_file(${rstFile} ${rstFile} @ONLY)
    endforeach()
endmacro()

copy_documentation(${SRC} ${DST})
update_placeholder(${SRC} ${DST})
