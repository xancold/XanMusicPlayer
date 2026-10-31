#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/interface.h>
#include <aml-psdk/game_sa/Events.h>
#include <aml-psdk/game_sa/base/Timer.h>
#include <aml-psdk/game_sa/entity/PlayerPed.h>
#include <aml-psdk/game_sa/entity/Vehicle.h>

#include <jni.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <strings.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <atomic>
#include <mutex>

#ifndef _SAUTILS_INTERFACE
#define _SAUTILS_INTERFACE

struct SCRIPT_COMMAND { uint16_t opCode; char params[16]; };
class CWidgetButton; class RpAtomic; class RwFrame; class CPhysical;
class CEntity; class CPed; class CObject; class CVehicle;

typedef void        (*OnSettingChangedFn)(int nOldValue, int nNewValue, void* pData);
typedef const char* (*OnSettingDrawedFn)(int nNewValue, void* pData);
typedef void        (*OnButtonPressedFn)(uintptr_t screen);
typedef void        (*OnPlayerProcessFn)(uintptr_t info);
typedef void*       (*LookingForTextureFn)(const char* name);
typedef void        (*SimpleFn)();
typedef void        (*SimpleDataFn)(void* data);

enum eLoadedGame { Unknown = 0, GTASA_2_00, GTAVC_1_09, GTA3_1_09, GTALCS_1_09, GTASA_2_10 };
enum eTypeOfSettings : unsigned char {
    SetType_Controller = 0, SetType_Game = 1, SetType_Display = 2,
    SetType_Audio = 3, SetType_Language = 4, SetType_Mods = 5, SETTINGS_COUNT
};
enum eWidgetState : unsigned char {
    WState_Touched = 0, WState_DoubleTapped, WState_Released, WState_HeldDown,
    WState_SwipedLeft, WState_SwipedRight, WState_SwipedUp, WState_SwipedDown,
    WIDGETSTATE_MAX
};
enum eRenderOfType : unsigned char {
    ROfType_Effects = 0, ROfType_Menu, ROfType_Hud, ROfType_Ped,
    ROfType_Vehicle, ROfType_Object, ROfType_RadarBlips, ROfType_MapBlips,
    RENDEROFTYPE_MAX
};
enum eSettingsTabButtonLoc : unsigned char {
    STB_MainMenu = 0, STB_Pause, STB_StartGame, STB_Settings, TABBUTTONLOC_MAX
};
enum ePoolType : unsigned char { POOLTYPE_PEDS, POOLTYPE_VEHICLES, POOLTYPE_OBJECTS, POOLTYPES_MAX };
enum eTexDBType : unsigned char { TEXDBTYPE_DXT, TEXDBTYPE_ETC, TEXDBTYPE_PVR, TEXDBTYPES_MAX };

