#define  _CRT_SECURE_NO_WARNINGS  1  
#include<iostream>
#include"tools.hpp"
#include <easyx.h>
#include <graphics.h>
#include<windows.h>
#include<vector>
#include<time.h>
#include<string>
#include<tchar.h>
#include<cstdio> 
#include <mmsystem.h>

#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib,"winmm.lib")
using namespace std;
//音量增加内容
//添加播放音量所需的头文件

//音量初始值  音量范围是 0-1000
const int FPS = 1000/60; //帧率
//const int FPS = 60; //帧率
ExMessage msg;
int currentVolume = 500;
bool isWin = false;
bool isLose = false;


const int frameDelay = 100;
IMAGE  skill0_left[5], skill0_right[5], skill1_right[8], skill1_left[8], skill2_right[16], skill2_left[16], skill3[12];//存储技能图片资源

//图片加载相关所有全局变量

IMAGE img_player_idle_left;        //玩家向左静止图像
IMAGE img_player_idle_right;    //玩家向右静止图像

IMAGE img_bk_1;                    //背景1
IMAGE img_bk_2;                    //背景2
IMAGE img_bk_3;                    //背景3
IMAGE img_bk_4;                    //背景4
IMAGE img_bk_5;                    //背景5
IMAGE img_bk_6;                    //背景6
IMAGE LoseView;                    //游戏结束界面
IMAGE menuBackGround;
struct Collision {
    double x, y, w, h;//碰撞体 左上角坐标，长 宽
};


void menuView();
void InitMenu();
void LoadIMAGE();
void putimage_alpha(int x, int y, IMAGE* img);
void SettingVeiw();
void InitSetting();
void DrawVolumeControl();
void ListenerChecker();
void IntroductionVeiw();
void InitIntroduction();
void GameLoop();
void FinishChecker();
void RoomInit();
void EndChecker();
void InitLoseView();
void InitWinView();
void winView();
void loseView();
void DrawPlatform(vector<Collision> platform);
void DrawRoom2();
void DrawRoom3();
inline void flip_image(IMAGE* src, IMAGE* dst)	//水平翻转函数
{
    int w = src->getwidth();
    int h = src->getheight();
    Resize(dst, w, h);
    DWORD* src_buffer = GetImageBuffer(src);
    DWORD* dst_buffer = GetImageBuffer(dst);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int idx_src = y * w + x;
            int idx_dst = y * w + (w - x - 1);
            dst_buffer[idx_dst] = src_buffer[idx_src];
        }
    }
}


struct Atlas
{
public:

    void load_from_file(LPCTSTR path_template, int num)
    {
        //初始化数组数据和长度
        img_list.clear();
        img_list.resize(num);

        TCHAR path_file[256];
        for (int i = 0; i < num; i++)
        {
            _stprintf_s(path_file, path_template, i);
            loadimage(&img_list[i], path_file);
        }
    }

    void clear()
    {
        img_list.clear();
    }

    int get_size()
    {
        return (int)img_list.size();
    }

    IMAGE* get_image(int idx)
    {
        if (idx < 0 || idx >= img_list.size())
            return nullptr;

        return &img_list[idx];
    }

    void add_image(const IMAGE& img)
    {
        img_list.push_back(img);
    }

private:
    std::vector<IMAGE> img_list;
};
//动画加载相关所有全局变量
Atlas atlas_player_move_left;        //玩家向左移动图集
Atlas atlas_player_move_right;        //玩家向右移动图集
Atlas atlas_player_attack_left;     //玩家向左攻击图像
Atlas atlas_player_attack_right;    //玩家向右攻击图像
Atlas atlas_player_attack_effect_left;  //玩家向左攻击剑气
Atlas atlas_player_attack_effect_right; //玩家向右攻击剑气

Atlas atlas_mosquito_idle_left;        //蚊子向左静止图集
Atlas atlas_mosquito_idle_right;    //蚊子向右静止图集
Atlas atlas_mosquito_move_left;        //蚊子向左移动图集
Atlas atlas_mosquito_move_right;    //蚊子向右移动图集
Atlas atlas_mosquito_attack_left;    //蚊子向左攻击图集
Atlas atlas_mosquito_attack_right;    //蚊子向右攻击图集
Atlas atlas_mosquito_death_left;    //蚊子向左死亡图集
Atlas atlas_mosquito_death_right;    //蚊子向右死亡图集

Atlas atlas_beetle_idle_left;        //蚊子向左静止图集
Atlas atlas_beetle_idle_right;    //蚊子向右静止图集
Atlas atlas_beetle_move_left;        //蚊子向左移动图集
Atlas atlas_beetle_move_right;    //蚊子向右移动图集
Atlas atlas_beetle_attack_left;    //蚊子向左攻击图集
Atlas atlas_beetle_attack_right;    //蚊子向右攻击图集
Atlas atlas_beetle_death_left;    //蚊子向左死亡图集
Atlas atlas_beetle_death_right;    //蚊子向右死亡图集

//动画相关
struct Animation
{
public:

    void reset()    //重置动画状态
    {
        timer = 0;
        idx_frame = 0;
    }

    void set_atlas(Atlas* new_atlas)    //设置动画所需要的图集
    {
        reset();
        atlas = new_atlas;
    }

    void set_loop(bool flag)    //设置动画是否需要循环播放
    {
        is_loop = flag;
    }

    void set_interval(int ms)    //设置帧间隔
    {
        interval = ms;
    }

    int get_idx_frame()        //获取当前帧索引
    {
        return idx_frame;
    }

    IMAGE* get_frame()
    {
        return atlas->get_image(idx_frame);
    }

    bool check_finished()
    {
        if (is_loop) return false;

        return (idx_frame == atlas->get_size() - 1);
    }

    void on_update(int delta)    //动画更新
    {
        timer += delta;
        if (timer >= interval)
        {
            timer = 0;
            idx_frame++;

            if (idx_frame >= atlas->get_size())
            {
                idx_frame = is_loop ? 0 : atlas->get_size() - 1;
            }
        }
    }

    void on_draw(int x, int y) const
    {
        putimage_alpha(x, y, atlas->get_image(idx_frame));
    }


private:
    int timer = 0;            //计时器
    int interval = 0;        //帧间隔
    int idx_frame = 0;        //帧索引
    bool is_loop = true;    //是否循环
    Atlas* atlas = nullptr;
};

void flip_atlas(Atlas& src, Atlas& dst)
{
    dst.clear();
    for (int i = 0; i < src.get_size(); i++)
    {
        IMAGE img_flipped;
        flip_image(src.get_image(i), &img_flipped);
        dst.add_image(img_flipped);
    }
}

Animation anim_player_move_left;
Animation anim_player_move_right;
Animation anim_player_attack_left;
Animation anim_player_attack_right;
Animation anim_player_attack_effect_left;
Animation anim_player_attack_effect_right;
Animation anim_mosquito_idle_left;
Animation anim_mosquito_idle_right;
Animation anim_mosquito_move_left;
Animation anim_mosquito_move_right;
Animation anim_mosquito_attack_left;
Animation anim_mosquito_attack_right;
Animation anim_mosquito_death_left;
Animation anim_mosquito_death_right;
Animation anim_beetle_idle_left;
Animation anim_beetle_idle_right;
Animation anim_beetle_move_left;
Animation anim_beetle_move_right;
Animation anim_beetle_attack_left;
Animation anim_beetle_attack_right;
Animation anim_beetle_death_left;
Animation anim_beetle_death_right;

void AnimationBuild()
{
    extern Animation anim_player_move_left;
    anim_player_move_left.set_atlas(&atlas_player_move_left);
    anim_player_move_left.set_interval(75);
    anim_player_move_left.set_loop(true);

    extern Animation anim_player_move_right;
    anim_player_move_right.set_atlas(&atlas_player_move_right);
    anim_player_move_right.set_interval(75);
    anim_player_move_right.set_loop(true);

    extern Animation anim_player_attack_left;
    anim_player_attack_left.set_atlas(&atlas_player_attack_left);
    anim_player_attack_left.set_interval(10000);
    anim_player_attack_left.set_loop(false);
                   
    extern Animation anim_player_attack_right;
    anim_player_attack_right.set_atlas(&atlas_player_attack_right);
    anim_player_attack_right.set_interval(10000);
    anim_player_attack_right.set_loop(false);

    extern Animation anim_player_attack_effect_left;
    anim_player_attack_effect_left.set_atlas(&atlas_player_attack_effect_left);
    anim_player_attack_effect_left.set_interval(50000);
    anim_player_attack_effect_left.set_loop(false);

    extern Animation anim_player_attack_effect_right;
    anim_player_attack_effect_right.set_atlas(&atlas_player_attack_effect_right);
    anim_player_attack_effect_right.set_interval(50000);
    anim_player_attack_effect_right.set_loop(false);

    extern Animation anim_mosquito_idle_left;
    anim_mosquito_idle_left.set_atlas(&atlas_mosquito_idle_left);
    anim_mosquito_idle_left.set_interval(75);
    anim_mosquito_idle_left.set_loop(true);

    extern Animation anim_mosquito_idle_right;
    anim_mosquito_idle_right.set_atlas(&atlas_mosquito_idle_right);
    anim_mosquito_idle_right.set_interval(75);
    anim_mosquito_idle_right.set_loop(true);

    extern Animation anim_mosquito_move_left;
    anim_mosquito_move_left.set_atlas(&atlas_mosquito_move_left);
    anim_mosquito_move_left.set_interval(75);
    anim_mosquito_move_left.set_loop(true);

    extern Animation anim_mosquito_move_right;
    anim_mosquito_move_right.set_atlas(&atlas_mosquito_move_right);
    anim_mosquito_move_right.set_interval(75);
    anim_mosquito_move_right.set_loop(true);

    extern Animation anim_mosquito_attack_left;
    anim_mosquito_attack_left.set_atlas(&atlas_mosquito_attack_left);
    anim_mosquito_attack_left.set_interval(75);
    anim_mosquito_attack_left.set_loop(true);

    extern Animation anim_mosquito_attack_right;
    anim_mosquito_attack_right.set_atlas(&atlas_mosquito_attack_right);
    anim_mosquito_attack_right.set_interval(75);
    anim_mosquito_attack_right.set_loop(true);

    extern Animation anim_mosquito_death_left;
    anim_mosquito_death_left.set_atlas(&atlas_mosquito_death_left);
    anim_mosquito_death_left.set_interval(75);
    anim_mosquito_death_left.set_loop(true);

    extern Animation anim_mosquito_death_right;
    anim_mosquito_death_right.set_atlas(&atlas_mosquito_death_right);
    anim_mosquito_death_right.set_interval(75);
    anim_mosquito_death_right.set_loop(true);

    extern Animation anim_beetle_idle_left;
    anim_beetle_idle_left.set_atlas(&atlas_beetle_idle_left);
    anim_beetle_idle_left.set_interval(75);
    anim_beetle_idle_left.set_loop(true);

    extern Animation anim_beetle_idle_right;
    anim_beetle_idle_right.set_atlas(&atlas_beetle_idle_right);
    anim_beetle_idle_right.set_interval(75);
    anim_beetle_idle_right.set_loop(true);

   extern Animation anim_beetle_move_left;
    anim_beetle_move_left.set_atlas(&atlas_beetle_move_left);
    anim_beetle_move_left.set_interval(75);
    anim_beetle_move_left.set_loop(true);

    extern Animation anim_beetle_move_right;
    anim_beetle_move_right.set_atlas(&atlas_beetle_move_right);
    anim_beetle_move_right.set_interval(75);
    anim_beetle_move_right.set_loop(true);

    extern Animation anim_beetle_attack_left;
    anim_beetle_attack_left.set_atlas(&atlas_beetle_attack_left);
    anim_beetle_attack_left.set_interval(75);
    anim_beetle_attack_left.set_loop(true);

    extern Animation anim_beetle_attack_right;
    anim_beetle_attack_right.set_atlas(&atlas_beetle_attack_right);
    anim_beetle_attack_right.set_interval(75);
    anim_beetle_attack_right.set_loop(true);

    extern Animation anim_beetle_death_left;
    anim_beetle_death_left.set_atlas(&atlas_beetle_death_left);
    anim_beetle_death_left.set_interval(75);
    anim_beetle_death_left.set_loop(true);

    extern Animation anim_beetle_death_right;
    anim_beetle_death_right.set_atlas(&atlas_beetle_death_right);
    anim_beetle_death_right.set_interval(75);
    anim_beetle_death_right.set_loop(true);

}

struct Button {
    int x, y, w, h;
    int lastTime=clock();
    int needTime=200;
    const char* text;
    bool isClicked = false; // 添加点击状态

    bool inButton() {
        return (msg.x >= x && msg.x <= x + w && msg.y >= y && msg.y <= y + h);
    }

    void update() {
        isClicked = false;
        peekmessage(&msg, EX_MOUSE);
        if (inButton()&& msg.message == WM_LBUTTONDOWN&&clock()-lastTime>=needTime) {
            isClicked = true;
            lastTime = clock();
            //flushmessage(EX_MOUSE); // 清除消息避免重复触发
        }
        
    }

    bool istrigger() {
        if (isClicked) {
            cout << "trigger" << endl;
        }
        return isClicked;
    }

    void init() {    // 确保文本不为空
        if (text == nullptr) {
            text = _T("");
        }

        double tw = w * 0.7;        // 文本宽度
        double th = h * 0.7;        // 文本高度
        double tx = x + (w - tw) / 2;
        double ty = y + (h - th) / 2;

        // 按钮边框
        setlinecolor(BLACK);
        setlinestyle(PS_SOLID, 3);

        // 只绘制默认状态，不处理悬停效果
        setfillcolor(RGB(52, 152, 219));
        fillroundrect(x, y, x + w, y + h, 30, 30);

        // 绘制文本
        settextstyle(th, tw / _tcslen(text), _T("宋体"));
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);
        outtextxy(tx, ty, text);
    }
};

