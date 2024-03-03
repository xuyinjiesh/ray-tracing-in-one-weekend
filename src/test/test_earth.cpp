// #include "test.h"

// #include <memory>
// #include "Camera.h"
// #include "hittable/HittableList.h"
// #include "hittable/Sphere.h"
// #include "material/Lambertian.h"
// #include "texture/ImageTexture.h"

// void test_earth() {
//   auto earth_texture = std::make_shared<ImageTexture>("earthmap.jpg");
//   auto earth_surface = std::make_shared<Lambertian>(earth_texture);
//   auto globe = std::make_shared<Sphere>(point3(0, 0, 0), 2, earth_surface);

//   Camera cam;

//   cam.aspect_ratio = 16.0 / 9.0;
//   cam.image_width = 400;
//   cam.samples_per_pixel = 100;
//   cam.max_depth = 50;
//   cam.background = Color(0.70, 0.80, 1.00);

//   cam.vfov = 20;
//   cam.lookfrom = point3(0, 0, 12);
//   cam.lookat = point3(0, 0, 0);
//   cam.vup = vec3(0, 1, 0);

//   cam.defocus_angle = 0;

//   auto important_sampling_objects = HittableListWithPDF();
//   cam.render(HittableList(globe), important_sampling_objects);
// }