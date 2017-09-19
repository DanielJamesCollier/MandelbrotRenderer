// std
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <complex>

// my
#include "sdl_module.hpp"

float map(float value, float istart, float istop, float ostart, float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
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
        int iterations = 15;

        float min_x = -2.0f;
        float max_x = 1.0f;
        float min_i = -1.5f;
        float max_i = 1.5f;

        while(running) {

            while (SDL_PollEvent(&sdl.m_event)) {

                if (sdl.m_event.type == SDL_QUIT) {
                    running = false;
                    break;
                }
            }
            
            sdl.clear_back_buffer();
    
            // Z = Z^2 + C
            for (auto y = 0; y < width; y++) {
                for (auto x = 0; x < height; x++) {

                    std::complex<float> c(map(x, 0, width, min_x, max_x),
                                          map(y, 0, height, min_i, max_i));

                    std::complex<float> z_previous(0, 0);
                    std::complex<float> z_current(0, 0);
                    
                    
                    int current_iteration = 0;


                    while (current_iteration < iterations) {
                        z_previous = z_current;

                        z_current = std::complex<float>(std::pow(z_previous.real(), 2) - std::pow(z_previous.imag(), 2) + c.real(), 
                                                        2.0f * z_previous.real() * z_previous.imag() + c.imag());


                        if (std::norm(z_current) > 4.0f) break; 

                        current_iteration++;
                    }

                    std::uint8_t blue = 0;

                    if (current_iteration != iterations) {
                        blue  = static_cast<int>(current_iteration) * 20;
                    }
                                           // alpha    + blue  
                    std::uint32_t colour = (255 << 24) + blue; 
                    pixels[x + y * width] = colour;

                }
            }

            sdl.copy_pixel_buffer_into_render_texture(pixels, sdl.get_renderer_width());
        }

    } catch (sdl_module_exception & e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {

    }

    std::cout << "Mandelbrot Renderer Finished!" << std::endl;
}