class ISAUtils {
public:
    virtual uintptr_t IsFLALoaded() = 0;
    virtual int AddSettingsItem(eTypeOfSettings typeOf, const char* name, int initVal, int minVal, int maxVal, OnSettingChangedFn fn, bool isSlider, void* sw) = 0;
    virtual int ValueOfSettingsItem(int settingId) = 0;
    virtual int AddClickableItem(eTypeOfSettings typeOf, const char* name, int initVal, int minVal, int maxVal, const char** switchesArray, OnSettingChangedFn fn) = 0;
    virtual int AddSliderItem(eTypeOfSettings typeOf, const char* name, int initVal, int minVal, int maxVal, OnSettingChangedFn fn, OnSettingDrawedFn fnDraw) = 0;
    virtual void AddButton(eTypeOfSettings typeOf, const char* name, OnButtonPressedFn fn) = 0;
    virtual uintptr_t* AddTextureDB(const char* name, bool registerMe) = 0;
    virtual int* GetSettingValuePointer(int settingId) = 0;
    virtual void AddIMG(const char* imgName) = 0;
    virtual eTypeOfSettings AddSettingsTab(const char* name, const char* textureName = "menu_mainsettings") = 0;
    virtual unsigned int GetCurrentMs() = 0;
    virtual float GetCurrentFPS() = 0;
    virtual uintptr_t GetTextureDB(const char* texDbName) = 0;
    virtual void RegisterTextureDB(uintptr_t textureDbPtr) = 0;
    virtual void UnregisterTextureDB(uintptr_t textureDbPtr) = 0;
    virtual uintptr_t GetTexture(const char* texName) = 0;
    virtual void AddOnWidgetsCreateListener(SimpleFn fn) = 0;
    virtual void AddPlayerUpdateListener(OnPlayerProcessFn fn, bool post = true) = 0;
    virtual int FindFreeWidgetId() = 0;
    virtual CWidgetButton* CreateWidget(int widgetId, int x, int y, float scale, const char* textureName = NULL) = 0;
    virtual int GetWidgetIndex(CWidgetButton* widget) = 0;
    virtual void SetWidgetIcon(CWidgetButton* widget, uintptr_t texturePtr) = 0;
    virtual void SetWidgetIcon(CWidgetButton* widget, const char* textureName) = 0;
    virtual void SetWidgetIcon2(CWidgetButton* widget, uintptr_t texturePtr) = 0;
    virtual void SetWidgetIcon2(CWidgetButton* widget, const char* textureName) = 0;
    virtual void ToggleWidget(CWidgetButton* widget, bool enable = true) = 0;
    virtual bool IsWidgetEnabled(int widgetId) = 0;
    virtual void ClearWidgetTapHistory(int widgetId) = 0;
    virtual void ClearWidgetTapHistory(CWidgetButton* widget) = 0;
    virtual int GetWidgetState(CWidgetButton* widget, eWidgetState stateToGet) = 0;
    virtual int GetWidgetState(int widgetId, eWidgetState stateToGet, bool doDoubleTapEff = true, int frames = 1) = 0;
    virtual void GetWidgetPos(int widgetId, float* x = NULL, float* y = NULL, float* sx = NULL, float* sy = NULL) = 0;
    virtual void SetWidgetPos(int widgetId, float x, float y, float sx, float sy) = 0;
    virtual void* LoadRwTextureFromPNG(const char* filename) = 0;
    virtual void* LoadRwTextureFromBMP(const char* filename) = 0;
    virtual void AddOnRWInitListener(SimpleFn fn) = 0;
    virtual void AddTextureLookupListener(LookingForTextureFn fn) = 0;
    virtual int AddClickableItem(eTypeOfSettings typeOf, const char* name, int initVal, int minVal, int maxVal, const char** switchesArray, OnSettingChangedFn fn, void* data) = 0;
    virtual int AddSliderItem(eTypeOfSettings typeOf, const char* name, int initVal, int minVal, int maxVal, OnSettingChangedFn fn, OnSettingDrawedFn fnDraw, void* data) = 0;
    virtual int ScriptCommand(const SCRIPT_COMMAND* pScriptCommand, ...) = 0;
    virtual void AddOnRenderListener(eRenderOfType typeOf, SimpleDataFn fn) = 0;
    virtual void AddSettingsTabButton(const char* name, SimpleDataFn fn, eSettingsTabButtonLoc loc = STB_Settings, const char* textureName = "menu_mainsettings", void* data = NULL) = 0;
    virtual bool LoadDFF(const char* name, bool doPrelit = false, RpAtomic** atomic = NULL, RwFrame** frame = NULL) = 0;
    virtual eLoadedGame GetLoadedGame() = 0;
    virtual uintptr_t GetLoadedGameLibAddress() = 0;
    virtual uintptr_t* AddTextureDBOfType(const char* name, eTexDBType type, bool registerMe = false) = 0;
    virtual int GetPoolSize(ePoolType poolType) = 0;
    virtual int GetPoolMemSize(ePoolType poolType) = 0;
    virtual int GetPoolIndex(void* entityPtr) = 0;
    virtual void* GetPoolMember(ePoolType poolType, int index) = 0;
    virtual bool IsGameInitialised() = 0;
    virtual void SetPosition(CPhysical* ent, float x, float y, float z, bool resetRotation = false) = 0;
    virtual void SetAngle(CPhysical* ent, unsigned char axis, float angle) = 0;
    virtual void SetAngle(CPhysical* ent, float x = -999, float y = -999, float z = -999) = 0;
    virtual void LoadModelId(int modelId) = 0;
    virtual void LoadArea(float x, float y) = 0;
    virtual CPed* CreatePed(int pedType, int modelId, float x, float y, float z, int* ref = NULL) = 0;
    virtual CPed* CreatePed(int pedType, int modelId, CVehicle* vehicle, int seat = -1, int* ref = NULL) = 0;
    virtual CVehicle* CreateVehicle(int modelId, float x, float y, float z, int* ref = NULL) = 0;
    virtual CObject* CreateObject(int modelId, float x, float y, float z, int* ref = NULL) = 0;
    virtual void MarkEntityAsNotNeeded(CEntity* ent) = 0;
    virtual void MarkModelAsNotNeeded(int modelId) = 0;
    virtual void PutPedInVehicle(CPed* ped, CVehicle* vehicle, int seat = -1) = 0;
};
#endif

MYMOD(net.xancold.xanmusicplayer, XanMusicPlayer, 3.11, XanCold)

static const char* kAuthor      = "XanCold";
static const char* kWebsite     = "https://xancold.blogspot.com";
static const char* kLibertyCity = "https://libertycity.net/user/Bdbbdbsbsnbd/";

static char g_szLogPath[512];
static char g_szMusicDir[512];
static char g_szConfigDir[512];
static char g_szConfigPath[512];
static time_t g_nConfigMTime = 0;
static u32 g_nNextReloadMs = 0;
static u32 g_nNextDebugMs = 0;

static bool g_bEnabled = true;
static bool g_bPlayOnVehicleOnly = true;

static void WriteLog(const char* fmt, ...) {
    if (!g_szLogPath[0]) return;
    FILE* f = fopen(g_szLogPath, "a"); if (!f) return;
    fprintf(f, "[%u] ", CTimer::GetTimeMS());
    va_list a; va_start(a, fmt); vfprintf(f, fmt, a); va_end(a);
    fputc('\n', f); fclose(f);
}
#define LOGI(...) do { logger->Info(__VA_ARGS__); WriteLog(__VA_ARGS__); } while(0)
#define LOGW(...) do { logger->Info(__VA_ARGS__); WriteLog(__VA_ARGS__); } while(0)

