#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "AABB.h"
#include "Hittable.h"
#include "vec3.h"

class HittableList : virtual public Hittable {
 public:
  class BVHNode;

 public:
  HittableList() {}
  HittableList(std::shared_ptr<Hittable> _object) { add(_object); }

 public:
  auto size() const -> size_t { return objects.size(); }
  auto empty() const -> bool { return objects.size() == 0; }
  auto add(std::shared_ptr<Hittable> object) -> void {
    objects.push_back(object);
    if (aabb.has_value()) {
      aabb.emplace(AABB(aabb.value(), object->bounding_box()));
    } else {
      aabb.emplace(object->bounding_box());
    }
  }
  auto hit(const Ray& ray, Interval ray_interval) const -> std::optional<HitRecord> override {
    std::optional<HitRecord> res_final = std::nullopt;
    double closest_so_far = ray_interval.max;
    for (const auto& object : objects) {
      auto res_temp = object->hit(ray, Interval(ray_interval.min, closest_so_far));
      if (res_temp.has_value()) {
        res_final.emplace(res_temp.value());
        closest_so_far = res_temp->t;
      }
    }
    return res_final;
  }
  auto bounding_box() const -> AABB override {
    return aabb.value();
  }

 protected:
  std::vector<std::shared_ptr<Hittable>> objects;
  std::optional<AABB> aabb;
};

class HittableListWithPDF : public HittableList, public HittableWithPDF {
 public:
  class HittableListPDF;

 public:
  auto add(std::shared_ptr<HittableWithPDF> object) -> void {
    HittableList::add(object);
    objects.push_back(object);
  }
  virtual auto hit(const Ray& ray, Interval ray_interval) const -> std::optional<HitRecord> override {
    return HittableList::hit(ray, ray_interval);
  }
  virtual auto bounding_box() const -> AABB override {
    return HittableList::bounding_box();
  }
  auto make_important_pdf(const point3& origin) const -> std::shared_ptr<PDF> override;

 private:
  std::vector<std::shared_ptr<HittableWithPDF>> objects;
};

class HittableListWithPDF::HittableListPDF : public PDF {
 public:
  HittableListPDF(const vec3& _origin, const HittableListWithPDF& _hittablelistwithpdf)
      : origin(_origin), hittablelistwithpdf(_hittablelistwithpdf) {}
  auto value(const vec3& direction) const -> double override {
    double weight = 1.0 / hittablelistwithpdf.objects.size();
    double sum = 0.0;
    for (const auto& object : hittablelistwithpdf.objects)
      sum += weight * object->make_important_pdf(origin)->value(direction);
    return sum;
  }
  auto generate() const -> vec3 override {
    int size = static_cast<int>(hittablelistwithpdf.objects.size());
    return hittablelistwithpdf.objects[random_int(0, size - 1)]->make_important_pdf(origin)->generate();
  }

 private:
  const vec3& origin;
  const HittableListWithPDF& hittablelistwithpdf;
};

class HittableList::BVHNode : public Hittable {
 public:
  BVHNode(const HittableList& _hittablelist)
      : BVHNode(_hittablelist.objects, 0, _hittablelist.objects.size()) {}
  BVHNode(const std::vector<std::shared_ptr<Hittable>>& _objects, size_t begin, size_t end) {
    auto objects = _objects;
    int axis = random_int(0, 2);

    auto comparator = std::bind(box_compare, std::placeholders::_1, std::placeholders::_2, axis);
    auto object_span = end - begin;
    if (object_span == 1) {
      left = right = objects[begin];
    } else if (object_span == 2) {
      if (comparator(objects[begin], objects[begin + 1])) {
        left = objects[begin];
        right = objects[begin + 1];
      } else {
        left = objects[begin + 1];
        right = objects[begin];
      }
    } else {
      std::sort(objects.begin() + begin, objects.begin() + end, comparator);
      auto mid = begin + object_span / 2;
      left = std::make_shared<BVHNode>(objects, begin, mid);
      right = std::make_shared<BVHNode>(objects, mid, end);
    }
    aabb = AABB(left->bounding_box(), right->bounding_box());
  }

 public:
  auto hit(const Ray& ray, Interval ray_interval) const -> std::optional<HitRecord> {
    if (!aabb.hit(ray, ray_interval))
      return std::nullopt;
    auto hit_record_left = left->hit(ray, ray_interval);
    auto hit_record_right = right->hit(ray, ray_interval);
    if (hit_record_left.has_value() && hit_record_right.has_value())
      return (hit_record_left->t < hit_record_right->t ? hit_record_left : hit_record_right);
    else if (hit_record_left.has_value())
      return hit_record_left;
    else if (hit_record_right.has_value())
      return hit_record_right;
    else
      return std::nullopt;
  }
  AABB bounding_box() const { return aabb; }

 private:
  static bool box_compare(const std::shared_ptr<Hittable>& object1, const std::shared_ptr<Hittable>& object2, int axis_index) {
    return object1->bounding_box().axis(axis_index).min < object2->bounding_box().axis(axis_index).min;
  }

 private:
  std::shared_ptr<Hittable> left;
  std::shared_ptr<Hittable> right;
  AABB aabb;
};
