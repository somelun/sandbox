#include "renderer.h"
#include "framebuffer.h"

Renderer::Renderer(uint16_t width, uint16_t height) {
    framebuffer_ = new Framebuffer(width, height);
}

Renderer::~Renderer() {
    delete framebuffer_;
}

void Renderer::Present() {
    //
}

void Renderer::DrawTriangle2D(const Color& color, Point p1, Point p2, Point p3) {
    // check for horizontal or vertical
    if ((p1.x == p2.x && p2.x == p3.x) || (p1.y == p2.y && p2.y == p3.y)) {
        return;
    }

    if (p2.y < p1.y) {
        std::swap(p1, p2);
    }

    if (p3.y < p1.y) {
        std::swap(p1, p3);
    }

    if (p3.y < p2.y) {
        std::swap(p2, p3);
    }

    if (p1.y == p2.y) {
        DrawTriangleTop(color, p1, p2, p3);
    } else {
        if (p2.y == p3.y) {
            DrawTriangleBottom(color, p1, p2, p3);
        } else {
            int new_x = p1.x + (int)(0.5f + (float)(p2.y - p1.y) * (float)(p3.x - p1.x) / (float)(p3.y - p1.y));

            DrawTriangleBottom(color, p1, {new_x, p2.y}, p2);
            DrawTriangleTop(color, p2, {new_x, p2.y}, p3);
        }
    }
}

void Renderer::DrawTriangleTop(const Color& color, Point p1, Point p2, Point p3) {
    // if (p1.y == p3.y || p2.y == p3.y) {
    //     return;
    // }
    //
    // if (p2.x < p1.x) {
    //     std::swap(p1, p2);
    // }
    //
    // int height = p3.y - p1.y;
    //
    // int dx_left  = ((p3.x - p1.x) << FIXP16_SHIFT) / height;
    // int dx_right = ((p3.x - p2.x) << FIXP16_SHIFT) / height;

    int min_clip_x = 0;
    int max_clip_x = 639;
    int min_clip_y = 0;
    int max_clip_y = 479;

    uint8_t* dest_framebuffer_ = nullptr;

    int mempitch = 3;

    float dx_right,    // the dx/dy ratio of the right edge of line
        dx_left,     // the dx/dy ratio of the left edge of line
        xs,xe,       // the starting and ending points of the edges
        height;      // the height of the triangle

    int temp_y,
        right,         // used by clipping
        left;

    uint8_t* dest_addr = NULL;

    if (p2.x < p1.x) {
        std::swap(p1, p2);
    }

    height = p3.y - p1.y;

    dx_left  = (p3.x - p1.x) / height;
    dx_right = (p3.x - p2.x) / height;

    xs = (float)p1.x;
    xe = (float)p2.x + 0.5f;

    if (p1.y < 0) {
        xs = xs + dx_left * (float)(-p1.y + min_clip_y);
        xe = xe + dx_right * (float)(-p1.y + min_clip_y);

        p1.y = 0;

    }

    if (p3.y > max_clip_y) {
        p3.y = max_clip_y;
    }

    dest_addr = dest_framebuffer_ + p1.y * mempitch;

    if (p1.x >= min_clip_x && p1.x <= max_clip_x &&
        p2.x >= min_clip_x && p2.x <= max_clip_x &&
        p3.x >= min_clip_x && p3.x <= max_clip_x) {

        for (temp_y = p1.y; temp_y <= p3.y; temp_y++, dest_addr += mempitch) {

            // memset((UCHAR*)dest_addr + (unsigned int)xs, color, (unsigned int)((int)xe - (int)xs + 1));

            // __asm__(
            //     "mov edi, dest"
            //     "mov ecx, count"
            //     "mov ax,  data"
            //     "rep stosw"
            // );

            DrawLine(color, xs, temp_y, xe, temp_y);

            xs += dx_left;
            xe += dx_right;

        }
    } else {
        for (temp_y = p1.y; temp_y <= p3.y; temp_y++, dest_addr += mempitch) {
            left  = (int)xs;
            right = (int)xe;

            xs += dx_left;
            xe += dx_right;

            if (left < min_clip_x) {
                left = min_clip_x;

                if (right < min_clip_x) {
                    continue;
                }
            }

            if (right > max_clip_x) {
                right = max_clip_x;

                if (left > max_clip_x) {
                    continue;
                }
            }

            // memset((UCHAR*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1));

            DrawLine(color, left, temp_y, right - left + 1, temp_y);
        }
    }
}

