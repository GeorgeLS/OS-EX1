#ifndef EXCERCISE_I_STAGES_H
#define EXCERCISE_I_STAGES_H

#include "Attributes.h"

#define CREATE_PAINT_PATH "Source/Main.c"
#define PAINT_CHECK_PATH1 "Source/Processes.c"
#define PAINT_CHECK_PATH2 "Include/Macros.h"
#define PAINT_CHECK_PATH3 "Source/Component.c"
#define CHECK_COMPOSE_PATH1 "Include/Component.h"
#define CHECK_COMPOSE_PATH2 "Include/Attributes.h"
#define CHECK_COMPOSE_PATH3 "Include/Processes.h"

__NO_RETURN void firstComponentCreator(void);
__NO_RETURN void secondComponentCreator(void);
__NO_RETURN void thirdComponentCreator(void);
__NO_RETURN void paint(void);
__NO_RETURN void firstComponentChecker(void);
__NO_RETURN void secondComponentChecker(void);
__NO_RETURN void thirdComponentChecker(void);
__NO_RETURN void composer(void);

#endif //EXCERCISE_I_STAGES_H