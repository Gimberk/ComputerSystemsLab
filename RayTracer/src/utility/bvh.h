#pragma once

#include <vector>

#include "../hittables/primitives/triangle.h"
#include "../hittables/aabb.h"

class bvh {
public:
	explicit bvh(const std::vector<std::shared_ptr<triangle>>* triangles);

	const hit_record intersect(const ray& r) const;

private:
	struct node {
		aabb bounds;

		int left = -1, right = -1;
		int start = 0, count = 0;

		node() {
			constexpr double inf = std::numeric_limits<double>::infinity();
			bounds = aabb(point3(inf, inf, inf), point3(-inf, -inf, -inf));
		}

		bool is_leaf() const { return count > 0; }
	};

	std::vector<node> nodes;
	std::vector<int> indices;

	const std::vector<std::shared_ptr<triangle>>* tris;

	int build(int start, int end);

	const hit_record intersect_node(const int index, const ray& r) const;
};