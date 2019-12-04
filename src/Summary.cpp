/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2016-2018 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <inttypes.h>
#include <stdio.h>
#include <uv.h>


#if defined(__APPLE__)
#   include <OpenCL/cl.h>
#else
#   include "3rdparty/CL/cl.h"
#endif


#include "common/cpu/Cpu.h"
#include "common/log/Log.h"
#include "core/Config.h"
#include "core/Controller.h"
#include "Summary.h"
#include "version.h"


static void print_cpu(xmrig::Config *config)
{
    if (config->isColors()) {
        Log::i()->text(GREEN_BOLD(" * ") WHITE_BOLD("%-13s") WHITE_BOLD("%s %sx64 %sAES"),
                       "CPU",
                       xmrig::Cpu::info()->brand(),
                       xmrig::Cpu::info()->isX64() ? "\x1B[1;32m" : "\x1B[1;31m-",
                       xmrig::Cpu::info()->hasAES() ? "\x1B[1;32m" : "\x1B[1;31m-");
    }
    else {
        Log::i()->text(" * %-13s%s %sx64 %sAES", "CPU", xmrig::Cpu::info()->brand(), xmrig::Cpu::info()->isX64() ? "" : "-", xmrig::Cpu::info()->hasAES() ? "" : "-");
    }
}


static void print_algo(xmrig::Config *config)
{
    Log::i()->text(config->isColors() ? GREEN_BOLD(" * ") WHITE_BOLD("%-13s%s, %sdonate=%d%%")
                                      : " * %-13s%s, %sdonate=%d%%",
                   "ALGO",
                   config->algorithm().name(),
                   config->isColors() && config->donateLevel() == 0 ? "\x1B[1;31m" : "",
                   config->donateLevel()
    );
}


#ifndef XMRIG_NO_CC
static void print_cc(xmrig::Config *config)
{
    if (config->ccHost() == nullptr) {
        return;
    }

    Log::i()->text(config->isColors() ? "\x1B[01;32m * \x1B[01;37mCC Server:   \x1B[01;36m%s:%d %s" : " * CC Server:   %s:%d %s",
                   config->ccHost(),
                   config->ccPort(),
                   config->ccUseTls() ? "(TLS)" : "");
}
#endif
static void print_commands(xmrig::Config *config)
{
    if (config->isColors()) {
        Log::i()->text(GREEN_BOLD(" * ") WHITE_BOLD("COMMANDS     ") MAGENTA_BOLD("h") WHITE_BOLD("ashrate, ")
                                                                     MAGENTA_BOLD("p") WHITE_BOLD("ause, ")
                                                                     MAGENTA_BOLD("r") WHITE_BOLD("esume"));
    }
    else {
        Log::i()->text(" * COMMANDS     'h' hashrate, 'p' pause, 'r' resume");
    }
}


void Summary::print(xmrig::Controller *controller)
{
    controller->config()->printVersions();
    print_cpu(controller->config());
    print_algo(controller->config());
    controller->config()->printPools();
    controller->config()->printAPI();

#   ifndef XMRIG_NO_CC
    print_cc(controller->config());
#   endif

    print_commands(controller->config());
}