//按钮
Button ListenButton[2];
Button Buttons[4];
Button BackButton;
int RoomId = 1;
struct Player {
    double x, y;//左上角坐标
    Collision collision;//碰撞体
    double speed;//x方向速度
    double vx, vy;//两个方向速度
    double gravity;//重力加速度
    double jumpForce;//跳跃力
    bool isGround;//是否在地面
    int dir;//方向+-
    int hp;//血量
    int maxhp;
    bool isFacingLeft = false;	//是否向左
    int damage = 50;	//伤害

    //鼠标消息处理对应变量
    bool isMoveLeft = false;//是否在左侧移动
    bool isMoveRight = false;//是否在右侧移动
    bool isAttack = false;//是否攻击

    //Attack涉及变量
    int interval = 500;
    int timer = interval;

    bool isSkillActive;			//是否正在释放技能
    int currentSkill;			//当前技能类型（1/2/3）
    int currentFrame;			//当前动画帧
    clock_t lastFrameTime;		//上一帧时间
    //冷却系统
    bool isSkillOnCooldown1;		//技能冷却状态
    bool isSkillOnCooldown2;
    bool isSkillOnCooldown3;
    clock_t lastSkillTime1;
    clock_t lastSkillTime2;
    clock_t lastSkillTime3;
    clock_t cooldownDuration1=8000;
    clock_t cooldownDuration2=8000;
    clock_t cooldownDuration3=8000;
    //技能状态结构体 存储技能释放时的位置和方向以及技能进行到了第几帧
    struct SkillState {
        bool active = false;//技能是否在进行中
        int frame = 0;//当前技能进行到了第几帧
        clock_t lastFrameTime = 0;//上一帧的释放时间
        double x1, y1;      // 技能触发时的位置
        int dir = 1;     // 技能触发时的方向
    };
    SkillState skill1State, skill2State, skill3State;//给三个技能都设置相应状体
};
struct Beetle
{
public:
    int hp;
    Collision collision;//碰撞体

    int atk;//攻击力

    vector<IMAGE>runLeft;//朝左走
    vector<IMAGE>runRight;//超右走
    vector<IMAGE>dieLeft;//超左死
    vector<IMAGE>dieRight;//朝右死
    vector<IMAGE>atkLeft;//左攻击
    vector<IMAGE>atkRight;//右攻击
    double speed;//速度
    int view;//视野，只要怪物和角色距离小于view就进入攻击状态
    int atkRange;//攻击距离，同视野
    double idleX1, idleX2;//待机状态的巡视范围
    bool right;//朝向
    bool following;//是否跟随角色
    bool attacking;//攻击中
    bool canAtk;//进入攻击范围
    bool deathSta, deathEnd;//播放死亡动画，直接return
    int deathInd;//死亡图片下标
    int runInd;//行走下标
    int atkInd;//攻击下标
    int atkLast;//上一次攻击时间 clock（）
    int atkTime;//攻击需要间隔
    int frame, frameMax;//攻击已间隔几帧    攻击图像之间需要间隔几帧
    int maxHp;//最大血量

    //动画相关变量
    Animation* animIdleLeft;
    Animation* animIdleRight;
    Animation* animMoveLeft;
    Animation* animMoveRight;
    Animation* animAttackLeft;
    Animation* animAttackRight;


};
struct Mosquito {
    //数值
    int maxHp, atk;//血量、攻击力
    Collision collision;//碰撞体
    double speed;//速度
    int view;//视野，只要怪物和角色距离小于view就进入攻击状态
    int atkRange;//攻击距离，同视野
    int atkW, atkH;
    int maxIdleDistance;//单向最大行走距离
    int atkTime;//攻击需要间隔
    int frame;//攻击已经延迟的帧数
    int frameMax;//攻击延迟几帧，暂定3

    //图集
    vector<IMAGE>runLeft;//朝左走
    vector<IMAGE>runRight;//超右走
    vector<IMAGE>dieLeft;//超左死
    vector<IMAGE>dieRight;//朝右死
    vector<IMAGE>atkLeft;//朝右攻击
    vector<IMAGE>atkRight;//朝左攻击

    //状态
    bool right;//朝向
    bool following;//是否跟随角色
    bool canAtk;//进入攻击距离
    bool attacking;//攻击中
    bool deathEnd;//死亡动画播放完毕,彻底死亡

    //图集下标
    int deathInd;//死亡图片下标
    int runInd;//待机下标
    int atkInd;//攻击下标

    //成对变量
    int hp;//血量--------最大血量maxHP
    int idleDistance;//行走距离------最大行走距离idleMaxDistance
    int atkLast;//上一次攻击时间 clock（）-----------攻击冷却时间atkTime
};
struct Novice {
    //数值
    int maxHp, atk;//血量、攻击力
    Collision collision;//碰撞体
    Collision collisionTmp;
    double speed;//速度
    int view;//视线长度
    int readysta;//开始观看的时间
    int readyTime;//需要的观看时长
    int atkRange;//近战攻击距离
    //int atkW, atkH;
    int atkTime;//攻击需要间隔
    int frame;//攻击已经延迟的帧数
    int frameMax;//攻击延迟几帧，暂定3
    vector<double>tag;//观看时长达到，就朝tag冲刺

    //图集
    vector<IMAGE>idleLeft;//朝左待机
    vector<IMAGE>idleRight;//超右待机
    vector<IMAGE>dieLeft;//超左死
    vector<IMAGE>dieRight;//朝右死
    vector<IMAGE>atkLeft;//朝右攻击
    vector<IMAGE>atkRight;//朝左攻击
    vector<IMAGE>dashLeft;//朝左攻击
    vector<IMAGE>dashRight;//朝右攻击
    int idleIndMax;
    int dieIndMax;
    int atkIndMax;
    int dashIndMax;

    //状态
    bool right;//朝向
    bool readydash;//蓄力中
    bool dash;//冲刺
    bool canAtk;//进入攻击距离
    bool attacking;//攻击中
    bool deathEnd;//死亡动画播放完毕,彻底死亡

    //图集下标
    int deathInd;//死亡图片下标
    int idleInd;//待机下标
    int atkInd;//攻击下标
    int dashInd;//冲刺状态

    //成对变量
    int hp;//血量--------最大血量maxHP
    int atkLast;//上一次攻击时间 clock（）-----------攻击冷却时间atkTime
};

//陷阱
struct Trap {
    int x, y, w, h;
    int interval = 2000;//伤害间隔为一秒，可调
    int timer_player = 0; //伤害判定计时器
    IMAGE img;
    int damage = 5;
};


//房间--------------------------------------------------------------------------------------

vector<Collision> platforms;//平台
vector<Beetle> Beetles;//甲壳虫
vector<Mosquito> Mosquitos;//蚊子
vector<Novice> Novices;//
vector<Trap> Traps;//陷阱
int EnemyCounter1;//敌人计数器
vector<Collision> platforms2;//平台
vector<Beetle> Beetles2;//甲壳虫
vector<Mosquito> Mosquitos2;//蚊子
vector<Novice> Novices2;//蚊子
vector<Trap> Traps2;//陷阱
int EnemyCounter2;//敌人计数器
vector<Collision> platforms3;//平台
vector<Beetle> Beetles3;//甲壳虫
vector<Mosquito> Mosquitos3;//蚊子
vector<Novice> Novices3;
vector<Trap> Traps3;
int EnemyCounter3;

double playerInitx = 20, playerInity = 250;//玩家初始化位置

//----------------------------------------------------------------玩家相关------------------------------------------------------------------------------------------------------

Player player;
//把int PlayercheckObstacleCollision();的位置移动到void Playermove();
//之前，并且我们只设置了一个房间没有设置房间号的索引所以把函数的参数删去了