static bool DirExists(const char* p) {
    if (!p || !p[0]) return false;
    DIR* d = opendir(p); if (!d) return false; closedir(d); return true;
}
static void EnsureSlash(char* p, size_t c) {
    size_t n = strlen(p); if (!n || n+1 >= c) return;
    if (p[n-1] != '/') { p[n] = '/'; p[n+1] = 0; }
}
static void CopyPath(char* d, size_t c, const char* s) {
    if (!s) s = ""; snprintf(d, c, "%s", s); EnsureSlash(d, c);
}
static bool ProbeGameRoot(char* o, size_t ol, const char* st, const char* ly, const char* pk) {
    if (!st || !st[0] || !ly || !pk || !pk[0]) return false;
    char r[512], f[512];
    snprintf(r, sizeof(r), "%s%s/%s/", st, ly, pk);
    snprintf(f, sizeof(f), "%sfiles/", r);
    if (DirExists(f) || DirExists(r)) { snprintf(o, ol, "%s", f); EnsureSlash(o, ol); return true; }
    return false;
}
static void AddPkg(const char** l, int& c, int m, const char* p) {
    if (!p || !p[0] || c >= m) return;
    for (int i = 0; i < c; ++i) if (!strcmp(l[i], p)) return;
    l[c++] = p;
}

static bool CountTracksInFolder(const char* dir, int* outCount) {
    if (!dir || !dir[0]) return false;
    DIR* d = opendir(dir);
    if (!d) return false;
    int n = 0;
    struct dirent* e;
    while ((e = readdir(d)) != nullptr) {
        if (e->d_name[0] == '.') continue;
        const char* ext = strrchr(e->d_name, '.');
        if (!ext) continue;
        if (strcasecmp(ext, ".mp3") != 0 && strcasecmp(ext, ".ogg") != 0 &&
            strcasecmp(ext, ".wav") != 0 && strcasecmp(ext, ".flac") != 0 &&
            strcasecmp(ext, ".m4a") != 0 && strcasecmp(ext, ".aac") != 0) continue;
        n++;
    }
    closedir(d);
    *outCount = n;
    return true;
}

static void ResolveGameFilesDir() {
    char g_szFilesDir[512] = {0};
    const char* af = aml->GetAndroidDataPath();
    if (af && af[0] && DirExists(af)) { CopyPath(g_szFilesDir, sizeof(g_szFilesDir), af); goto out; }
    {
        char sb[256];
        const char* s = aml->GetInternalPath();
        if (!s || !s[0]) s = "/sdcard/";
        CopyPath(sb, sizeof(sb), s);
        const char* ss[3]; int ns = 0;
        ss[ns++] = sb;
        if (strcmp(sb, "/sdcard/")) ss[ns++] = "/sdcard/";
        if (strcmp(sb, "/storage/emulated/0/")) ss[ns++] = "/storage/emulated/0/";
        const char* pk[4]; int np = 0;
        AddPkg(pk, np, 4, aml->GetCurrentGame());
        AddPkg(pk, np, 4, aml->GetRealCurrentGame());
        AddPkg(pk, np, 4, "com.rockstargames.gtasa");
        const char* ly[] = { "Android_unprotected/data", "Android/data" };
        for (int i = 0; i < ns; ++i)
            for (const char* l : ly)
                for (int j = 0; j < np; ++j)
                    if (ProbeGameRoot(g_szFilesDir, sizeof(g_szFilesDir), ss[i], l, pk[j])) goto out;
        if (af && af[0]) { CopyPath(g_szFilesDir, sizeof(g_szFilesDir), af); goto out; }
        snprintf(g_szFilesDir, sizeof(g_szFilesDir), "%sAndroid/data/com.rockstargames.gtasa/files/", sb);
    }
out:
    snprintf(g_szConfigDir, sizeof(g_szConfigDir), "%sXanCold/XanMusicPlayer", g_szFilesDir);
    mkdir(g_szConfigDir, 0777);
    snprintf(g_szConfigPath, sizeof(g_szConfigPath), "%s/config.ini", g_szConfigDir);
    snprintf(g_szLogPath, sizeof(g_szLogPath), "%s/log.txt", g_szConfigDir);

    char parent[512];
    snprintf(parent, sizeof(parent), "%s", g_szFilesDir);
    size_t plen = strlen(parent);
    if (plen >= 6 && strcmp(parent + plen - 6, "files/") == 0) {
        parent[plen - 6] = 0;
    }

    char candLegacy[512], candFiles[512];
    snprintf(candLegacy, sizeof(candLegacy), "%sXanMusicPlayer/", parent);
    snprintf(candFiles,  sizeof(candFiles),  "%sXanMusicPlayer/", g_szFilesDir);

    int tracksLegacy = 0, tracksFiles = 0;
    bool legacyExists = CountTracksInFolder(candLegacy, &tracksLegacy);
    bool filesExists  = CountTracksInFolder(candFiles,  &tracksFiles);

    if (legacyExists && tracksLegacy > 0) {
        snprintf(g_szMusicDir, sizeof(g_szMusicDir), "%s", candLegacy);
        LOGI("Music dir: %s (tracks=%d, source=legacy-parent)", g_szMusicDir, tracksLegacy);
        return;
    }

    if (filesExists && tracksFiles > 0) {
        snprintf(g_szMusicDir, sizeof(g_szMusicDir), "%s", candFiles);
        LOGI("Music dir: %s (tracks=%d, source=files)", g_szMusicDir, tracksFiles);
        return;
    }

    if (legacyExists) {
        snprintf(g_szMusicDir, sizeof(g_szMusicDir), "%s", candLegacy);
        LOGW("Music dir EMPTY (legacy-parent): %s - put tracks here", g_szMusicDir);
        return;
    }

    if (filesExists) {
        snprintf(g_szMusicDir, sizeof(g_szMusicDir), "%s", candFiles);
        LOGW("Music dir EMPTY (files): %s - put tracks here", g_szMusicDir);
        return;
    }

    snprintf(g_szMusicDir, sizeof(g_szMusicDir), "%s", candLegacy);
    mkdir(g_szMusicDir, 0777);
    LOGW("Music dir CREATED (legacy-parent): %s - put tracks here", g_szMusicDir);
}