void Renderer::DrawTriangleBottom(const Color& color, Point p1, Point p2, Point p3) {
    float dx_right,    // the dx/dy ratio of the right edge of line
        dx_left,     // the dx/dy ratio of the left edge of line
        xs,xe,       // the starting and ending points of the edges
        height;      // the height of the triangle

    int temp_y,
        right,         // used by clipping
        left;

    int min_clip_x = 0;
    int max_clip_x = 639;
    int min_clip_y = 0;
    int max_clip_y = 479;

    if (p3.x < p2.x) {
        std::swap(p2, p3);
    }

    height = p3.y - p1.y;

    dx_left  = (p2.x - p1.x) / height;
    dx_right = (p3.x - p1.x) / height;

    xs = (float)p1.x;
    xe = (float)p1.x; // +(float)0.5;

    if (p1.y < min_clip_y) {
        xs = xs + dx_left * (float)(-p1.y + min_clip_y);
        xe = xe + dx_right * (float)(-p1.y + min_clip_y);

        p1.y = min_clip_y;
    }

    if (p3.y > max_clip_y) {
        p3.y = max_clip_y;
    }

    if (p1.x >= min_clip_x && p1.x <= max_clip_x &&
        p2.x >= min_clip_x && p2.x <= max_clip_x &&
        p3.x >= min_clip_x && p3.x <= max_clip_x) {

        for (temp_y = p1.y; temp_y <= p3.y; temp_y++/*, dest_addr += mempitch*/) {
            // memset((UCHAR  *)dest_addr+(unsigned int)xs, color,(unsigned int)((int)xe-(int)xs+1));

            // std::cout << xs << ", " << xe << std::endl;

            DrawLine(color, xs, temp_y, xe, temp_y);

        // std::cout << temp_y << ", " << xs << ", " << (int)xe - (int)xs + 1 << std::endl;

            xs += dx_left;
            xe += dx_right;
        }
    } else {
        for (temp_y = p1.y; temp_y <= p3.y; temp_y++/*, dest_addr += mempitch*/) {
            left  = (int)xs;
            right = (int)xe;

            xs += dx_left;
            xe += dx_right;

            if (left < min_clip_x) {
                left = min_clip_x;

                if (right < min_clip_x) {
                    continue;
                }
            }

            if (right > max_clip_x) {
                right = max_clip_x;

                if (left > max_clip_x) {
                    continue;
                }
            }

            // memset((UCHAR  *)dest_addr+(unsigned int)left, color,(unsigned int)(right-left+1));

            DrawLine(color, left, temp_y, right - left + 1, temp_y);

        }
    }
}

void Renderer::DrawQuad(const Color& color, Point p1, Point p2, Point p3, Point p4) {
    DrawTriangle2D(color, p1, p2, p3);
    DrawTriangle2D(color, p2, p3, p4);
}

// fills the whole screen with one color
void Renderer::FillScreen(const Color& color) {
    uint8_t* data = framebuffer_->get_data();

    size_t size = framebuffer_->get_width() * framebuffer_->get_height() * 4;

    //

    // return;
    for (size_t i = 0; i < size; i += 4) {
        data[i]     = color.r();
        data[i + 1] = color.g();
        data[i + 2] = color.b();
        data[i + 3] = color.a();
    }
    // TODO:
    // __asm__(//"lea dword rdi, data"
            // "mov dword rdi, data"
            // "mov ecx, size"
            // "mov eax, color"
            //"rep stosw"
        //mov edi, dest   ; edi points to destination memory
        //    mov ecx, count  ; number of 16-bit words to move
        //    mov ax,  data   ; 16-bit data
        //    rep stosw       ; move data
        // );
        //
}

void Renderer::DrawPixel(const Color& color, uint16_t x, uint16_t y) {
    uint8_t* data = framebuffer_->get_data();

    uint32_t index = (y * framebuffer_->get_width() + x) * 4;
    data[index]     = color.r();
    data[index + 1] = color.g();
    data[index + 2] = color.b();
    data[index + 3] = color.a();
}

// https://github.com/ssloy/tinyrenderer/wiki/Lesson-1-Bresenham%E2%80%99s-Line-Drawing-Algorithm
void Renderer::DrawLine(const Color& color, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int16_t dx = x1 - x0;
    int16_t dy = y1 - y0;
    uint16_t derror2 = std::abs(dy) * 2;
    uint16_t error2 = 0;
    uint16_t y = y0;
    for (size_t x = x0; x <= x1; ++x) {
        if (steep) {
            DrawPixel(color, y, x);
        } else {
            DrawPixel(color, x, y);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1: -1);
            error2 -= dx * 2;
        }
    }
}
