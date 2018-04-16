
#find FLTK
set(FLTK_SKIP_FLUID true)
find_package(FLTK)
execute_process(COMMAND fltk-config --ldflags OUTPUT_VARIABLE FLTK_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

cmake_dependent_option(ENABLE_QUICKTEST "LimeSuite device utility" ON "ENABLE_LIBRARY;FLTK_FOUND" OFF)
add_feature_info(LimeSDR-QuickTest ENABLE_QUICKTEST "LimeSDR-QuickTest Utility")

if(ENABLE_QUICKTEST)
    include_directories(${FLTK_INCLUDE_DIR})
    add_executable(LimeSDR-QuickTest LimeSDRTest.cpp TestGUI.cpp resources/resource.rc  ${PROJECT_SOURCE_DIR}/external/kissFFT/kiss_fft.c)
    target_link_libraries(LimeSDR-QuickTest
		      LimeSuite
		      ${FLTK_LDFLAGS}
		      ${FLTK_LIBRARIES}
		      ${GLUT_LIBRARY}
		      )
    install(TARGETS LimeSDR-QuickTest DESTINATION bin)
endif()





