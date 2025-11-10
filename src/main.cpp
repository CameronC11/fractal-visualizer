#include <SFML/Graphics.hpp>
#include <iostream>
#include <complex>

constexpr unsigned int WIN_WIDTH = 900u;
constexpr unsigned int WIN_HEIGHT = 900u;
constexpr unsigned int VIEWPORT_SIZE = 900u;

int zoom_lvl = 1;
constexpr int ZOOM_FACTOR = 2;

constexpr double MIN_REAL = -2.0;
constexpr double MAX_REAL = 1.0;
constexpr double MIN_IMAG = -1.5;
constexpr double MAX_IMAG = 1.5;
double min_real = MIN_REAL;
double max_real = MAX_REAL;
double min_imag = MIN_IMAG;
double max_imag = MAX_IMAG;

int mandelbrot(std::complex<double> c, int max_iter) {
    std::complex<double> z = 0;
    int iter = 0;
    while (std::abs(z) < 2 && iter < max_iter) {
        z = z * z + c;
        iter++;
    }
    return iter;
}

void displayMandelbrot(sf::Image &img, const int max_iter = 25u) {
    const unsigned int HEIGHT = img.getSize().y;
    const unsigned int WIDTH = img.getSize().x;

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            // Map pixel coords to complex plan coords
            double x_0 = min_real + (static_cast<double>(x) / WIDTH) * (max_real - min_real);
            double y_0 = min_imag + (static_cast<double>(y) / HEIGHT) * (max_imag - min_imag);
            std::complex<double> c(x_0, y_0);

            int iter = mandelbrot(c, max_iter);

            if (iter == max_iter) {
                img.setPixel(sf::Vector2u(x, y), sf::Color::Black);
            }
            else {
                double factor = std::sqrt(static_cast<double>(iter) / max_iter);
                double intensity = std::round(max_iter * factor);
                img.setPixel(sf::Vector2u(x, y), sf::Color(0, 0, intensity*10));
            }
        }
    }
}

void zoomIn(int x, int y) {
    double x_0 = min_real + (static_cast<double>(x) / VIEWPORT_SIZE) * (max_real - min_real);
    double y_0 = min_imag + (static_cast<double>(y) / VIEWPORT_SIZE) * (max_imag - min_imag);
    std::complex<double> c(x_0, y_0);
    min_real = (x_0 + min_real) / ZOOM_FACTOR;
    max_real = (x_0 + max_real) / ZOOM_FACTOR;
    min_imag = (y_0 + min_imag) / ZOOM_FACTOR;
    max_imag = (y_0 + max_imag) / ZOOM_FACTOR;
    zoom_lvl++;
}

void zoomOut(int x, int y) {
    zoom_lvl--;
    if (zoom_lvl <= 1) {
        zoom_lvl = 1;
        min_real = -2.0;
        max_real = 1.0;
        min_imag = -1.5;
        max_imag = 1.5;
        return;
    }
    double x_0 = min_real + (static_cast<double>(x) / VIEWPORT_SIZE) * (max_real - min_real);
    double y_0 = min_imag + (static_cast<double>(y) / VIEWPORT_SIZE) * (max_imag - min_imag);
    std::complex<double> c(x_0, y_0);
    std::cout << "c = " << c.real() << " + " << c.imag() << "\n";
    min_real = (c.real() + min_real) * ZOOM_FACTOR;
    max_real = (c.real() + max_real) * ZOOM_FACTOR;
    min_imag = (c.imag() + min_imag) * ZOOM_FACTOR;
    max_imag = (c.imag() + max_imag) * ZOOM_FACTOR;
}

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({WIN_WIDTH, WIN_HEIGHT}), "Fractal Visualizer");
    window.setFramerateLimit(144);

    sf::Image img(sf::Vector2u(WIN_HEIGHT, WIN_HEIGHT), sf::Color::White);

    while (window.isOpen())
    {
        displayMandelbrot(img, 25u * (log2(static_cast<double>(zoom_lvl)) + 1));

        // Detect events
        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (auto *scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
                if (scroll->delta > 0) {
                    std::cout << "Zoom in at " << "x: " << scroll->position.x << " y: " << scroll->position.y << "\n";
                    zoomIn(scroll->position.x, scroll->position.y);
                }
                else {
                    std::cout << "Zoom out at " << "x: " << scroll->position.x << " y: " << scroll->position.y << "\n";
                    zoomOut(scroll->position.x, scroll->position.y);
                }
            }
        }

        sf::Texture texture;
        if (!texture.loadFromImage(img)) { return 0; }
        sf::Sprite sprite(texture);

        window.clear();
        window.draw(sprite);
        window.display();
    }
}
