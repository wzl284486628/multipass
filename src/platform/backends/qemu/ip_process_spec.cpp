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

#include "ip_process_spec.h"

namespace mp = multipass;

mp::IPProcessSpec::IPProcessSpec()
{

}

QString mp::IPProcessSpec::program() const
{
    return QStringLiteral("ip");
}

QString mp::IPProcessSpec::apparmor_profile() const
{
    QString profile_template(R"END(
#include <tunables/global>
profile %1 flags=(attach_disconnected) {
  #include <abstractions/base>

  capability net_admin,
  capability sys_module,
  network netlink raw,

  /etc/iproute2/*  r,  # read when adding switch

  /dev/net/tun     rw, # for tun/tap device creation/deletion
}
    )END");

    return profile_template.arg(apparmor_profile_name());
}
