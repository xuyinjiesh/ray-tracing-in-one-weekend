#include <memory>
#include "Camera.h"
#include "Color.h"
#include "hittable/Hittable.h"
#include "hittable/HittableList.h"
#include "hittable/Sphere.h"
#include "material/Dielectric.h"
#include "material/Lambertian.h"
#include "material/Material.h"
#include "material/Metal.h"
#include "test.h"
#include "texture/CheckerTexture.h"

void test_rt_book_1_final() {
  
  using namespace std;
  HittableList world;

  auto checker = make_shared<CheckerTexture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));
  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, make_shared<Lambertian>(checker)));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<Material> Sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = Color::random() * Color::random();
          Sphere_material = make_shared<Lambertian>(albedo);
          // auto center2 = center + vec3(0, random_double(0,.5), 0);
          world.add(make_shared<Sphere>(center, 0.2, Sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = Color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          Sphere_material = make_shared<Metal>(albedo, fuzz);
          world.add(make_shared<Sphere>(center, 0.2, Sphere_material));
        } else {
          // glass
          Sphere_material = make_shared<Dielectric>(1.5);
          world.add(make_shared<Sphere>(center, 0.2, Sphere_material));
        }
      }
    }
  }

  auto material1 = make_shared<Dielectric>(1.5);
  world.add(make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
  world.add(make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));

  world = HittableList(make_shared<HittableList::BVHNode>(world));

  Camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.background = Color(0.70, 0.80, 1.00);

  cam.vfov = 20;
  cam.lookfrom = point3(13, 2, 3);
  cam.lookat = point3(0, 0, 0);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 0.02;
  cam.focus_dist = 10.0;

  cam.render(world, HittableListWithPDF());
}