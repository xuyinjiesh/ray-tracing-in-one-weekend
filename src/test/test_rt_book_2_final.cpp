#include <memory>
#include "Camera.h"
#include "Color.h"
#include "hittable/ConstantMedium.h"
#include "hittable/Hittable.h"
#include "hittable/HittableList.h"
#include "hittable/Quadrilateral.h"
#include "hittable/RotateY.h"
#include "hittable/Sphere.h"
#include "hittable/Translate.h"
#include "material/Dielectric.h"
#include "material/DiffuseLight.h"
#include "material/Lambertian.h"
#include "material/Metal.h"
#include "test.h"
#include "texture/ImageTexture.h"
#include "texture/NoiseTexture.h"

void test_rt_book_2_final() {
  using namespace std;
  HittableList boxes1;
  auto ground = make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

  int boxes_per_side = 20;
  for (int i = 0; i < boxes_per_side; i++) {
    for (int j = 0; j < boxes_per_side; j++) {
      auto w = 100.0;
      auto x0 = -1000.0 + i * w;
      auto z0 = -1000.0 + j * w;
      auto y0 = 0.0;
      auto x1 = x0 + w;
      auto y1 = random_double(1, 101);
      auto z1 = z0 + w;
      std::shared_ptr<Hittable> box = make_box(point3(x0, y0, z0), point3(x1, y1, z1), ground);
      boxes1.add(box);
    }
  }

  HittableList world;

  world.add(make_shared<HittableList::BVHNode>(boxes1));

  auto light = make_shared<DiffuseLight>(Color(7, 7, 7));
  world.add(make_shared<Quadrilateral>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

  auto center1 = point3(400, 400, 200);
  auto center2 = center1 + vec3(30, 0, 0);
  auto sphere_material = make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
  // world.add(make_shared<Sphere>(center1, center2, 50, sphere_material));

  world.add(make_shared<Sphere>(point3(260, 150, 45), 50, make_shared<Dielectric>(1.5)));
  world.add(make_shared<Sphere>(
      point3(0, 150, 145), 50, make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)));

  auto boundary = make_shared<Sphere>(point3(360, 150, 145), 70, make_shared<Dielectric>(1.5));
  world.add(boundary);
  world.add(make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
  boundary = make_shared<Sphere>(point3(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
  world.add(make_shared<ConstantMedium>(boundary, .0001, Color(1, 1, 1)));

  auto emat = make_shared<Lambertian>(make_shared<ImageTexture>("earthmap.jpg"));
  world.add(make_shared<Sphere>(point3(400, 200, 400), 100, emat));
  auto pertext = make_shared<NoiseTexture>(0.1);
  world.add(make_shared<Sphere>(point3(220, 280, 300), 80, make_shared<Lambertian>(pertext)));

  HittableList boxes2;
  auto white = make_shared<Lambertian>(Color(.73, .73, .73));
  int ns = 1000;
  for (int j = 0; j < ns; j++) {
    boxes2.add(make_shared<Sphere>(point3::random(0, 165), 10, white));
  }

  world.add(make_shared<Translate>(
      make_shared<RotateY>(
          make_shared<HittableList::BVHNode>(boxes2), 15),
      vec3(-100, 270, 395)));

  Camera cam;

  cam.aspect_ratio = 1.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 1000;
  cam.max_depth = 20;
  cam.background = Color(0, 0, 0);

  cam.vfov = 40;
  cam.lookfrom = point3(478, 278, -600);
  cam.lookat = point3(278, 278, 0);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 0;

  auto important_sampling_objects = HittableListWithPDF();
  important_sampling_objects.add(make_shared<Quadrilateral>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));
  cam.render(world, important_sampling_objects);
}