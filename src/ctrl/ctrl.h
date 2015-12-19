/**
 * Helper interface / utilities to write our GUI a bit less painfully and with profiling.
 */
#ifndef SOLEMNSKY_CTRL_CTRL_H
#define SOLEMNSKY_CTRL_CTRL_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <functional>

/**
 * Little wrapper around the SFML drawing system, allowing for a simpler design of the methods that use it.
 */
class Frame {
private:
    sf::RenderWindow &window;

    sf::Transform parentTransform;
    std::stack<sf::Transform> transformStack;
    std::stack<float> alphaStack;

    const sf::Color alphaScaleColor(const sf::Color);

public:
    Frame(sf::RenderWindow &window);

    /**
     * Resetting / creating parent transform.
     */
    void resize();
    void reset();

    /**
     * Affine transformation / alpha value stack.
     */
    void pushTransform(const sf::Transform transform);
    void popTransform();
    void withTransform(const sf::Transform transform, std::function<void()> fn);
    void pushAlpha(const float alpha);
    void popAlpha();
    void withAlpha(const float alpha, std::function<void()> fn);
    void withAlphaTransform(const float alpha, const sf::Transform transform, std::function<void()> fn);

    /**
     * Drawing stuff, a fully const wrapper over SFML's Drawable system.
     */
    void drawCircle(const float radius, const sf::Color color=sf::Color());
};

/**
 * A user interface with everything you'd expect.
 */
class Control {
public:
    virtual void tick(double delta) = 0; // in milliseconds
    virtual void render(Frame &frame) = 0;
    virtual void handle(sf::Event event) = 0;
};

/**
 * A demo user interface.
 */
class DemoControl : public Control {
private:
    double time = 0;
public:
    void tick(double delta) override;
    void render(Frame &frame) override;
    void handle(sf::Event event) override;
};

/**
 * Run an interface with SFML, praised be.
 */
void run_sfml(Control &face);

#endif //SOLEMNSKY_CTRL_CTRL_H
