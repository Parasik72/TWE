#ifndef UR_COMMANDS_HPP
#define UR_COMMANDS_HPP

#include "camera-component/add-camera-component-command.hpp"
#include "camera-component/remove-camera-component-command.hpp"
#include "camera-component/change-camera-component-state-command.hpp"

#include "light-component.hpp/add-light-component-command.hpp"
#include "light-component.hpp/remove-light-component-command.hpp"
#include "light-component.hpp/change-light-component-state-command.hpp"

#include "mesh-component/add-mesh-component-command.hpp"
#include "mesh-component/remove-mesh-component-command.hpp"
#include "mesh-component/change-mesh-component-state-command.hpp"

#include "mesh-renderer-component/remove-mesh-renderer-component-command.hpp"
#include "mesh-renderer-component/change-mesh-rendere-component-state-command.hpp"

#include "physics-component/add-physics-component-command.hpp"
#include "physics-component/remove-physics-component-command.hpp"
#include "physics-component/set-collider-type-physics-component-command.hpp"
#include "physics-component/change-physics-component-properties-command.hpp"

#include "script-component/add-script-component-command.hpp"
#include "script-component/remove-script-component-command.hpp"
#include "script-component/set-script-component-behavior-command.hpp"
#include "script-component/set-script-component-enabled-state-command.hpp"

#include "transform-component/change-transform-component-state.hpp"
#include "name-component/change-name-component-state-command.hpp"

#include "entity/create-entity-command.hpp"
#include "entity/remove-entity-command.hpp"
#include "entity/copy-entity-command.hpp"
#include "entity/drag-and-drop-entity-command.hpp"

#endif