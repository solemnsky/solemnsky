#include "util/telegraph.h"
#include "util/methods.h"
#include "gtest/gtest.h"
#include <SFML/Graphics.hpp>

/**
 * Make sure sf::Transform is behaving correctly.
 */

class TransformTest: public testing::Test {
  public:
    TransformTest() { }
};

/**
 * Things work properly.
 */
TEST_F(TransformTest, BasicTest) {
  sf::Transform transform;
  ASSERT_EQ(transform.getMatrix()[0], 1.0f);

  sf::Transform otherTransform = sf::Transform().translate(0, 0);
  ASSERT_EQ(transform.getMatrix()[0], 1.0f);

  sf::Transform combinedTransform{transform};
  combinedTransform.combine(otherTransform);
  ASSERT_EQ(transform.getMatrix()[0], 1.0f);
}