static void Trim(char* s) {
    if (!s) return;
    char* p = s;
    while (*p && isspace((unsigned char)*p)) ++p;
    if (p != s) memmove(s, p, strlen(p) + 1);
    size_t n = strlen(s);
    while (n && isspace((unsigned char)s[n-1])) s[--n] = 0;
    char* c = strpbrk(s, ";#");
    if (c) { *c = 0; n = strlen(s); while (n && isspace((unsigned char)s[n-1])) s[--n] = 0; }
}

static void WriteDefaultConfig() {
    FILE* f = fopen(g_szConfigPath, "w"); if (!f) return;
    fprintf(f,
        "; XanMusicPlayer v3.11\n"
        "; Author: %s\n"
        "; Website: %s\n"
        "; LibertyCity: %s\n"
        ";\n"
        "; Enabled=1               -> mod active\n"
        "; PlayOnVehicleOnly=1     -> music only plays while in a vehicle\n"
        "; PlayOnVehicleOnly=0     -> music plays anytime (on foot also plays)\n"
        ";\n"
        "[XanMusicPlayer]\n"
        "Enabled=1\n"
        "PlayOnVehicleOnly=1\n",
        kAuthor, kWebsite, kLibertyCity);
    fclose(f);
}

static bool LoadConfig() {
    FILE* f = fopen(g_szConfigPath, "r"); if (!f) return false;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        Trim(line);
        if (!line[0] || line[0] == '[' || line[0] == ';' || line[0] == '#') continue;
        char* eq = strchr(line, '='); if (!eq) continue;
        *eq = 0;
        char* k = line; char* v = eq + 1;
        Trim(k); Trim(v);
        if      (!strcasecmp(k, "Enabled"))           g_bEnabled = (atoi(v) != 0);
        else if (!strcasecmp(k, "PlayOnVehicleOnly")) g_bPlayOnVehicleOnly = (atoi(v) != 0);
    }
    fclose(f); return true;
}

static void ReloadConfig(bool force) {
    if (!g_szConfigPath[0]) return;
    struct stat st;
    if (stat(g_szConfigPath, &st) != 0) { WriteDefaultConfig(); return; }
    if (!force && st.st_mtime == g_nConfigMTime) return;
    if (LoadConfig()) {
        g_nConfigMTime = st.st_mtime;
        LOGI("Config | Enabled=%d PlayOnVehicleOnly=%d", g_bEnabled?1:0, g_bPlayOnVehicleOnly?1:0);
    }
}

static void SanitizeName(const char* src, char* dst, size_t dstSize) {
    size_t j = 0; bool lastWasSpace = false;
    for (size_t i = 0; src[i] && j < dstSize - 1; ++i) {
        unsigned char c = (unsigned char)src[i];
        if (c >= 0x20 && c <= 0x7E) {
            if (c == ' ') { if (lastWasSpace) continue; lastWasSpace = true; }
            else lastWasSpace = false;
            dst[j++] = c;
        }
    }
    while (j > 0 && dst[j-1] == ' ') j--;
    dst[j] = '\0';
}
static void TruncateName(const char* src, char* dst, size_t dstSize, size_t maxLen) {
    size_t len = strlen(src);
    if (len <= maxLen) { snprintf(dst, dstSize, "%s", src); return; }
    const char* ext = strrchr(src, '.');
    size_t extLen = 0;
    if (ext && ext != src) { extLen = strlen(ext); if (extLen > 10) extLen = 0; }
    size_t prefixLen = (maxLen > extLen + 3) ? (maxLen - extLen - 3) : 3;
    if (extLen > 0) snprintf(dst, dstSize, "%.*s...%.*s", (int)prefixLen, src, (int)extLen, ext);
    else            snprintf(dst, dstSize, "%.*s...", (int)(maxLen > 3 ? maxLen - 3 : 3), src);
}
static void MakeDisplayName(const char* src, char* dst, size_t dstSize, size_t maxLen) {
    char sanitized[512];
    SanitizeName(src, sanitized, sizeof(sanitized));
    TruncateName(sanitized, dst, dstSize, maxLen);
}

