#pragma once

#include <filesystem>
#include <functional>

#include "Drawing/IPointDrawer.h"
#include "Models/CanvasData.h"
#include "Models/GraphicalPrimitives.h"
#include "Models/Serialization.h"

using OnCanvasUpdate = std::function<void(Models::CanvasData)>;

/**
 * @brief Main controller class of application
 *
 */
class ApplicationController {
public:
  /**
   *
   * @brief Wait stop signal
   *
   */
  void AwaitForStop() {}

  /**
   * @brief Select primitive for adding to canvas
   *
   */
  void SelectPrimitiveForAdding(Models::GraphicalPrimitive primitive) {}
  /**
   * @brief Notifies about click on canvas.
   * Can select element on canvas or add element depending on current state
   * of application controller
   */
  void OnCanvasClick(uint32_t x, uint32_t y) {}

  /**
   * @brief Creates new document. Cleans current canvas.
   *
   */
  void NewDocument() {}

  /**
   * @brief Loads saved canvas state from specified file.
   *
   */
  void LoadDocument(const std::filesystem::path &filePath) {}

  /**
   * @brief Saves canvas state to specified file.
   *
   */
  void SaveDocument(const std::filesystem::path &filePath) {}
};