//玩家函数声明
void PlayerGetMsg();
int PlayercheckObstacleCollisionX2();
int PlayercheckObstacleCollisionY2();
void Playermove();
void PlayerJump();
void PlayeruseSkill(int skillType);
void PlayerupdateSkillAnimation();
void PlayerupdateCooldown();
void PlayerDrawSkill();
void PlayerPhysicsLogic();
void loadskill_skill();
void PlayerrenderPlayer();
void PlayerUpdate();
void ShowHp();
void FinishChecker();
int PlayercheckObstacleCollisionY1();
int PlayercheckObstacleCollisionX1();
bool isOverlapping(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
void PlayerAttack(int delta, vector<Mosquito>& enemys, vector<Beetle>& Beetles,vector<Novice>& Novices);
void PlayerAttack(int delta, vector<Novice>& Novices);
void PlayerAttack(int delta, vector<Mosquito>& enemys, vector<Beetle>& Beetles);
void PlayerSkillAttack(int delta, vector<Novice>& Novices);
void PlayerSkillAttack(int delta, vector<Mosquito>& enemys, vector<Beetle>& beetles);
void PlayerPhysicsLogic() {
    player.isGround = false;
    player.vy += player.gravity;
    int collisionResult;
    double lastPosY = player.collision.y;
    player.collision.y += player.vy;
    if (RoomId == 2) {
        collisionResult = PlayercheckObstacleCollisionY2();
    }
    else {
        collisionResult = PlayercheckObstacleCollisionY1();
    }
    if (collisionResult == 2) { // 底部碰撞
        player.collision.y = lastPosY;
        player.vy = 0;
        player.isGround = true;
    }
    else if (collisionResult == 1) { // 顶部碰撞
        player.collision.y = lastPosY;
        player.vy = 0;
    }

    player.y = player.collision.y;
}
void PlayerrenderPlayer() {
    if (player.vx == 0) {
        if (player.isFacingLeft) {
            drawImg(player.x, player.y, &img_player_idle_left);
        }
        else {
            drawImg(player.x, player.y, &img_player_idle_right);
        }
    }
    else {
        if (player.isFacingLeft) {
            drawImg(player.x, player.y, &img_player_idle_left);
        }
        else {
            drawImg(player.x, player.y, &img_player_idle_right);
        }
    }
}
 //加载技能图片（用户定义函数名）
void loadskill_skill() {
    for (int i = 0; i < 8; i++) {
        char str[500];
        sprintf_s(str, "skill1_left\\1-%d.png", i);
        loadimage(skill1_left + i, str);
        sprintf_s(str, "skill1_right\\1-%d.png", i);
        loadimage(skill1_right + i, str);
    }
    for (int i = 0; i < 16; i++) {
        char str[500];
        sprintf_s(str, "skill2_left\\2-%d.png", i);
        loadimage(skill2_left + i, str);
        sprintf_s(str, "skill2_right\\2-%d.png", i);
        loadimage(skill2_right + i, str);
    }
    for (int i = 0; i < 12; i++) {
        char str[500];
        sprintf_s(str, "skill3\\3-%d.png", i);
        loadimage(skill3 + i, str);
    }
}
 //初始化玩家状态（用户定义函数名）
void PlayerInit() {
    player.x = 1000;
    player.y = 300;
    player.collision = { 500, 300, 57, 121 };
    //player.speed = 12;
    player.speed = 8;
    player.vx = 0;
    player.vy = 0;
    player.gravity = 5;
    //player.jumpForce = 12;
    player.jumpForce = 50;
    player.isGround = true;
    player.dir = 1;
    player.maxhp = 1000;
    player.hp = 1000;
}

//Room2检测
int PlayercheckObstacleCollisionX2() {
    double player_x1 = player.collision.x;
    double player_y1 = player.collision.y;
    double player_x2 = player_x1 + player.collision.w;
    double player_y2 = player_y1 + player.collision.h;
    int collisionDir = 0; // 0=无碰撞，3=左侧，4=右侧

    for (const auto& platform : platforms2) {
        double platform_x1 = platform.x;
        double platform_y1 = platform.y;
        double platform_x2 = platform.x + platform.w;
        double platform_y2 = platform.y + platform.h;

        // 快速排除不相交情况
        if (player_x2 <= platform_x1 || player_x1 >= platform_x2 ||
            player_y2 <= platform_y1 || player_y1 >= platform_y2) {
            continue;
        }

        double overlapLeft = player_x2 - platform_x1;
        double overlapRight = platform_x2 - player_x1;

        if (overlapLeft < overlapRight) {
            collisionDir = 3; // 左侧碰撞
        }
        else {
            collisionDir = 4; // 右侧碰撞
        }
        break; // 只处理第一个碰撞
    }
    return collisionDir;
}
// 只检测Y方向碰撞
int PlayercheckObstacleCollisionY2() {
    double player_x1 = player.collision.x;
    double player_y1 = player.collision.y;
    double player_x2 = player_x1 + player.collision.w;
    double player_y2 = player_y1 + player.collision.h;
    int collisionDir = 0; // 0=无碰撞，1=顶部，2=底部

    for (const auto& platform : platforms2) {
        double platform_x1 = platform.x;
        double platform_y1 = platform.y;
        double platform_x2 = platform.x + platform.w;
        double platform_y2 = platform.y + platform.h;

        // 快速排除不相交情况
        if (player_x2 <= platform_x1 || player_x1 >= platform_x2 ||
            player_y2 <= platform_y1 || player_y1 >= platform_y2) {
            continue;
        }

        double overlapTop = player_y2 - platform_y1;
        double overlapBottom = platform_y2 - player_y1;

        if (overlapTop < overlapBottom) {
            collisionDir = 1; // 顶部碰撞
            player.isGround = true;
        }
        else {
            collisionDir = 2; // 底部碰撞
        }
        break; // 只处理第一个碰撞
    }
    return collisionDir;
}
//Room1检测
int PlayercheckObstacleCollisionX1() {
    double player_x1 = player.collision.x;
    double player_y1 = player.collision.y;
    double player_x2 = player_x1 + player.collision.w;
    double player_y2 = player_y1 + player.collision.h;
    int collisionDir = 0; // 0=无碰撞，3=左侧，4=右侧

    for (const auto& platform : platforms) {
        double platform_x1 = platform.x;
        double platform_y1 = platform.y;
        double platform_x2 = platform.x + platform.w;
        double platform_y2 = platform.y + platform.h;

        // 快速排除不相交情况
        if (player_x2 <= platform_x1 || player_x1 >= platform_x2 ||
            player_y2 <= platform_y1 || player_y1 >= platform_y2) {
            continue;
        }

        double overlapLeft = player_x2 - platform_x1;
        double overlapRight = platform_x2 - player_x1;

        if (overlapLeft < overlapRight) {
            collisionDir = 3; // 左侧碰撞
        }
        else {
            collisionDir = 4; // 右侧碰撞
        }
        break; // 只处理第一个碰撞
    }
    return collisionDir;
}
// 只检测Y方向碰撞
int PlayercheckObstacleCollisionY1() {
    double player_x1 = player.collision.x;
    double player_y1 = player.collision.y;
    double player_x2 = player_x1 + player.collision.w;
    double player_y2 = player_y1 + player.collision.h;
    int collisionDir = 0; // 0=无碰撞，1=顶部，2=底部

    for (const auto& platform : platforms) {
        double platform_x1 = platform.x;
        double platform_y1 = platform.y;
        double platform_x2 = platform.x + platform.w;
        double platform_y2 = platform.y + platform.h;

        // 快速排除不相交情况
        if (player_x2 <= platform_x1 || player_x1 >= platform_x2 ||
            player_y2 <= platform_y1 || player_y1 >= platform_y2) {
            continue;
        }

        double overlapTop = player_y2 - platform_y1;
        double overlapBottom = platform_y2 - player_y1;

        if (overlapTop < overlapBottom) {
            collisionDir = 1; // 顶部碰撞
            player.isGround = true;
        }
        else {
            collisionDir = 2; // 底部碰撞
        }
        break; // 只处理第一个碰撞
    }
    return collisionDir;
}



// 处理水平移动（用户定义函数名）
void Playermove() {
    double lastPosX = player.collision.x;
    player.collision.x += player.vx;
    if (player.collision.x <= 0) player.collision.x = 0;
    if (player.collision.x + player.collision.w >= 1260) player.collision.x = 1260 - player.collision.w;

    int collisionResult;
    if(RoomId==2)
    {
        collisionResult = PlayercheckObstacleCollisionX2();
    }
    else {
        collisionResult = PlayercheckObstacleCollisionX1();
    }
    if (collisionResult == 3 || collisionResult == 4) {
        player.collision.x = lastPosX;
        player.vx = 0;
    }

    player.x = player.collision.x;
}
// 跳跃函数（用户定义函数名）
void PlayerJump() {
    if (player.isGround) {
        player.vy = -player.jumpForce;
        player.isGround = false;
    }
}
// 使用技能函数（用户定义函数名）
void PlayeruseSkill(int skillType) {
    clock_t now = clock();
    switch (skillType) {
    case 1: // 技能1
        if (!player.isSkillOnCooldown1) {
            // 修正技能1的位置计算，基于玩家碰撞体的边缘
            player.skill1State = { true, 0, now,
                player.collision.x + (player.dir == 1 ? player.collision.w : -190), // 调整为合适的偏移量
                player.collision.y + player.collision.h - 250, player.dir };
            player.isSkillOnCooldown1 = true;
            player.lastSkillTime1 = now;
        }
        break;
    case 2: // 技能2
        if (!player.isSkillOnCooldown2) {
            // 修正技能2的位置计算，基于玩家碰撞体的边缘
            player.skill2State = { true, 0, now,
                player.collision.x + (player.dir == 1 ? player.collision.w : -255), // 调整为合适的偏移量
                player.collision.y, player.dir };
            player.isSkillOnCooldown2 = true;
            player.lastSkillTime2 = now;
        }
        break;
    case 3: // 技能3
        if (!player.isSkillOnCooldown3) {
            player.skill3State = { true, 0, now,
                player.collision.x, player.collision.y, player.dir };
            player.isSkillOnCooldown3 = true;
            player.lastSkillTime3 = now;
        }
        break;
    }
}
// 绘制技能动画（用户定义函数名）
void PlayerDrawSkill() {
    // 技能3优先渲染
    if (player.skill3State.active) {
        drawImg(player.skill3State.x1 - 150, player.skill3State.y1 - 250, skill3 + player.skill3State.frame);
    }

    // 技能2
    if (player.skill2State.active) {
        double x = player.skill2State.x1;
        // 修正技能2在左侧的渲染位置
        if (player.skill2State.dir == -1) {
            x = player.skill2State.x1 - 40 * player.skill2State.frame; // 向左扩展
        }
        else {
            x = player.skill2State.x1 + 40 * player.skill2State.frame; // 向右扩展
        }
        drawImg(x, player.skill2State.y1,
            player.skill2State.dir == 1 ? skill2_right + player.skill2State.frame : skill2_left + player.skill2State.frame);
    }

    // 技能1
    if (player.skill1State.active) {
        drawImg(player.skill1State.x1, player.skill1State.y1,
            player.skill1State.dir == 1 ? skill1_right + player.skill1State.frame : skill1_left + player.skill1State.frame);
    }

    // 冷却状态显示
    settextcolor(YELLOW);
    settextstyle(16, 0, _T("宋体"));
    outtextxy(10, 10, _T("技能状态:"));

    // 技能1冷却
    if (player.isSkillOnCooldown1) {
        settextcolor(RED);
        double t = max(0.0, (player.cooldownDuration1 - (clock() - player.lastSkillTime1)) / 1000.0);
        TCHAR txt[50];
        _stprintf_s(txt, _T("K: %.1f秒"), t);
        outtextxy(10, 30, txt);
    }
    else {
        settextcolor(GREEN);
        outtextxy(10, 30, _T("K: 就绪"));
    }

    // 技能2冷却
    if (player.isSkillOnCooldown2) {
        settextcolor(RED);
        double t = max(0.0, (player.cooldownDuration2 - (clock() - player.lastSkillTime2)) / 1000.0);
        TCHAR txt[50];
        _stprintf_s(txt, _T("L: %.1f秒"), t);
        outtextxy(10, 50, txt);
    }
    else {
        settextcolor(GREEN);
        outtextxy(10, 50, _T("L: 就绪"));
    }

    // 技能3冷却
    if (player.isSkillOnCooldown3) {
        settextcolor(RED);
        double t = max(0.0, (player.cooldownDuration3 - (clock() - player.lastSkillTime3)) / 1000.0);
        TCHAR txt[50];
        _stprintf_s(txt, _T("I: %.1f秒"), t);
        outtextxy(10, 70, txt);
    }
    else {
        settextcolor(GREEN);
        outtextxy(10, 70, _T("I: 就绪"));
    }
}
bool Mosquitodie = true;
bool Beetledie = true;
void PlayerGetMsg() {
    while (peekmessage(&msg, EX_KEY)) {
        switch (msg.message) {
        case WM_KEYDOWN:
            switch (msg.vkcode) {
            case 'A': player.vx = -player.speed; player.dir = -1; break;
            case 'D': player.vx = player.speed; player.dir = 1; break;
            case VK_SPACE: PlayerJump(); break;
            case 'K': PlayeruseSkill(1); break;
            case 'L': PlayeruseSkill(2); break;
            case 'I': PlayeruseSkill(3); break;
            case 'J': player.isAttack = true; break;
                //case VK_ESCAPE: EndBatchDraw(); return 0;
            }
            break;
        case WM_KEYUP:
            switch (msg.vkcode) {
            case 'A': player.vx = 0; break;
            case 'D': player.vx = 0; break;
            case 'J': player.isAttack = false; break;
            }
            break;
        }
    }
}
void PlayerAttack(int delta, vector<Mosquito>& enemys, vector<Beetle>& Beetles,vector<Novice>& Novices) {

    if (player.isAttack && player.timer >= player.interval) {
        if (enemys.size() > 0)
        {
            for (auto& enemy : enemys)
            {
                if (player.dir == -1) {
                    if (player.collision.x - player.collision.w <= enemy.collision.x + enemy.collision.w && player.collision.x >= enemy.collision.x && player.collision.y <= enemy.collision.y + enemy.collision.h && player.collision.y + player.collision.h >= enemy.collision.y) {
                        enemy.hp -= player.damage;
                    }
                }
                else {
                    if (player.collision.x + player.collision.w <= enemy.collision.x + enemy.collision.w && player.collision.x + 2 * player.collision.w >= enemy.collision.x && player.collision.y <= enemy.collision.y + enemy.collision.h && player.collision.y + player.collision.h >= enemy.collision.y) {
                        enemy.hp -= player.damage;
                    }
                }
            }
        }
        if (Beetles.size() > 0)
        {
            for (auto& Beetle : Beetles)
            {
                if (player.dir == -1) {
                    if (player.collision.x - player.collision.w <= Beetle.collision.x + Beetle.collision.w && player.collision.x >= Beetle.collision.x && player.collision.y <= Beetle.collision.y + Beetle.collision.h && player.collision.y + player.collision.h >= Beetle.collision.y) {
                        Beetle.hp -= player.damage;
                    }
                }
                else {
                    if (player.collision.x + player.collision.w <= Beetle.collision.x + Beetle.collision.w && player.collision.x + 2 * player.collision.w >= Beetle.collision.x && player.collision.y <= Beetle.collision.y + Beetle.collision.h && player.collision.y + player.collision.h >= Beetle.collision.y) {
                        Beetle.hp -= player.damage;
                    }
                }
            }
        }
        if (Novices.size() > 0)
        {
            for (auto& Beetle : Novices)
            {
                if (player.dir == -1) {
                    if (player.collision.x - player.collision.w <= Beetle.collision.x + Beetle.collision.w && player.collision.x >= Beetle.collision.x && player.collision.y <= Beetle.collision.y + Beetle.collision.h && player.collision.y + player.collision.h >= Beetle.collision.y) {
                        Beetle.hp -= player.damage;
                    }
                }
                else {
                    if (player.collision.x + player.collision.w <= Beetle.collision.x + Beetle.collision.w && player.collision.x + 2 * player.collision.w >= Beetle.collision.x && player.collision.y <= Beetle.collision.y + Beetle.collision.h && player.collision.y + player.collision.h >= Beetle.collision.y) {
                        Beetle.hp -= player.damage;
                    }
                }
            }
        }
        player.timer = 0;
    }
    player.timer += delta;

}
void PlayerAttack(int delta, vector<Mosquito>& enemys, vector<Beetle>& Beetles) {
    if (player.isAttack && player.timer >= player.interval) {
        if (enemys.size() > 0)
        {
            for (auto& enemy : enemys)
            {
                if (player.dir == -1) {
                    if (player.collision.x - player.collision.w <= enemy.collision.x + enemy.collision.w && player.collision.x >= enemy.collision.x && player.collision.y <= enemy.collision.y + enemy.collision.h && player.collision.y + player.collision.h >= enemy.collision.y) {
                        enemy.hp -= player.damage;
                    }
                }
                else {
                    if (player.collision.x + player.collision.w <= enemy.collision.x + enemy.collision.w && player.collision.x + 2 * player.collision.w >= enemy.collision.x && player.collision.y <= enemy.collision.y + enemy.collision.h && player.collision.y + player.collision.h >= enemy.collision.y) {
                        enemy.hp -= player.damage;
                    }
                }
            }
        }
        if (Beetles.size() > 0)
        {
            for (auto& Beetle : Beetles)
            {
                if (player.dir == -1) {
                    if (player.collision.x - player.collision.w <= Beetle.collision.x + Beetle.collision.w && player.collision.x >= Beetle.collision.x && player.collision.y <= Beetle.collision.y + Beetle.collision.h && player.collision.y + player.collision.h >= Beetle.collision.y) {
                        Beetle.hp -= player.damage;
                    }
                }
                else {
                    if (player.collision.x + player.collision.w <= Beetle.collision.x + Beetle.collision.w && player.collision.x + 2 * player.collision.w >= Beetle.collision.x && player.collision.y <= Beetle.collision.y + Beetle.collision.h && player.collision.y + player.collision.h >= Beetle.collision.y) {
                        Beetle.hp -= player.damage;
                    }
                }
            }
        }
        player.timer = 0;
    }
    player.timer += delta;
}
void PlayerAttack(int delta, vector<Novice>& Novices) {
    if (player.isAttack && player.timer >= player.interval) {
        if (Novices.size() > 0)
        {
            for (auto& Beetle : Novices)
            {
                if (player.dir == -1) {
                    if (player.collision.x - player.collision.w <= Beetle.collision.x + Beetle.collision.w && player.collision.x >= Beetle.collision.x && player.collision.y <= Beetle.collision.y + Beetle.collision.h && player.collision.y + player.collision.h >= Beetle.collision.y) {
                        Beetle.hp -= player.damage;
                    }
                }
                else {
                    if (player.collision.x + player.collision.w <= Beetle.collision.x + Beetle.collision.w && player.collision.x + 2 * player.collision.w >= Beetle.collision.x && player.collision.y <= Beetle.collision.y + Beetle.collision.h && player.collision.y + player.collision.h >= Beetle.collision.y) {
                        Beetle.hp -= player.damage;
                    }
                }
            }
        }
        player.timer = 0;
    }
    player.timer += delta;
}
// 碰撞检测函数：判断两个矩形是否重叠
bool isOverlapping(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 &&
        y1 < y2 + h2 && y1 + h1 > y2);
}

