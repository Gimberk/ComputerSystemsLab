#include "bvh.h"

#include <algorithm>
#include <numeric>

int bvh::build(const int start, const int end) {
	const int node_index = static_cast<int>(nodes.size());
	nodes.emplace_back();

	aabb bounds;
	aabb centroid_bounds;

	for (int i = 0; i < end; i++) {
		const triangle& tri = *(*tris)[indices[i]];

		bounds.expand(tri.get_bounds());
		centroid_bounds.expand(tri.get_centroid());
	}

	const int count = end - start;

	if (count <= 4) {
		nodes[node_index].bounds = bounds;
		nodes[node_index].start = start;
		nodes[node_index].count = count;

		return node_index;
	}

	const int axis = centroid_bounds.longest_axis();
	const int mid = start + count / 2;

	std::nth_element(
		indices.begin() + start,
		indices.begin() + mid,
		indices.begin() + end,
		[&](int a, int b) {
			return (*tris)[a]->get_centroid()[axis] < (*tris)[b]->get_centroid()[axis];
		}
	);

	const int left = build(start, mid), right = build(mid, end);

	nodes[node_index].bounds = bounds;
	nodes[node_index].left = left;
	nodes[node_index].right = right;

	return node_index;
}

bvh::bvh(const std::vector<std::shared_ptr<triangle>>* triangles) : tris(triangles) {
	indices.resize(tris->size());
	std::iota(indices.begin(), indices.end(), 0);

	if (!indices.empty()) build(0, static_cast<int>(indices.size()));
}

const hit_record bvh::intersect(const ray& r) const {
	if (nodes.empty()) return hit_record();

	return intersect_node(0, r);
}

const hit_record bvh::intersect_node(const int index, const ray& r) const {
	const node& n = nodes[index];

	if (!n.bounds.intersect(r)) return hit_record();

	if (n.is_leaf()) {
		primitive* closest = nullptr;
		double closest_t = std::numeric_limits<double>::infinity();

		for (int i = n.start; i < n.start + n.count; i++) {
			const hit_record hit = (*tris)[indices[i]]->intersect(r);

			if (hit.valid && hit.t > 0.001 && hit.t < closest_t) {
				closest_t = hit.t;
				closest = (*tris)[indices[i]].get();
			}
		}

		return hit_record(closest_t, closest);
	}

	const hit_record a = intersect_node(n.left, r), b = intersect_node(n.right, r);

	if (!a.valid) return b;
	if (!b.valid) return a;

	return a.t < b.t ? a : b;
}