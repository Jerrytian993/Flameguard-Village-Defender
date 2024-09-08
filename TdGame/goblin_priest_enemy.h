#ifndef _GOBLIN_PRIEST_ENEMY_H_   // 防止头文件被重复包含
#define _GOBLIN_PRIEST_ENEMY_H_

#include "enemy.h"                // 包含敌人基类
#include "config_manager.h"       // 包含配置管理器
#include "resources_manager.h"    // 包含资源管理器

// 哥布林牧师敌人类定义，继承自Enemy类
class GoblinPriestEnemy : public Enemy
{
public:
    // 构造函数
    GoblinPriestEnemy()
    {
        // 获取资源管理器中的纹理资源池，并从中获取哥布林牧师的相关纹理
        static const ResourcesManager::TexturePool& texture_pool = ResourcesManager::instance()->get_texture_pool();
        static SDL_Texture* tex_goblin_priest = texture_pool.find(ResID::Tex_GoblinPriest)->second;
        static SDL_Texture* tex_goblin_priest_sketch = texture_pool.find(ResID::Tex_GoblinPriestSketch)->second;

        // 从配置管理器中获取哥布林牧师的属性模板
        static ConfigManager::EnemyTemplate& goblin_priest_template = ConfigManager::instance()->goblin_priest_template;

        // 定义动画使用的帧索引列表
        static const std::vector<int> idx_list_up = { 5, 6, 7, 8, 9 };
        static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4 };
        static const std::vector<int> idx_list_left = { 15, 16, 17, 18, 19 };
        static const std::vector<int> idx_list_right = { 10, 11, 12, 13, 14 };

        // 设置各个方向的动画属性，包括循环播放、间隔时间和帧数据
        anim_up.set_loop(true); anim_up.set_interval(0.15);
        anim_up.set_frame_data(tex_goblin_priest, 5, 4, idx_list_up);
        anim_down.set_loop(true); anim_down.set_interval(0.15);
        anim_down.set_frame_data(tex_goblin_priest, 5, 4, idx_list_down);
        anim_left.set_loop(true); anim_left.set_interval(0.15);
        anim_left.set_frame_data(tex_goblin_priest, 5, 4, idx_list_left);
        anim_right.set_loop(true); anim_right.set_interval(0.15);
        anim_right.set_frame_data(tex_goblin_priest, 5, 4, idx_list_right);

        // 设置"草图"状态下的动画属性
        anim_up_sketch.set_loop(true); anim_up_sketch.set_interval(0.15);
        anim_up_sketch.set_frame_data(tex_goblin_priest_sketch, 5, 4, idx_list_up);
        anim_down_sketch.set_loop(true); anim_down_sketch.set_interval(0.15);
        anim_down_sketch.set_frame_data(tex_goblin_priest_sketch, 5, 4, idx_list_down);
        anim_left_sketch.set_loop(true); anim_left_sketch.set_interval(0.15);
        anim_left_sketch.set_frame_data(tex_goblin_priest_sketch, 5, 4, idx_list_left);
        anim_right_sketch.set_loop(true); anim_right_sketch.set_interval(0.15);
        anim_right_sketch.set_frame_data(tex_goblin_priest_sketch, 5, 4, idx_list_right);

        // 初始化哥布林牧师的各项属性值
        max_hp = goblin_priest_template.hp;
        max_speed = goblin_priest_template.speed;
        damage = goblin_priest_template.damage;
        reward_ratio = goblin_priest_template.reward_ratio;
        recover_interval = goblin_priest_template.recover_interval;
        recover_range = goblin_priest_template.recover_range;
        recover_intensity = goblin_priest_template.recover_intensity;

        // 设置敌人的尺寸和初始化生命值与速度
        size.x = 48, size.y = 48;
        hp = max_hp, speed = max_speed;

        // 设置技能恢复的定时器
        timer_skill.set_wait_time(recover_interval);
    }

    ~GoblinPriestEnemy() = default;  // 默认析构函数
};

#endif // !_GOBLIN_PRIEST_ENEMY_H_

