/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2016-2018 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 *
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

#ifndef XMRIG_CONFIGLOADER_PLATFORM_H
#define XMRIG_CONFIGLOADER_PLATFORM_H


#ifdef _MSC_VER
#   include "getopt/getopt.h"
#else
#   include <getopt.h>
#endif


#include "common/interfaces/IConfig.h"
#include "version.h"


namespace xmrig {


static char const short_options[] = "a:c:khBp:Px:r:R:s:T:o:u:O:Vl:S";


static struct option const options[] = {
    { "algo",                 1, nullptr, xmrig::IConfig::AlgorithmKey      },
    { "api-access-token",     1, nullptr, xmrig::IConfig::ApiAccessTokenKey },
    { "api-port",             1, nullptr, xmrig::IConfig::ApiPort           },
    { "api-worker-id",        1, nullptr, xmrig::IConfig::ApiWorkerIdKey    },
    { "api-id",               1, nullptr, xmrig::IConfig::ApiIdKey          },
    { "api-ipv6",             0, nullptr, xmrig::IConfig::ApiIPv6Key        },
    { "api-no-restricted",    0, nullptr, xmrig::IConfig::ApiRestrictedKey  },
    { "background",           0, nullptr, xmrig::IConfig::BackgroundKey     },
    { "config",               1, nullptr, xmrig::IConfig::ConfigKey         },
    { "donate-level",         1, nullptr, xmrig::IConfig::DonateLevelKey    },
    { "dry-run",              0, nullptr, xmrig::IConfig::DryRunKey         },
    { "keepalive",            0, nullptr, xmrig::IConfig::KeepAliveKey      },
    { "log-file",             1, nullptr, xmrig::IConfig::LogFileKey        },
    { "nicehash",             0, nullptr, xmrig::IConfig::NicehashKey       },
    { "no-color",             0, nullptr, xmrig::IConfig::ColorKey          },
    { "no-watch",             0, nullptr, xmrig::IConfig::WatchKey          },
    { "variant",              1, nullptr, xmrig::IConfig::VariantKey        },
    { "pass",                 1, nullptr, xmrig::IConfig::PasswordKey       },
    { "print-time",           1, nullptr, xmrig::IConfig::PrintTimeKey      },
    { "retries",              1, nullptr, xmrig::IConfig::RetriesKey        },
    { "retry-pause",          1, nullptr, xmrig::IConfig::RetryPauseKey     },
    { "syslog",               0, nullptr, xmrig::IConfig::SyslogKey         },
    { "url",                  1, nullptr, xmrig::IConfig::UrlKey            },
    { "user",                 1, nullptr, xmrig::IConfig::UserKey           },
    { "user-agent",           1, nullptr, xmrig::IConfig::UserAgentKey      },
    { "userpass",             1, nullptr, xmrig::IConfig::UserpassKey       },
    { "rig-id",               1, nullptr, xmrig::IConfig::RigIdKey          },
    { "tls",                  0, nullptr, xmrig::IConfig::TlsKey            },
    { "tls-fingerprint",      1, nullptr, xmrig::IConfig::FingerprintKey    },
    { "opencl-affinity",      1, nullptr, xmrig::IConfig::OclAffinityKey    },
    { "opencl-devices",       1, nullptr, xmrig::IConfig::OclDevicesKey     },
    { "opencl-launch",        1, nullptr, xmrig::IConfig::OclLaunchKey      },
    { "opencl-platform",      1, nullptr, xmrig::IConfig::OclPlatformKey    },
    { "opencl-strided-index", 1, nullptr, xmrig::IConfig::OclSridedIndexKey },
    { "opencl-mem-chunk",     1, nullptr, xmrig::IConfig::OclMemChunkKey    },
    { "opencl-unroll-factor", 1, nullptr, xmrig::IConfig::OclUnrollKey      },
    { "opencl-unroll",        1, nullptr, xmrig::IConfig::OclUnrollKey      },
    { "opencl-comp-mode",     1, nullptr, xmrig::IConfig::OclCompModeKey    },
    { "no-cache",             0, nullptr, xmrig::IConfig::OclCacheKey       },
    { "print-platforms",      0, nullptr, xmrig::IConfig::OclPrintKey       },
    { "opencl-loader",        1, nullptr, xmrig::IConfig::OclLoaderKey      },
    { "cc-url",               1, nullptr, xmrig::IConfig::CCUrlKey },
    { "cc-access-token",      1, nullptr, xmrig::IConfig::CCAccessTokenKey },
    { "cc-worker-id",         1, nullptr, xmrig::IConfig::CCWorkerIdKey },
    { "cc-update-interval-s", 1, nullptr, xmrig::IConfig::CCUpdateIntervalKey },
    { "cc-use-tls",           0, nullptr, xmrig::IConfig::CCUseTlsKey },
    { "cc-use-remote-logging",       0, nullptr, xmrig::IConfig::CCUseRemoteLoggingKey },
    { "cc-upload-config-on-startup", 0, nullptr, xmrig::IConfig::CCUploadConfigOnStartupKey },
    { "start-cmd",            1, nullptr, xmrig::IConfig::StartCmdKey },
    { "reboot-cmd",           1, nullptr, xmrig::IConfig::RebootCmdKey },
    { "cc-reboot-cmd",        1, nullptr, xmrig::IConfig::RebootCmdKey },
    { "rig-watchdog",         0, nullptr, xmrig::IConfig::RigWatchdogKey },
    { "reboot-on-cardcrash",  0, nullptr, xmrig::IConfig::RebootOnCardcrashKey },
    { "min-rig-hashrate",     1, nullptr, xmrig::IConfig::MinRigHashrateKey },
    { "daemonized",           0, nullptr, xmrig::IConfig::DaemonizedKey },
    { nullptr,                0, nullptr, 0 }
};


static struct option const config_options[] = {
    { "algo",              1, nullptr, xmrig::IConfig::AlgorithmKey   },
    { "background",        0, nullptr, xmrig::IConfig::BackgroundKey  },
    { "colors",            0, nullptr, xmrig::IConfig::ColorKey       },
    { "donate-level",      1, nullptr, xmrig::IConfig::DonateLevelKey },
    { "dry-run",           0, nullptr, xmrig::IConfig::DryRunKey      },
    { "log-file",          1, nullptr, xmrig::IConfig::LogFileKey     },
    { "print-time",        1, nullptr, xmrig::IConfig::PrintTimeKey   },
    { "retries",           1, nullptr, xmrig::IConfig::RetriesKey     },
    { "retry-pause",       1, nullptr, xmrig::IConfig::RetryPauseKey  },
    { "syslog",            0, nullptr, xmrig::IConfig::SyslogKey      },
    { "user-agent",        1, nullptr, xmrig::IConfig::UserAgentKey   },
    { "watch",             0, nullptr, xmrig::IConfig::WatchKey       },
    { "opencl-platform",   1, nullptr, xmrig::IConfig::OclPlatformKey },
    { "cache",             0, nullptr, xmrig::IConfig::OclCacheKey    },
    { "opencl-loader",     1, nullptr, xmrig::IConfig::OclLoaderKey   },
    { "autosave",          0, nullptr, xmrig::IConfig::AutoSaveKey    },
    { "start-cmd",         1, nullptr, xmrig::IConfig::StartCmdKey    },
    { "reboot-cmd",        1, nullptr, xmrig::IConfig::RebootCmdKey   },
    { "rig-watchdog",      0, nullptr, xmrig::IConfig::RigWatchdogKey },
    { "reboot-on-cardcrash",  0, nullptr, xmrig::IConfig::RebootOnCardcrashKey },
    { "min-rig-hashrate",  1, nullptr, xmrig::IConfig::MinRigHashrateKey },
    { "daemonized",        0, nullptr, xmrig::IConfig::DaemonizedKey  },
    { nullptr,             0, nullptr, 0 }
};


static struct option const api_options[] = {
    { "port",          1, nullptr, xmrig::IConfig::ApiPort           },
    { "access-token",  1, nullptr, xmrig::IConfig::ApiAccessTokenKey },
    { "worker-id",     1, nullptr, xmrig::IConfig::ApiWorkerIdKey    },
    { "ipv6",          0, nullptr, xmrig::IConfig::ApiIPv6Key        },
    { "restricted",    0, nullptr, xmrig::IConfig::ApiRestrictedKey  },
    { "id",            1, nullptr, xmrig::IConfig::ApiIdKey          },
    { nullptr,         0, nullptr, 0 }
};


static struct option const cc_client_options[] = {
    { "url",                        1, nullptr, xmrig::IConfig::CCUrlKey },
    { "access-token",               1, nullptr, xmrig::IConfig::CCAccessTokenKey },
    { "worker-id",                  1, nullptr, xmrig::IConfig::CCWorkerIdKey },
    { "update-interval-s",          1, nullptr, xmrig::IConfig::CCUpdateIntervalKey },
    { "use-tls",                    0, nullptr, xmrig::IConfig::CCUseTlsKey },
    { "use-remote-logging",         0, nullptr, xmrig::IConfig::CCUseRemoteLoggingKey },
    { "upload-config-on-startup",   1, nullptr, xmrig::IConfig::CCUploadConfigOnStartupKey },
    { "start-cmd",                  1, nullptr, xmrig::IConfig::StartCmdKey },
    { "reboot-cmd",                 1, nullptr, xmrig::IConfig::RebootCmdKey },
    { 0, 0, 0, 0 }
};


} /* namespace xmrig */

#endif /* XMRIG_CONFIGLOADER_PLATFORM_H */
