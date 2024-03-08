#include "test.h"

#include <iostream>
#include "Camera.h"
#include "hittable/HittableList.h"
#include "hittable/Quadrilateral.h"
#include "hittable/RotateY.h"
#include "hittable/Sphere.h"
#include "hittable/Translate.h"
#include "material/Dielectric.h"
#include "material/DiffuseLight.h"
#include "material/Lambertian.h"
#include "material/Metal.h"


void test_rt_book_3_final() {
  HittableList world;

  // Cornell box sides
  auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
  auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
  auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));

  world.add(std::make_shared<Quadrilateral>(point3(555, 0, 0), vec3(0, 0, 555), vec3(0, 555, 0), green));
  world.add(std::make_shared<Quadrilateral>(point3(0, 0, 0), vec3(0, 0, 555), vec3(0, 555, 0), red));
  world.add(std::make_shared<Quadrilateral>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
  world.add(std::make_shared<Quadrilateral>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 0, -555), white));
  world.add(std::make_shared<Quadrilateral>(point3(555, 0, 555), vec3(-555, 0, 0), vec3(0, 555, 0), white));

  // Light
  auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));
  world.add(std::make_shared<Quadrilateral>(point3(213, 554, 227), vec3(130, 0, 0), vec3(0, 0, 105), light));

  // Box 1
  std::shared_ptr<Material> aluminum = std::make_shared<Metal>(Color(0.8, 0.85, 0.88), 0.0);
  std::shared_ptr<HittableWithPDF> box1_1 = make_box(point3(0, 0, 0), point3(165, 330, 165), white);
  std::shared_ptr<Hittable> box1_2 = box1_1;
  box1_2 = std::make_shared<RotateY>(box1_2, 15);
  box1_2 = std::make_shared<Translate>(box1_2, vec3(265, 0, 295));
  world.add(box1_2);

  // Box 2
  // std::shared_ptr<Hittable> box2 = make_box(point3(0,0,0), point3(165,165,165), white);
  // box2 = std::make_shared<RotateY>(box2, -18);
  // box2 = std::make_shared<Translate>(box2, vec3(130,0,65));
  // world.add(box2);

  // Glass Sphere
  auto glass = std::make_shared<Dielectric>(1.5);
  world.add(std::make_shared<Sphere>(point3(190, 90, 190), 90, glass));

  // Special Sampling objects
  HittableListWithPDF important_sampling_objects;
  auto m = std::shared_ptr<Material>();
  important_sampling_objects.add(std::make_shared<Quadrilateral>(point3(213, 554, 227), vec3(130, 0, 0), vec3(0, 0, 105), m));
  important_sampling_objects.add(std::make_shared<Sphere>(point3(190, 90, 190), 90, m));

  Camera camera;

  camera.aspect_ratio = 1.0;
  camera.image_width = 600;
  camera.samples_per_pixel = 100;
  camera.max_depth = 50;
  camera.background = Color(0, 0, 0);

  camera.vfov = 40.0;
  camera.lookfrom = point3(278, 278, -800);
  camera.lookat = point3(278, 278, 0);
  camera.vup = vec3(0, 1, 0);

  camera.defocus_angle = 0.0;

  world = HittableList(std::make_shared<HittableList::BVHNode>(world));

  camera.render(world, important_sampling_objects);
}
