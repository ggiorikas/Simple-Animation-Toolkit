#include "raylib.h"
#include <functional>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <cstdio>
#include <cmath>

template <class T>
T quantize(T x, T step) {
    return int(x / step) * step;
}

template <class T>
struct Animation {

    T alpha;
    T duration;
    T triggerAt;
    bool triggered, active;

    std::vector<std::function<void(void)>> onStartCallbacks, onEndCallbacks;

    void init() {
        duration = 1;
        reset();
    }

    void clearCallbacks() {
        onStartCallbacks.clear();
        onEndCallbacks.clear();
    }

    void reset() {
        alpha = 0;
        active = false;
        triggerAt = -1;
        triggered = false;
    }

    void start() {
        reset();
        active = true;
        for (auto & f : onStartCallbacks) { f(); }
    }

    void update(T dt, T t) {
        if (triggerAt >= 0 && !triggered && t >= triggerAt) {
            start();
            triggered = true;
        }

        if (!active) { return; }

        alpha += dt / duration;

        if (alpha > 1) {
            active = false;
            alpha = 1;
            for (auto & f : onEndCallbacks) { f(); }
        }
    }

    T elapsedTime() { return alpha * duration; }

    void onStart(std::function<void(void)> f) {
        onStartCallbacks.push_back(f);
    }

    void onEnd(std::function<void(void)> f) {
        onEndCallbacks.push_back(f);
    }

};

typedef Animation<float> AnimationF;
typedef Animation<double> AnimationD;

struct Shaker {

    AnimationF ofsCycle;
    AnimationF fullShake;

    float xmag = 15;
    float ymag = 15;

    float xofs = 0;
    float yofs = 0;

    void init() {
        fullShake.duration = 0.5;
        ofsCycle.duration = 0.025;

        ofsCycle.onStart([&](){
            float a = pow(1 - fullShake.alpha, 2.5);
            xofs = (rand() * 1.0 / RAND_MAX * 2 * xmag - xmag) * a;
            yofs = (rand() * 1.0 / RAND_MAX * 2 * ymag - ymag) * a;
        });

        ofsCycle.onEnd([&](){
            xofs = 0;
            yofs = 0;

            if (fullShake.active) {
                ofsCycle.reset();
                ofsCycle.start();
            }
        });
    }

    void shake() {
        fullShake.reset();
        ofsCycle.reset();

        fullShake.start();
        ofsCycle.start();
    }

    void update(float dt, float t) {
        fullShake.update(dt, t);
        ofsCycle.update(dt, t);
    }

};

struct Triangle {
    Vector2 p1;
    Vector2 p2;
    Vector2 p3;
};

struct Textbox {

    float x, y, w, h;

    Color color, textColorFront, textColorBack;

    Font font;
    Sound meow;

    bool visible, textOn;
    std::string text;

    Shaker shaker;

    std::vector<Triangle> decoration;

    void init() {
        x = 140;
        y = 80;
        w = 200;
        h = 120;

        color = ORANGE;
        textColorFront = GREEN;
        textColorBack = DARKGREEN;
        visible = false;
        textOn = false;

        shaker.init();
    }

    void displayText(std::string str) {
        text = str;
        visible = true;
        textOn = true;

        shaker.shake();
        PlaySound(meow);
    }

    void update(float dt, float t) {
        shaker.update(dt, t);
    }

    void render(float dt) {
        if (!visible) { return; }

        float xofs = quantize(shaker.xofs, 2.f);
        float yofs = quantize(shaker.yofs, 2.f);

        DrawRectangle(x + xofs, y + yofs, w, h, color);

        for (auto & tri : decoration) {
            auto v1 = Vector2{tri.p1.x + xofs, tri.p1.y + yofs};
            auto v2 = Vector2{tri.p2.x + xofs, tri.p2.y + yofs};
            auto v3 = Vector2{tri.p3.x + xofs, tri.p3.y + yofs};
            DrawTriangle(v1, v2, v3, color);
        }

        if (textOn) {
            float xofst = xofs + 10;
            float yofst = yofs + 10;
            
            for (int i = -1; i <= 1; ++ i) {
                for (int j = -1; j <= 1; ++ j) {
                    DrawTextEx(font, text.c_str(), Vector2{x + i * 1 + xofst, y + j * 1 + yofst}, 18, 3, textColorBack);            
                }
            }

            DrawTextEx(font, text.c_str(), Vector2{x + xofst, y + yofst}, 18, 3, textColorFront);
        }
    }
};

struct Kittens {

    int curFrame, preFrame;

    std::vector<Texture2D> frames;
    std::vector<char> frameLoaded;

    AnimationF leftLook, rightLook;

