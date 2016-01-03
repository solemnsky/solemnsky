#include <assert.h>
#include "resources.h"

const static std::vector<ResRecord> data{ // edit this side-by-side with resources.h
    {"fonts/arial.ttf",                 false},
    {"render-3d/test_1/player_200.png", true, 200, 200, 2, 15},
    {"render-2d/placeholder/title.png", false}
};

static bool asserted{false}; // make sure we have enough entries in data
#define ASSERT if (!asserted) { assert(data.size() == (unsigned long long int) Res::LAST); asserted = true; }
// TODO: move this check to compile-time? >_>

std::string filepathTo(const Res res) {
  ASSERT;
  const ResRecord &record = recordOf(res);
  return "../../media/" + record.path;
}

ResRecord recordOf(const Res res) {
  ASSERT;
  return data[(unsigned long long int) res];
}

