#include "Quadrilateral.h"
#include <complex>
#include <memory>
#include "hittable/Hittable.h"
#include "hittable/HittableList.h"
#include "vec3.h"

Quadrilateral::Quadrilateral(const point3& _Q, const vec3& _u, const vec3& _v, std::shared_ptr<Material> _material)
    : Q(_Q), u(_u), v(_v), material(_material) {
  auto cross_u_v = cross(u, v);
  cache.n = unit_vector(cross_u_v);
  cache.D = dot(cache.n, Q);
  cache.w = cross_u_v / dot(cross_u_v, cross_u_v);
  cache.area = cross_u_v.length();
}

auto Quadrilateral::hit(const Ray& ray, Interval ray_interval) const -> std::optional<HitRecord> {

  // No hit if the ray is parallel to the plane.
  if (fabs(dot(cache.n, ray.direction)) < 1e-8)
    return std::nullopt;

  // Return false if the hit point parameter t is outside the ray interval.
  auto t = (cache.D - dot(cache.n, ray.origin)) / dot(cache.n, ray.direction);
  if (ray_interval.contains(t) == false)
    return std::nullopt;

  // Determine the hit point lies within the planar shape using its plane coordinates.
  auto p = ray.at(t);
  auto planar_hitpt_vector  = p - Q;
  auto a = dot(cache.w, cross(planar_hitpt_vector, v));
  auto b = dot(cache.w, cross(u, planar_hitpt_vector));

  static auto is_interior = []  (double a, double b) -> bool {
    if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
      return false;
    return true;
  };

  if (!is_interior(a, b))
    return std::nullopt;

  // Ray hits the 2D shape; set the hit record and return true.
  HitRecord hit_record;
  hit_record.p = p;
  hit_record.t = t;
  hit_record.u = a;
  hit_record.v = b;
  hit_record.material = material;
  hit_record.set_face_normal(ray, cache.n); 
  return hit_record;
}

auto Quadrilateral::bounding_box() const -> AABB {
  return AABB(Q, Q + u + v).pad();
}


auto Quadrilateral::make_important_pdf(const point3& origin) const -> std::shared_ptr<PDF> {
  return std::make_shared<QuadrilateralPDF>(origin, *this);
}


auto make_box(const point3& a, const point3& b, std::shared_ptr<Material> material) -> std::shared_ptr<HittableListWithPDF> {
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = std::make_shared<HittableListWithPDF>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->add(std::make_shared<Quadrilateral>(point3(min.x(), min.y(), max.z()),  dx,  dy, material)); // front
    sides->add(std::make_shared<Quadrilateral>(point3(max.x(), min.y(), max.z()), -dz,  dy, material)); // right
    sides->add(std::make_shared<Quadrilateral>(point3(max.x(), min.y(), min.z()), -dx,  dy, material)); // back
    sides->add(std::make_shared<Quadrilateral>(point3(min.x(), min.y(), min.z()),  dz,  dy, material)); // left
    sides->add(std::make_shared<Quadrilateral>(point3(min.x(), max.y(), max.z()),  dx, -dz, material)); // top
    sides->add(std::make_shared<Quadrilateral>(point3(min.x(), min.y(), min.z()),  dx,  dz, material)); // bottom

    return sides;  
}