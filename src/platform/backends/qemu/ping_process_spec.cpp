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

#include "ping_process_spec.h"

namespace mp = multipass;

QString mp::PingProcessSpec::process() const
{
    return QStringLiteral("ping");
}

QString mp::PingProcessSpec::apparmor_profile() const
{
    QString profile_template(R"END(
#include <tunables/global>
profile %1 flags=(attach_disconnected) {
  #include <abstractions/base>

  capability net_raw,

  network inet    dgram,
  network inet6   dgram,
  network inet    raw,
  network inet6   raw,
}
    )END");

    return profile_template.arg(apparmor_profile_name());
}
