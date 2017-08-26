SET(OTF_SETUP_TARGET_NAME libotf)

SET(OTF_SETUP_HEADERS
    Task.h
    Master.h
    Node.h
    CmdLine.h
    SimpleMaster.h
    Subprocess.h
)

SET(OTF_SETUP_SOURCES
    ${JSONCPP_SOURCE_FILES}
    
    Task.cpp
    Master.cpp
    Node.cpp
    CmdLine.cpp
    SimpleMaster.cpp
    Subprocess.cpp
)

INCLUDE_DIRECTORIES(AFTER ${PROJECT_SOURCE_DIR}/src/ )
INCLUDE_DIRECTORIES(AFTER ${PROJECT_SOURCE_DIR}/3rdparty/mongoose/ )

OTF_SETUP_LIBRARY()





