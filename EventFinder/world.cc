#include "world.h"
#include <algorithm>

namespace viagogo {
namespace eventfinder {

int World::next_event_id_ = 0;

World::World(const Coordinate<int>& top_left, const Coordinate<int>& bottom_right)
	: top_left_(top_left), bottom_right_(bottom_right) {
	width_ = bottom_right_.get_x() - top_left_.get_x() + 1;
	depth_ = top_left_.get_y() - bottom_right_.get_y() + 1;
	locations_.reserve(width_ * depth_);
	for (auto x = top_left.get_x(); x <= bottom_right.get_x(); x++) {
		for (auto y = bottom_right.get_y(); y <= top_left.get_y(); y++) {
			locations_.push_back(Location(Coordinate<int>(x, y)));
		}
	}
}

World::~World() { }

void World::add_event(std::vector<Ticket>& tickets, int x, int y) {
	auto location = get_location(x, y);
	auto event = new Event(next_event_id_++, tickets);
	location->set_event(event);
}

std::vector<Location*> World::find_events(Coordinate<int> location, int limit) {
	std::vector<Location*> closest_locations;
	closest_locations.reserve(limit);
	// Check boundaries and limit. If out of bounds return empty list
	if (limit <= 0 || location.get_x() < top_left_.get_x() || location.get_y() > top_left_.get_y() ||
		location.get_x() > bottom_right_.get_x() || location.get_y() < bottom_right_.get_y()) {
		return closest_locations;
	}
	// Check if there is something going on in users location
	auto scan_location = get_location(location.get_x(), location.get_y());
	if (scan_location->get_event() != nullptr) {
		closest_locations.push_back(scan_location);
		--limit;
	}
	//Scan boundaries
	auto top_left_x = location.get_x(), top_left_y = location.get_y(),
			bottom_right_x = location.get_x(), bottom_right_y = location.get_y();
	auto previous_max_distance = 1;
	while (limit != 0 || (top_left_x != top_left_.get_x() && top_left_y != top_left_.get_y() &&
		bottom_right_x != bottom_right_.get_x() && bottom_right_y != bottom_right_.get_y())) {
		top_left_x = (top_left_x - 1 < top_left_.get_x()) ? top_left_.get_x() : top_left_x - 1;
		top_left_y = (top_left_y + 1 > top_left_.get_y()) ? top_left_.get_y() : top_left_y + 1;
		bottom_right_x = (bottom_right_x + 1 > bottom_right_.get_x()) ? bottom_right_.get_x() : bottom_right_x + 1;
		bottom_right_y = (bottom_right_y - 1 < bottom_right_.get_y()) ? bottom_right_.get_y() : bottom_right_y - 1;
		Coordinate<int> top_left(top_left_x, top_left_y), bottom_right(bottom_right_x, bottom_right_y);
		auto max_distance = std::max(location.distance_to(top_left), location.distance_to(bottom_right));
		for (auto d = previous_max_distance; d < max_distance; d++) {
			for (auto i = 0; i < d + 1; i++) {
				auto x1 = location.get_x() - d + i;
				auto y1 = location.get_y() - i;
				if (x1 < top_left_x) x1 = top_left_x;
				if (y1 < bottom_right_y) y1 = bottom_right_y;
				scan_location = get_location(x1, y1);
				if (scan_location->get_event() != nullptr && limit != 0) {
					closest_locations.push_back(scan_location);
					--limit;
				}
				auto x2 = location.get_x() + d - i;
				auto y2 = location.get_y() + i;
				if (x2 < bottom_right_x) x2 = bottom_right_x;
				if (y2 > top_left_y) y2 = top_left_y;
				scan_location = get_location(x2, y2);
				if (scan_location->get_event() != nullptr && limit != 0) {
					closest_locations.push_back(scan_location);
					--limit;
				}
			}
			for (auto i = 1; i < d; i++) {
				auto x1 = location.get_x() - i;
				auto y1 = location.get_y() + d - i;
				if (x1 < top_left_x) x1 = top_left_x;
				if (y1 > top_left_y) y1 = top_left_y;
				scan_location = get_location(x1, y1);
				if (scan_location->get_event() != nullptr && limit != 0) {
					closest_locations.push_back(scan_location);
					--limit;
				}
				auto x2 = location.get_x() + d - i;
				auto y2 = location.get_y() - i;
				if (x2 > bottom_right_x) x2 = bottom_right_x;
				if (y2 < bottom_right_y) y2 = bottom_right_y;
				scan_location = get_location(x2, y2);
				if (scan_location->get_event() != nullptr && limit != 0) {
					closest_locations.push_back(scan_location);
					--limit;
				}
			}
		}

		//for (auto x = top_left_x; x <= bottom_right_x; x++) {
		//	if (x != location.get_x() || top_left_y != location.get_y()) {
		//		scan_location = get_location(x, top_left_y);
		//		if (scan_location->get_event() != nullptr && limit != 0) {
		//			closest_locations.push_back(scan_location);
		//			--limit;
		//		}
		//	}
		//	if (x != location.get_x() || bottom_right_y != location.get_y()) {
		//		scan_location = get_location(x, bottom_right_y);
		//		if (scan_location->get_event() != nullptr && limit != 0) {
		//			closest_locations.push_back(scan_location);
		//			--limit;
		//		}
		//	}
		//}
		//for (auto y = bottom_right_y + 1; y < top_left_y; y++) {
		//	if (y != location.get_x() || top_left_y != location.get_y()) {
		//		scan_location = get_location(top_left_x, y);
		//		if (scan_location->get_event() != nullptr && limit != 0) {
		//			closest_locations.push_back(scan_location);
		//			--limit;
		//		}
		//	}
		//	if (y != location.get_x() || bottom_right_y != location.get_y()) {
		//		scan_location = get_location(bottom_right_x, y);
		//		if (scan_location->get_event() != nullptr && limit != 0) {
		//			closest_locations.push_back(scan_location);
		//			--limit;
		//		}
		//	}
		//}
		previous_max_distance = max_distance;
	}
	return closest_locations;
}

Location* World::get_location(int x, int y) {
	// Map to internal storage representation
	x -= top_left_.get_x();
	y -= bottom_right_.get_y();
	return &locations_[x * width_ + y];
}

}
}