// 技能动画更新（用户定义函数名）
void PlayerupdateSkillAnimation() {
    clock_t now = clock();

    // 技能1动画
    if (player.skill1State.active) {
        if (now - player.skill1State.lastFrameTime >= 50) {
            player.skill1State.frame++;
            player.skill1State.lastFrameTime = now;
            if (player.skill1State.frame >= 4) player.skill1State.active = false;
        }
    }

    // 技能2动画
    if (player.skill2State.active) {
        if (now - player.skill2State.lastFrameTime >= frameDelay) {
            player.skill2State.frame++;
            player.skill2State.lastFrameTime = now;
            if (player.skill2State.frame >= 16) player.skill2State.active = false;
        }
    }

    // 技能3动画
    if (player.skill3State.active) {
        if (now - player.skill3State.lastFrameTime >= 50) {
            player.skill3State.frame++;
            player.skill3State.lastFrameTime = now;
            if (player.skill3State.frame >= 12) player.skill3State.active = false;
        }
    }
}
// 技能伤害判定
void PlayerSkillAttack(int delta, vector<Mosquito>& enemys, vector<Beetle>& beetles,vector<Novice>& Novices)
{
    // 三技能在释放的第5帧进行伤害判定
    if (player.skill3State.active && player.skill3State.frame == 5) {
        int x1 = player.skill3State.x1 - 150;
        int y1 = player.skill3State.y1 - 250;
        int w = 290;
        int h = 270;
        int x2 = x1 + w;
        int y2 = y1 + h; // 修正：原为 y2 = x2 + h

        // 对蚊子敌人造成伤害
        if(enemys.size() > 0)
        {
            for (auto& enemy : enemys) {
                if (isOverlapping(x1, y1, w, h,
                    enemy.collision.x, enemy.collision.y,
                    enemy.collision.w, enemy.collision.h)) {
                    enemy.hp -= 250;
                }
            }
        }

        // 对甲虫敌人造成伤害
        if (beetles.size() > 0)
        {
            for (auto& beetle : beetles) {
                if (isOverlapping(x1, y1, w, h,
                    beetle.collision.x, beetle.collision.y,
                    beetle.collision.w, beetle.collision.h)) {
                    beetle.hp -= 250;
                }
            }
        }

        if (Novices.size() > 0)
        {
            for (auto& beetle : Novices) {
                if (isOverlapping(x1, y1, w, h,
                    beetle.collision.x, beetle.collision.y,
                    beetle.collision.w, beetle.collision.h)) {
                    beetle.hp -= 250;
                }
            }
        }
    }

    // 2技能在释放的每一帧都进行伤害判定
    if (player.skill2State.active) {
        // 根据方向和释放帧数计算技能范围
        int x1 = player.skill2State.x1 + 40 * player.skill2State.frame * player.skill2State.dir;
        int y1 = player.skill2State.y1;
        int w = 255;
        int h = 90;
        int x2 = x1 + w;
        int y2 = y1 + h;

        // 对蚊子敌人造成伤害
        for (auto& enemy : enemys) {
            if (isOverlapping(x1, y1, w, h,
                enemy.collision.x, enemy.collision.y,
                enemy.collision.w, enemy.collision.h)) {
                enemy.hp -= player.damage / 8; // 假设伤害为1/8
            }
        }

        // 对甲虫敌人造成伤害
        for (auto& beetle : beetles) {
            if (isOverlapping(x1, y1, w, h,
                beetle.collision.x, beetle.collision.y,
                beetle.collision.w, beetle.collision.h)) {
                beetle.hp -= player.damage / 8; // 假设伤害为1/8
            }
        }

        for (auto& beetle : Novices) {
            if (isOverlapping(x1, y1, w, h,
                beetle.collision.x, beetle.collision.y,
                beetle.collision.w, beetle.collision.h)) {
                beetle.hp -= player.damage/8;
            }
        }
    }

    // 一技能在释放的第3帧进行伤害判定
    if (player.skill1State.active && player.skill1State.frame == 2) {
        int x1 = player.skill1State.x1;
        int y1 = player.skill1State.y1;
        int w = 190;
        int h = 250;
        int x2 = x1 + w;
        int y2 = y1 + h; // 修正：原为技能2的frame检查

        // 对蚊子敌人造成伤害
        for (auto& enemy : enemys) {
            if (isOverlapping(x1, y1, w, h,
                enemy.collision.x, enemy.collision.y,
                enemy.collision.w, enemy.collision.h)) {
                enemy.hp -= 500;
            }
        }

        // 对甲虫敌人造成伤害
        for (auto& beetle : beetles) {
            if (isOverlapping(x1, y1, w, h,
                beetle.collision.x, beetle.collision.y,
                beetle.collision.w, beetle.collision.h)) {
                beetle.hp -= 500;
            }
        }

        for (auto& beetle : Novices) {
            if (isOverlapping(x1, y1, w, h,
                beetle.collision.x, beetle.collision.y,
                beetle.collision.w, beetle.collision.h)) {
                beetle.hp -= 500;
            }
        }
    }
}
void PlayerSkillAttack(int delta, vector<Mosquito>& enemys, vector<Beetle>& beetles)
{
    // 三技能在释放的第5帧进行伤害判定
    if (player.skill3State.active && player.skill3State.frame == 5) {
        int x1 = player.skill3State.x1 - 150;
        int y1 = player.skill3State.y1 - 250;
        int w = 290;
        int h = 270;
        int x2 = x1 + w;
        int y2 = y1 + h; // 修正：原为 y2 = x2 + h

        // 对蚊子敌人造成伤害
        if (enemys.size() > 0)
        {
            for (auto& enemy : enemys) {
                if (isOverlapping(x1, y1, w, h,
                    enemy.collision.x, enemy.collision.y,
                    enemy.collision.w, enemy.collision.h)) {
                    enemy.hp -= 250;
                }
            }
        }

        // 对甲虫敌人造成伤害
        if (beetles.size() > 0)
        {
            for (auto& beetle : beetles) {
                if (isOverlapping(x1, y1, w, h,
                    beetle.collision.x, beetle.collision.y,
                    beetle.collision.w, beetle.collision.h)) {
                    beetle.hp -= 250;
                }
            }
        }
    }

    // 2技能在释放的每一帧都进行伤害判定
    if (player.skill2State.active) {
        // 根据方向和释放帧数计算技能范围
        int x1 = player.skill2State.x1 + 40 * player.skill2State.frame * player.skill2State.dir;
        int y1 = player.skill2State.y1;
        int w = 255;
        int h = 90;
        int x2 = x1 + w;
        int y2 = y1 + h;

        // 对蚊子敌人造成伤害
        for (auto& enemy : enemys) {
            if (isOverlapping(x1, y1, w, h,
                enemy.collision.x, enemy.collision.y,
                enemy.collision.w, enemy.collision.h)) {
                enemy.hp -= player.damage / 8; // 假设伤害为1/8
            }
        }

        // 对甲虫敌人造成伤害
        for (auto& beetle : beetles) {
            if (isOverlapping(x1, y1, w, h,
                beetle.collision.x, beetle.collision.y,
                beetle.collision.w, beetle.collision.h)) {
                beetle.hp -= player.damage / 8; // 假设伤害为1/8
            }
        }

        for (auto& beetle : Novices) {
            if (isOverlapping(x1, y1, w, h,
                beetle.collision.x, beetle.collision.y,
                beetle.collision.w, beetle.collision.h)) {
                beetle.hp -= player.damage / 8;
            }
        }
    }

    // 一技能在释放的第3帧进行伤害判定
    if (player.skill1State.active && player.skill1State.frame == 2) {
        int x1 = player.skill1State.x1;
        int y1 = player.skill1State.y1;
        int w = 190;
        int h = 250;
        int x2 = x1 + w;
        int y2 = y1 + h; // 修正：原为技能2的frame检查

        // 对蚊子敌人造成伤害
        for (auto& enemy : enemys) {
            if (isOverlapping(x1, y1, w, h,
                enemy.collision.x, enemy.collision.y,
                enemy.collision.w, enemy.collision.h)) {
                enemy.hp -= 500;
            }
        }

        // 对甲虫敌人造成伤害
        for (auto& beetle : beetles) {
            if (isOverlapping(x1, y1, w, h,
                beetle.collision.x, beetle.collision.y,
                beetle.collision.w, beetle.collision.h)) {
                beetle.hp -= 500;
            }
        }

    }
}
void PlayerSkillAttack(int delta, vector<Novice>& Novices)
{
    // 三技能在释放的第5帧进行伤害判定
    if (player.skill3State.active && player.skill3State.frame == 5) {
        int x1 = player.skill3State.x1 - 150;
        int y1 = player.skill3State.y1 - 250;
        int w = 290;
        int h = 270;
        int x2 = x1 + w;
        int y2 = y1 + h; // 修正：原为 y2 = x2 + h


        if (Novices.size() > 0)
        {
            for (auto& beetle : Novices) {
                if (isOverlapping(x1, y1, w, h,
                    beetle.collision.x, beetle.collision.y,
                    beetle.collision.w, beetle.collision.h)) {
                    beetle.hp -= 250;
                }
            }
        }
    }

    // 2技能在释放的每一帧都进行伤害判定
    if (player.skill2State.active) {
        // 根据方向和释放帧数计算技能范围
        int x1 = player.skill2State.x1 + 40 * player.skill2State.frame * player.skill2State.dir;
        int y1 = player.skill2State.y1;
        int w = 255;
        int h = 90;
        int x2 = x1 + w;
        int y2 = y1 + h;

        for (auto& beetle : Novices) {
            if (isOverlapping(x1, y1, w, h,
                beetle.collision.x, beetle.collision.y,
                beetle.collision.w, beetle.collision.h)) {
                beetle.hp -= player.damage / 8;
            }
        }
    }

    // 一技能在释放的第3帧进行伤害判定
    if (player.skill1State.active && player.skill1State.frame == 2) {
        int x1 = player.skill1State.x1;
        int y1 = player.skill1State.y1;
        int w = 190;
        int h = 250;
        int x2 = x1 + w;
        int y2 = y1 + h; // 修正：原为技能2的frame检查

        for (auto& beetle : Novices) {
            if (isOverlapping(x1, y1, w, h,
                beetle.collision.x, beetle.collision.y,
                beetle.collision.w, beetle.collision.h)) {
                beetle.hp -= 500;
            }
        }
    }
}
// 冷却更新（用户定义函数名）
void PlayerupdateCooldown() {
    clock_t now = clock();
    if (player.isSkillOnCooldown1 && now - player.lastSkillTime1 >= player.cooldownDuration1) {
        player.isSkillOnCooldown1 = false;
    }
    if (player.isSkillOnCooldown2 && now - player.lastSkillTime2 >= player.cooldownDuration2) {
        player.isSkillOnCooldown2 = false;
    }
    if (player.isSkillOnCooldown3 && now - player.lastSkillTime3 >= player.cooldownDuration3) {
        player.isSkillOnCooldown3 = false;
    }
}

void PlayerRenderer() {

    anim_player_move_left.on_update(60);
    anim_player_move_right.on_update(60);
    anim_player_attack_left.on_update(60);
    anim_player_attack_right.on_update(60);
    anim_player_attack_effect_left.on_update(60);
    anim_player_attack_effect_right.on_update(60);

    rectangle(player.x, player.y, player.x + player.collision.w, player.y + player.collision.h);
    if (player.isAttack && player.timer >= player.interval)
    {
        if (player.dir == 1) {
            anim_player_attack_right.on_draw(player.x, player.y);
            anim_player_attack_effect_right.on_draw(player.x - 30 , player.y);
        }
        else {
            anim_player_attack_left.on_draw(player.x, player.y);
            anim_player_attack_effect_left.on_draw(player.x - 100, player.y);
        }
    }
    else {
        if (player.vx == 0.0) {
            anim_player_move_right.reset();
            anim_player_move_left.reset();
            if (player.dir == 1) {
                drawImg(player.x, player.y, &img_player_idle_right);
            }
            else {
                drawImg(player.x, player.y, &img_player_idle_left);
            }
        }
        else {
            if (player.dir == 1) {
                anim_player_move_right.on_draw(player.x, player.y);
            }
            else {
                anim_player_move_left.on_draw(player.x, player.y);
            }
        }
    }
   
}
void PlayerUpdate() {
    PlayerGetMsg();
    PlayerRenderer();
    Playermove();
    PlayerPhysicsLogic();
    PlayerupdateSkillAnimation();
    PlayerupdateCooldown();
    PlayerDrawSkill();
    if(RoomId==2)
    {
        PlayerAttack(60, Mosquitos2, Beetles2);
        PlayerSkillAttack(60, Mosquitos2, Beetles2);
    }
    else if(RoomId==1){
        PlayerAttack(60, Mosquitos, Beetles);
        PlayerSkillAttack(60, Mosquitos, Beetles);
    }
    else if (RoomId == 3) {
        PlayerAttack(60, Novices3);
        PlayerSkillAttack(60, Novices3);
    }
    ShowHp();
}
void ShowHp()
{
    double rate = player.hp * 1.0 / player.maxhp;
    setfillcolor(RED);
    rectangle(player.collision.x, player.collision.y - 10, player.collision.x + player.collision.w, player.collision.y);
    fillrectangle(player.collision.x, player.collision.y - 10, player.collision.x + player.collision.w * rate, player.collision.y);
}


//------------------------------------------------------------敌人相关------------------------------------------------------------------------------------

double Distance(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}


void NoviceInit(Novice& self, double stax, double ground);
void NoviceBehaviour(Novice& self, const vector<Collision>& obstacles);
vector<double> NoviceCheckPlayer(Novice& self);
void NoviceDrawImage(Novice& self);
void NoviceDash(Novice& self, const vector<Collision>& obstacles);
bool NoviceCollisionChecker(Novice& self, const vector<Collision>& obstacles);
bool NoviceAttackCheck(Novice& self, double distance);
void NoviceAttack(Novice& self);
void NoviceDeath(Novice& self);
void NoviceShowHp(Novice& self);
void NoviceAttackWait(Novice& self);
void NoviceDrawDash(Novice& self);
void NoviceFollowing(Novice& self, const vector<Collision>& obstacles);

