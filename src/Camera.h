#pragma once
#include <iostream>
#include <memory>
#include <omp.h>
#include <optional>
#include <ostream>
#include "assert.h"
#include "Color.h"
#include "hittable/Hittable.h"
#include "Interval.h"
#include "hittable/HittableList.h"
#include "material/Material.h"
#include "pdf/MixturePDF.h"
#include "pdf/PDF.h"
#include "Ray.h"
#include "vec3.h"

struct Camera {
  double aspect_ratio = 1.0;      // Ratio of image width over height
  int    image_width = 100;       // Rendered image width in pixel count
  int    samples_per_pixel = 10;  // Count of random samples for each pixel
  int    max_depth = 10;          // Maximum number of ray bounces into scene
  Color  background;              // Scene background color

  double vfov     = 90;                // Vertical view angle (field of view)
  point3 lookfrom = point3(0, 0, -1);  // Point camera is looking from
  point3 lookat   = point3(0, 0, 0);   // Point camera is looking at
  vec3   vup      = vec3(0, 1, 0);     // Camera-relative "up" direction

  double defocus_angle = 0;   // Variation angle of rays through each pixel
  double focus_dist    = 10;  // Distance from camera lookfrom point to plane of perfect focus

  void render(const Hittable& world, const HittableListWithPDF& important_sampling_objects) {
    initialize();
    
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    auto time_begin = omp_get_wtime();
    for (int j = 0; j < image_height; ++j) {
      std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
      for (int i = 0; i < image_width; ++i) {
        Color pixel_color(0, 0, 0);
        for (int s_j = 0; s_j < sqrt_spp; ++s_j) {
          for (int s_i = 0; s_i < sqrt_spp; ++s_i) {
            Ray r = get_ray(i, j, s_i, s_j);
            pixel_color += ray_color(r,   max_depth, world, important_sampling_objects);
          }
        }
        write_color(std::cout, pixel_color, samples_per_pixel);
      }
    }
    auto time_end = omp_get_wtime();
    std::cerr << "The render took " << time_end - time_begin << " secends.\n";

    std::clog << "\rDone.                 \n";
  }

 private:
  int    image_height;    // Rendered image height
  int    sqrt_spp;        // Square root of number of samples per pixel
  double recip_sqrt_spp;  // 1 / sqrt_spp
  point3 center;          // Camera center
  point3 pixel00_loc;     // Location of pixel 0, 0
  vec3   pixel_delta_u;   // Offset to pixel to the right
  vec3   pixel_delta_v;   // Offset to pixel below
  vec3   u, v, w;         // Camera frame basis vectors
  vec3   defocus_disk_u;  // Defocus disk horizontal radius
  vec3   defocus_disk_v;  // Defocus disk vertical radius

  void initialize() {
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    center = lookfrom;

    // Determine viewport dimensions.
    auto theta = degrees_to_radians(vfov);
    auto h = tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

    sqrt_spp = static_cast<int>(sqrt(samples_per_pixel));
    recip_sqrt_spp = 1.0 / sqrt_spp;

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
    vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

    // Calculate the horizontal and vertical delta vectors to the next pixel.
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors.
    auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }

  Ray get_ray(int i, int j, int s_i, int s_j) const {
    // Get a randomly-sampled camera ray for the pixel at location i,j, originating from
    // the camera defocus disk, and randomly sampled around the pixel location.

    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
    auto pixel_sample = pixel_center + pixel_sample_square(s_i, s_j);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;

    return Ray(ray_origin, ray_direction);
  }

  vec3 pixel_sample_square(int s_i, int s_j) const {
    // Returns a random point in the square surrounding a pixel at the origin, given
    // the two subpixel indices.
    auto px = -0.5 + recip_sqrt_spp * (s_i + random_double());
    auto py = -0.5 + recip_sqrt_spp * (s_j + random_double());
    return (px * pixel_delta_u) + (py * pixel_delta_v);
  }

  vec3 pixel_sample_disk(double radius) const {
    // Generate a sample from the disk of given radius around a pixel at the origin.
    auto p = radius * random_in_unit_disk();
    return (p[0] * pixel_delta_u) + (p[1] * pixel_delta_v);
  }

  point3 defocus_disk_sample() const {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }

  Color ray_color(const Ray& ray_in, int depth, const Hittable& world, const HittableListWithPDF& important_sampling_objects) const {
    
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
      return Color(0, 0, 0);

    // If the ray hits nothing, return the background color.
    std::optional<HitRecord> hit_record = world.hit(ray_in, Interval(0.001, infinity));
    if (!hit_record.has_value())
      return background;
    
    Color color_from_emission = hit_record->material->emitted(ray_in, hit_record.value());
    std::optional<ScatterRecord> scatter_record = hit_record->material->scatter(ray_in, hit_record.value());
    if (!scatter_record.has_value())
      return color_from_emission;

    if (scatter_record->mode == ScatterRecord::ScatterMode::DIRECT_RAY) {
      return scatter_record->attenuation * ray_color(scatter_record->ray_out.value(), depth - 1, world, important_sampling_objects);
    } else {

      std::shared_ptr<PDF> mixture_pdf = nullptr;
      if (!important_sampling_objects.empty()) {
        auto special_pdf = important_sampling_objects.make_important_pdf(hit_record->p);
        special_pdf->generate();
        mixture_pdf = std::make_shared<MixturePDF>(special_pdf, scatter_record->pdf_out);
      } else {
        mixture_pdf = scatter_record->pdf_out; 
      }
      
      Ray ray_out = Ray(hit_record->p, mixture_pdf->generate());
      auto scatter_pdf_val = scatter_record->pdf_out->value(ray_out.direction);
      auto mixture_pdf_val = mixture_pdf->value(ray_out.direction);
      
      Color sample_color = ray_color(ray_out, depth - 1, world, important_sampling_objects);
      Color color_from_scatter = (scatter_record->attenuation * scatter_pdf_val * sample_color) / mixture_pdf_val;

      return color_from_emission + color_from_scatter;
    }
  }
};