#define MAX_TRACKS 256
#define MAX_NAME_LEN 256
#define DISPLAY_MAX_LEN 50

static JavaVM* gVM = nullptr;
static jobject gPlayer = nullptr;
static std::mutex g_playerMutex;

static char g_playlist[MAX_TRACKS][MAX_NAME_LEN];
static int  g_nTrackCount = 0;
static int  g_nCurrentIdx = -1;
static char g_szCurrentTrack[MAX_NAME_LEN];

static int  g_nMode = 0;

static std::atomic<bool>     g_bActive(false);
static std::atomic<uint32_t> g_nLastPlayMs(0);
static std::atomic<uint32_t> g_nCurrentTrackDurationMs(0);
static std::atomic<bool>     g_bPendingNext(false);
static std::atomic<bool>     g_bShutdown(false);

static thread_local bool t_bAttachedByUs = false;

static JNIEnv* AttachJni() {
    t_bAttachedByUs = false;
    if (!gVM) return nullptr;
    JNIEnv* env = nullptr;
    jint r = gVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (r == JNI_OK && env) return env;
    if (gVM->AttachCurrentThread(&env, nullptr) == JNI_OK) {
        t_bAttachedByUs = true;
        return env;
    }
    return nullptr;
}
static void DetachJni() {
    if (t_bAttachedByUs && gVM) { gVM->DetachCurrentThread(); t_bAttachedByUs = false; }
}
static void ReleasePlayerInternal(JNIEnv* env) {
    if (!gPlayer || !env) return;
    jclass cls = env->GetObjectClass(gPlayer);
    if (cls) {
        jmethodID stop    = env->GetMethodID(cls, "stop", "()V");
        jmethodID release = env->GetMethodID(cls, "release", "()V");
        if (stop)    { env->CallVoidMethod(gPlayer, stop);    if (env->ExceptionCheck()) env->ExceptionClear(); }
        if (release) { env->CallVoidMethod(gPlayer, release); if (env->ExceptionCheck()) env->ExceptionClear(); }
        env->DeleteLocalRef(cls);
    }
    env->DeleteGlobalRef(gPlayer);
    gPlayer = nullptr;
}

static CVehicle* FindPlayerVehicleRobust() {
    CVehicle* v = FindPlayerVehicle(-1, false);
    if (v) return v;
    CPed* p = FindPlayerPed(-1);
    if (p && p->m_pVehicle) return (CVehicle*)p->m_pVehicle;
    return nullptr;
}
static bool IsPlayerInVehicle() {
    return FindPlayerVehicleRobust() != nullptr;
}

static void ScanMusicFolder() {
    g_nTrackCount = 0;
    if (!g_szMusicDir[0]) { LOGI("Scan: music dir empty"); return; }

    DIR* d = opendir(g_szMusicDir);
    if (!d) { LOGI("Scan: opendir failed for %s", g_szMusicDir); return; }

    struct dirent* entry;
    while ((entry = readdir(d)) != nullptr && g_nTrackCount < MAX_TRACKS) {
        if (entry->d_name[0] == '.') continue;
        const char* ext = strrchr(entry->d_name, '.');
        if (!ext) continue;
        if (strcasecmp(ext, ".mp3") != 0 && strcasecmp(ext, ".ogg") != 0 &&
            strcasecmp(ext, ".wav") != 0 && strcasecmp(ext, ".flac") != 0 &&
            strcasecmp(ext, ".m4a") != 0 && strcasecmp(ext, ".aac") != 0) continue;
        snprintf(g_playlist[g_nTrackCount], MAX_NAME_LEN, "%s", entry->d_name);
        g_nTrackCount++;
    }
    closedir(d);

    for (int i = 0; i < g_nTrackCount - 1; i++)
        for (int j = i + 1; j < g_nTrackCount; j++)
            if (strcasecmp(g_playlist[i], g_playlist[j]) > 0) {
                char tmp[MAX_NAME_LEN];
                memcpy(tmp, g_playlist[i], MAX_NAME_LEN);
                memcpy(g_playlist[i], g_playlist[j], MAX_NAME_LEN);
                memcpy(g_playlist[j], tmp, MAX_NAME_LEN);
            }

    LOGI("Scan: %d tracks from %s", g_nTrackCount, g_szMusicDir);
    if (g_nTrackCount == 0) {
        LOGW("Scan: 0 tracks - put .mp3/.ogg files in: %s", g_szMusicDir);
    }
}

