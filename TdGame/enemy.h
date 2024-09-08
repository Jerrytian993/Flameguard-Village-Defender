#ifndef _ENEMY_H_   // 如果未定义_ENEMY_H_，则定义之
#define _ENEMY_H_

// 引入需要的头文件
#include "timer.h"       // 包含定时器类
#include "route.h"       // 包含路线类
#include "vector2.h"     // 包含二维向量类
#include "animation.h"   // 包含动画类
#include "config_manager.h"  // 包含配置管理类

#include <functional>    // 包含C++标准函数库中的功能

// 敌人类定义
class Enemy
{
public:
    typedef std::function<void(Enemy* enemy)> SkillCallback;  // 定义一个函数类型，用于技能释放的回调

public:
    // 构造函数
    Enemy()
    {
        // 设置技能释放的定时器，不是单次触发
        timer_skill.set_one_shot(false);
        // 设置技能释放时的回调函数
        timer_skill.set_on_timeout([&]() { on_skill_released(this); });

        // 设置"草图"显示的定时器，单次触发
        timer_sketch.set_one_shot(true);
        // 设置"草图"显示的持续时间
        timer_sketch.set_wait_time(0.075);
        // 设置"草图"结束时的回调函数，结束后不再显示"草图"
        timer_sketch.set_on_timeout([&]() { is_show_sketch = false; });

        // 设置速度恢复的定时器，单次触发
        timer_restore_speed.set_one_shot(true);
        // 设置速度恢复时的回调函数，恢复到最大速度
        timer_restore_speed.set_on_timeout([&]() { speed = max_speed; });
    }

    ~Enemy() = default;   // 默认析构函数

    // 每帧更新敌人状态
    void on_update(double delta)
    {
        // 更新所有定时器
        timer_skill.on_update(delta);
        timer_sketch.on_update(delta);
        timer_restore_speed.on_update(delta);

        // 计算移动距离
        Vector2 move_distance = velocity * delta;
        Vector2 target_distance = position_target - position;
        position += move_distance < target_distance ? move_distance : target_distance;

        // 如果到达目标点
        if (target_distance.approx_zero())
        {
            idx_target++;  // 目标索引递增
            refresh_position_target();  // 刷新下一个目标位置

            // 更新方向
            direction = (position_target - position).normalize();
        }

        // 根据方向和速度更新速度向量
        velocity.x = direction.x * speed * SIZE_TILE;
        velocity.y = direction.y * speed * SIZE_TILE;

        // 根据速度向量的水平和垂直速度决定显示哪个动画
        bool is_show_x_anim = abs(velocity.x) >= abs(velocity.y);
        if (is_show_sketch)
        {
            if (is_show_x_anim)
                anim_current = velocity.x > 0 ? &anim_right_sketch : &anim_left_sketch;
            else
                anim_current = velocity.y > 0 ? &anim_down_sketch : &anim_up_sketch;
        }
        else
        {
            if (is_show_x_anim)
                anim_current = velocity.x > 0 ? &anim_right : &anim_left;
            else
                anim_current = velocity.y > 0 ? &anim_down : &anim_up;
        }

        // 更新当前动画帧
        anim_current->on_update(delta);
    }

    // 渲染敌人到屏幕
    void on_render(SDL_Renderer* renderer)
    {
        static SDL_Rect rect;
        static SDL_Point point;
        static const int offset_y = 2;
        static const Vector2 size_hp_bar = { 40, 8 };
        static const SDL_Color color_border = { 116, 185, 124, 255 };
        static const SDL_Color color_content = { 226, 255, 194, 255 };

        // 设置渲染位置
        point.x = (int)(position.x - size.x / 2);
        point.y = (int)(position.y - size.y / 2);

        // 渲染当前动画
        anim_current->on_render(renderer, point);

        // 如果敌人受伤，则显示血条
        if (hp < max_hp)
        {
            rect.x = (int)(position.x - size_hp_bar.x / 2);
            rect.y = (int)(position.y - size.y / 2 - size_hp_bar.y - offset_y);
            rect.w = (int)(size_hp_bar.x * (hp / max_hp));
            rect.h = (int)size_hp_bar.y;
            SDL_SetRenderDrawColor(renderer, color_content.r, color_content.g, color_content.b, color_content.a);
            SDL_RenderFillRect(renderer, &rect);

            rect.w = (int)size_hp_bar.x;
            SDL_SetRenderDrawColor(renderer, color_border.r, color_border.g, color_border.b, color_border.a);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }

    // 其他成员函数的实现省略，包括设置技能回调、处理血量变化、调整速度等功能...

protected:
    Vector2 size;  // 敌人的尺寸

    Timer timer_skill;  // 技能释放的定时器

    Animation anim_up;  // 向上移动的动画
    Animation anim_down;  // 向下移动的动画
    Animation anim_left;  // 向左移动的动画
    Animation anim_right;  // 向右移动的动画
    Animation anim_up_sketch;  // 向上移动的"草图"动画
    Animation anim_down_sketch;  // 向下移动的"草图"动画
    Animation anim_left_sketch;  // 向左移动的"草图"动画
    Animation anim_right_sketch;  // 向右移动的"草图"动画

    double hp = 0;  // 当前血量
    double max_hp = 0;  // 最大血量
    double speed = 0;  // 当前速度
    double max_speed = 0;  // 最大速度
    double damage = 0;  // 造成的伤害
    double reward_ratio = 0;  // 奖励系数
    double recover_interval = 0;  // 恢复间隔
    double recover_range = 0;  // 恢复范围
    double recover_intensity = 0;  // 恢复强度

private:
    Vector2 position;  // 当前位置
    Vector2 velocity;  // 当前速度向量
    Vector2 direction;  // 当前移动方向

    bool is_valid = true;  // 是否有效

    Timer timer_sketch;  // "草图"显示的定时器
    bool is_show_sketch = false;  // 是否显示"草图"

    Animation* anim_current = nullptr;  // 当前显示的动画

    SkillCallback on_skill_released;  // 技能释放的回调函数

    Timer timer_restore_speed;  // 速度恢复的定时器

    const Route* route = nullptr;  // 路径
    int idx_target = 0;  // 当前路径的目标索引
    Vector2 position_target;  // 下一个目标位置

private:
    // 根据路线刷新目标位置
    void refresh_position_target()
    {
        const Route::IdxList& idx_list = route->get_idx_list();

        if (idx_target < idx_list.size())
        {
            const SDL_Point& point = idx_list[idx_target];
            static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;

            position_target.x = rect_tile_map.x + point.x * SIZE_TILE + SIZE_TILE / 2;
            position_target.y = rect_tile_map.y + point.y * SIZE_TILE + SIZE_TILE / 2;
        }
    }

};

#endif // !_ENEMY_H_

