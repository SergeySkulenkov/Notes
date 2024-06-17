cmake_minimum_required(VERSION 3.5)


configure_file(hunvisapi.h.in hunvisapi.h)
   
set(MODULE_FILES
    affentry.cxx
    affixmgr.cxx
    csutil.cxx
    filemgr.cxx
    hashmgr.cxx
    hunspell.cxx
    hunzip.cxx
    phonet.cxx
    replist.cxx
    suggestmgr.cxx
    atypes.hxx
    hunspell.h
    hunspell.hxx
    w_char.hxx
    ${CMAKE_CURRENT_BINARY_DIR}/hunvisapi.h
)


add_library(HunspellLib SHARED ${MODULE_FILES})

target_include_directories(HunspellLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS HunspellLib DESTINATION HunspellLib-bin)
install(FILES hunspell.h DESTINATION HunspellLib-include)

