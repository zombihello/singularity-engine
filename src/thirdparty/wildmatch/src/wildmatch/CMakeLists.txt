# BS yehor.pohuliaka - BEGIN - Wildmatch integration
# add_library(wildmatch SHARED wildmatch.c)
# add_library(wildmatch-cxx SHARED wildmatch.cpp)
# 
# install(TARGETS wildmatch wildmatch-cxx EXPORT wildmatch DESTINATION ${CMAKE_INSTALL_LIBDIR})
# install(FILES wildmatch.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
# install(FILES wildmatch.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

add_library(wildmatch STATIC wildmatch.c)
install(TARGETS wildmatch EXPORT wildmatch DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES wildmatch.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
# BS yehor.pohuliaka - END - Wildmatch integration
