#include "stdafx.h"
#include "ExternalEditor.h"
#include "GS_Editor.h"
#include "Game/Playfield.h"
#include "Game/Objects/Unit.h"
#include "Game/Config/GameConfig.h"
#include "GameStates/GSLevel.h"
#include "IO/TouchScreen/TouchPad.h"
#include <fstream>
#include <sstream>
#include <set>
#include <cstdio>

ExternalEditor::ExternalEditor(const std::string& directory)
    : m_directory(directory), m_sequence(0), m_paused(false), m_autoStart(false), m_touchDown(false), m_lastStatus(0) {}

void ExternalEditor::update()
{
    if (m_directory.empty()) return;
    CPlayfield* field = CPlayfield::GetInstance();
    if (m_touchDown) { CTouchPad::EventTouchUp(0, 240, 160); m_touchDown = false; }
    // Test launches continue past the native loading screen's tap-to-play prompt.
    if (m_autoStart && field->GetProgress() == 100)
    {
        CTouchPad::EventTouchDown(0, 240, 160);
        m_touchDown = true;
        m_autoStart = false;
    }
    const bool ready = !g_unitTypes.empty() && __MGR_STATES()->stackSize() > 0;
    std::ifstream input((m_directory + "/command.txt").c_str());
    long long sequence = 0;
    std::string command;
    int level = 0, day = 0;
    if (ready && input >> sequence >> command >> level >> day && sequence > m_sequence)
    {
        m_sequence = sequence;
        m_error.clear();
        if (command == "play")
        {
            // Refresh the runtime's cached index after structural level edits.
            array<int>& counts = const_cast<array<int>&>(getNumberOfDaysPerLevel());
            counts.clear();
            const array<int>& fresh = getNumberOfDaysPerLevel();
            if (level < 0 || level >= (int)fresh.size() || day < 0 || day >= fresh[level])
                m_error = "Invalid level or day";
            else
            {
                if (g_editor) g_editor->exit();
                field->StartNewLevel(level);
                if (day != 0)
                {
                    field->m_CurrentDay = day;
                    field->WriteProfileStats();
                }
                m_paused = false;
                m_autoStart = true;
            }
        }
        else if (command == "pause") m_paused = true;
        else if (command == "resume") m_paused = false;
        else if (command == "die" && field->isValid() && field->IsLevelLoaded() && field->getGate())
        {
            const bool invulnerable = CGameConfig::Invulnerable;
            CGameConfig::Invulnerable = false;
            field->getGate()->damageGate(99999);
            CGameConfig::Invulnerable = invulnerable;
        }
        else if (command == "overlay")
        {
            GS_Editor::GetInstance()->init();
            extern bool g_visualizeWaves;
            g_visualizeWaves = !g_visualizeWaves;
        }
        else m_error = "Command unavailable";
        m_lastStatus = 0;
    }
    const unsigned int now = __IRR_DEVICE()->getTimer()->getRealTime();
    if (m_lastStatus && now - m_lastStatus < 100) return;
    m_lastStatus = now;
    const bool valid = ready && field->isValid() && field->IsLevelLoaded();
    std::ostringstream json;
    json << "{\"ready\":" << (ready ? "true" : "false")
         << ",\"valid\":" << (valid ? "true" : "false")
         << ",\"paused\":" << (m_paused || g_editor ? "true" : "false")
         << ",\"seq\":" << m_sequence << ",\"error\":\"" << m_error << "\""
         << ",\"level\":" << field->m_CurrentLevel << ",\"day\":" << field->m_CurrentDay
         << ",\"loadingProgress\":" << field->GetProgress()
         << ",\"applicationPaused\":" << (__APP()->isPaused() ? "true" : "false")
         << ",\"gateHealth\":" << (valid && field->getGate() ? field->getGate()->getHealthRatio() : 0)
         << ",\"time\":" << (valid ? field->getTime() : 0) << ",\"units\":[";
    if (valid)
    {
        std::set<int> living;
        const TUnitList& units = field->getUnits();
        for (TUnitList::ConstIterator it = units.begin(); it != units.end(); ++it) living.insert((*it)->m_id);
        const array<SUnitAtTime>& waves = field->getWaves();
        bool first = true;
        for (unsigned int i = 0; i < waves.size(); ++i)
        {
            if (!first) json << ',';
            first = false;
            const char* state = (int)i == field->getWavePos() ? "next" : (int)i > field->getWavePos() ? "future" : living.count(i << 8) ? "alive" : "dead";
            json << "{\"time\":" << waves[i].time << ",\"type\":" << waves[i].type << ",\"state\":\"" << state << "\"}";
        }
        for (unsigned int i = 0; i < field->m_extraSpawns.size(); ++i)
        {
            const UnitTypeAndId& unit = field->m_extraSpawns[i];
            if (!first) json << ',';
            first = false;
            json << "{\"time\":" << unit.time << ",\"type\":" << unit.unitType << ",\"extra\":true,\"state\":\"" << (living.count(unit.id) ? "alive" : "dead") << "\"}";
        }
    }
    json << "]}";
    const std::string temporary = m_directory + "/status.tmp", destination = m_directory + "/status.json";
    std::ofstream output(temporary.c_str(), std::ios::binary);
    output << json.str(); output.close();
#ifdef WIN32
    MoveFileExA(temporary.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING);
#else
    std::rename(temporary.c_str(), destination.c_str());
#endif
}