void NoviceInit(Novice& self, double stax, double ground)
{
    //血量、攻击力、碰撞体、速度、视野、攻击范围、巡视区域、朝右、（能攻击、跟随玩家、开始死亡、已死亡  全false）、图像数组下标
    self.hp = 3000;
    self.atk = 30;
    self.collision = { stax,ground - 352,192,352 };
    self.collisionTmp = { stax,ground - 137,210,137 };
    self.speed = 6;
    self.view = 300;
    self.atkRange = 200;
    //self.atkW = 70;
    //self.atkH = 120;
    self.right = true;
    self.canAtk = self.attacking = self.dash = self.deathEnd = false;
    self.deathInd = 0;
    self.idleInd = 0;
    self.atkInd = 0;
    //攻击时间
    self.readyTime = 500;
    self.readysta = 0;
    self.atkLast = clock();
    self.atkTime = 1000;
    self.maxHp = self.hp;
    self.frame = 0;
    self.frameMax = 3;

    //左右行走，左右死亡,左右攻击
    self.idleLeft.resize(10);
    self.idleRight.resize(10);
    self.dieLeft.resize(3);
    self.dieRight.resize(3);
    self.atkLeft.resize(8);
    self.atkRight.resize(8);
    self.dashLeft.resize(12);
    self.dashRight.resize(12);
    char name[100] = "";
    for (int i = 0; i < 10; i++)
    {
        sprintf_s(name, "assets/enemy2/Idle/left-%d.png", i);
        loadimage(&self.idleLeft[i], name);
        sprintf_s(name, "assets/enemy2/Idle/right-%d.png", i);
        loadimage(&self.idleRight[i], name);
    }
    for (int i = 0; i < 3; i++)
    {
        sprintf_s(name, "assets/enemy2/death/left-%d.png", i);
        loadimage(&self.dieLeft[i], name);
        sprintf_s(name, "assets/enemy2/death/right-%d.png", i);
        loadimage(&self.dieRight[i], name);
    }
    for (int i = 0; i < 8; i++)
    {
        sprintf_s(name, "assets/enemy2/attack/left-%d.png", i);
        loadimage(&self.atkLeft[i], name);
        sprintf_s(name, "assets/enemy2/attack/right-%d.png", i);
        loadimage(&self.atkRight[i], name);
    }
    for (int i = 0; i < 12; i++)
    {
        sprintf_s(name, "assets/enemy2/run/left-%d.png", i);
        loadimage(&self.dashLeft[i], name);
        sprintf_s(name, "assets/enemy2/run/right-%d.png", i);
        loadimage(&self.dashRight[i], name);
    }
    self.idleIndMax = 10;
    self.dieIndMax = 3;
    self.atkIndMax = 8;
    self.dashIndMax = 12;
}
void NoviceBehaviour(Novice& self, const vector<Collision>& obstacles)
{
    rectangle(self.collision.x, self.collision.y, self.collision.x + self.collision.w, self.collision.y + self.collision.h);
    //死亡相关
    //彻底死亡
    if (self.deathEnd)return;
    //播放死亡动画
    if (self.hp <= 0)
    {
        NoviceDeath(self);
        return;
    }

    NoviceShowHp(self);//展示血条
    if (!self.dash)
    {
        self.tag = NoviceCheckPlayer(self);
    }
    else NoviceCheckPlayer(self);
    NoviceAttackCheck(self, self.tag[2]);//攻击状态
    if (self.dash)
    {
        printf("冲刺\n");
        NoviceDash(self, obstacles);
    }
    else if (self.readydash)
    {
        printf("准备冲刺\n");
        NoviceAttackWait(self);
    }
    else if (self.attacking)//攻击状态
    {
        printf("攻击\n");
        NoviceAttack(self);
    }
    else if (self.canAtk)
    {
        printf("准备攻击\n");
        NoviceAttackWait(self);

    }
    else
    {
        printf("追击\n");
        NoviceFollowing(self, obstacles);
    }
}
vector<double> NoviceCheckPlayer(Novice& self)
{

    //返回角色矩形左右中点与怪物矩形左右中点中距离最近的点及距离
    vector<double> re;
    re.push_back(player.collision.x + player.collision.w / 2);
    re.push_back(player.collision.y + player.collision.h / 2);
    double d = Distance(re[0], re[1], self.collision.x + self.collision.w / 2, self.collision.y + self.collision.h / 2);
    re.push_back(d);
    //double tmpx= self.collision.x + self.collision.w / 2, tmpy= self.collision.y + self.collision.h / 2;
    Collision tmp1 = { 0.0, self.collision.y, self.collision.x + self.collision.w - self.view, self.collision.y + self.collision.h };
    Collision tmp2 = { self.collision.x + self.view, self.collision.y, 1280, self.collision.y + self.collision.h };
    //rectangle(tmp1.x, tmp1.y, tmp1.x + tmp1.w, tmp1.y + tmp1.h);
    //rectangle(tmp2.x, tmp2.y, tmp2.x + tmp2.w, tmp2.y + tmp2.h);
    if (re[0] >= tmp1.x && re[0] <= tmp1.x + tmp1.w && re[1] >= tmp1.y && re[1] <= tmp1.y + tmp1.h)
    {
        if (!self.readydash)
        {
            self.readydash = true;
            self.readysta = clock();
        }
        else
        {
            if (clock() - self.readysta >= self.readyTime)
            {
                self.right = false;
                self.dash = true;
                self.readydash = false;
            }
        }
    }
    else if (re[0] >= tmp2.x && re[0] <= tmp2.x + tmp2.w && re[1] >= tmp2.y && re[1] <= tmp2.y + tmp2.h)
    {
        if (!self.readydash)
        {
            self.readydash = true;
            self.readysta = clock();
        }
        else
        {
            if (clock() - self.readysta >= self.readyTime)
            {
                self.right = true;
                self.dash = true;
                self.readydash = false;
            }
        }
    }
    else
    {
        self.readydash = false;
    }
    return re;
}
void NoviceDrawImage(Novice& self)
{
    if (self.right)drawImg(self.collision.x - 17, self.collision.y - 41, &self.idleRight[self.idleInd++]);
    else drawImg(self.collision.x - 17, self.collision.y - 41, &self.idleLeft[self.idleInd++]);
    if (self.idleInd == self.idleIndMax)self.idleInd = 0;
}
void NoviceDash(Novice& self, const vector<Collision>& obstacles)
{
    int oldX = self.collision.x;
    int op = (self.right) ? 1 : -1;
    self.collision.x += self.speed * op;
    if (NoviceCollisionChecker(self, obstacles))self.collision.x = oldX;//检查x碰撞并回退

    int oldY = self.collision.y;
    self.collision.y += (self.collision.y + self.collision.h < player.collision.y + player.collision.h) ? self.speed : -self.speed;
    if (NoviceCollisionChecker(self, obstacles))self.collision.y = oldY;
    NoviceDrawDash(self);
}
bool NoviceCollisionChecker(Novice& self, const vector<Collision>& obstacles) {//碰撞检测
    for (const auto& ob : obstacles)
    {
        if (self.collision.x >= ob.x + ob.w || self.collision.x + self.collision.w <= ob.x
            || self.collision.y >= ob.y + ob.h || self.collision.y + self.collision.h <= ob.y)continue;
        return true;
    }
    return false;
}
bool NoviceAttackCheck(Novice& self, double distance)
{
    if (distance <= self.atkRange)
    {
        self.canAtk = true;
        if (self.tag[0] <= self.collision.x + self.collision.w / 2)self.right = false;
        else self.right = true;
        if (clock() - self.atkLast >= self.atkTime)
        {
            self.attacking = true;
            /*self.atkLast = clock();*/
        }
    }
    else self.canAtk = false;
    return self.attacking;
}
void NoviceAttack(Novice& self)
{
    if (self.atkInd == self.atkIndMax)//结束帧特判----伤害判定和后摇
    {
        //确保每一帧都有怪物贴图
        drawImg(self.collision.x - 111, self.collision.y - 45, self.right ? &self.atkRight[self.atkInd - 1] : &self.atkLeft[self.atkInd - 1]);
        if (self.frame == self.frameMax)
        {
            self.atkLast = clock();
            self.atkInd = 0;
            self.frame = 0;
            self.attacking = false;
            double x1, x2, y1, y2;//伤害区域
            if (self.right)
            {
                x1 = self.collision.x + self.collision.w - 70;
                y1 = self.collision.y + 170;
                x2 = x1 + 200;
                y2 = y1 + 200;
            }
            else
            {
                x1 = self.collision.x - 130;
                y1 = self.collision.y + 170;
                x2 = self.collision.x + 50;
                y2 = y1 + 200;
            }
            //rectangle(x1, y1, x2, y2);

            //检测是否攻击到
            if (player.collision.x > x2 || player.collision.x + player.collision.w<x1
                || player.collision.y>y2 || player.collision.y + player.collision.h < y1) {
            }
            else //攻击到
            {
                player.hp -= self.atk;//伤害判定
            }
        }
        else
        {
            self.frame += 1;
        }

    }
    else
    {
        drawImg(self.collision.x - 111, self.collision.y - 45, self.right ? &self.atkRight[self.atkInd] : &self.atkLeft[self.atkInd]);
        if (self.frame == self.frameMax)
        {
            self.atkInd += 1;
            self.frame = 0;
        }
        else self.frame += 1;
    }
}
void NoviceDeath(Novice& self) {

    if (self.deathInd == self.dieIndMax)
    {
        drawImg(self.collision.x - 40, self.collision.y - 90, self.right ? &self.dieRight[self.deathInd - 1] : &self.dieLeft[self.deathInd - 1]);
        if (self.frame == self.frameMax)
        {
            self.frame = 0;
            self.deathEnd = true;
            self.deathInd = 0;
            if (RoomId == 2)
            {
                EnemyCounter2 -= 1;
            }
            else {
                EnemyCounter1 -= 1;
            }
        }
        else self.frame += 1;
    }
    else
    {
        drawImg(self.collision.x - 40, self.collision.y - 90, self.right ? &self.dieRight[self.deathInd] : &self.dieLeft[self.deathInd]);
        if (self.frame == self.frameMax)
        {
            self.frame = 0;
            self.deathInd += 1;
        }
        else self.frame += 1;
    }
}
void NoviceShowHp(Novice& self)
{
    double rate = self.hp * 1.0 / self.maxHp;
    setfillcolor(RED);
    rectangle(self.collision.x, self.collision.y - 10, self.collision.x + self.collision.w, self.collision.y);
    fillrectangle(self.collision.x, self.collision.y - 10, self.collision.x + self.collision.w * rate, self.collision.y);
}
void NoviceAttackWait(Novice& self)
{
    NoviceDrawImage(self);
}
void NoviceDrawDash(Novice& self)
{
    if (self.dashInd == self.dashIndMax)//结束帧特判----伤害判定和后摇
    {
        //确保每一帧都有怪物贴图
        drawImg(self.collision.x - 20, self.collision.y - 60, self.right ? &self.dashRight[self.dashInd - 1] : &self.dashLeft[self.dashInd - 1]);
        if (self.frame == self.frameMax)
        {
            self.dashInd = 0;
            self.frame = 0;
            self.dash = false;

            // ---------------------------------------------修改伤害判定----------------------------------------------------
            double x1, x2, y1, y2;//伤害区域
            if (self.right)
            {
                x1 = self.collision.x + self.collision.w;
                y1 = self.collision.y;
                x2 = 1280;
                y2 = y1 + self.collision.h;
            }
            else
            {
                x1 = 0;
                y1 = self.collision.y;
                x2 = self.collision.x;
                y2 = y1 + self.collision.h;
            }
            //rectangle(x1, y1, x2, y2);

            //检测是否攻击到
            if (player.collision.x > x2 || player.collision.x + player.collision.w<x1
                || player.collision.y>y2 || player.collision.y + player.collision.h < y1) {
            }
            else //攻击到
            {
                player.hp -= self.atk / 2;//伤害判定
            }
        }
        else
        {
            self.frame += 1;
        }

    }
    else
    {
        self.collisionTmp.x = self.collision.x + self.collision.w - self.collisionTmp.w;
        self.collisionTmp.y = self.collision.y + self.collision.h - self.collisionTmp.h;
        if (self.dashInd >= 5 && self.dashInd <= 9)drawImg(self.collisionTmp.x - 43, self.collisionTmp.y - 64, self.right ? &self.dashRight[self.dashInd] : &self.dashLeft[self.dashInd]);
        else drawImg(self.collision.x - 20, self.collision.y - 60, self.right ? &self.dashRight[self.dashInd] : &self.dashLeft[self.dashInd]);
        if (self.frame == self.frameMax)
        {

            if (self.dashInd >= 5 && self.dashInd <= 9 && !(player.collision.x > self.collisionTmp.x + self.collisionTmp.w
                || player.collision.x + player.collision.w < self.collisionTmp.x
                || player.collision.y>self.collisionTmp.y + self.collisionTmp.h || player.collision.y + player.collision.h < self.collisionTmp.y))
            {
                rectangle(self.collisionTmp.x, self.collisionTmp.y, self.collisionTmp.x + self.collisionTmp.w, self.collisionTmp.y + self.collisionTmp.h);
                player.hp -= self.atk / 2;
            }
            else if ((self.atkInd < 5 || self.atkInd>9) && !(player.collision.x > self.collision.x + self.collision.w
                || player.collision.x + player.collision.w < self.collision.x
                || player.collision.y>self.collision.y + self.collision.h || player.collision.y + player.collision.h < self.collision.y))
            {
                rectangle(self.collision.x, self.collision.y, self.collision.x + self.collision.w, self.collision.y + self.collision.h);
                player.hp -= self.atk / 2;
            }


            self.dashInd += 1;
            self.frame = 0;
        }
        else self.frame += 1;
    }
}
void NoviceFollowing(Novice& self, const vector<Collision>& obstacles)
{
    int oldX = self.collision.x;
    self.collision.x += (self.collision.x + self.collision.w / 2 < player.collision.x + player.collision.w / 2) ? self.speed : -self.speed;
    if (NoviceCollisionChecker(self, obstacles))self.collision.x = oldX;
    int oldY = self.collision.y;
    self.collision.y += (self.collision.y + self.collision.h < player.collision.y + player.collision.h) ? self.speed : -self.speed;
    if (NoviceCollisionChecker(self, obstacles))self.collision.y = oldY;
    NoviceDrawImage(self);
}

void MosquitoInit(Mosquito& self, double stax, double ground);
void MosquitoBehaviour(Mosquito& self, const vector<Collision>& obstacles);
vector<double> MosquitoCheckPlayer(Mosquito& self);
void MosquitoIdle(Mosquito& self, const vector<Collision>& obstacles);
void MosquitoDrawImage(Mosquito& self);
void MosquitoRun(Mosquito& self, int tagx, int tagy, const vector<Collision>& obstacles);
bool MosquitoCollisionChecker(Mosquito& self, const vector<Collision>& obstacles);
bool MosquitoAttackCheck(Mosquito& self, double distance);
void MosquitoAttack(Mosquito& self);
void MosquitoDeath(Mosquito& self);
void MosquitoShowHp(Mosquito& self);
void MosquitoAttackWait(Mosquito& self);

