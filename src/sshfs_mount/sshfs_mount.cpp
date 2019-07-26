/*
 * Copyright (C) 2017-2019 Canonical, Ltd.
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

#include <multipass/sshfs_mount/sshfs_mount.h>

#include <multipass/exceptions/sshfs_missing_error.h>
#include <multipass/logging/log.h>
#include <multipass/ssh/ssh_session.h>
#include <multipass/sshfs_mount/sftp_server.h>
#include <multipass/utils.h>

#include <multipass/format.h>

namespace mp = multipass;
namespace mpl = multipass::logging;

namespace
{
constexpr auto category = "sshfs mount";

void check_sshfs_is_running(mp::SSHSession& session, mp::SSHProcess& sshfs_process, const std::string& source,
                            const std::string& target)
{
    using namespace std::literals::chrono_literals;

    // Make sure sshfs actually runs
    std::this_thread::sleep_for(250ms);
    if (session.run_ssh_cmd_for_status(fmt::format("pgrep -fx \".*sshfs.*{}.*{}\"", source, target)) != 0)
        throw std::runtime_error(sshfs_process.read_std_error());
}

void check_sshfs_exists(mp::SSHSession& session)
{
    try
    {
        session.run_ssh_cmd("which sshfs");
    }
    catch (const std::exception& e)
    {
        mpl::log(mpl::Level::warning, category,
                 fmt::format("Unable to determine if 'sshfs' is installed: {}", e.what()));
        throw mp::SSHFSMissingError();
    }
}

void make_target_dir(mp::SSHSession& session, const std::string& target)
{
    session.run_ssh_cmd(fmt::format("sudo mkdir -p \"{}\"", target));
}

void set_owner_for(mp::SSHSession& session, const std::string& target)
{
    auto vm_user = session.run_ssh_cmd("id -nu");
    auto vm_group = session.run_ssh_cmd("id -ng");
    mp::utils::trim_end(vm_user);
    mp::utils::trim_end(vm_group);

    session.run_ssh_cmd(fmt::format("sudo chown {}:{} \"{}\"", vm_user, vm_group, target));
}

auto create_sshfs_process(mp::SSHSession& session, const std::string& source, const std::string& target)
{
    check_sshfs_exists(session);
    make_target_dir(session, target);
    set_owner_for(session, target);

    auto sshfs_process = session.exec(fmt::format(
        "sudo sshfs -o slave -o nonempty -o transform_symlinks -o allow_other :\"{}\" \"{}\"", source, target));

    check_sshfs_is_running(session, sshfs_process, source, target);

    return sshfs_process;
}

auto make_sftp_server(mp::SSHSession&& session, const std::string& source, const std::string& target,
                      const std::unordered_map<int, int>& gid_map, const std::unordered_map<int, int>& uid_map)
{
    mpl::log(mpl::Level::debug, category,
             fmt::format("{}:{} {}(source = {}, target = {}, …): ", __FILE__, __LINE__, __FUNCTION__, source, target));

    auto sshfs_proc =
        create_sshfs_process(session, mp::utils::escape_char(source, '"'), mp::utils::escape_char(target, '"'));

    auto output = session.run_ssh_cmd("id -u");
    mpl::log(mpl::Level::debug, category,
             fmt::format("{}:{} {}(): `id -u` = {}", __FILE__, __LINE__, __FUNCTION__, output));
    auto default_uid = std::stoi(output);
    output = session.run_ssh_cmd("id -g");
    mpl::log(mpl::Level::debug, category,
             fmt::format("{}:{} {}(): `id -g` = {}", __FILE__, __LINE__, __FUNCTION__, output));
    auto default_gid = std::stoi(output);

    return std::make_unique<mp::SftpServer>(std::move(session), std::move(sshfs_proc), source, gid_map, uid_map,
                                            default_uid, default_gid);
}

} // namespace anonymous

mp::SshfsMount::SshfsMount(SSHSession&& session, const std::string& source, const std::string& target,
                           const std::unordered_map<int, int>& gid_map, const std::unordered_map<int, int>& uid_map)
    : sftp_server{make_sftp_server(std::move(session), source, target, gid_map, uid_map)}, sftp_thread{[this] {
          sftp_server->run();
          emit finished();
      }}
{
}

mp::SshfsMount::~SshfsMount()
{
    stop();
}

void mp::SshfsMount::stop()
{
    sftp_server->stop();
    if (sftp_thread.joinable())
        sftp_thread.join();
}