static void ConfigureAudioRouting(JNIEnv* env, jobject p, jclass cls) {
    jmethodID setStreamType = env->GetMethodID(cls, "setAudioStreamType", "(I)V");
    if (setStreamType) {
        env->CallVoidMethod(p, setStreamType, 3);
        if (env->ExceptionCheck()) { env->ExceptionClear(); LOGI("setAudioStreamType: EXCEPTION"); }
        else LOGI("setAudioStreamType: OK (STREAM_MUSIC)");
    }
    jclass attrBuilderCls = env->FindClass("android/media/AudioAttributes$Builder");
    if (attrBuilderCls) {
        jmethodID builderCtor = env->GetMethodID(attrBuilderCls, "<init>", "()V");
        jobject builder = builderCtor ? env->NewObject(attrBuilderCls, builderCtor) : nullptr;
        if (builder) {
            jmethodID setUsage = env->GetMethodID(attrBuilderCls, "setUsage", "(I)Landroid/media/AudioAttributes$Builder;");
            if (setUsage) { env->CallObjectMethod(builder, setUsage, 1); if (env->ExceptionCheck()) env->ExceptionClear(); }
            jmethodID setContentType = env->GetMethodID(attrBuilderCls, "setContentType", "(I)Landroid/media/AudioAttributes$Builder;");
            if (setContentType) { env->CallObjectMethod(builder, setContentType, 2); if (env->ExceptionCheck()) env->ExceptionClear(); }
            jmethodID build = env->GetMethodID(attrBuilderCls, "build", "()Landroid/media/AudioAttributes;");
            if (build) {
                jobject attrs = env->CallObjectMethod(builder, build);
                if (attrs && !env->ExceptionCheck()) {
                    jmethodID setAttrs = env->GetMethodID(cls, "setAudioAttributes", "(Landroid/media/AudioAttributes;)V");
                    if (setAttrs) {
                        env->CallVoidMethod(p, setAttrs, attrs);
                        if (env->ExceptionCheck()) { env->ExceptionClear(); LOGI("setAudioAttributes: EXCEPTION"); }
                        else LOGI("setAudioAttributes: OK");
                    }
                    env->DeleteLocalRef(attrs);
                } else if (env->ExceptionCheck()) env->ExceptionClear();
            }
            env->DeleteLocalRef(builder);
        }
        env->DeleteLocalRef(attrBuilderCls);
    }
}

static bool PlayIndex(int idx) {
    if (idx < 0 || idx >= g_nTrackCount) {
        LOGI("PlayIndex: idx=%d out of range (count=%d)", idx, g_nTrackCount);
        return false;
    }
    if (!g_bEnabled) return false;

    if (g_bPlayOnVehicleOnly && !IsPlayerInVehicle()) {
        g_nCurrentIdx = idx;
        snprintf(g_szCurrentTrack, MAX_NAME_LEN, "%s", g_playlist[idx]);
        LOGI("Buffered (not in vehicle) idx=%d", idx);
        return false;
    }

    if (!gVM) return false;

    std::lock_guard<std::mutex> lock(g_playerMutex);
    JNIEnv* env = AttachJni();
    if (!env) return false;

    ReleasePlayerInternal(env);

    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s%s", g_szMusicDir, g_playlist[idx]);

    jclass cls = env->FindClass("android/media/MediaPlayer");
    if (!cls) { DetachJni(); return false; }

    jmethodID ctor    = env->GetMethodID(cls, "<init>", "()V");
    jmethodID setDS   = env->GetMethodID(cls, "setDataSource", "(Ljava/lang/String;)V");
    jmethodID prepare = env->GetMethodID(cls, "prepare", "()V");
    jmethodID start   = env->GetMethodID(cls, "start", "()V");
    jmethodID getDur  = env->GetMethodID(cls, "getDuration", "()I");

    if (!ctor || !setDS || !prepare || !start) {
        env->DeleteLocalRef(cls); DetachJni(); return false;
    }
    jobject p = env->NewObject(cls, ctor);
    if (!p) { env->DeleteLocalRef(cls); DetachJni(); return false; }

    jstring jpath = env->NewStringUTF(fullPath);
    env->CallVoidMethod(p, setDS, jpath);
    env->DeleteLocalRef(jpath);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        env->DeleteLocalRef(p); env->DeleteLocalRef(cls); DetachJni();
        LOGI("setDataSource failed: %s", fullPath);
        return false;
    }

    ConfigureAudioRouting(env, p, cls);

    env->CallVoidMethod(p, prepare);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        env->DeleteLocalRef(p); env->DeleteLocalRef(cls); DetachJni();
        LOGI("prepare failed: %s", fullPath);
        return false;
    }

    int durMs = 0;
    if (getDur) {
        durMs = env->CallIntMethod(p, getDur);
        if (env->ExceptionCheck()) { env->ExceptionClear(); durMs = 0; }
    }
    g_nCurrentTrackDurationMs.store(durMs > 0 ? (uint32_t)durMs : 0);

    env->CallVoidMethod(p, start);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        env->DeleteLocalRef(p); env->DeleteLocalRef(cls); DetachJni();
        return false;
    }

    gPlayer = env->NewGlobalRef(p);
    env->DeleteLocalRef(p); env->DeleteLocalRef(cls); DetachJni();

    g_nCurrentIdx = idx;
    snprintf(g_szCurrentTrack, MAX_NAME_LEN, "%s", g_playlist[idx]);
    g_nLastPlayMs.store(CTimer::GetTimeMS());
    g_bActive.store(true);
    g_bPendingNext.store(false);

    char display[DISPLAY_MAX_LEN + 16];
    MakeDisplayName(g_szCurrentTrack, display, sizeof(display), DISPLAY_MAX_LEN);
    LOGI("Now Playing [%d/%d]: %s (dur=%ums)", idx + 1, g_nTrackCount, display, durMs);
    return true;
}