void MosquitoInit(Mosquito& self, double stax, double ground)
{
    //血量、攻击力、碰撞体、速度、视野、攻击范围、巡视区域、朝右、（能攻击、跟随玩家、开始死亡、已死亡  全false）、图像数组下标
    self.hp = 500;
    self.atk = 20;
    self.collision = { stax,ground - 88,98,88 };
    self.speed = 6;
    /*self.view = 200;
    self.atkRange = 50;*/
    self.view = 100;
    self.atkRange = 50;
    self.atkW = 70;
    self.atkH = 120;
    self.right = true;
    self.canAtk = self.attacking = self.following = self.deathEnd = false;
    self.deathInd = 0;
    self.runInd = 0;
    self.atkInd = 0;
    //行走距离
    self.maxIdleDistance = 300;
    self.idleDistance = 0;
    //攻击时间
    self.atkLast = clock();
    self.atkTime = 1000;
    self.maxHp = self.hp;
    self.frame = 0;
    self.frameMax = 1;
    /*self.followingFrame = 0;
    self.followingFrameMax = 30;*/
    ////玩家数组
    //self.tag.resize(3);
    //self.tag = MosquitoCheckPlayer(self);

    //左右行走，左右死亡,左右攻击
    self.runLeft.resize(5);
    self.runRight.resize(5);
    self.dieLeft.resize(3);
    self.dieRight.resize(3);
    self.atkLeft.resize(4);
    self.atkRight.resize(4);
    char name[100] = "";
    for (int i = 0; i < 5; i++)
    {
        sprintf_s(name, "assets/enemy0/Idle/left-%d.png", i);
        loadimage(&self.runLeft[i], name);
        sprintf_s(name, "assets/enemy0/Idle/right-%d.png", i);
        loadimage(&self.runRight[i], name);
    }
    for (int i = 0; i < 3; i++)
    {
        sprintf_s(name, "assets/enemy0/death/left-%d.png", i);
        loadimage(&self.dieLeft[i], name);
        sprintf_s(name, "assets/enemy0/death/right-%d.png", i);
        loadimage(&self.dieRight[i], name);
    }
    for (int i = 0; i < 4; i++)
    {
        sprintf_s(name, "assets/enemy0/attack/left-%d.png", i);
        loadimage(&self.atkLeft[i], name);
        sprintf_s(name, "assets/enemy0/attack/right-%d.png", i);
        loadimage(&self.atkRight[i], name);
    }
}
void MosquitoBehaviour(Mosquito& self, const vector<Collision>& obstacles)
{
    //死亡相关
    //彻底死亡
    if (self.deathEnd)return;
    //播放死亡动画
    if (self.hp <= 0)
    {
        MosquitoDeath(self);
        return;
    }

    MosquitoShowHp(self);//展示血条
    vector<double> tag = MosquitoCheckPlayer(self);
    MosquitoAttackCheck(self, tag[2]);//攻击状态

    if (self.attacking)//攻击状态
    {
        MosquitoAttack(self);
    }
    else if (self.canAtk)
    {
        MosquitoAttackWait(self);
    }
    else if (self.following)//跟随状态
    {
        MosquitoRun(self, tag[0], tag[1], obstacles);
    }
    else
    {
        MosquitoIdle(self, obstacles);//待机状态
    }
}
vector<double> MosquitoCheckPlayer(Mosquito& self)
{
    double px1 = player.collision.x, px2 = player.collision.x + player.collision.w;
    double py1 = player.collision.y + player.collision.h / 2, py2 = player.collision.y + player.collision.h / 2;
    double mx1 = self.collision.x, mx2 = self.collision.x + self.collision.w;
    double my1 = self.collision.y + self.collision.h / 2, my2 = self.collision.y + self.collision.h / 2;
    double minpx, minpy, minmx, minmy;
    double d11 = Distance(px1, py1, mx1, my1);
    double d12 = Distance(px1, py1, mx2, my2);
    double d21 = Distance(px2, py2, mx1, my1);
    double d22 = Distance(px2, py2, mx2, my2);
    double d = min(min(d11, d12), min(d21, d22));
    if (d == d11) { minpx = px1; minpy = py1; minmx = mx1; minmy = my1; }
    else if (d == d12) { minpx = px1; minpy = py1; minmx = mx2; minmy = my2; }
    else if (d == d21) { minpx = px2; minpy = py2; minmx = mx1; minmy = my1; }
    else if (d == d22) { minpx = px2; minpy = py2; minmx = mx2; minmy = my2; }
    //返回角色矩形左右中点与怪物矩形左右中点中距离最近的点及距离
    vector<double> re;
    re.push_back(minpx);
    re.push_back(minpy);
    re.push_back(d);
    if (d <= self.view)
    {
        if (d == d11) { self.right = false; }
        else if (d == d12) { self.right = true; }
        else if (d == d21) { self.right = false; }
        else if (d == d22) { self.right = true; }
        self.following = true;
    }
    else
    {
        self.following = false;
    }
    return re;
}
void MosquitoIdle(Mosquito& self, const vector<Collision>& obstacles) {
    //达到最大巡视距离或碰壁，反向
    self.idleDistance += (int)self.speed;
    double oldX = self.collision.x;
    if (self.right)self.collision.x += self.speed;
    else self.collision.x -= self.speed;

    if (MosquitoCollisionChecker(self, obstacles))
    {
        self.right = !self.right;
        self.idleDistance = 0;
        self.collision.x = oldX;
    }
    if (self.idleDistance >= self.maxIdleDistance)
    {
        self.right = !self.right;
        self.idleDistance = 0;
    }
    MosquitoDrawImage(self);
}
void MosquitoDrawImage(Mosquito& self)
{
    if (self.right)drawImg(self.collision.x - 6, self.collision.y - 98, &self.runRight[self.runInd++]);
    else drawImg(self.collision.x - 6, self.collision.y - 98, &self.runLeft[self.runInd++]);
    if (self.runInd == 5)self.runInd = 0;
}
void MosquitoRun(Mosquito& self, int tagx, int tagy, const vector<Collision>& obstacles)
{
    int oldX = self.collision.x;
    int oldY = self.collision.y;

    int op = (tagx >= (self.collision.x + self.collision.w / 2)) ? 1 : -1;
    self.collision.x += self.speed * op;
    if (MosquitoCollisionChecker(self, obstacles))self.collision.x = oldX;//检查y碰撞并回退

    op = (tagy >= (self.collision.y + self.collision.h / 2)) ? 1 : -1;
    self.collision.y += self.speed * op;
    if (MosquitoCollisionChecker(self, obstacles))self.collision.y = oldY;//检查y碰撞并回退
    MosquitoDrawImage(self);
}
bool MosquitoCollisionChecker(Mosquito& self, const vector<Collision>& obstacles) {//碰撞检测1:x   2:y
    for (const auto& ob : obstacles)
    {
        if (self.collision.x >= ob.x + ob.w || self.collision.x + self.collision.w <= ob.x
            || self.collision.y >= ob.y + ob.h || self.collision.y + self.collision.h <= ob.y)continue;
        return true;
    }
    return false;
}
bool MosquitoAttackCheck(Mosquito& self, double distance)
{
    if (distance <= self.atkRange)
    {
        self.canAtk = true;
        if (clock() - self.atkLast >= self.atkTime)
        {
            self.attacking = true;
            self.atkLast = clock();
        }
    }
    else self.canAtk = false;
    return self.attacking;
}
void MosquitoAttack(Mosquito& self)
{
    if (self.atkInd == 4)//结束帧特判----伤害判定和后摇
    {
        //确保每一帧都有怪物贴图
        drawImg(self.collision.x - 6, self.collision.y - 98, self.right ? &self.atkRight[self.atkInd - 1] : &self.atkLeft[self.atkInd - 1]);
        if(self.frame==self.frameMax)
        {
            self.atkInd = 0;
            self.frame = 0;
            self.attacking = false;
            double x1, x2, y1, y2;//伤害区域
            if (self.right)
            {
                x1 = self.collision.x + self.collision.w;
                y1 = self.collision.y;
                x2 = x1 + self.atkW;
                y2 = y1 + self.atkH;
            }
            else
            {
                x1 = self.collision.x - self.atkW;
                y1 = self.collision.y;
                x2 = self.collision.x;
                y2 = y1 + self.atkH;
            }
            //rectangle(x1, y1, x2, y2);

            //检测是否攻击到
            if (player.collision.x > x2 || player.collision.x + player.collision.w<x1
                || player.collision.y>y2 || player.collision.y + player.collision.h < y1) {
            }
            else //攻击到
            {
                player.hp -= self.atk;//伤害判定
            }
        }else
        {
            self.frame += 1;
        }
        
    }
    else
    {
        drawImg(self.collision.x - 6, self.collision.y - 98, self.right ? &self.atkRight[self.atkInd] : &self.atkLeft[self.atkInd]);
        if (self.frame == self.frameMax)
        {
            self.atkInd += 1;
            self.frame = 0;
        }
        else self.frame += 1;
    }
}
void MosquitoDeath(Mosquito& self) {

    if (self.deathInd == 3)
    {
        drawImg(self.collision.x - 6, self.collision.y - 98, self.right ? &self.dieRight[self.deathInd - 1] : &self.dieLeft[self.deathInd - 1]);
        if (self.frame == self.frameMax)
        {
            self.frame = 0;
            self.deathEnd = true;
            self.deathInd = 0;
            if(RoomId==2)
            {
                EnemyCounter2 -= 1;
            }
            else {
                EnemyCounter1 -= 1;
            }
        }
        else self.frame += 1;
    }
    else
    {
        drawImg(self.collision.x - 6, self.collision.y - 98, self.right ? &self.dieRight[self.deathInd] : &self.dieLeft[self.deathInd]);
        if (self.frame == self.frameMax)
        {
            self.frame = 0;
            self.deathInd += 1;
        }
        else self.frame += 1;
    }
}
void MosquitoShowHp(Mosquito& self)
{
    double rate = self.hp * 1.0 / self.maxHp;
    setfillcolor(RED);
    rectangle(self.collision.x, self.collision.y - 10, self.collision.x + self.collision.w, self.collision.y);
    fillrectangle(self.collision.x, self.collision.y - 10, self.collision.x + self.collision.w * rate, self.collision.y);
}
void MosquitoAttackWait(Mosquito& self)
{
    MosquitoDrawImage(self);
}

//Beetle //灰名
void BeetleInit(Beetle& self, double stax, double ground, double idleX1, double idleX2);
void BeetleBehaviour(Beetle& self, const vector<Collision>& obstacles);

//声明Beetle函数
vector<double> BeetleCheckPlayer(Beetle& self);
void BeetleIdle(Beetle& self, const vector<Collision>& obstacles);
void BeetleDrawIdle(Beetle& self);
void BeetleRun(Beetle& self, int tagx, int tagy, const vector<Collision>& obstacles);
bool BeetleCollisionChecker(Beetle& self, const vector<Collision>& obstacles);
bool BeetleFrontEmpty(Beetle& self, const vector<Collision>& obstacles);
bool BeetleAttackCheck(Beetle& self, double distance);
void BeetleAttack(Beetle& self);
void BeetleDeath(Beetle& self);
void BeetleShowHp(Beetle& self);
void BeetleAttackWait(Beetle& self);

