#pragma once

#include "Application/ApplicationController.h"

#include "BaseUI.h"
#include "Drawing/Drawer.h"
#include "Models/CanvasData.h"
#include "Models/Drawing.h"

namespace UI {

class Canvas : public BaseComponent, public Drawing::IPointDrawer {
public:
  Canvas(ApplicationController &) {
    // Saves reference to application controller
    // Subscribe to canvas update
    // and calls Draw on each update
  }

  void OnClick(const ClickMeta &cm) override {
    // Calculate click position on canvas and notify ApplicationController about
    // click
  }

  void Draw(const Models::Point &) const override {
    // Drawing point on canvas
  }

private:
  void DrawAll(const Models::CanvasData &data) {
    // Drawing all changed primitives on canvas.
    for (const auto &prim : data.primitives) {
      m_drawer.Draw(*this, prim.second);
    }
  }

private:
  Drawing::Drawer m_drawer;
};

} // namespace UI