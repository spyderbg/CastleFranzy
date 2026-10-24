# XPlayer

Gameloft's **client-side online services SDK** (from the "World Online" team).

- **Version:** 0.0.6
- **Released:** 09/07/2009
- **Author:** online-adm@gameloft.com
- **Language:** C++
- **Platforms:** Win32, iPhone, Mac

XPlayer is a C++ library that abstracts all of the game's online and multiplayer
features behind a set of high-level modules. It lets the game use accounts,
matchmaking, multiplayer, chat, leaderboards, and social features without
implementing raw sockets/HTTP itself. This folder also ships as a demo of the
XPlayer APIs.

## Directory layout

| Path       | Description                                                        |
| ---------- | ------------------------------------------------------------------ |
| `include/` | Public API headers, organized by feature module                    |
| `src/`     | Demo/sample source (Game, Gles, Online, StressTest, System, UnitTest) |
| `lib/`     | Prebuilt libraries per platform (`win32/`, `IPhone/`, `mac/`, `build/`) |
| `doc/`     | Documentation (Doxygen config, tutorials, specs, changelog)        |
| `config/`  | Configuration files                                                |
| `platform/`| Platform-specific code                                             |
| `tools/`   | Utility tools                                                      |
| `res.zip`  | Bundled resources                                                  |

## Feature modules

Each module is exposed through a `GLXPlayer*.h` header under `include/`:

- **Login** (`Login/GLXPlayerLogin.h`) — register, login/logout, profile, password/email/country management
- **Lobby** (`Lobby/GLXPlayerLobby.h`) — create/join/leave lobbies, matchmaking, game parameters, kick, start game
- **Multiplayer** (`Multiplayer/`) — server-relay, P2P (UDP/TCP), and local multiplayer
- **Chat** (`Chat/GLXPlayerChat.h` + `Chat/gloox/`) — XMPP/Jabber-based chat rooms and roster
- **Leaderboard** (`Leaderboard/GLXPlayerLeaderboard.h`) — score upload and rankings
- **Buddy** (`Buddy/`) — friend list management
- **Message** (`Message/`) — in-game and user messages
- **User** (`User/`) — user profile, avatars, reputation, store data
- **Download** (`Download/`) — file download functionality
- **YouTube** (`YouTube/`) — video upload/integration
- **MMO** — map in/out, move, online/offline opcodes

## Configuration

Feature flags are defined in `include/XPlayer_defines.h` (e.g. `ENABLE_ONLINE`,
`ENABLE_MULTIPLAYER`, `MP_ENABLE_P2P`, `ENABLE_DOWNLOAD`, `ENABLE_M7_SUPPORT`).
Remove a definition to disable the corresponding online feature.

## Documentation

Detailed documentation lives in `doc/`:

- `Changelog.txt` — version history
- `XPlayer Tutorial.odt` — usage tutorial
- `iPhone Online Function Specification.odt` — iPhone online spec
- `youtube ext Xplayer doc.odt` — YouTube extension docs
- `doxygen_config` / `doxygen_java_config` — Doxygen generation configs