//实现Beetle函数
void BeetleInit(Beetle& self, double stax, double ground, double idleX1, double idleX2)
{
    //血量、攻击力、碰撞体、速度、视野、攻击范围、巡视区域、朝右、（能攻击、跟随玩家、开始死亡、已死亡  全false）、图像数组下标
    self.hp = 500;
    self.atk = 20;
    self.collision = { stax,ground - 59,95,59 };
    self.speed = 6;
    /*self.view = 200;
    self.atkRange = 100;*/
    self.view = 200;
    self.atkRange = 50;
    self.idleX1 = idleX1;
    self.idleX2 = idleX2;
    self.right = true;
    self.canAtk = self.attacking = self.following = self.deathSta = self.deathEnd = false;
    self.deathInd = 0;
    self.runInd = 0;
    //攻击时间
    self.atkLast = clock();
    self.atkTime = 500;
    self.frame = 0;
    self.frameMax = 1;
    //最大血量
    self.maxHp = self.hp;

    //左右行走，左右死亡
    self.runLeft.resize(4);
    self.runRight.resize(4);
    self.dieLeft.resize(5);
    self.dieRight.resize(5);
    self.atkLeft.resize(4);
    self.atkRight.resize(4);
    char name[100] = "";
    for (int i = 0; i < 4; i++)
    {
        sprintf_s(name, "assets/enemy1/idle/left-%d.png", i);
        loadimage(&self.runLeft[i], name);
        sprintf_s(name, "assets/enemy1/idle/right-%d.png", i);
        loadimage(&self.runRight[i], name);
    }
    for (int i = 0; i < 5; i++)
    {
        sprintf_s(name, "assets/enemy1/death/left-%d.png", i);
        loadimage(&self.dieLeft[i], name);
        sprintf_s(name, "assets/enemy1/death/right-%d.png", i);
        loadimage(&self.dieRight[i], name);
    }
    for (int i = 0; i < 4; i++)
    {
        sprintf_s(name, "assets/enemy1/attack/left-%d.png", i);
        loadimage(&self.atkLeft[i], name);
        sprintf_s(name, "assets/enemy1/attack/right-%d.png", i);
        loadimage(&self.atkRight[i], name);
    }

}
void BeetleBehaviour(Beetle& self, const vector<Collision>& obstacles)
{
    if (self.deathEnd)return;
    if (self.hp <= 0)
    {
        BeetleDeath(self);//播放死亡动画
        return;
    }
    BeetleShowHp(self);//展示血条
    vector<double>tag = BeetleCheckPlayer(self);
    BeetleAttackCheck(self, tag[2]);

    if (self.attacking)
    {
        BeetleAttack(self);
    }
    else if (self.canAtk)
    {
        BeetleAttackWait(self);
    }
    else if (self.following)
    {
        BeetleRun(self, tag[0], tag[1], obstacles);
    }
    else
    {
        BeetleIdle(self, obstacles);
    }
}
vector<double> BeetleCheckPlayer(Beetle& self)
{
    //返回角色矩形中点
    vector<double> re;
    re.push_back(player.collision.x + player.collision.w / 2);
    re.push_back(player.collision.y + player.collision.h / 2);
    double distance = Distance(re[0], re[1], self.collision.x + self.collision.w / 2, self.collision.y + self.collision.h / 2);
    re.push_back(distance);
    if (distance <= self.view)
    {
        self.right = (re[0] >= self.collision.x + self.collision.w / 2) ? true : false;
        self.following = true;
    }
    else self.following = false;
    return re;
}
void BeetleIdle(Beetle& self, const vector<Collision>& obstacles) {
    //在idleX1和idlwX2中游走
    //如果当前不在idleX1和idleX2之间就先返回再开始游走
    self.collision.x += self.right ? self.speed : -self.speed;
    if (self.collision.x <= self.idleX1)
    {
        self.right = true;//转向
        if (self.idleX1 - self.collision.x <= self.speed)
            self.collision.x = self.idleX1;//补齐与idlex1的误差
    }
    else if (self.collision.x + self.collision.w >= self.idleX2)
    {
        self.right = false;//转向
        if (self.collision.x + self.collision.w - self.idleX2 <= self.speed)
            self.collision.x -= self.collision.x + self.collision.w - self.idleX2;//补齐与idlex2的误差
    }
    //处理碰撞与踏空
    BeetleCollisionChecker(self, obstacles);
    BeetleFrontEmpty(self, obstacles);
    BeetleDrawIdle(self);
}
void BeetleDrawIdle(Beetle& self)
{
    if (self.right)drawImg(self.collision.x - 10, self.collision.y - 22, &self.runRight[self.runInd++]);
    else drawImg(self.collision.x - 10, self.collision.y - 22, &self.runLeft[self.runInd++]);
    if (self.runInd == 4)self.runInd = 0;
}
void BeetleRun(Beetle& self, int tagx, int tagy, const vector<Collision>& obstacles)
{
    int op = (tagx >= (self.collision.x + self.collision.w / 2)) ? 1 : -1;
    self.collision.x += self.speed * op;
    BeetleCollisionChecker(self, obstacles);//检查碰撞并消除误差
    BeetleFrontEmpty(self, obstacles);//检查前方非空并消除误差
    BeetleDrawIdle(self);
}
bool BeetleCollisionChecker(Beetle& self, const vector<Collision>& obstacles) {//碰撞检测和与玩家进行碰撞检测
    bool checker = false;
    for (const auto& ob : obstacles)
    {
        if (self.collision.x >= ob.x + ob.w || self.collision.x + self.collision.w <= ob.x
            || self.collision.y >= ob.y + ob.h || self.collision.y + self.collision.h <= ob.y)continue;
        checker = true;
        if (self.right)
        {
            int dif = self.collision.x + self.collision.w - ob.x;
            self.collision.x -= dif;
        }
        else
        {
            int dif = ob.x + ob.w - self.collision.x;
            self.collision.x += dif;
        }
    }

    return checker;
}
bool BeetleFrontEmpty(Beetle& self, const vector<Collision>& obstacles) {//前方非空，确保不会掉下平台
    //right：选右下角那个点   left：左下角
    int tmpx = self.collision.x + (self.right ? self.collision.w : 0);
    int tmpy = self.collision.y + self.collision.h;
    for (const auto& ob : obstacles)
    {
        if (tmpx >= ob.x && tmpx <= ob.x + ob.w && tmpy >= ob.y && tmpy <= ob.y + ob.h)//在某个障碍物内部
        {
            return true;
        }
    }
    self.collision.x += self.speed * (self.right ? -1 : 1);//复原
    return false;
}
bool BeetleAttackCheck(Beetle& self, double distance)
{
    if (distance <= self.atkRange)
    {
        self.canAtk = true;
        if (clock() - self.atkLast >= self.atkTime)
        {
            self.attacking = true;
            self.atkLast = clock();
        }
    }
    else self.canAtk = false;
    return self.attacking;
}
void BeetleAttack(Beetle& self)
{
    if (self.atkInd == 4)
    {
        drawImg(self.collision.x - 10, self.collision.y - 22, self.right ? &self.atkRight[3] : &self.atkLeft[3]);
        if (self.frame == self.frameMax)
        {
            self.attacking = false;
            self.frame = 0;
            self.atkInd = 0;
            double x1, y1, x2, y2;
            if (self.right)
            {
                x1 = self.collision.x + self.collision.w - 50;
                x2 = self.collision.x + self.collision.w + 50;
                y1 = self.collision.y;
                y2 = self.collision.y + self.collision.h;
            }
            else
            {
                x1 = self.collision.x - 50;
                x2 = self.collision.x + 50;
                y1 = self.collision.y;
                y2 = self.collision.y + self.collision.h;
            }
            //rectangle(x1, y1, x2, y2);
            //检测是否攻击到
            if (player.collision.x > x2 || player.collision.x + player.collision.w<x1
                || player.collision.y>y2 || player.collision.y + player.collision.h < y1) {
            }
            else //攻击到
            {
                player.hp -= self.atk;//伤害判定
            }
        }else
        {
            self.frame += 1;
        }
    }
    else
    {
        drawImg(self.collision.x - 10, self.collision.y - 22, self.right ? &self.atkRight[self.atkInd] : &self.atkLeft[self.atkInd]);
        if (self.frame == self.frameMax)
        {
            self.frame = 0;
            self.atkInd += 1;
        }
        else
        {
            self.frame += 1;
        }
    }
}
void BeetleDeath(Beetle& self) {
    if (self.deathInd == 5)
    {
        drawImg(self.collision.x - 10, self.collision.y - 22, self.right ? &self.dieRight[self.deathInd - 1] : &self.dieLeft[self.deathInd - 1]);
        if (self.frame == self.frameMax)
        {
            self.frame = 0;
            self.deathEnd = true;
            self.deathInd = 0;
            if(RoomId==2)
            {
                EnemyCounter2 -= 1;
            }
            else {
                EnemyCounter1 -= 1;
            }
        }
        else self.frame += 1;
    }
    else
    {
        drawImg(self.collision.x - 10, self.collision.y - 22, self.right ? &self.dieRight[self.deathInd] : &self.dieLeft[self.deathInd]);
        if (self.frame == self.frameMax)
        {
            self.frame = 0;
            self.deathInd += 1;
        }
        else self.frame += 1;
    }
}
void BeetleShowHp(Beetle& self)
{
    double rate = self.hp * 1.0 / self.maxHp;
    setfillcolor(RED);
    rectangle(self.collision.x, self.collision.y - 10, self.collision.x + self.collision.w, self.collision.y);
    fillrectangle(self.collision.x, self.collision.y - 10, self.collision.x + self.collision.w * rate, self.collision.y);
}
void BeetleAttackWait(Beetle& self)
{
    BeetleDrawIdle(self);
}

//----------------------------------------------------------------陷阱相关------------------------------------------------------------------------------------------------------
bool TrapCollisionCheckerPlayer(Trap& trap, Player& player)
{
    return (player.collision.x + player.collision.w >= trap.x && player.collision.x <= trap.x + trap.w && player.collision.y <= trap.y + trap.h && player.collision.y + player.collision.h >= trap.y);
}
void TrapDamagePlayer(Trap& trap, Player& player, int delta)
{
    if (TrapCollisionCheckerPlayer(trap, player))
    {
        if (trap.timer_player >= trap.interval)
        {
            player.hp -= trap.damage;
            trap.timer_player = 0;
        }
        trap.timer_player += delta;	//每次调用时加上一帧的时间
    }
    else
    {
        trap.timer_player = trap.interval;	//Player离开尖刺陷阱时，立刻使timer满足Player被伤害的条件
    }
}
void DrawTrap(vector<Trap>& traps)
{
    for (Trap& trap : traps)
    {
        putimage_alpha(trap.x, trap.y, &trap.img);
    }

}
void TrapBuildAndAffect(vector<Trap>& traps, Player& player, int delta)//文若改  新增函数引用所有trap相关函数
{
    for (Trap& trap : traps)
    {

        TrapDamagePlayer(trap, player, delta);

    }



}


