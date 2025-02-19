#ifndef _LIFTER_SETTINGS_H_
#define _LIFTER_SETTINGS_H_

#include "JSONFileManager.h"

#include <mutex>
#include <memory>

#include <spdlog/spdlog.h>

namespace settings {

class Foo;

class LifterSettings : public JSONFileManager {

private:
    // see the note in CodegenSettings.h for more details.
    class Foo {};
    explicit LifterSettings();
    static void initInstance();

    static std::shared_ptr<LifterSettings> m_instance;
    static std::once_flag m_flag;
    std::shared_ptr<spdlog::logger> logger;

    // settings variables
    std::string m_serverUrl;
    std::string m_binaryPath;
    static inline const std::string m_fileName = "../settings/LifterSettings.json";

public:
    // dummy constructor
    explicit LifterSettings(Foo foo) {};

    static std::shared_ptr<LifterSettings> getInstance();

    std::string getBinaryName() const;

    void writeSettingsToFile();
    void readSettingsFromFile();

    // delete copy/move constructors and assignment/move operators.
    LifterSettings(LifterSettings const&) = delete;
    LifterSettings(LifterSettings&&) = delete;
    void operator=(LifterSettings const&) = delete;
    void operator=(LifterSettings&&) = delete;

    std::string getServerUrl() const;
    void setServerUrl(const std::string& serverUrl);
    std::string getBinaryPath() const;
    void setBinaryPath(const std::string& binaryPath);
};

}   // namespace settings

#endif