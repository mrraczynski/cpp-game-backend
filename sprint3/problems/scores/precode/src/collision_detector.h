#pragma once

#include "geom.h"

#include <algorithm>
#include <vector>
#include <optional>

namespace collision_detector {

    struct CollectionResult {
        bool IsCollected(double collect_radius) const {
            return proj_ratio >= 0 && proj_ratio <= 1 && sq_distance <= collect_radius * collect_radius;
        }

        // квадрат расстояния до точки
        double sq_distance;

        // доля пройденного отрезка
        double proj_ratio;
    };

    // Движемся из точки a в точку b и пытаемся подобрать точку c.
    // Эта функция реализована в уроке.
    std::optional<CollectionResult> TryCollectPoint(geom::Point2D a, geom::Point2D b, geom::Point2D c);

    struct Item {
        geom::Point2D position;
        double width;
        size_t item_id;
    };

    struct Gatherer {
        geom::Point2D start_pos;
        geom::Point2D end_pos;
        double width;
        size_t gatherer_id;
    };

    class ItemGathererProvider {
    public:
        ItemGathererProvider(const std::vector<Item>& items, const std::vector<Gatherer>& gatherers)
            : items_(items)
            , gatherers_(gatherers) {}
        size_t ItemsCount() const { return items_.size(); }
        Item GetItem(size_t idx) const { return items_[idx]; }
        size_t GatherersCount() const { return gatherers_.size(); }
        Gatherer GetGatherer(size_t idx) const { return gatherers_[idx]; }
    private:
        const std::vector<Item>& items_;
        const std::vector<Gatherer>& gatherers_;
    };

    struct GatheringEvent {
        size_t item_id;
        size_t gatherer_id;
        double sq_distance;
        double time;
    };

    // Эту функцию вам нужно будет реализовать в соответствующем задании.
    // При проверке ваших тестов она не нужна - функция будет линковаться снаружи.
    std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider);

}  // namespace collision_detector