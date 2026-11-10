#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <windows.h>
#include <cmath>

using namespace geode::prelude;

static bool g_held = false;
static int g_threshold = 0;

static void setMute(bool down) {
    if (g_held == down) return;
    g_held = down;

    auto vk = static_cast<WORD>(Mod::get()->getSettingValue<int64_t>("vk"));
    INPUT in{};
    in.type = INPUT_KEYBOARD;
    in.ki.wVk = vk;
    in.ki.wScan = static_cast<WORD>(MapVirtualKey(vk, MAPVK_VK_TO_VSC));
    in.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
    SendInput(1, &in, sizeof(INPUT));
}

static std::string levelKey(GJGameLevel* level) {
    int id = level->m_levelID.value();
    if (id != 0) return fmt::format("lvl_{}", id);
    return fmt::format("local_{}", std::string(level->m_levelName));
}

static int thresholdFor(GJGameLevel* level) {
    int v = Mod::get()->getSavedValue<int>(levelKey(level), -1);
    if (v >= 0) return v;
    return static_cast<int>(Mod::get()->getSettingValue<int64_t>("percent"));
}

static CCMenuItemSpriteExtra* makeMuteBtn(CCObject* target, SEL_MenuHandler handler) {
    CCSprite* spr = nullptr;
    if (CCTextureCache::get()->addImage("discord.png"_spr, false)) {
        spr = CircleButtonSprite::createWithSprite(
            "discord.png"_spr, 1.0f, CircleBaseColor::Blue, CircleBaseSize::Medium
        );
    }
    if (!spr) spr = ButtonSprite::create("Auto-mute");
    spr->setScale(0.8f);
    return CCMenuItemSpriteExtra::create(spr, target, handler);
}

class MutePopup : public geode::Popup {
protected:
    std::string m_key;
    CCLabelBMFont* m_label = nullptr;
    Slider* m_slider = nullptr;

    bool init(GJGameLevel* level) {
        if (!Popup::init(300.f, 140.f)) return false;

        this->setTitle("Discord Auto Mute");
        m_key = levelKey(level);
        int cur = thresholdFor(level);

        m_label = CCLabelBMFont::create("", "bigFont.fnt");
        m_label->setScale(0.8f);
        m_mainLayer->addChildAtPosition(m_label, Anchor::Center, ccp(0, 15));

        m_slider = Slider::create(this, menu_selector(MutePopup::onSlide), 1.f);
        m_slider->setValue(cur / 100.f);
        m_slider->updateBar();
        m_mainLayer->addChildAtPosition(m_slider, Anchor::Center, ccp(0, -15));

        this->updateLabel(cur);
        return true;
    }

    void updateLabel(int pct) {
        m_label->setString(pct == 0 ? "Off" : fmt::format("{}%", pct).c_str());
    }

    void onSlide(CCObject* sender) {
        float v = static_cast<SliderThumb*>(sender)->getValue();
        int pct = static_cast<int>(std::round(v * 100.f));
        this->updateLabel(pct);
        Mod::get()->setSavedValue<int>(m_key, pct);
        if (PlayLayer::get()) g_threshold = pct;
    }

public:
    static MutePopup* create(GJGameLevel* level) {
        auto ret = new MutePopup();
        if (ret->init(level)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

class $modify(MuteLayer, PlayLayer) {
    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        g_threshold = thresholdFor(m_level);
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        bool dead = m_player1 && m_player1->m_isDead;
        setMute(g_threshold > 0 && !dead && this->getCurrentPercentInt() >= g_threshold);
    }

    void resetLevel() {
        setMute(false);
        PlayLayer::resetLevel();
    }

    void levelComplete() {
        setMute(false);
        PlayLayer::levelComplete();
    }

    void onQuit() {
        setMute(false);
        PlayLayer::onQuit();
    }
};

class $modify(MutePause, PauseLayer) {
    void customSetup() {
        setMute(false);
        PauseLayer::customSetup();
        auto btn = makeMuteBtn(this, menu_selector(MutePause::onMuteBtn));
        btn->setID("auto-mute-btn"_spr);
        auto menu = CCMenu::create();
        menu->setID("auto-mute-menu"_spr);
        menu->addChild(btn);
        auto win = CCDirector::get()->getWinSize();
        menu->setPosition({win.width - 33.f, win.height - 70.f});
        this->addChild(menu);
    }

    void onMuteBtn(CCObject*) {
        if (auto pl = PlayLayer::get()) MutePopup::create(pl->m_level)->show();
    }
};

class $modify(MuteInfo, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        auto btn = makeMuteBtn(this, menu_selector(MuteInfo::onMuteBtn));
        btn->setID("auto-mute-btn"_spr);
        auto menu = CCMenu::create();
        menu->setID("auto-mute-menu"_spr);
        menu->addChild(btn);
        menu->setPosition({68.f, 112.f});
        this->addChild(menu);
        return true;
    }

    void onMuteBtn(CCObject*) {
        MutePopup::create(m_level)->show();
    }
};
