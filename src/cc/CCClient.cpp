/* XMRigCC
 * Copyright 2017-     BenDr0id    <ben@graef.in>
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

#include <cstring>
#include <sstream>
#include <fstream>
#include <3rdparty/rapidjson/stringbuffer.h>
#include <3rdparty/rapidjson/prettywriter.h>

#include "version.h"
#include "api/NetworkState.h"

#include "CCClient.h"
#include "App.h"
#include "ControlCommand.h"

#ifdef TYPE_AMD_GPU
#include "common/cpu/Cpu.h"
#include "common/log/Log.h"
#include "common/log/RemoteLog.h"
#include "common/Platform.h"
#include "core/Config.h"
#else
#include "Cpu.h"
#include "Mem.h"
#include "log/Log.h"
#include "log/RemoteLog.h"
#include "Platform.h"
#include "api/NetworkState.h"
#endif

#include "workers/Workers.h"
#include "workers/Hashrate.h"

#if _WIN32
#   include "winsock2.h"
#else

#   include "unistd.h"

#endif


CCClient* CCClient::m_self = nullptr;
uv_mutex_t CCClient::m_mutex;

#ifdef TYPE_AMD_GPU
CCClient::CCClient(xmrig::Config* config, uv_async_t* async)
#else
CCClient::CCClient(Options* config, uv_async_t* async)
#endif
        : m_config(config),
          m_async(async)
{
    uv_mutex_init(&m_mutex);

    m_self = this;

    std::string clientId;
    if (config->ccWorkerId()) {
        clientId =m_self->m_config->ccWorkerId();
    } else {
        char hostname[128];
        memset(hostname, 0, sizeof(hostname));
        gethostname(hostname, sizeof(hostname) - 1);
        clientId = std::string(hostname);
    }

    m_clientStatus.setClientId(clientId);
    m_clientStatus.setVersion(Version::string());


#ifdef TYPE_AMD_GPU
    m_clientStatus.setCpuBrand(xmrig::Cpu::info()->brand());
    m_clientStatus.setCpuAES(xmrig::Cpu::info()->hasAES());
    m_clientStatus.setCpuX64(xmrig::Cpu::info()->isX64());
    m_clientStatus.setCurrentThreads(static_cast<int>(config->threads().size()));
    m_clientStatus.setCurrentAlgoName(config->algorithm().name());
#else
    m_clientStatus.setCpuBrand(Cpu::brand());
    m_clientStatus.setCpuAES(Cpu::hasAES());
    m_clientStatus.setCpuSockets(static_cast<int>(Cpu::sockets()));
    m_clientStatus.setCpuCores(static_cast<int>(Cpu::cores()));
    m_clientStatus.setCpuThreads(static_cast<int>(Cpu::threads()));
    m_clientStatus.setCpuX64(Cpu::isX64());
    m_clientStatus.setCpuL2(static_cast<int>(Cpu::l2()));
    m_clientStatus.setCpuL3(static_cast<int>(Cpu::l3()));
    m_clientStatus.setCurrentThreads(static_cast<int>(config->threads()));
    m_clientStatus.setCurrentAlgoName(config->algoName());
#endif

    m_startTime = std::chrono::system_clock::now();

    if (config->ccToken() != nullptr) {
        m_authorization = std::string("Bearer ") + m_self->m_config->ccToken();
    }

    uv_thread_create(&m_thread, CCClient::onThreadStarted, this);
}

CCClient::~CCClient()
{
    uv_timer_stop(&m_timer);
    m_self = nullptr;
}

void CCClient::updateHashrate(const Hashrate* hashrate)
{
    if (m_self) {
        uv_mutex_lock(&m_mutex);

        m_self->m_clientStatus.setHashrateShort(hashrate->calc(Hashrate::ShortInterval));
        m_self->m_clientStatus.setHashrateMedium(hashrate->calc(Hashrate::MediumInterval));
        m_self->m_clientStatus.setHashrateLong(hashrate->calc(Hashrate::LargeInterval));
        m_self->m_clientStatus.setHashrateHighest(hashrate->highest());

        uv_mutex_unlock(&m_mutex);
    }
}


void CCClient::updateNetworkState(const xmrig::NetworkState& network)
{
    if (m_self) {
        uv_mutex_lock(&m_mutex);

        m_self->m_clientStatus.setCurrentStatus(Workers::isEnabled() ? ClientStatus::RUNNING : ClientStatus::PAUSED);
        m_self->m_clientStatus.setCurrentPool(network.pool);
        m_self->m_clientStatus.setSharesGood(network.accepted);
        m_self->m_clientStatus.setSharesTotal(network.accepted + network.rejected);
        m_self->m_clientStatus.setHashesTotal(network.total);
        m_self->m_clientStatus.setAvgTime(network.avgTime());

#ifdef TYPE_AMD_GPU
        m_self->m_clientStatus.setHashFactor(0);
        m_self->m_clientStatus.setHugepagesEnabled(false);
        m_self->m_clientStatus.setHugepages(false);
        m_self->m_clientStatus.setTotalPages(0);
        m_self->m_clientStatus.setTotalHugepages(0);
        m_self->m_clientStatus.setCurrentPowVariantName(xmrig::Algorithm::getVariantName(network.powVariant));
#else
        m_self->m_clientStatus.setHashFactor(Mem::hashFactor());
        m_self->m_clientStatus.setHugepagesEnabled(Mem::isHugepagesEnabled());
        m_self->m_clientStatus.setHugepages(Mem::isHugepagesAvailable());
        m_self->m_clientStatus.setTotalPages(Mem::getTotalPages());
        m_self->m_clientStatus.setTotalHugepages(Mem::getTotalHugepages());
        m_self->m_clientStatus.setCurrentPowVariantName(getPowVariantName(network.powVariant));
#endif

        uv_mutex_unlock(&m_mutex);
    }
}

#ifdef TYPE_AMD_GPU
void CCClient::updateGpuInfo(const std::vector<GpuContext *> gpuContext)
{
    if (m_self) {
        uv_mutex_lock(&m_mutex);

        m_self->m_clientStatus.clearGPUInfoList();

        for (auto gpu : gpuContext) {
            GPUInfo gpuInfo;
            gpuInfo.setName(gpu->name.data());
            gpuInfo.setCompMode(gpu->compMode);
            gpuInfo.setComputeUnits(gpu->computeUnits);
            gpuInfo.setDeviceIdx(gpu->deviceIdx);
            gpuInfo.setFreeMem(gpu->freeMem);
            gpuInfo.setWorkSize(gpu->workSize);
            gpuInfo.setMaxWorkSize(gpu->maximumWorkSize);
            gpuInfo.setMemChunk(gpu->memChunk);
            gpuInfo.setRawIntensity(gpu->rawIntensity);

            m_self->m_clientStatus.addGPUInfo(gpuInfo);
        }

        uv_mutex_unlock(&m_mutex);
    }
}
#endif

void CCClient::publishClientStatusReport()
{
    std::string requestUrl = "/client/setClientStatus?clientId=" + m_self->m_clientStatus.getClientId();
    std::string requestBuffer = m_self->m_clientStatus.toJsonString();

    auto res = performRequest(requestUrl, requestBuffer, "POST");
    if (!res) {
        LOG_ERR("[CC-Client] error: unable to performRequest POST -> http://%s:%d%s",
                m_self->m_config->ccHost(), m_self->m_config->ccPort(), requestUrl.c_str());
    } else if (res->status != 200) {
        LOG_ERR("[CC-Client] error: \"%d\" -> http://%s:%d%s", res->status,m_self->m_config->ccHost(),
                m_self->m_config->ccPort(), requestUrl.c_str());
    } else {
        ControlCommand controlCommand;
        if (controlCommand.parseFromJsonString(res->body)) {
            if (controlCommand.getCommand() == ControlCommand::START) {
                if (!Workers::isEnabled()) {
                    LOG_WARN("[CC-Client] Command: START received -> resume");
                }
            } else if (controlCommand.getCommand() == ControlCommand::STOP) {
                if (Workers::isEnabled()) {
                    LOG_WARN("[CC-Client] Command: STOP received -> pause");
                }
            } else if (controlCommand.getCommand() == ControlCommand::UPDATE_CONFIG) {
                LOG_WARN("[CC-Client] Command: UPDATE_CONFIG received -> update config");
                updateConfig();
            } else if (controlCommand.getCommand() == ControlCommand::PUBLISH_CONFIG) {
                LOG_WARN("[CC-Client] Command: PUBLISH_CONFIG received -> publish config");
                publishConfig();
            }else if (controlCommand.getCommand() == ControlCommand::RESTART) {
                LOG_WARN("[CC-Client] Command: RESTART received -> trigger restart");
            } else if (controlCommand.getCommand() == ControlCommand::SHUTDOWN) {
                LOG_WARN("[CC-Client] Command: SHUTDOWN received -> quit");
            } else if (controlCommand.getCommand() == ControlCommand::REBOOT) {
                LOG_WARN("[CC-Client] Command: REBOOT received -> trigger reboot");
            }

            m_self->m_async->data = reinterpret_cast<void*>(controlCommand.getCommand());
            uv_async_send(m_self->m_async);
        } else {
            LOG_ERR("[CC-Client] Unknown command received from CC Server.");
        }
    }
}

void CCClient::updateConfig()
{
    std::string requestUrl = "/client/getConfig?clientId=" + m_self->m_clientStatus.getClientId();
    std::string requestBuffer;

    auto res = performRequest(requestUrl, requestBuffer, "GET");
    if (!res) {
        LOG_ERR("[CC-Client] error: unable to performRequest GET -> http://%s:%d%s",
                m_self->m_config->ccHost(), m_self->m_config->ccPort(), requestUrl.c_str());
    } else if (res->status != 200) {
        LOG_ERR("[CC-Client] error: \"%d\" -> http://%s:%d%s", res->status, m_self->m_config->ccHost(),
                m_self->m_config->ccPort(), requestUrl.c_str());
    } else {
        rapidjson::Document document;
        if (!document.Parse(res->body.c_str()).HasParseError()) {
            std::ofstream clientConfigFile(m_self->m_config->fileName());
            if (clientConfigFile) {
                rapidjson::StringBuffer buffer(0, 65536);
                rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
                writer.SetMaxDecimalPlaces(10);
                document.Accept(writer);

                clientConfigFile << buffer.GetString();
                clientConfigFile.close();

                LOG_WARN("[CC-Client] Config updated. -> trigger restart");
            } else {
                LOG_ERR("[CC-Client] Not able to store client config to file %s.", m_self->m_config->fileName());
            }
        } else {
            LOG_ERR("[CC-Client] Not able to store client config. received client config is broken!");
        }
    }
}

void CCClient::publishConfig()
{
    std::string requestUrl = "/client/setClientConfig?clientId=" + m_self->m_clientStatus.getClientId();

    std::stringstream data;
    std::ifstream clientConfig(m_self->m_config->fileName());

    if (clientConfig) {
        data << clientConfig.rdbuf();
        clientConfig.close();
    }

    if (data.tellp() > 0) {
        rapidjson::Document document;
        document.Parse(data.str().c_str());

        if (!document.HasParseError()) {
            rapidjson::StringBuffer buffer(0, 65536);
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            writer.SetMaxDecimalPlaces(10);
            document.Accept(writer);

            auto res = performRequest(requestUrl, buffer.GetString(), "POST");
            if (!res) {
                LOG_ERR("[CC-Client] error: unable to performRequest POST -> http://%s:%d%s",
                        m_self->m_config->ccHost(), m_self->m_config->ccPort(), requestUrl.c_str());
            } else if (res->status != 200) {
                LOG_ERR("[CC-Client] error: \"%d\" -> http://%s:%d%s", res->status, m_self->m_config->ccHost(),
                        m_self->m_config->ccPort(), requestUrl.c_str());
            }
        } else {
            LOG_ERR("Not able to send config. Client config %s is broken!", m_self->m_config->fileName());
        }
    } else {
        LOG_ERR("Not able to load client config %s. Please make sure it exists!", m_self->m_config->fileName());
    }
}

std::shared_ptr<httplib::Response> CCClient::performRequest(const std::string& requestUrl,
                                                            const std::string& requestBuffer,
                                                            const std::string& operation)
{
    std::shared_ptr<httplib::Client> cli;

#   ifndef XMRIG_NO_TLS
    if (m_self->m_config->ccUseTls()) {
        cli = std::make_shared<httplib::SSLClient>(m_self->m_config->ccHost(), m_self->m_config->ccPort(), 10);
    } else {
#   endif
        cli = std::make_shared<httplib::Client>(m_self->m_config->ccHost(), m_self->m_config->ccPort(), 10);
#   ifndef XMRIG_NO_TLS
    }
#   endif

    httplib::Request req;
    req.method = operation;
    req.path = requestUrl;
    req.set_header("Host", "");
    req.set_header("Accept", "*/*");
    req.set_header("User-Agent", Platform::userAgent());
    req.set_header("Accept", "application/json");
    req.set_header("Content-Type", "application/json");

    if (!m_self->m_authorization.empty()) {
        req.set_header("Authorization", m_self->m_authorization.c_str());
    }

    if (!requestBuffer.empty()) {
        req.body = requestBuffer;
    }

    auto res = std::make_shared<httplib::Response>();

    return cli->send(req, *res) ? res : nullptr;
}

void CCClient::refreshUptime()
{
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_self->m_startTime);

    m_self->m_clientStatus.setUptime(static_cast<uint64_t>(uptime.count()));
}

void CCClient::refreshLog()
{
    m_self->m_clientStatus.setLog(RemoteLog::getRows());
}

void CCClient::onThreadStarted(void* handle)
{
    if (m_self) {
        uv_loop_init(&m_self->m_client_loop);

        uv_timer_init(&m_self->m_client_loop, &m_self->m_timer);
        uv_timer_start(&m_self->m_timer, CCClient::onReport,
                       static_cast<uint64_t>(m_self->m_config->ccUpdateInterval() * 1000),
                       static_cast<uint64_t>(m_self->m_config->ccUpdateInterval() * 1000));

        if (m_self->m_config->ccUploadConfigOnStartup()) {
            m_self->publishConfig();
        }

        uv_run(&m_self->m_client_loop, UV_RUN_DEFAULT);
    }
}

void CCClient::onReport(uv_timer_t* handle)
{
    if (m_self) {
        m_self->refreshUptime();
        m_self->refreshLog();

        m_self->publishClientStatusReport();
    }
}