    int dir;

    void init() {
        curFrame = 0;
        preFrame = -1;

        frames.resize(100);
        frameLoaded.resize(100);

        leftLook.init();
        rightLook.init();

        leftLook.duration = rightLook.duration = 8;

        dir = -1;
    }

    void loadFrame(int fid) {
        if (!frameLoaded[fid]) {
            char fname[100];
            sprintf(fname, "assets/kittens/frames/kittens_%d.png", fid);
            frames[fid] = LoadTexture(fname);
            SetTextureFilter(frames[fid], TEXTURE_FILTER_BILINEAR);
            frameLoaded[fid] = true;
        }
    }

    void unloadFrame(int fid) {
        if (frameLoaded[fid]) {
            UnloadTexture(frames[fid]);
            frameLoaded[fid] = false;
        }
    }

    void update(float dt, float t) {
        leftLook.update(dt, t);
        rightLook.update(dt, t);

        if (leftLook.active) {
            curFrame = 12 + leftLook.alpha * 62;
        } else if (rightLook.active) {
            curFrame = 12 + (1 - rightLook.alpha) * 62;
        }

        loadFrame(curFrame);

        if (curFrame != preFrame && preFrame >= 0) {
            unloadFrame(preFrame);
        }

        preFrame = curFrame;
    }

    void render(float dt, float sw, float sh) {
        auto & t = frames[curFrame];

        DrawTexturePro(
            frames[curFrame], 
            Rectangle{0, 0, (float)t.width, (float)t.height}, 
            Rectangle{0, 0, sw, sh}, 
            Vector2{0, 0}, 0, WHITE
        );
    }
};

struct Mouse {

    float x, y, xmin, xmax, scale;

    std::vector<Texture2D> frames;
    int curFrame, dir;
    
    std::vector<int> frameSeqLeft[2], frameSeqRight[2];
    int seqTurn;

    AnimationF leftWalk, rightWalk, jumpCycle;

    void init(int sw, int sh) {
        for (int i = 1; i <= 10; ++ i) {
            char fname[100];
            sprintf(fname, "assets/mouse/frames/mouse_%d.png", i);
            frames.push_back(LoadTexture(fname));
            SetTextureFilter(frames.back(), TEXTURE_FILTER_BILINEAR);
        }

        scale = 0.35;

        xmin = - frames[0].width * scale - 15;
        x = xmax = sw + 15;
        y = sh - frames[0].height * scale - 5;

        curFrame = 0;

        dir = -1;
        seqTurn = 0;
        frameSeqLeft[0] = {0, 1, 2};
        frameSeqLeft[1] = {0, 3, 4};
        frameSeqRight[0] = {5, 6, 7};
        frameSeqRight[1] = {5, 8, 9};

        leftWalk.init();
        rightWalk.init();
        jumpCycle.init();

        leftWalk.duration = rightWalk.duration = 8;
        jumpCycle.duration = 0.5;
    }

    void update(float dt, float t) {
        leftWalk.update(dt, t);
        rightWalk.update(dt, t);
        jumpCycle.update(dt, t);

        if (leftWalk.active) {
            dir = -1;
        } else if (rightWalk.active) {
            dir = 1;
        } else {
            return;
        }

        auto & frameSeq = dir < 0 ? frameSeqLeft[seqTurn] : frameSeqRight[seqTurn];

        float a = sin(jumpCycle.alpha * 3.14159);
        curFrame = frameSeq[int(a * frameSeq.size()) % frameSeq.size()];

        if (dir < 0) {
            x = xmax * (1 - leftWalk.alpha) + xmin * leftWalk.alpha; 
        } else {
            x = xmin * (1 - rightWalk.alpha) + xmax * rightWalk.alpha; 
        }
    }

    void render(float dt) {
        float a = sin(jumpCycle.alpha * 3.14159);
        
        DrawTextureEx(
            frames[curFrame], 
            Vector2{quantize(x, 3.f), quantize(y - a * 9, 3.f)}, 
            0, scale, WHITE
        );
    }
};


struct MusicPlayer {

    Music music;
    AnimationF fading;

    void init() {
        fading.init();
        fading.duration = 1;
        fading.onEnd([&]() { stop(); });
    }

    void reset() {
        StopMusicStream(music);
        SetMusicVolume(music, 1);
        fading.reset();
    }

    void start() {
        reset();
        PlayMusicStream(music);
    }

    bool playing() { return IsMusicStreamPlaying(music); }

    void fade() { fading.start(); }

    void stop() { StopMusicStream(music); }

