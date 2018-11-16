/*
 * Copyright (C) 2018 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MULTIPASS_PING_PROCESS_SPEC_H
#define MULTIPASS_PING_PROCESS_SPEC_H

#include <multipass/process_spec.h>

namespace multipass
{

class PingProcessSpec : public ProcessSpec
{
public:
    PingProcessSpec() = default;

    QString program() const override;
    QString apparmor_profile() const override;
};

} // namespace multipass

#endif // PING_PROCESS_SPEC_H
