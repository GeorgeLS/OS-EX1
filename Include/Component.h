#ifndef EXCERCISE_I_COMPONENT_H
#define EXCERCISE_I_COMPONENT_H

typedef enum {
    Type1,
    Type2,
    Type3
} ComponentType;

typedef struct component {
    char id[13];
    ComponentType type;
    unsigned long processingTime;
} Component;

Component *createComponent(ComponentType type);

#endif //EXCERCISE_I_COMPONENT_H
