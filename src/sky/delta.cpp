#include "delta.h"

namespace sky {

void PlaneDelta::apply(Plane &plane) {
  if (nameDelta) plane.name = *nameDelta;
  if (tuningDelta) plane.tuning = *tuningDelta;

  // reset or update plane.vital
  if (!vitalDelta) {
    plane.vital.reset();
  } else {
    // make sure plane.vital is constructed and correctly so
    if (tuningDelta or !plane.vital)
      plane.vital.emplace(PlaneVital(plane.tuning, {0, 0}, 0));

    plane.vital = *vitalDelta;
  }
}

}
