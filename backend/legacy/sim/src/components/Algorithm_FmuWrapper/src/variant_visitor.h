/********************************************************************************
 * Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/


#pragma once

template <class... visitor>
struct variant_visitor : visitor...
{
    using visitor::operator()...;
};

template <class... visitor>
variant_visitor(visitor...) -> variant_visitor<visitor...>;
