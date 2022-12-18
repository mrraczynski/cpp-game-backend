#include "collision_detector.h"
#include <cassert>

namespace collision_detector {

CollectionResult TryCollectPoint(geom::Point2D a, geom::Point2D b, geom::Point2D c) {
    // Проверим, что перемещение ненулевое.
    // Тут приходится использовать строгое равенство, а не приближённое,
    // пскольку при сборе заказов придётся учитывать перемещение даже на небольшое
    // расстояние.
    assert(b.x != a.x || b.y != a.y);
    const double u_x = c.x - a.x;
    const double u_y = c.y - a.y;
    const double v_x = b.x - a.x;
    const double v_y = b.y - a.y;
    const double u_dot_v = u_x * v_x + u_y * v_y;
    const double u_len2 = u_x * u_x + u_y * u_y;
    const double v_len2 = v_x * v_x + v_y * v_y;
    const double proj_ratio = u_dot_v / v_len2;
    const double sq_distance = u_len2 - (u_dot_v * u_dot_v) / v_len2;

    return CollectionResult(sq_distance, proj_ratio);
}

// В задании на разработку тестов реализовывать следующую функцию не нужно -
// она будет линковаться извне.

std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider) {
    std::vector<GatheringEvent> result;
    const auto items_count = provider.ItemsCount();
    const auto gatherers_count = provider.GatherersCount();

    for (size_t gatherer_idx = 0; gatherer_idx < gatherers_count; ++gatherer_idx) {
        const auto gatherer = provider.GetGatherer(gatherer_idx);

        for (size_t item_idx = 0; item_idx < items_count; ++item_idx) {
            const auto item = provider.GetItem(item_idx);
            const auto collection_result = TryCollectPoint(gatherer.start_pos, gatherer.end_pos, item.position);

            if (collection_result.IsCollected(gatherer.width)) {
                GatheringEvent event{ item_idx, 
                    gatherer_idx, 
                    collection_result.sq_distance, 
                    collection_result.proj_ratio 
                };
                result.push_back(event);
            }
        }
    }

    std::sort(std::begin(result), std::end(result), [](const auto& T1, const auto& T2) {
        return T1.time < T2.time;
        });

    return result;
}


}  // namespace collision_detector