static void PlayNext() {
    if (g_nTrackCount == 0) return;
    PlayIndex((g_nCurrentIdx + 1) % g_nTrackCount);
}
static void PlayPrev() {
    if (g_nTrackCount == 0) return;
    PlayIndex((g_nCurrentIdx - 1 + g_nTrackCount) % g_nTrackCount);
}
static void PlayRandomNext() {
    if (g_nTrackCount == 0) return;
    if (g_nTrackCount == 1) { PlayIndex(0); return; }
    int idx;
    do { idx = rand() % g_nTrackCount; } while (idx == g_nCurrentIdx);
    PlayIndex(idx);
}

static bool IsTrackFinished() {
    if (!g_bActive.load()) return false;
    uint32_t now = CTimer::GetTimeMS();
    uint32_t elapsed = now - g_nLastPlayMs.load();
    uint32_t dur = g_nCurrentTrackDurationMs.load();
    if (dur > 0 && elapsed > dur + 5000) {
        LOGI("Track finished (elapsed fallback)");
        return true;
    }
    std::lock_guard<std::mutex> lock(g_playerMutex);
    if (!gPlayer || !gVM) return false;
    JNIEnv* env = AttachJni();
    if (!env) return false;
    jclass cls = env->GetObjectClass(gPlayer);
    if (!cls) { DetachJni(); return false; }

    bool posFinished = false, playingNow = true;
    jmethodID mPos = env->GetMethodID(cls, "getCurrentPosition", "()I");
    jmethodID mDur = env->GetMethodID(cls, "getDuration", "()I");
    jmethodID mPlaying = env->GetMethodID(cls, "isPlaying", "()Z");

    if (mPos && mDur) {
        jint pos = env->CallIntMethod(gPlayer, mPos);
        if (env->ExceptionCheck()) { env->ExceptionClear(); pos = -1; }
        jint d = env->CallIntMethod(gPlayer, mDur);
        if (env->ExceptionCheck()) { env->ExceptionClear(); d = -1; }
        if (d > 0 && pos >= 0 && pos >= (d - 500)) posFinished = true;
        if (d > 0) g_nCurrentTrackDurationMs.store((uint32_t)d);
    }
    if (mPlaying) {
        jboolean playing = env->CallBooleanMethod(gPlayer, mPlaying);
        if (env->ExceptionCheck()) { env->ExceptionClear(); playingNow = false; }
        else playingNow = (playing == JNI_TRUE);
    }
    env->DeleteLocalRef(cls);
    DetachJni();
    return posFinished || !playingNow;
}

static void DispatchNextIfPending() {
    if (!g_bPendingNext.load()) return;
    g_bPendingNext.store(false);
    if (g_nMode == 1)      PlayIndex(g_nCurrentIdx);
    else if (g_nMode == 2) PlayRandomNext();
    else                   PlayNext();
}
static void StopPlayerAndDeactivate() {
    std::lock_guard<std::mutex> lock(g_playerMutex);
    JNIEnv* env = AttachJni();
    if (env) { ReleasePlayerInternal(env); DetachJni(); }
    g_bActive.store(false);
    g_bPendingNext.store(false);
    g_nLastPlayMs.store(0);
    LOGI("Music stopped");
}

static char g_szNowPlayingText[DISPLAY_MAX_LEN + 16];
static void OnModeChanged(int, int newVal, void*) { g_nMode = newVal; LOGI("Mode -> %d", newVal); }
static void OnDummy(int, int, void*) { }

static void OnEnabledChanged(int oldVal, int newVal, void*) {
    g_bEnabled = (newVal != 0);
    LOGI("Enabled -> %d", g_bEnabled ? 1 : 0);
    if (!g_bEnabled && g_bActive.load()) StopPlayerAndDeactivate();
}

static void OnPlayOnVehicleOnlyChanged(int oldVal, int newVal, void*) {
    g_bPlayOnVehicleOnly = (newVal != 0);
    LOGI("PlayOnVehicleOnly -> %d (inVeh=%d active=%d tracks=%d)",
         g_bPlayOnVehicleOnly ? 1 : 0, IsPlayerInVehicle()?1:0,
         g_bActive.load()?1:0, g_nTrackCount);

    if (g_bPlayOnVehicleOnly) {
        if (g_bActive.load() && !IsPlayerInVehicle()) {
            LOGI("Toggle ON + on foot -> stop");
            StopPlayerAndDeactivate();
        }
    } else {
        if (!g_bActive.load() && g_nTrackCount > 0) {
            int idx = (g_nCurrentIdx >= 0) ? g_nCurrentIdx : 0;
            LOGI("Toggle OFF -> start music idx=%d", idx);
            PlayIndex(idx);
        }
    }
}

static const char* NowPlayingText(int, void*) {
    if (g_nTrackCount == 0 || g_nCurrentIdx < 0) return "(no tracks)";
    MakeDisplayName(g_szCurrentTrack, g_szNowPlayingText, sizeof(g_szNowPlayingText), DISPLAY_MAX_LEN);
    return g_szNowPlayingText;
}
static void OnNextClicked(uintptr_t)  { PlayNext(); }
static void OnPrevClicked(uintptr_t)  { PlayPrev(); }
static void OnReloadClicked(uintptr_t) {
    ScanMusicFolder();
    if (g_nTrackCount > 0) PlayIndex(0);
}

