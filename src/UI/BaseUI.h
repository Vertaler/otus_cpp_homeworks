/**
 * @file BaseUI.h
 * @brief Contains Base UI components, not specific for application
 * @version 0.1
 * @date 2021-12-19
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

#include <map>
#include <memory>
#include <string>

namespace UI {

/**
 * @brief Various meta data related to mouse interaction, e.g. cursor positon,
 * mouse button
 *
 */
struct ClickMeta {
  // TBD
};

/**
 * @brief Base class for all UI components
 *
 */
class BaseComponent {
public:
  explicit BaseComponent(BaseComponent *parent = nullptr) : m_parent(parent) {}
  // Make pure virtual to deny class instantiation
  virtual ~BaseComponent() = 0;

  virtual void OnClick(const ClickMeta &cm) {
    if (m_parent)
      m_parent->OnClick(cm);
  }

protected:
  BaseComponent *Parent() { return m_parent; }

private:
  BaseComponent *m_parent{};
};

inline BaseComponent::~BaseComponent() = default;

using ComponentId = std::string;

class CompositeComponent : public BaseComponent {
public:
  using BaseComponent::BaseComponent;

  virtual ~CompositeComponent() = default;

  void AddComponent(ComponentId componentId,
                    std::unique_ptr<BaseComponent> component) {
    m_components.emplace(std::move(componentId), std::move(component));
  }

  // TODO: We should return iterator which returns reference to BaseComponent
  // on dereferencing instead of std::unique_ptr
  // Here we leave it as is because this homework is more about interfaces than
  // implemantion.
  auto begin() { return m_components.begin(); }
  auto end() { return m_components.cend(); }

private:
  std::map<ComponentId, std::unique_ptr<BaseComponent>> m_components;
};

/**
 * @brief Container for all UI components
 *
 */
class Window : public CompositeComponent {
public:
  Window() : CompositeComponent(nullptr) {}
};

class Button : public BaseComponent {
public:
  using BaseComponent::BaseComponent;
};

class Input : public BaseComponent {
public:
  using BaseComponent::BaseComponent;

  std::string GetContent() const { return "TODO"; }
};

} // namespace UI