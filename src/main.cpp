// std
#include <complex>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <thread>
#include <chrono>
#include <tuple>

// my
#include "sdl_module.hpp"

double map(double value, double istart, double istop, double ostart, double ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

std::tuple<int, int, int> get_rgb_smooth(int n, int iter_max) {
  	// map n on the 0..1 interval
  	double t = (double)n/(double)iter_max;
  
  	// Use smooth polynomials for r, g, b
  	int r = (int)(9*(1-t)*t*t*t*255);
  	int g = (int)(15*(1-t)*(1-t)*t*t*255);
  	int b =  (int)(8.5*(1-t)*(1-t)*(1-t)*t*255);
    return std::tuple<int, int, int>(r, g, b);
}

int 
main() {
    std::cout << "Mandelbrot Renderer Started!" << std::endl;
    
    try {
        sdl_module sdl("Mandelbrot Renderer", 500, 500);

        auto width = sdl.get_renderer_width();
        auto height = sdl.get_renderer_height();

        std::vector<std::uint32_t> pixels(sdl.get_renderer_width() * sdl.get_renderer_height());
        std::fill(std::begin(pixels), std::end(pixels), 0);

        bool running = true;
        int max_iterations = 500;

        double min_x = -2.0f;
        double max_x = 1.2f;
        double min_i = -1.7f;
        double max_i = 1.7f;

        double zoom = 1.0;

        double center_x = 0.0f;
        double center_y = 0.0f;

        std::uint8_t time = 0;

        while(running) {

            while (SDL_PollEvent(&sdl.m_event)) {

                if (sdl.m_event.type == SDL_QUIT) {
                    running = false;
                    break;
                }
            }
            
            sdl.clear_back_buffer();
            std::fill(std::begin(pixels), std::end(pixels), 0); 

            auto start = std::chrono::system_clock::now();
            std::complex<double> c(0);
            std::complex<double> z(0);
           
            for (auto y = 0; y < width; y++) {
                for (auto x = 0; x < height; x++) {
                    
                    z = {0, 0}; 
                    c = {map(x, 0.0f, width, min_x, max_x) * zoom, map(y, 0.0f, height, min_i, max_i) * zoom};
                    
                    int iteration = 0; 
            
                    while (std::norm(z) < 4.0f && iteration < max_iterations) {
                        z = z * z + c; // mandelbrot calculation
                        iteration++;
                    }
                    
                    auto colour_tuple = get_rgb_smooth(iteration, max_iterations); 
                    
                                            // alpha                   red                          green                              blue  
                    std::uint32_t colour = (255 << 24) + (std::get<0>(colour_tuple) << 16) + (std::get<1>(colour_tuple) << 8) + std::get<2>(colour_tuple);  
                    pixels[x + y * width] = colour;

                }
            }

            sdl.copy_pixel_buffer_into_render_texture(pixels, sdl.get_renderer_width());

            auto end = std::chrono::system_clock::now();
            auto passed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "frame time: " << passed.count() << "ms" << std::endl;

        }

    } catch (sdl_module_exception & e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {

    }

    std::cout << "Mandelbrot Renderer Finished!" << std::endl;
}
