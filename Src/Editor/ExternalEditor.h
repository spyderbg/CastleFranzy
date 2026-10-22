#pragma once
#include <string>

// Optional local file bridge, enabled only with --editor-bridge.
class ExternalEditor
{
public:
    explicit ExternalEditor(const std::string& directory);
    void update();
    bool paused() const { return m_paused; }
private:
    std::string m_directory;
    std::string m_error;
    long long m_sequence;
    bool m_paused;
    bool m_autoStart;
    bool m_touchDown;
    unsigned int m_lastStatus;
};
