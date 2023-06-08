set(PROJECT_NAME use_all)

add_library(${PROJECT_NAME}
	"Type/TypeSystem.h"
	"Macro/MacroDef.h"
	"Macro/MacroDef.cpp"
    "Macro/InputUtilities.h"
    "Macro/InputUtilities.cpp"
 )

 include_directories(".") # So we can include sub folders 

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)