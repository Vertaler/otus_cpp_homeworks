
#include "Application/ApplicationController.h"
#include "UI/BaseUI.h"
#include "UI/Canvas.h"

// UI handlers should be in main.cpp according homework requirements
namespace UI {

// All these buttons calls corresponding application methods
// Save/Load buttons also gets filename from input
class RemoveSelectedPrimitiveButton : public Button {};
class SelectPointPrimitiveButton : public Button {};
class SelectLinePrimitiveButton : public Button {};
class SelectCircleButton : public Button {};
class SelectTriangleButton : public Button {};
class SelectRectanglePrimitiveButton : public Button {};
class NewDocumentButton : public Button {};
class SaveDocumentButton : public Button {};
class LoadDocumentButton : public Button {};

class ApplicationWindow : Window {
public:
  ApplicationWindow(const ApplicationController &controller) {
    // Saves reference to application controller creates application specific
    // buttons and canvas
  }
};

} // namespace UI

int main(int argc, const char *const argv[]) {
  ApplicationController controller;
  UI::ApplicationWindow applicationWindow{controller};
  controller.AwaitForStop();
  return 0;
}