ON_MOD_LOAD() {
    logger->SetTag("XanMusicPlayer");
    ResolveGameFilesDir();

    FILE* lf = fopen(g_szLogPath, "w");
    if (lf) {
        fprintf(lf, "=== XanMusicPlayer v3.11 ===\n");
        fprintf(lf, "Author: %s\n", kAuthor);
        fprintf(lf, "Website: %s\n", kWebsite);
        fprintf(lf, "LibertyCity: %s\n", kLibertyCity);
        fclose(lf);
    }

    ReloadConfig(true);

    LOGI("=== XanMusicPlayer v3.11 ===");
    LOGI("Author: %s", kAuthor);
    LOGI("Website: %s", kWebsite);
    LOGI("LibertyCity: %s", kLibertyCity);
    LOGI("Config: %s", g_szConfigPath);
    LOGI("Music dir: %s", g_szMusicDir);

    gVM = aml->GetJavaVM();
    LOGI("JavaVM = %p", gVM);
    srand((unsigned)time(nullptr));

    ScanMusicFolder();

    if (g_nTrackCount == 0) {
        LOGW("=== NO TRACKS FOUND ===");
        LOGW("Put .mp3/.ogg/.wav/.flac/.m4a/.aac files in:");
        LOGW("  %s", g_szMusicDir);
        LOGW("Then press 'Reload Folder' in menu");
    }

    ISAUtils* sautils = (ISAUtils*)GetInterface("SAUtils");
    LOGI("SAUtils = %p", sautils);
    if (sautils) {
        eTypeOfSettings tab = sautils->AddSettingsTab("XanMusicPlayer");
        if ((int)tab >= 0) {
            static const char* pOnOff[] = { "OFF", "ON" };
            static const char* pModes[] = { "Loop Directory", "Loop One", "Shuffle" };

            sautils->AddClickableItem(tab, "Enable Mod", g_bEnabled ? 1 : 0, 0, 1, pOnOff, OnEnabledChanged, nullptr);
            sautils->AddClickableItem(tab, "Play On Vehicle Only", g_bPlayOnVehicleOnly ? 1 : 0, 0, 1, pOnOff, OnPlayOnVehicleOnlyChanged, nullptr);
            sautils->AddClickableItem(tab, "Playback Mode", g_nMode, 0, 2, pModes, OnModeChanged, nullptr);
            sautils->AddSliderItem(tab, "Now Playing", 0, 0, 1, OnDummy, NowPlayingText, nullptr);
            sautils->AddButton(tab, "> Next Track", OnNextClicked);
            sautils->AddButton(tab, "< Prev Track", OnPrevClicked);
            sautils->AddButton(tab, "Reload Folder", OnReloadClicked);
            LOGI("Menu setup complete");
        }
    }

    Events::gameProcessEvent += []() {
        u32 now = CTimer::GetTimeMS();
        if (now >= g_nNextReloadMs) {
            g_nNextReloadMs = now + 3000;
            ReloadConfig(false);
        }

        if (now >= g_nNextDebugMs) {
            g_nNextDebugMs = now + 3000;
            LOGI("State | inVeh=%d active=%d enabled=%d onVehOnly=%d tracks=%d",
                 IsPlayerInVehicle()?1:0, g_bActive.load()?1:0,
                 g_bEnabled?1:0, g_bPlayOnVehicleOnly?1:0, g_nTrackCount);
        }

        if (!g_bEnabled) {
            if (g_bActive.load()) StopPlayerAndDeactivate();
            return;
        }

        bool shouldPlay = (!g_bPlayOnVehicleOnly) || IsPlayerInVehicle();

        if (shouldPlay) {
            if (!g_bActive.load() && g_nTrackCount > 0) {
                int idx = (g_nCurrentIdx >= 0) ? g_nCurrentIdx : 0;
                LOGI("Start music idx=%d (onVehOnly=%d)", idx, g_bPlayOnVehicleOnly?1:0);
                PlayIndex(idx);
            } else {
                DispatchNextIfPending();
                if (g_bActive.load()) {
                    uint32_t t = CTimer::GetTimeMS();
                    if (t - g_nLastPlayMs.load() >= 2000 && IsTrackFinished()) {
                        LOGI("Track ended");
                        g_bPendingNext.store(true);
                    }
                }
            }
        } else {
            if (g_bActive.load()) StopPlayerAndDeactivate();
        }
    };

    std::thread([]() {
        while (!g_bShutdown.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (!g_bEnabled || !g_bActive.load() || g_bPendingNext.load()) continue;
            if (g_bPlayOnVehicleOnly && !IsPlayerInVehicle()) continue;
            uint32_t now = CTimer::GetTimeMS();
            if (now - g_nLastPlayMs.load() < 2000) continue;
            if (!IsTrackFinished()) continue;
            LOGI("Track ended (bg poll)");
            g_bPendingNext.store(true);
        }
    }).detach();

    LOGI("=== Load complete ===");
}