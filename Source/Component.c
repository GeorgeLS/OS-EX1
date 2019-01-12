#include "../Include/Component.h"
#include "../Include/Macros.h"

Component *createComponent(ComponentType type) {
    Component *new_component = __MALLOC(1, Component);
    GENERATE_ID(new_component->id);
    new_component->type = type;
    __TIME_MS(new_component->processingTime);
    return new_component;
}