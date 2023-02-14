#define _USE_MATH_DEFINES

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <format>

#include "../src/collision_detector.h"

using namespace std::literals;
using namespace collision_detector;

namespace Catch {
    template<>
    struct StringMaker<collision_detector::GatheringEvent> {
        static std::string convert(collision_detector::GatheringEvent const& value) {
            return std::format("({},{},{},{})",
                value.gatherer_id, value.item_id, value.sq_distance, value.time);
        }
    };
}  // namespace Catch 

// Напишите здесь тесты для функции collision_detector::FindGatherEvents

template <typename Range>
struct IsPermutationMatcher : Catch::Matchers::MatcherGenericBase {
    IsPermutationMatcher(Range range)
        : range_{ std::move(range) } {
        std::sort(std::begin(range_), std::end(range_));
    }
    IsPermutationMatcher(IsPermutationMatcher&&) = default;

    template <typename OtherRange>
    bool match(OtherRange other) const {
        using std::begin;
        using std::end;

        std::sort(begin(other), end(other));
        return std::equal(begin(range_), end(range_), begin(other), end(other));
    }

    std::string describe() const override {
        // Описание свойства, проверяемого матчером:
        return "Is permutation of: " + Catch::rangeToString(range_);
    }

private:
    Range range_;
};

template<typename Range>
IsPermutationMatcher<Range> IsPermutation(Range&& range) {
    return IsPermutationMatcher<Range>{std::forward<Range>(range)};
}

class ItemGathererProviderReal : public ItemGathererProvider {
public:
    void SetItems(const std::vector<Item>& _items) { items_ = _items; }
    void SetGatherers(const std::vector<Gatherer>& _gatherers) { gatherers_ = _gatherers; }
    size_t ItemsCount() const override { return items_.size(); }
    Item GetItem(size_t idx) const override { return items_[idx]; }
    size_t GatherersCount() const override { return gatherers_.size(); }
    Gatherer GetGatherer(size_t idx) const override { return gatherers_[idx]; }
private:
    std::vector<Item> items_;
    std::vector<Gatherer> gatherers_;
};

SCENARIO("Event registration") {
    GIVEN("Items and one gatherer") {
        ItemGathererProviderReal provider;
        std::vector<Item> items;
        for (int i = 1; i <= 3; i++)
        {
            Item item;
            item.position = geom::Point2D(i, i);
            item.width = 1;
            items.push_back(item);
        }
        provider.SetItems(items);
        
        WHEN("All items on trajectory") {
            std::vector<Gatherer> gatherers;
            Gatherer gatherer;
            gatherer.start_pos = {0, 0};
            gatherer.end_pos = {5, 5};
            gatherer.width = 1;
            gatherers.push_back(gatherer);
            provider.SetGatherers(gatherers);
            THEN("All items gathered")
            {
                auto events = FindGatherEvents(provider);
                CHECK(events.size() == 3);                
            }
        }

        WHEN("Some items on trajectory") {
            std::vector<Gatherer> gatherers;
            Gatherer gatherer;
            gatherer.start_pos = { 0, 0 };
            gatherer.end_pos = { 2, 4 };
            gatherer.width = 1;
            gatherers.push_back(gatherer);
            provider.SetGatherers(gatherers);
            THEN("Some items gathered")
            {
                auto events = FindGatherEvents(provider);
                CHECK(events.size() == 2);
            }
        }

        WHEN("No items on trajectory") {
            std::vector<Gatherer> gatherers;
            Gatherer gatherer;
            gatherer.start_pos = { 0, 0 };
            gatherer.end_pos = { -5, -5 };
            gatherer.width = 1;
            gatherers.push_back(gatherer);
            provider.SetGatherers(gatherers);
            THEN("No items gathered")
            {
                auto events = FindGatherEvents(provider);
                CHECK(events.size() == 0);
            }
        }
    }

    GIVEN("Three items and two gatherers") {
        ItemGathererProviderReal provider;
        std::vector<Item> items;

        Item item1;
        item1.position = geom::Point2D(1.5 , 3.5);
        item1.width = 1;
        items.push_back(item1);

        Item item2;
        item2.position = geom::Point2D(2.5, 1);
        item2.width = 1;
        items.push_back(item2);

        Item item3;
        item3.position = geom::Point2D(4.5, 3);
        item3.width = 1;
        items.push_back(item3);

        provider.SetItems(items);

        WHEN("First gatherer passes items one and two, second gatherer passes item one") {
            std::vector<Gatherer> gatherers;

            Gatherer gatherer1;
            gatherer1.start_pos = { 3, 0 };
            gatherer1.end_pos = { 1, 5.5 };
            gatherer1.width = 1;
            gatherers.push_back(gatherer1);

            Gatherer gatherer2;
            gatherer2.start_pos = { 3.5, 5 };
            gatherer2.end_pos = { 0, 3 };
            gatherer2.width = 1;
            gatherers.push_back(gatherer2);

            provider.SetGatherers(gatherers);

            THEN("First gatherer gets 2 items, second gets 1 item")
            {
                auto events = FindGatherEvents(provider);
                CHECK(events.size() == 3);
                CHECK(events[0].gatherer_id == 0);
                CHECK(events[0].item_id == 1);
                CHECK(events[1].gatherer_id == 1);
                CHECK(events[1].item_id == 0);
                CHECK(events[2].gatherer_id == 0);
                CHECK(events[2].item_id == 0);
            }
        }
        
    }
}
