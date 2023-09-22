/********************************************************************************
* Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* http://www.eclipse.org/legal/epl-2.0.
*
* SPDX-License-Identifier: EPL-2.0
********************************************************************************/

#pragma once

#include <cmath>

namespace openpass
{
// Borges, Carlos F. "An Improved Algorithm for hypot (a, b)." arXiv preprint arXiv:1904.09481 (2019).
[[nodiscard]] double constexpr hypot(double a, double b)
{
    if (a == 0.0) return std::abs(b);
    if (b == 0.0) return std::abs(a);
    const auto h = std::sqrt(std::fma(a,a,b*b));
    const auto h_sq = h * h ;
    const auto a_sq = a * a;
    const auto x = std::fma(-b, b, h_sq-a_sq) + std::fma(h, h, -h_sq) - std::fma(a, a, -a_sq);
    return h - x/(2*h);
}
}