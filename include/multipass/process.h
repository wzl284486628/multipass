/*
 * Copyright (C) 2019 Canonical, Ltd.
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

#ifndef MULTIPASS_PROCESS_H
#define MULTIPASS_PROCESS_H

#include <QProcess>
#include <memory>
#include <multipass/logging/level.h>

namespace multipass
{

class Process : public QProcess
{
    Q_OBJECT
public:
    using UPtr = std::unique_ptr<Process>;

    Process(multipass::logging::Level log_level);
    virtual ~Process() = default;

    void start(const QStringList& extra_arguments = QStringList());

    bool run_and_return_status(const QStringList& extra_arguments = QStringList(), const int timeout = 30000);
    QString run_and_return_output(const QStringList& extra_arguments = QStringList(), const int timeout = 30000);

protected:
    void run_and_wait_until_finished(const QStringList& extra_arguments = QStringList(), const int timeout = 30000);
};

} // namespace multipass

#endif // MULTIPASS_PROCESS_H