//透明背景putimage函数
inline void putimage_alpha(int dst_x, int dst_y, IMAGE* img)
{
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(GetWorkingImage()), dst_x, dst_y, w, h,
        GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

//房间相关
void RoomInit() {
    isWin = false;
    isLose = false;

    RoomId = 1;
    loadskill_skill();

    // 清空之前的平台和敌人
    platforms.clear();
    platforms2.clear();
    Beetles.clear();
    Beetles2.clear();
    Novices.clear();
    Mosquitos.clear();
    Mosquitos2.clear();
    Novices2.clear();

    // 初始化房间1的平台
    platforms.push_back({ 0, 640, 1280, 80 });

    // 初始化房间1的敌人
    Beetles.resize(1);
    Mosquitos.resize(1);
    BeetleInit(Beetles[0], 1000, 640, 800, 1000);
    MosquitoInit(Mosquitos[0], 500, 400);
    EnemyCounter1 = Beetles.size() + Mosquitos.size()+Novices.size();

    //初始化房间1的陷阱  
    for (int i = 0; i < 2; i++)
    {
        Trap trap;
        trap.w = 83, trap.h = 58;
        trap.x = 600 + i * trap.w, trap.y = 640 - trap.h + 10;
        loadimage(&trap.img, _T("assets/trap.png"));
        Traps.push_back(trap);
    }

    // 初始化房间2的平台
    platforms2.push_back({ 400, 500, 150, 20 });
    platforms2.push_back({ 800, 500, 200, 20 });
    platforms2.push_back({ 200, 400, 200, 20 });
    platforms2.push_back({ 500, 250, 150, 20 });
    platforms2.push_back({ 850, 150, 150, 20 });
    platforms2.push_back({ 0, 640, 1280, 80 });

    // 初始化房间2的敌人
    Beetles2.resize(2);
    Mosquitos2.resize(3);
    BeetleInit(Beetles2[0], 800, 500, 800, 1000);
    BeetleInit(Beetles2[1], 200, 400, 200, 400);
    MosquitoInit(Mosquitos2[0], 500, 400);
    MosquitoInit(Mosquitos2[1], 600, 600);
    MosquitoInit(Mosquitos2[2], 250, 200);
    EnemyCounter2 = Beetles2.size() + Mosquitos2.size()+Novices2.size();

    // 初始化房间2的陷阱
    for (int i = 0; i < 1; i++)                                                   
    {
        Trap trap;
        trap.w = 83, trap.h = 58;
        trap.x = 500 + i * trap.w + 30, trap.y = 250 - trap.h + 10;
        loadimage(&trap.img, _T("assets/trap.png"));
        Traps2.push_back(trap);
    }

    for (int i = 0; i < 1; i++)
    {
        Trap trap;
        trap.w = 83, trap.h = 58;
        trap.x = 400 + i * trap.w + 50, trap.y = 500 - trap.h + 10;
        loadimage(&trap.img, _T("assets/trap.png"));
        Traps2.push_back(trap);
    }

    for (int i = 0; i < 5; i++)
    {
        Trap trap;
        trap.w = 83, trap.h = 58;
        trap.x = 200 + i * trap.w, trap.y = 640 - trap.h + 10;
        loadimage(&trap.img, _T("assets/trap.png"));
        Traps2.push_back(trap);
    }

    //初始化房间3
    platforms3.push_back({ 0, 640, 1280, 80 });

    Novices3.resize(1);
    NoviceInit(Novices3[0], 200, 200);
    EnemyCounter3 = Novices3.size();

    for (int i = 0; i < 2; i++)
    {
        Trap trap;
        trap.w = 83, trap.h = 58;
        trap.x = 600 + i * trap.w, trap.y = 640 - trap.h + 10;
        loadimage(&trap.img, _T("assets/trap.png"));
        Traps3.push_back(trap);
    }

    // 设置玩家初始位置（房间1）
    player.x = 1000;
    player.y = 300;
    player.collision.x = player.x;
    player.collision.y = player.y;
    player.hp = player.maxhp; // 重置血量
}
void LoadIMAGE() //文若
{
    loadimage(&img_bk_1, _T("assets/bk/1.jpg"));
    loadimage(&img_bk_2, _T("assets/bk/2.jpg"));
    loadimage(&img_bk_3, _T("assets/bk/3.jpg"));
    loadimage(&img_bk_4, _T("assets/bk/4.jpg"));
    loadimage(&img_bk_5, _T("assets/bk/5.jpg"));
    loadimage(&img_bk_6, _T("assets/bk/6.jpg"));
    loadimage(&LoseView, _T("assets/bk/Lose.png"));
    
    loadimage(&menuBackGround, _T("assets/bk/Menu.jpg"));

    loadimage(&img_player_idle_left, _T("assets/Idle/Idle/1.png"));
    loadimage(&img_player_idle_right, _T("assets/Idle/Idle/1_rotated.png"));

    atlas_player_move_left.load_from_file(_T("assets/R/R%d.png"), 8);
    atlas_player_move_right.load_from_file(_T("assets/R1/R%d_rotated.png"), 8);
    atlas_player_attack_left.load_from_file(_T("Slash/Slash/%d.png"), 15);
    flip_atlas(atlas_player_attack_left, atlas_player_attack_right);
    atlas_player_attack_effect_left.load_from_file(_T("Slash/SlashEffect/%d.png"), 4);
    flip_atlas(atlas_player_attack_effect_left, atlas_player_attack_effect_right);

    atlas_mosquito_idle_left.load_from_file(_T("assets/enemy0/idle/left-%d.png"), 5);
    atlas_mosquito_idle_right.load_from_file(_T("assets/enemy0/idle/right-%d.png"), 5);
    atlas_mosquito_move_left.load_from_file(_T("assets/enemy0/run/left-%d.png"), 5);
    atlas_mosquito_move_right.load_from_file(_T("assets/enemy0/run/right-%d.png"), 5);
    atlas_mosquito_attack_left.load_from_file(_T("assets/enemy0/attack/left-%d.png"), 4);
    atlas_mosquito_attack_right.load_from_file(_T("assets/enemy0/attack/right-%d.png"), 4);
    atlas_mosquito_death_left.load_from_file(_T("assets/enemy0/death/left-%d.png"), 3);
    atlas_mosquito_death_right.load_from_file(_T("assets/enemy0/death/right-%d.png"), 3);

    atlas_beetle_idle_left.load_from_file(_T("assets/enemy1/idle/left-%d.png"), 4);
    atlas_beetle_idle_right.load_from_file(_T("assets/enemy1/idle/right-%d.png"), 4);
    atlas_beetle_move_left.load_from_file(_T("assets/enemy1/run/left-%d.png"), 4);
    atlas_beetle_move_right.load_from_file(_T("assets/enemy1/run/right-%d.png"), 4);
    atlas_beetle_attack_left.load_from_file(_T("assets/enemy1/attack/left-%d.png"), 4);
    atlas_beetle_attack_right.load_from_file(_T("assets/enemy1/attack/right-%d.png"), 4);
    atlas_beetle_death_left.load_from_file(_T("assets/enemy1/death/left-%d.png"), 5);
    atlas_beetle_death_right.load_from_file(_T("assets/enemy1/death/right-%d.png"), 5);



}
//界面相关
int main() {
    initgraph(1280, 720);
    LoadIMAGE();
    AnimationBuild();
    
    menuView();//进入主菜单
}
void menuView() {
    
    InitMenu();//初始化主菜单

    BeginBatchDraw();
    while (1) {
        for (int i = 0; i < 4; i++) {
            //printf("%d：\n", i);
            Buttons[i].update();
        }
        if (Buttons[0].istrigger())
        {
            GameLoop();//游戏主循环
            InitMenu();
        }
        if (Buttons[1].istrigger())
        {
            SettingVeiw();//设置界面
            InitMenu();
        }
            
        if (Buttons[2].istrigger())
        {   
            IntroductionVeiw();//介绍界面
            InitMenu();
        }
        if (Buttons[3].istrigger())
           exit(0);
        FlushBatchDraw();
    }

    EndBatchDraw();
}

//介绍界面
void IntroductionVeiw() {
    InitIntroduction();
    return;
}
void InitIntroduction() {
    //初始化团队介绍界面
    cleardevice();

    IMAGE img;
    loadimage(&img, _T("./assets/团队介绍背景图片.png"));
    putimage(0, 0, &img);
    FlushBatchDraw();
    Sleep(3000);
    return;
    //BackButton.init();
}
//游戏界面
void DrawPlatform(const vector<Collision> platforms) {
    setfillcolor(BLACK);
    for (Collision platform : platforms) {
        solidrectangle((int)platform.x, (int)platform.y, (int)(platform.x + platform.w), (int)(platform.y + platform.h));
    }
}
void DrawRoom2() {
    setbkcolor(WHITE);
    //cleardevice();
    putimage(0,0,&img_bk_5);
    DrawPlatform(platforms2);
    DrawTrap(Traps2);
}
void EndChecker() {
    if (Novices3[0].hp <= 0) {
        isWin = true;
    }
    else if (player.hp <= 0) {
        isLose = true;
    }
}
void winView() {
    InitWinView();
    BeginBatchDraw();
    BackButton.init();
    while (1) {
        BackButton.update();

        FlushBatchDraw();
        if (BackButton.istrigger()) {
            InitMenu();
            EndBatchDraw();
            return;
        }
    }
}
void loseView() {
    InitLoseView();
    BeginBatchDraw();
    BackButton.init();
    while (1) {
        BackButton.update();

        FlushBatchDraw();
        if (BackButton.istrigger()) {
            InitMenu();
            EndBatchDraw();
            return;
        }
    }
}
void DrawRoom1() {
    setbkcolor(WHITE);
    //cleardevice();
    putimage(0, 0, &img_bk_5);
    DrawPlatform(platforms);
    DrawTrap(Traps);
}
void DrawRoom3() {
    setbkcolor(WHITE);
    //cleardevice();
    putimage(0, 0, &img_bk_5);
    DrawPlatform(platforms3);
    DrawTrap(Traps3);
}
void GameLoop() {
    RoomInit();
    BeginBatchDraw();
    PlayerInit();
    while (!isWin && !isLose) {
        int sta = clock();
        cleardevice();
        FinishChecker();
        EndChecker();

        if(RoomId==2)
        {
            DrawRoom2();
            for (auto& beetle : Beetles2) {
                BeetleBehaviour(beetle, platforms2);
            }
            for (auto& mosquito : Mosquitos2) {
                MosquitoBehaviour(mosquito, platforms2);
            }

            TrapBuildAndAffect(Traps2, player, 60);
        }
        else if (RoomId == 1) {
            DrawRoom1();
            for (auto& beetle : Beetles) {
                BeetleBehaviour(beetle, platforms);
            }
            for (auto& mosquito : Mosquitos) {
                MosquitoBehaviour(mosquito, platforms);
            }

            TrapBuildAndAffect(Traps, player, 60);
        }
        else if (RoomId == 3) {
            DrawRoom3();
            for (auto& novice : Novices3) {
                NoviceBehaviour(novice, platforms3);
            }
            
            TrapBuildAndAffect(Traps3, player, 60);
        }



        PlayerUpdate();


        if (isWin||isLose) break;
        FlushBatchDraw();
        int run = clock() - sta;
        if (FPS > run)Sleep(FPS - run);
    }
    EndBatchDraw();
    if (isWin)winView();
    else if (isLose)loseView();
}
void FinishChecker()
{
    if (RoomId == 1 && EnemyCounter1 <= 0) {
        RoomId = 2;
        player.x = 1000;  // 房间2的安全位置
        player.y = 450;  // 在平台上方
        player.collision.x = player.x;
        player.collision.y = player.y;
    }
    if (RoomId == 2 && EnemyCounter2 <= 0) {
        RoomId = 3;
        player.x = 200;  
        player.y = 450; 
        player.collision.x = player.x;
        player.collision.y = player.y;
    }
}
void InitLoseView() {
    // 设置红色背景
    //setbkcolor(RED);
    //cleardevice();
    putimage(0, 0, &LoseView);
    //// 设置文本样式
    //settextstyle(200, 0, _T("微软雅黑"));
    //settextcolor(BLACK);

    //// 计算文本位置
    //int textWidth = textwidth(_T("飞舞"));
    //int textHeight = textheight(_T("飞舞"));
    //int x = (1280 - textWidth) / 2;
    //int y = 200;

    //// 绘制失败文本
    //outtextxy(x, y, _T("飞舞"));

    // 按钮参数（与胜利界面保持一致）
    int btnWidth = 200;
    int btnHeight = 50;
    int btnY = 400;

}
void InitWinView() {
    // 设置背景颜色（浅绿色背景）
    setbkcolor(RGB(144, 238, 144));
    cleardevice();

    // 设置文本样式
    settextstyle(80, 0, _T("微软雅黑"));
    settextcolor(BLACK);

    // 计算文本位置
    int textWidth = textwidth(_T("YOU WIN"));
    int textHeight = textheight(_T("YOU WIN"));
    int x = (1280 - textWidth) / 2;
    int y = 200;

    // 绘制胜利文本
    outtextxy(x, y, _T("YOU WIN"));

    // 按钮参数
    int btnWidth = 200;
    int btnHeight = 50;
    int btnY = 400;

}
//设置界面
void InitMusic() {
    MCIERROR ret = mciSendString("open ./PVZ.mp3 alias HK", 0, NULL, NULL);
    if (ret != 0)
    {
        char err[100] = { 0 };
        mciGetErrorString(ret, err, sizeof(err));
        puts(err);
    }
    mciSendString("play HK repeat", 0, NULL, NULL);
}
void InitMenu(){
    InitMusic();
    // 设置深蓝色背景
    setbkcolor(RGB(30, 32, 40));
    cleardevice();


    putimage(0, 0, &menuBackGround);
    // 调整按钮位置到屏幕左半部分 (0-640)
    Buttons[0].x = 320 - 150, Buttons[0].y = 200, Buttons[0].w = 300, Buttons[0].h = 60;
    Buttons[1].x = 320 - 150, Buttons[1].y = 280, Buttons[1].w = 300, Buttons[1].h = 60;
    Buttons[2].x = 320 - 150, Buttons[2].y = 360, Buttons[2].w = 300, Buttons[2].h = 60;
    Buttons[3].x = 320 - 150, Buttons[3].y = 440, Buttons[3].w = 300, Buttons[3].h = 60;

    // 按钮文本保持不变
    Buttons[0].text = _T("开始游戏");
    Buttons[1].text = _T("游戏设置");
    Buttons[2].text = _T("游戏说明");
    Buttons[3].text = _T("退出游戏");

    ListenButton[0].x = 860, ListenButton[0].y = 290, ListenButton[0].w = 60, ListenButton[0].h = 50;
    ListenButton[1].x = 360, ListenButton[1].y = 290, ListenButton[1].w = 60, ListenButton[1].h = 50;

    BackButton.x = 50, BackButton.y = 600, BackButton.w = 120, BackButton.h = 50; BackButton.text = _T("返回");
    ListenButton[0].text = _T("+");       // 加号按钮
    ListenButton[1].text = _T("-");       // 减号按钮


    // 绘制标题 - 在左半屏幕居中
    settextstyle(60, 0, _T("微软雅黑"));  // 减小字体大小以适应左半屏幕
    settextcolor(WHITE);
    const TCHAR* title = _T("空洞骑士");

    int titleWidth = textwidth(title);

    outtextxy(320 - titleWidth / 2, 80, title);


    settextstyle(30, 0, _T("微软雅黑"));  // 减小按钮字体大小
    settextcolor(RGB(220, 220, 220));

    for (int i = 0; i < 4; i++) {
        Button& btn = Buttons[i];

        // 绘制发光边框
        setlinecolor(RGB(92, 132, 156));
        setlinestyle(PS_SOLID, 3);
        roundrect(btn.x - 5, btn.y - 5, btn.x + btn.w + 5, btn.y + btn.h + 5, 15, 15);

        // 绘制按钮主体
        setfillcolor(RGB(45, 49, 66));
        solidroundrect(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h, 10, 10);

        // 居中文字
        int textX = btn.x + (btn.w - textwidth(btn.text)) / 2;
        int textY = btn.y + (btn.h - textheight(btn.text)) / 2;
        outtextxy(textX, textY, btn.text);
    }

    // 绘制底部版权信息 - 放在左半屏幕底部
    settextstyle(18, 0, _T("微软雅黑"));
    settextcolor(RGB(150, 150, 150));
    outtextxy(320 - 100, 720 - 40, _T("Team Cherry"));
}
void SettingVeiw() {
    InitSetting();
    while (1) {
        BackButton.update();
        if (BackButton.istrigger())//如果点击返回，则退出该函数，重新进行主菜单的循环
        {
            InitMenu();
            return;
        }
        ListenerChecker();//判断音量键是否被触发
        FlushBatchDraw();
    }
}
void InitSetting() {
    DrawVolumeControl();
    //初始化设置界面
}
void DrawVolumeControl() {

    cleardevice();
    putimage(0, 0, &img_bk_6);
    BackButton.init();
    ListenButton[0].init();
    ListenButton[1].init();
    // 计算各元素位置，使用相对坐标
    int centerX = 640;
    int titleY = 100;
    int volumeBarY = 300;
    int buttonY = 290;
    int textY = 350;

    // 绘制标题
    settextcolor(WHITE);
    settextstyle(40, 0, _T("微软雅黑"));
    outtextxy(centerX - 150, titleY, _T("自定义音量控制"));

    // 绘制音量条背景
    setfillcolor(DARKGRAY);
    fillrectangle(centerX - 200, volumeBarY, centerX + 200, volumeBarY + 30);

    // 绘制当前音量
    setfillcolor(GREEN);
    int volumeWidth = (currentVolume * 400) / 1000;
    fillrectangle(centerX - 200, volumeBarY, centerX - 200 + volumeWidth, volumeBarY + 30);

    // 绘制音量值文本
    TCHAR volumeText[20];
    _stprintf_s(volumeText, _T("音量: %d%%"), currentVolume / 10);
    settextstyle(30, 0, _T("微软雅黑"));
    outtextxy(centerX - 60, textY, volumeText);

    // 绘制音量增减按钮
    setfillcolor(LIGHTGRAY);
    fillroundrect(centerX - 280, buttonY, centerX - 220, buttonY + 50, 10, 10);  // 减号按钮
    fillroundrect(centerX + 220, buttonY, centerX + 280, buttonY + 50, 10, 10);  // 加号按钮

    settextcolor(BLACK);
    settextstyle(40, 0, _T("微软雅黑"));
    outtextxy(centerX - 260, buttonY + 5, _T("-"));
    outtextxy(centerX + 235, buttonY + 5, _T("+"));
}
void ListenerChecker() {//判断音量键触发，也可以不加按钮，单纯改成按下+-调节会简单一些
    ListenButton[0].update();
    ListenButton[1].update();
    if (ListenButton[0].istrigger()) {
        if (currentVolume < 1000) {
            currentVolume += 100;

            DrawVolumeControl();
            // 修正音量设置命令
            char cmd[100];
            sprintf_s(cmd, "setaudio HK volume to %d", currentVolume);
            mciSendStringA(cmd, 0, 0, NULL);
        }
    }
    //调整逻辑
    else if (ListenButton[1].istrigger()) {
        if (currentVolume > 0) {
            currentVolume -= 100;
            DrawVolumeControl();
            // 修正音量设置命令
            char cmd[100];
            sprintf_s(cmd, "setaudio HK volume to %d", currentVolume);
            mciSendStringA(cmd, 0, 0, NULL);
            //调整逻辑
        }
    }
}