    void update(float dt, float t) {
        UpdateMusicStream(music);

        fading.update(dt, t);

        if (fading.active) {
            SetMusicVolume(music, 1 - fading.alpha);
        }
    }
};

int main() {
    SetTraceLogLevel(LOG_WARNING);

    const int screenWidth = 1024;
    const int screenHeight = 576;

    InitWindow(screenWidth, screenHeight, "Lazy Kittens VS Brave Mouse");
    InitAudioDevice();

    MusicPlayer mplayer;
    mplayer.init();
    mplayer.music = LoadMusicStream("assets/sneaky-adventure.mp3");    

    Font myFont = LoadFont("assets/font/press-start.ttf");
    
    Textbox txt[3];
    for (int i = 0; i < 3; ++ i) {
        txt[i].init();
        txt[i].font = myFont;
        char fname[100];
        sprintf(fname, "assets/kittens/meow%d.wav", i + 1);
        txt[i].meow = LoadSound(fname);
    }

    txt[0].x -= 25;
    txt[0].w += 25;
    [&](Textbox & t) {
        t.decoration.push_back({
            Vector2{t.x + t.w, t.y + 2*t.h/3}, 
            Vector2{t.x + t.w, t.y + t.h - t.h/8}, 
            Vector2{365, 175}
        });
    }(txt[0]);

    txt[1].color = YELLOW;
    txt[1].x = 760 - 35;
    txt[1].w += 35;
    txt[1].y = 310;
    [&](Textbox & t) {
        t.decoration.push_back({
            Vector2{t.x + t.w/8, t.y}, 
            Vector2{t.x + t.w/4, t.y}, 
            Vector2{775, 285}
        });
    }(txt[1]);

    txt[2].color = GOLD;
    txt[2].x = 30;
    txt[2].y = 270;
    [&](Textbox & t) {
        t.decoration.push_back({
            Vector2{t.x + t.w, t.y + 2*t.h/3}, 
            Vector2{t.x + t.w, t.y + t.h - t.h/8}, 
            Vector2{255, 365}
        });
    }(txt[2]);

    int curMsgId = 0;
    std::vector<std::pair<int, std::string>> allMessages = {
        {0, "WHAT  IS\nTHIS GUY\nDOING?"},
        {1, "LOOKS LIKE\nHE'S AFTER\nTHE CHEESE"},
        {2, "*YAAWWWN*"},
        {0, "SHOULD WE\nSTOP HIM?\n"},
        {1, "NAH, LET'S\nWAIT FOR\nMOM"},
        {2, "I'LL GO\nBACK TO\nSLEEP"},
    };

    Mouse mouse;
    mouse.init(screenWidth, screenHeight);
    
    Kittens kittens;
    kittens.init();

    kittens. leftLook.onStart([&]() { mouse. leftWalk.start(); });
    kittens.rightLook.onStart([&]() { mouse.rightWalk.start(); });

    kittens. leftLook.onEnd([&]() { kittens.rightLook.start(); });
    kittens.rightLook.onEnd([&]() { kittens. leftLook.start(); });

    mouse.jumpCycle.onEnd([&]() { 
        mouse.seqTurn = 1 - mouse.seqTurn; 
        mouse.jumpCycle.start(); 
    });

    mouse.jumpCycle.triggerAt = 0.55;
    kittens.leftLook.start();

    Vector2 mousePoint;

    float curTime = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (dt > 0.1) {
            printf("dt check hit\n");
            dt = 0.1;
        }

        curTime += dt;

        if (IsKeyPressed(KEY_SPACE)) {
            mousePoint = GetMousePosition();
            printf("curTime: %f\n", curTime);
            printf("mousePoint: %d %d\n\n", (int)mousePoint.x, (int)mousePoint.y);
            
            for (int i = 0; i < 3; ++ i) {
                txt[i].visible = false;
            }
            
            if (curMsgId < allMessages.size()) {
                auto & msg = allMessages[curMsgId++];
                txt[msg.first].displayText(msg.second);
            } else {
                curMsgId = 0;
            }
        }

        if (IsKeyPressed(KEY_M)) {
            if (mplayer.playing()) {
                mplayer.fade();
            } else {
                mplayer.start();
            }
        }

        kittens.update(dt, curTime);
        for (int i = 0; i < 3; ++ i) {
            txt[i].update(dt, curTime);
        }
        mouse.update(dt, curTime);
        mplayer.update(dt, curTime);

        BeginDrawing();
            ClearBackground(BLACK);
            kittens.render(dt, screenWidth, screenHeight);
            for (int i = 0; i < 3; ++ i) {
                txt[i].render(dt);
            }
            mouse.render(dt);
        EndDrawing();

        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1ms);
        }
    }

    CloseWindow();    
}